#include "EnemyFactory.h"
#include "Enemy.h"
#include "Mummy.h"
#include "Bat.h"
#include "UniquePtrTemplate.h"

EnemyFactory EnemyFactory::_instace;

bool EnemyFactory::Load() {
	assert(_pSpriteResourceList != nullptr);
	if (_pSpriteResourceList == nullptr) {
		return false;
	}

	// TODO �����I�ɁA��ɗp�ӂ����g�p����G�̎�ނ̃��X�g�ɉ�����Load����悤��.
	Mummy::pSetSpriteResource(pCreateObject(*_pSpriteResourceList));
	if (!Mummy::Load()) {
		return false;
	}
	if (!Bat::Load()) {
		return false;
	}
	return true;

}

bool EnemyFactory::Create(){

	if (_queue.empty()) {	// �L���[����Ȃ�I��.
		return true;
	}
	// �L���[�𑖍�.

	bool isInstanced = false;
	for (const auto& q : _queue) {

		//�s����ID�������牽�����Ȃ�.
		if (q.id >= EnemyID::EnumEnemyID_Num) {
			continue;
		}

		isInstanced = false;

		// �G���X�g�𑖍�.
		for (auto& e : _enemyList) {
			if (!e->GetOnActive()) {	// �A�N�e�B�u�łȂ�������������.
				// id�̓G�𐶐�.
				switch (q.id)
				{
				case EnemyID::Mummy:
					e.reset(new Mummy(q.point));
					break;

				case EnemyID::Bat:
					e.reset(new Bat(q.point));
					break;

				case EnemyID::EnumEnemyID_Num:
					break;
				default:
					break;
				}

				assert(e != nullptr);
				if (e != nullptr) {			// null�`�F�b�N.
					EnemyInit(e.get(), q);	// ���������G��������.
					isInstanced = true;		// �󂫂ɑ΂��Đ�������.
					break;					// �G���X�g�̑�������E�o.
				}
				else {
					_queue.clear();
					return false;	// ���������ӏ���nullptr��������G���[�ŏI��.
				}
			}
		}

		// �̈悪�G�o��������ɒB���Ă�����V�K�����͈͂ɂ͍s������continue.
		if (_enemyList.size() >= ENEMY_MAX) {
			continue;			
		}

		if (isInstanced) {	// �����܂łŐ����ł��Ă���.
			continue;		// ���̃L���[��.
		}
		else {				// �����܂łŐ����ł��Ă��Ȃ�.
			// �V���ȗv�f��ǉ����Đ�������.
			switch (q.id)
			{
			case EnemyID::Mummy:
				_enemyList.emplace_back(new Mummy(q.point));
				break;

			case EnemyID::Bat:
				_enemyList.emplace_back(new Bat(q.point));
				break;

			case EnemyID::EnumEnemyID_Num:
				break;
			default:
				break;
			}
			auto index = _enemyList.size() - 1;
			assert(&_enemyList.at(index) != nullptr);
			if (&_enemyList.at(index) != nullptr) {			// null�`�F�b�N.
				EnemyInit(_enemyList.at(index).get(), q);
			}
			else {
				_queue.clear();
				return false;	// ���������ӏ���nullptr��������G���[�ŏI��.
			}
		}
	}
	_queue.clear();
	return true;
}

bool EnemyFactory::Enqueue(EnemyCreateQueue queue) {
	if (_queue.size() < ENEMY_MAX) {	// �L���[�̒ǉ��͍ő�ł��G�o������܂�.
		_queue.push_back(queue);
		return true;
	}
	return false;
}

bool EnemyFactory::Enqueue(EnemyID id, Point2D point, Camera* pCamera, Player* pPlayer, bool isRight) {
	return Enqueue({ id, point, pCamera, pPlayer, isRight });
}

/// <summary>
/// Enemy�I�u�W�F�N�g�̏����ݒ���s���܂�.
/// </summary>
/// <param name="pEnemyObj">�ݒ肷��I�u�W�F�N�g�̃|�C���^</param>
/// <param name="queue">�������L���[</param>
void EnemyFactory::EnemyInit(Enemy* pEnemyObj, EnemyCreateQueue queue) {
	pEnemyObj->Init();
	pEnemyObj->pSetCamera(queue.pCamera);
	pEnemyObj->SetIsLeft(!queue.isRight);
	pEnemyObj->pSetPlayer(queue.pPlayer);

	assert(_pBlockList != nullptr);
	if (_pBlockList != nullptr) {
		pEnemyObj->pSetBlockList(_pBlockList);
	}

	assert(_pThroughBlockList != nullptr);
	if (_pThroughBlockList != nullptr) {
		pEnemyObj->pSetThroughBlockList(_pThroughBlockList);
	}
}

/// <summary>
/// ���p�I�����ɕK�����s���Ă�������.
/// </summary>
void EnemyFactory::Finalize() {
	_pBlockList = nullptr;
	_pThroughBlockList = nullptr;
	_pSpriteResourceList = nullptr;
	_queue.clear();
	_enemyList.clear();
}