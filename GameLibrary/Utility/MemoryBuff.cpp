#include "MemoryBuff.h"

namespace Utility {

	/// <summary>
	/// 領域を確保します.
	/// </summary>
	/// <param name="areaNum">領域の数</param>
	/// <param name="objSize">領域一つあたりのバイト数</param>
	/// <returns></returns>
	bool Utility::MemoryPool::init(size_t areaNum, size_t objSize) {

		// どちらかが0以下なら失敗.
		if (areaNum <= 0 || objSize <= 0) {
			return false;
		}

		// メモリを確保.
		if (!memoryAlloc(areaNum * objSize)) {
			return false;
		}
		objSize_ = objSize;

		// 末尾から順に管理情報を埋める.
		freeIdList_.resize(areaNum);
		for (size_t i = areaNum; i > 0; --i) {
			freeIdList_.push(i - 1);
		}
		return true;
	}

	void Utility::MemoryPool::refresh() {

		// 空き領域リストを降順ソートする.
		std::sort(freeIdList_.begin(), freeIdList_.end(), [](size_t a, size_t b) {
			return a > b;
			});


		size_t s = size();				// 領域数.
		if (s != freeIdList_.size()) {	// 領域数と空き領域数が違う場合のみデストラクタを実行する

			// 各領域が未使用かどうかチェックしていく.
			for (int memId = 0; memId < s; ++memId) {

				// 未使用だったら空き領域リストをpop.
				if (!freeIdList_.empty() && memId == freeIdList_.back()) {
					freeIdList_.pop();
				}
				else {	// 使用中だったらデストラクタを実行.
					reinterpret_cast<IMemoryPool*>(&root_[memId * objSize_])->~IMemoryPool();
				}
			}
		}

		// メモリを解放.
		MemoryBuff::refresh();
		objSize_ = 0;
		freeIdList_.refresh();
	}
}