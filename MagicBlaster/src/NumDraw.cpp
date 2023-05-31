#include "NumDraw.h"
#include "Engine/Graphics.h"

// private

// �����擾�@0�̏ꍇ0���Ԃ��Ă��鎖�ɒ���.
unsigned NumDraw::NumDraw::GetDigit(unsigned long long num) {
	unsigned digit = 0;
	while (num != 0) {
		num /= 10;
		digit++;
	}
	return digit;
}

SpriteObj& NumDraw::NumDraw::GetNextSprite()
{
	auto size = _spriteList.size();			// ����̃T�C�Y���擾.
	bool isCapacityShortage = false;
	if (_spriteNum < size) {				// ���ꂩ��g�p����X�v���C�g�����݂��Ă���Ȃ�.
		return _spriteList.at(_spriteNum);	// ���̂܂܂��̃C���f�b�N�X�̗v�f��n��.
	}
	else {													// �T�C�Y���s�����Ă����ꍇ.
		
		if (_spriteNum >= _spriteList.capacity()) {	// ����Ƀ��\�[�X�̃��������s�����Ă����ꍇ�@�ł��鎖�Ȃ�A�����ɓ���Ȃ��悤�Ɏ��O���U�[�u�����邱��.
			// �R�s�[�R���X�g���N�^�̎������g���ƐV�I�u�W�F�N�g��S�ēo�^�����I�u�W�F�N�g��S�ēo�^�����̏��ɂȂ�A�������𖳈Âɑ����g���\��������̂ň�U�S�ēo�^����.
			for (auto& e : _spriteList) {
				e.Unregister();
			}
			isCapacityShortage = true;
		}

		_spriteList.emplace_back();							// �V���ȗv�f��ǉ�.

		if (isCapacityShortage) {
			for (auto& e : _spriteList) {					// �S�ēo�^	�������m�ۂ�vector�ɔC����.
				_spriteResouce.Register(&e);
			}
		}
		else {
			_spriteResouce.Register(&_spriteList.at(size));	// ���̗v�f��o�^ �����͒ǉ��O�̃T�C�Y�����̂܂܃C���f�b�N�X�ɂł���.
		}
		return _spriteList.at(size);						// ���̗v�f��Ԃ�.
	}
}

void NumDraw::NumDraw::SetParam(SpriteObj& sprite, const Param& param, const Point2D& position)
{
	// �A�N�e�B�u��Ԃ�.
	sprite.SetOnActive(true);

	// �X�P�[����ݒ�.
	sprite.scale.x = param.scale.x;
	sprite.scale.y = param.scale.y;

	// �J�����ɉe�������ǂ�����ݒ�.
	sprite.cameraRelative = param.cameraRelative;

	// ���W�ݒ�.
	sprite.SetLocalPosition(position);

	sprite.color = { param.color.r, param.color.g, param.color.b, param.color.a };	// �����x���� - 1�����l�ɂ��鎖�ŁA�����x�����̂܂܉摜�ɓK�p����.
}


// public

// ���[�h.
bool NumDraw::NumDraw::Load(size_t reserveNum)
{
	if (!Graphics::Get().texMap.LoadFromFile(_spriteResouce.GetTexture(), L"Res/num_01.png")) {
		return false;
	}

	// �X�v���C�g�̗̈���m��.
	_spriteResouce.Reserve(reserveNum);
	_spriteList.reserve(reserveNum);

	for (auto& e : _spriteList) {
		_spriteResouce.Register(&e);
	}

	// �Z�����X�g�̍쐬.
	if (_spriteResouce.CreateCellList(10, 1, IMAGE_X, IMAGE_Y) != 0) {
		return false;
	};
	return true;
}

// �`��O�̏���������.
void NumDraw::NumDraw::ReadyToDraw()
{
	for (auto itr = _spriteList.begin() + _spriteNum; itr != _spriteList.end(); ++itr) {	// _spriteNum�ȍ~�̗v�f���A�N�e�B�u��.
		itr->SetOnActive(false);
	}
	_spriteNum = 0;	// �X�v���C�g�̎g�p����������.
}

void NumDraw::NumDraw::SetDrawParam(const Param& param)
{
	// �������擾.
	unsigned digit = GetDigit(param.num);
	unsigned maxDigit = (digit > param.digit ? digit : param.digit);	// �\�������Ɣ�r���āA���傫�������擾.

	// �ő�0�̏ꍇ�A����1�Ƃ���.
	if (maxDigit == 0) {
		maxDigit = 1;
	}

	float length = param.scale.x * IMAGE_X;								// �ꕶ��������̕�.
	float allLength = static_cast<float>(maxDigit) * length;			// �S�̕�.

	// �n�_��I��.
	auto position = param.position;
	switch (param.alignment)
	{
	case ALIGN_LEFT:
		position.x += allLength;		// ���̕��Y����.
		break;

	case ALIGN_CENTER:
		position.x += allLength * 0.5f;	// ���̔����̕��Y����.
		break;

	case ALIGN_RIGHT:				// �Ȃɂ����Ȃ�.
		break;

	default:
		assert(false);					// �ُ�Ȓl.
		break;
	}
	position.x -= length * 0.5f;		// �ꕶ�����̔����Y����.

	switch (param.verticalAlign)
	{
	case VER_ALIGN_TOP:
		position.y += IMAGE_Y * param.scale.y * 0.5f;	// �����̔����̕��Y����.
		break;

	case VER_ALIGN_CENTER:			// �Ȃɂ����Ȃ�.
		break;

	case VER_ALIGN_BOTTOM:
		position.y -= IMAGE_Y * param.scale.y * 0.5f;	// �����̔����̕��Y����.
		break;

	default:
		assert(false);					// �ُ�Ȓl.
		break;
	}

	auto num = param.num;

	unsigned i = 0;
	for (; i < digit; ++i) {
		auto& sprite = GetNextSprite();	// ���̃X�v���C�g���擾.

		assert(num != 0);				// �d�l��0�͗��Ȃ��͂��c.		
		sprite.animeController.SetCellIndex(num % 10);	// ���݂�1���ڂ����o���摜��I��.
		num /= 10;										// ��������炷.


		SetParam(sprite, param, position);

		position.x -= length;				// ���W�𐔎��������.

		++_spriteNum;						// �X�v���C�g�̎g�p�����C���N�������g.
	}

	// �c��̕����ɂ�0��\��.
	for (; i < maxDigit; ++i) {
		auto& sprite = GetNextSprite();	// ���̃X�v���C�g���擾.

		sprite.animeController.SetCellIndex(0);			// 0�̉摜��I��.

		SetParam(sprite, param, position);

		position.x -= length;				// ���W�𐔎��������.

		++_spriteNum;						// �X�v���C�g�̎g�p�����C���N�������g.
	}
}