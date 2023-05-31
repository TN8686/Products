#include "GamePad.h"
#include <Windows.h>
#include <Xinput.h>
#include <random>
#include <vector>
#include <iterator>

// TODO サンプルの形では振動の二つのモーターに対して個別にイベントを設定できない　要修正.
// 概要を学習しつつ、後でDirectInputにも対応したい.

namespace { // unnamed namespace

/**
* モーターの種類.
*/
	enum VibrationType
	{
		VibrationType_Low, ///< 低周波モーター.
		VibrationType_High, ///< 高周波モーター.
		countof_VibrationType ///< モーターの種類数.
	};

	/**
	* 振動制御パラメータ.
	*/
	struct VibrationUnit
	{
		float currentTime; ///< 振動経過時間.
		float gateTime; ///< 総振動時間.
		float startVelocity; ///< 振動開始時の強さ.
		float targetVelocity; ///< 振動終了時の強さ.
	};

	/**
	* 振動状態.
	*/
	struct VibrationState
	{
		VibrationState() : sequenceNo(-1) {}

		int sequenceNo; ///< 振動シーケンスの番号.
		size_t index; ///< シーケンス内の再生位置.
		float deltaTime; ///< 再生時間.
		VibrationUnit unit[countof_VibrationType]; ///< モーター毎のパラメータ.
	};

	/**
	* 振動イベント(アニメーション定義用).
	*/
	struct VibrationEvent
	{
		float deltaTime; ///< 開始時間.
		VibrationType type; ///< モーターの種類.
		float gateTime; ///< 振動時間.
		float velocity; ///< 振動の強さ.
	};
	typedef std::vector<VibrationEvent> VibrationSequence; ///< 振動シーケンス.

	// とりあえずの振動サンプル.
	/// 低周波振動.
	const VibrationEvent ve00[] = {
	  { 0.0f, VibrationType_Low, 1.0f, 1.0f },
	  //{ 1.0f, VibrationType_Low, 1.0f, 1.0f },
	};

	/// 高周波振動.
	const VibrationEvent ve01[] = {
	  { 0.0f, VibrationType_High, 1.0f, 5.0f },
	  //{ 1.0f, VibrationType_High, 1.0f, 1.0f },
	};

	/// 少し長い低周波振動.
	const VibrationEvent ve02[] = {
	  { 0.0f, VibrationType_Low, 0.5f, 1.0f },
	  { 0.5f, VibrationType_Low, 0.5f, 0.1f },
	  { 0.5f, VibrationType_Low, 0.5f, 1.0f },
	  { 0.5f, VibrationType_Low, 1.0f, 0.0f },
	};


	VibrationState vibrationState[countof_GamePadId]; ///< コントローラー毎の振動状態
	std::vector<VibrationSequence> vibrationList; ///< 振動シーケンスのリスト.

	/// XInputのキーとGamePadのキーの変換データ.
	struct ConversionData
	{
		uint32_t di; ///< XInputのキー.
		uint32_t gp; ///< GamePadのキー.
	};

	/// XInputのキーとGamePadのキーの変換表.
	const ConversionData conversionMap[] = {
		{ XINPUT_GAMEPAD_DPAD_UP, GamePad::DPAD_UP },
		{ XINPUT_GAMEPAD_DPAD_DOWN, GamePad::DPAD_DOWN },
		{ XINPUT_GAMEPAD_DPAD_LEFT, GamePad::DPAD_LEFT },
		{ XINPUT_GAMEPAD_DPAD_RIGHT, GamePad::DPAD_RIGHT },
		{ XINPUT_GAMEPAD_START, GamePad::START },
		{ XINPUT_GAMEPAD_BACK, GamePad::SELECT },
		{ XINPUT_GAMEPAD_LEFT_THUMB, GamePad::LSB },
		{ XINPUT_GAMEPAD_RIGHT_THUMB, GamePad::RSB },
		{ XINPUT_GAMEPAD_LEFT_SHOULDER, GamePad::L },
		{ XINPUT_GAMEPAD_RIGHT_SHOULDER, GamePad::R },
		{ XINPUT_GAMEPAD_A, GamePad::A },
		{ XINPUT_GAMEPAD_B, GamePad::B },
		{ XINPUT_GAMEPAD_X, GamePad::X },
		{ XINPUT_GAMEPAD_Y, GamePad::Y },
	};
	const double minInterval = 3.0; ///< コントローラー接続チェックの最短待ち時間(秒).
	const double maxInterval = 5.0; ///< コントローラー接続チェックの最長待ち時間(秒).

