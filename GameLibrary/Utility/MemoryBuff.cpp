#include "MemoryBuff.h"

namespace Utility {

	/// <summary>
	/// �̈���m�ۂ��܂�.
	/// </summary>
	/// <param name="areaNum">�̈�̐�</param>
	/// <param name="objSize">�̈�������̃o�C�g��</param>
	/// <returns></returns>
	bool Utility::MemoryPool::init(size_t areaNum, size_t objSize) {

		// �ǂ��炩��0�ȉ��Ȃ玸�s.
		if (areaNum <= 0 || objSize <= 0) {
			return false;
		}

		// ���������m��.
		if (!memoryAlloc(areaNum * objSize)) {
			return false;
		}
		objSize_ = objSize;

		// �������珇�ɊǗ����𖄂߂�.
		freeIdList_.resize(areaNum);
		for (size_t i = areaNum; i > 0; --i) {
			freeIdList_.push(i - 1);
		}
		return true;
	}

	void Utility::MemoryPool::refresh() {

		// �󂫗̈惊�X�g���~���\�[�g����.
		std::sort(freeIdList_.begin(), freeIdList_.end(), [](size_t a, size_t b) {
			return a > b;
			});


		size_t s = size();				// �̈搔.
		if (s != freeIdList_.size()) {	// �̈搔�Ƌ󂫗̈搔���Ⴄ�ꍇ�̂݃f�X�g���N�^�����s����

			// �e�̈悪���g�p���ǂ����`�F�b�N���Ă���.
			for (int memId = 0; memId < s; ++memId) {

				// ���g�p��������󂫗̈惊�X�g��pop.
				if (!freeIdList_.empty() && memId == freeIdList_.back()) {
					freeIdList_.pop();
				}
				else {	// �g�p����������f�X�g���N�^�����s.
					reinterpret_cast<IMemoryPool*>(&root_[memId * objSize_])->~IMemoryPool();
				}
			}
		}

		// �����������.
		MemoryBuff::refresh();
		objSize_ = 0;
		freeIdList_.refresh();
	}
}