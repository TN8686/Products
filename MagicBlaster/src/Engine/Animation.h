#pragma once

#include <stdint.h>
#include <vector>
#include <DirectXMath.h>

/**
* アニメーションデータ.
*/
struct AnimationData
{
	uint32_t cellIndex; ///< セルデータリスト内のインデックス.
	double time; ///< 表示する時間(秒).
	float rotation = 0; ///< 画像の回転角(ラジアン).
	DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }; ///< 画像の拡大率.
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; ///< 画像の色.
	bool flipHorizontal = false;	// 左右反転.
	bool flipVertical = false;		// 上下反転.
};


/**
* 単一のアニメーションを構成するアニメーションデータのリスト.
*/
struct AnimationSequence{
	std::vector<AnimationData> animationData;
	bool isLoop = true;			///< ループフラグ.
};

/**
* アニメーションシーケンスのリスト.
*
* インデックス指定によって、再生するアニメーションを取得できるようにする.
*/
typedef std::vector<AnimationSequence> AnimationList;

/**
* アニメーション制御クラス.
*
* - 毎フレームUpdateを呼ぶことでアニメーションが進行する.
* - アニメーションを切り替えるにはSetSeqIndexを呼ぶ.
* - 現在のセルインデックスを取得するにはGetCellIndexを呼ぶ.
* - デフォルトコンストラクタあるいは引数にnullptrを指定して構築した場合、アニメーションリストを持たない状態になる.
*   アニメーションリストを持たない状態では、SetCellIndex関数でセルインデックスを設定できる.
*/
class AnimationController
{
public:
	AnimationController() = default;
	explicit AnimationController(const AnimationList* al);
	~AnimationController() = default;
	AnimationController& operator=(const AnimationController&) = default;

	void SetSeqIndex(uint32_t index);
	void Reset();
	void Update(double delta);
	void SetCellIndex(uint32_t index);
	AnimationData GetAnimationData() const;
	size_t GetSeqCount() const;
	bool IsFinished() const;
	uint32_t GetSeqIndex() const { return seqIndex; }
	//void SetLoop(bool);

private:
	const AnimationList* list = nullptr; ///< アニメーションシーケンスのリスト.
	uint32_t seqIndex = 0; ///< 再生中のシーケンスインデックス.
	uint32_t cellIndex = 0; ///< 再生中のセルインデックス.
	double time = 0; ///< 再生時間.
	bool isFinished = false;	///< 再生終了フラグ.
	//bool isLoop = true;			///< ループフラグ.
};