	/// コントローラー接続チェック用待ち時間生成器.
	std::mt19937 randomGenerator;


	/**
	* XBOX360コントローラーの状態をGamePad構造体に反映する.
	*
	* @param id    コントローラーID.
	* @param delta 前回の呼び出しからの経過時間.
	*/
	void ApplyControllerState(uint32_t id, double delta)
	{
		GamePad& gamepad = GetGamePad(id);
		// 接続待ち時間チェック.
		if (gamepad.connectionCheckInterval > 0.0) {
			gamepad.connectionCheckInterval -= delta;
			return;
		}
		gamepad.connectionCheckInterval = 0.0;

		// コントローラーの状態の取得に成功しなかったら待ち時間を再設定.
		XINPUT_STATE state;
		if (XInputGetState(id, &state) != ERROR_SUCCESS) {
			const std::uniform_real_distribution<double> range(minInterval, maxInterval);	// 幅をランダムにし、複数の接続確認が同一フレームにならないように.
			gamepad.connectionCheckInterval = range(randomGenerator);
			gamepad.buttons &= ~GamePad::CONNECTED;
			return;
		}

		// 対応表を元にGamePadクラスのキーに変換.
		gamepad.buttons = GamePad::CONNECTED;	// CONNECTEDで初期化してキーボード入力を無効.
		for (auto e : conversionMap) {
			if (state.Gamepad.wButtons & e.di) {
				gamepad.buttons |= e.gp;
			}
		}

		// LTとRTを二値に変換.
		if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			gamepad.buttons |= GamePad::LT;
		}
		if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			gamepad.buttons |= GamePad::RT;
		}

		// スティック入力を十字キー入力に変換.
		if (state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_LEFT;
		}
		else if (state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_RIGHT;
		}
		if (state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_DOWN;
		}
		else if (state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_UP;
		}

	}


	/**
	* モーター用の振動状態を更新.
	*
	* @param unit 更新する振動ユニット.
	* @param delta 前回の呼び出しからの経過時間.
	*
	* @return 振動パラメータ.
	*/
	WORD UpdateVibrationUnit(VibrationUnit& unit, float delta)
	{
		float ratio = 1.0f;
		if (unit.currentTime + delta < unit.gateTime) {
			unit.currentTime += delta;	// 現在の振動時間を更新.
			ratio = unit.currentTime / unit.gateTime;	// 比率を計算.
		}
		// 振動の強さを線形補完して返す.
		return static_cast<WORD>((unit.startVelocity +
			(unit.targetVelocity - unit.startVelocity) * ratio) * 65535.0f);
	}

	/**
	* コントローラーの振動状態を更新.
	*
	* @param id コントローラーID.
	* @param delta 前回の呼び出しからの経過時間.
	*/
	void UpdateVibration(uint32_t id, float delta)
	{
		VibrationState& vibState = vibrationState[id];

		// 振動状態を取得し、シーケンス未設定なら振動0.
		if (vibState.sequenceNo < 0) {
			XINPUT_VIBRATION vib = {};
			XInputSetState(id, &vib);
			return;
		}

		// 振動アニメーション時間を更新.
		const VibrationSequence& vibSeq = vibrationList[vibState.sequenceNo];
		vibState.deltaTime += static_cast<float>(delta);

		// 再生時間に到達したイベントを取得し、振動ユニットに設定.
		for (; vibState.index < vibSeq.size(); ++vibState.index) {
			const VibrationEvent& vibEvent = vibSeq[vibState.index];
			if (vibState.deltaTime < vibEvent.deltaTime) {
				vibState.unit[vibEvent.type].currentTime += static_cast<float>(delta);
				break;
			}
			vibState.deltaTime -= vibEvent.deltaTime;
			vibState.unit[vibEvent.type].currentTime = 0.0f;
			vibState.unit[vibEvent.type].gateTime = vibEvent.gateTime;
			vibState.unit[vibEvent.type].startVelocity =
				vibState.unit[vibEvent.type].targetVelocity;
			vibState.unit[vibEvent.type].targetVelocity = vibEvent.velocity;
		}

		// 振動ユニットの状態を更新し、コントローラーに反映.
		XINPUT_VIBRATION vib = {
		  UpdateVibrationUnit(vibState.unit[VibrationType_Low], delta),
		  UpdateVibrationUnit(vibState.unit[VibrationType_High], delta)
		};
		XInputSetState(id, &vib);
		
		}


} // unnamed namespace

