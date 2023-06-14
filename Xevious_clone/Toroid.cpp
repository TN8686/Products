#include "Toroid.h"

/* 
* コンストラクタの引数は、なるべく整理してEnemyクラスのコンストラクタの引数の順番と一致させてください
* その下でEnemyクラスのコンストラクタを呼んでいます　改行していますが、一行の構文です　「 子クラス名(子クラス引数リスト) : 親クラス名(親クラス引数リスト) 」となっています
* また、全ての敵で一応X座標Y座標を指定できるようにする事にしました　出現Y座標が本来固定の敵も引数に用意してください
* Enemyクラスのコンストラクタには、リテラルな値か、参照可能な値が使用できます　子クラスのコンストラクタの引数から入って来た値も参照可能です
* Enemyクラスのコンストラクタの引数にあるものは、ここで代入してください　引数を入れる時に引数リストも表示されると思います
* 
* その後、個別の敵で追加した変数や、タイプでスコアが変わる物などはコンストラクタの中で代入してください
* 
* IMAGE_HANDLE_STRUCT型はEnemyクラスのヘッダーに定義されている、敵が使用する全ての画像ハンドル情報を一纏めにした構造体です
* EnemyManagerの中で画像ロードしてこの型の変数に全てのハンドルを保持しています
* その変数のポインタを参照する事で、全ての画像にアクセスできる状態にしています
* Draw関数で使用するので、Enemy.hで構造を確認しておいて下さい
*/
Toroid::Toroid(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::TOROID), x, y, type, 0, imageHandleP, scoreManagerP) {

	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);		// X、Y移動速度を設定
	m_AccelerationX = 0.0625;				// X加速度
	m_isRightAway = false;					// 右逃走フラグ
	m_isLeftAway = false;					// 左逃走フラグ
	m_AnimationSum = 0;						// 画像参照オフセット
	m_Flame = 2;							// フレームカウント
	m_Score = 3;							// スコア
}


/*
* 地上敵用のCalcと空中敵用のCalcで引数が異なります　注意してください
* 地上用はBaraを参照してください
* 
* 引数に現在の地上スクロール速度と被弾情報を持つようにしました
* 地上スクロール速度を使用する場合はこちらを参照してください（使うのは空中敵では多分タルケンのみ？）
*/
void Toroid::Calc(float playerX, float playerY, float groundMove, bool isHit) {
	/*
	* 一般の敵はDamageFunctionに被弾情報を渡すだけで被弾処理ができます　一応関数の中身も読んでおいて下さい
	* DamageFunctionは必ず一番最初に呼んでから、Switch文に移行してください
	*/
	DamageFunction(isHit);	// 被弾情報処理

	/*
	* ここのSwitch文で自身の状態変数に応じて処理を変えます
	* 通常の行動はACTIVEの欄に書きます　長ければ関数に切り出して、それをACTIVEの欄で呼ぶ形にしても良いです
	* そして、BROKENの欄でBrokenCalcを呼ぶのが基本構造になります
	*/
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):

		// 逃走判定
		if (!m_isRightAway && !m_isLeftAway &&									// 逃走フラグがどちらも立っていない
			((int)playerX / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) < 2 &&		// かつ自機との座標差が2キャラクタ未満
			((int)playerX / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) >= -2) {	// かつ自機との座標差が-2キャラクタ以上	原作での左右でのズレを再現
			if (playerX >= m_X) {
				m_isRightAway = true;		// 右から来てる時は右に逃げるフラグをON
				if (m_Type == 1) {		// 弾を撃つタイプの場合
					m_Shot();				// 弾を発射
				}
			}
			else {
				m_isLeftAway = true;		// 左から来てる時は左に逃げるフラグをON
				if (m_Type == 1) {		// 弾を撃つタイプの場合
					m_Shot();				// 弾を発射
				}
			}
		}

		// 逃走処理
		if (m_isRightAway) {
			m_SpeedX -= m_AccelerationX;	// 右に加速していく

			// アニメーション処理
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum -= 1;
				if (m_AnimationSum < 0) {
					m_AnimationSum = 7;
				}
				m_Flame += 2;
			}

		}
		if (m_isLeftAway) {
			m_SpeedX += m_AccelerationX;	// 左に加速していく

			// アニメーション処理
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum += 1;
				if (m_AnimationSum >= 8) {
					m_AnimationSum = 0;
				}
				m_Flame += 2;
			}

		}

		//速度を座標に反映
		m_X += m_SpeedX;
		m_Y += m_SpeedY;

		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}


/*
* Calcと同様Switch文で呼び分けます
* 引数にcolorIDを取るようにしました
* 参照する画像によってその値の使い方は異なるため、必ずIMAGE_HANDLE_STRUCT構造体と画像ソースを確認しておいて下さい
* ただし、計算式を変更する必要は無いはずです
*/
// 描画
void Toroid::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][8 + m_AnimationSum], true);	// 引数からカラー設定
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}