#pragma once

namespace EnemyManagerConst {
	static constexpr int GROUND_NUM = 16;				// 地上物数
	static constexpr int BACURA_NUM = 16;				// バキュラ数
	static constexpr int AERIAL_NUM = 6;				// 空中敵数
	static constexpr int SPARIO_NUM = 19;				// 通常弾（スパリオ）数

	static constexpr int LITTLE_SIZE = 16;				// 小型敵等のサイズ
	static constexpr int BIG_SIZE = 32;					// 大型敵のサイズ

	static constexpr int HURTBOX_ORIGIN = 7;			// 敵食らい判定の始点
	static constexpr int HURTBOX_SIZE = 5;				// 敵食らい判定のサイズ

	static constexpr int IMAGE_NAME_BUF = 256;			// テキスト幅

	static constexpr int ENEMY_COLOR_NUM = 5;			// 敵のカラーパターン数

	static constexpr int LITTLE_IMG_DIVSION_NUM = 80;	// 小サイズ一般敵画像分割数
	static constexpr int LITTLE_IMG_DIVSION_X = 8;		// の、X分割数
	static constexpr int LITTLE_IMG_DIVSION_Y = 10;		// と、Y分割数

	static constexpr int BIG_IMG_DIVSION_NUM = 28;		// 大サイズ一般敵画像分割数
	static constexpr int BIG_IMG_DIVSION_X = 7;			// の、X分割数
	static constexpr int BIG_IMG_DIVSION_Y = 4;			// と、Y分割数

	static constexpr int SPARIO_IMG_DIVSION_NUM = 72;	// 弾画像分割数
	static constexpr int SPARIO_IMG_DIVSION_X = 12;		// の、X分割数
	static constexpr int SPARIO_IMG_DIVSION_Y = 6;		// と、Y分割数

	static constexpr int ZAKATO_IMG_DIVSION_NUM = 30;	// ザカート系出現、ギドスパリオ破壊画像分割数
	static constexpr int ZAKATO_IMG_DIVSION_X = 5;		// の、X分割数
	static constexpr int ZAKATO_IMG_DIVSION_Y = 6;		// と、Y分割数

	static constexpr int BROKEN_IMG_DIVSION_NUM = 14;	// 破壊画像分割数
	static constexpr int BROKEN_IMG_DIVSION_X = 7;		// の、X分割数
	static constexpr int BROKEN_IMG_DIVSION_Y = 2;		// と、Y分割数

	static constexpr int ANDOR_GENESIS_BODY_IMG_DIVSION_NUM = 6;
	static constexpr int ANDOR_GENESIS_BODY_IMG_DIVSION_X = 3;
	static constexpr int ANDOR_GENESIS_BODY_IMG_DIVSION_Y = 2;

	static constexpr int ANDOR_GENESIS_PARTS_IMG_DIVSION_NUM = 25;
	static constexpr int ANDOR_GENESIS_PARTS_IMG_DIVSION_X = 5;
	static constexpr int ANDOR_GENESIS_PARTS_IMG_DIVSION_Y = 5;

	static constexpr int CARACTER_X_MIN = 1;			// キャラクタ単位のX座標最小値
	static constexpr int CARACTER_X_MAX = 25;			// キャラクタ単位のX座標最大値

	static constexpr int ENEMY_TABLE_NUM = 256;			// 敵パターンの数
	static constexpr int AREA_DATA_GROUND_MAX = 64;		// 地上に配置できるオブジェクトの数

	static constexpr int AERIAL_TABLE_PATTERN_NUM = 2;	// 空中敵テーブル一つにつき2種類の敵　名前変えた方が良い気がするけど思いつかないな…

	static constexpr int LEVEL_NUM = 4;					// レベル数　4段階

	static constexpr int LINE_PROCESS_NUM = 2;			// ライン処理データの数

}