/**
* ゲームパッドの初期化.
*/
void InitializeGamePad()
{
	randomGenerator.seed(std::random_device()());	// 最初の括弧はコンストラクタで、次の括弧は関数呼び出し演算子のもの.

	// 振動シーケンスリストの作成.
	// とりあえずのベタ書き 本来的にはファイルから読み込み.
	static const struct {
		const VibrationEvent* begin;
		const VibrationEvent* end;
	} seqList[] = {
	  { std::begin(ve00), std::end(ve00) },
	  { std::begin(ve01), std::end(ve01) },
	  { std::begin(ve02), std::end(ve02) },
	};
	vibrationList.reserve(_countof(seqList));
	for (const auto& e : seqList) {
		vibrationList.emplace_back(e.begin, e.end);
	}

}

/**
* ゲームパッドの状態を取得する.
*
* @param id 取得するゲームパッドのID.
*           0~countof_GamePadId以外の値を渡すとダミーデータを返す.
*           ダミーデータは常に何の入力もない状態になる.
*
* @return idに対応するゲームパッドの状態.
*/
GamePad& GetGamePad(uint32_t id)
{
	if (id >= countof_GamePadId) {
		static GamePad dummy;
		dummy = {};
		return dummy;
	}
	static GamePad gamepad[countof_GamePadId] = {};
	return gamepad[id];
}

/**
* ゲームパッドの状態を更新する.
*
* @param delta 前回の呼び出しからの経過時間.
*/
void UpdateGamePad(double delta)
{
	for (uint32_t id = 0; id < countof_GamePadId; ++id) {
		ApplyControllerState(id, delta);
		GamePad& gamepad = GetGamePad(id);
		gamepad.buttonDown = ~gamepad.prevButtons & gamepad.buttons;	// 前回押されていなかったボタンが押されていた場合は初回入力とする.
		gamepad.prevButtons = gamepad.buttons;	// 前回押されていたボタンとして記録しておく
		//UpdateVibration(id, static_cast<float>(delta));
	}
}

/**
* コントローラーを振動させる.
*
* @param id    コントローラーID.
* @param seqNo 振動シーケンスNo.
*/
void VibrateGamePad(uint32_t id, uint32_t seqNo)
{
	if (id < countof_GamePadId && seqNo < vibrationList.size()) {
		vibrationState[id].sequenceNo = seqNo;
		vibrationState[id].index = 0;
		vibrationState[id].deltaTime = 0.0f;
	}
}

/**
* 振動シーケンスリストの要素数を取得する.
*
* @return 振動シーケンスリストの要素数.
*/
size_t GetVibrationListSize()
{
	return vibrationList.size();
}
