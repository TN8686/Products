#include "Player.h"
#include "EnemyManager.h"

// �R���X�g���N�^
Player::Player(SoundManager* pSoundManager) {
	m_width = PLAYER_SIZE;											// ��
	m_height = PLAYER_SIZE;											// ����
	m_speed = 1.0f;													// Y�����A�΂ߕ����ւ̈ړ����x
	m_speed_x = 1.5f;												// X�����݂̂ւ̈ړ����x
	LoadDivGraph("img/Xevious_OBJ_solvalou.png", PLAYER_IMG_DIVSION_NUM, PLAYER_IMG_DIVSION_NUM, 1, m_width, m_height, m_image, FALSE);	// �摜	�S�̂�6�����@��6�����@�c1����
	LoadDivGraph("img/Xevious_OBJ_solvalouBroken.png", BROKEN_IMG_DIVSION_NUM, BROKEN_IMG_DIVSION_NUM, 1, BROKEN_IMG_SIZE, BROKEN_IMG_SIZE, m_brokenImage, FALSE);	// �摜	�S�̂�6�����@��6�����@�c1����

	m_isInvincible = false;											// ���G�t���O
	m_isDraw = true;												// �`��t���O
	m_isFlip = false;												// �j��`�掞�̔��]�t���O
	m_pSoundManager = pSoundManager;								// �T�E���h�}�l�[�W���|�C���^

	Init();
}

void Player::Init() {
	m_x = static_cast<float>(TEMP_SCREEN_WIDTH / 2.0f);							// ����X���W
	m_y = static_cast<float>(TEMP_SCREEN_HEIGHT - m_height - CHARACTER_SIZE);	// ����Y���W
	m_onActive = true;															// �A�N�e�B�u���
	m_isHit = false;															// �q�b�g���
	m_brokenAnimID = 0;															// �j��A�j���[�V����ID
	m_frameCount = 0;															// �j��A�j���[�V�����p�t���[���J�E���g
	m_stock = STOCK_INIT;														// �����c�@
	}

void Player::Restart(){
	m_x = static_cast<float>(TEMP_SCREEN_WIDTH / 2.0f);							// ����X���W
	m_y = static_cast<float>(TEMP_SCREEN_HEIGHT - m_height - CHARACTER_SIZE);	// ����Y���W
	m_onActive = true;															// �A�N�e�B�u���
	m_isHit = false;															// �q�b�g���
	m_brokenAnimID = 0;															// �j��A�j���[�V����ID
	m_frameCount = 0;															// �j��A�j���[�V�����p�t���[���J�E���g
	m_stock--;																	// �X�g�b�N�����炷
}

// Get�֐��n
float Player::GetX() {
	return m_x;
}
float Player::GetY() {
	return m_y;
}
int Player::GetStock() {
	return m_stock;
}
bool Player::GetOnActive() {
	return m_onActive;
}

// Set�֐��n
void Player::SetIsHit(bool isHit) {
	m_isHit = isHit;
}

// �G�N�X�e���h
void Player::Extend() {
	m_stock++;
}

// �v�Z�֐�
void Player::Calc(char keyBuf[], int padBuf, int frameCount) {
	
	if (m_isHit) {			// �q�b�g����
		if (!m_isInvincible) {
			m_onActive = false;	// �A�N�e�B�u��Ԃ�false��
			if (!m_pSoundManager->GameStartBgmCheck()) {
				m_pSoundManager->BgmStop();
			}
			m_pSoundManager->NoisePlay(static_cast<int>(NoiseSeId::Miss));
		}
		else {
			m_isDraw = !m_isDraw;
		}
	}
	else {
		m_isDraw = true;
	}

	if (m_onActive) {

		// �㉺�ړ�
		if (padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) {	// ��
			m_y -= m_speed;
		}
		if (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) {	// ��
			m_y += m_speed;
		}

		// ���E�ړ��@���E�ړ����Ǝ΂߈ړ����ŉ��̈ړ����x���Ⴄ
		if (padBuf & PAD_INPUT_LEFT || keyBuf[KEY_INPUT_LEFT]) {	// ��
			if (!(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && !(padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) ||	// �������������Ă��Ȃ���
				(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN])) {	// �܂��͏������������Ă��鎞�A���ړ�����
				m_x += m_speed_x;
			}
			else {												// �ǂ���ł��Ȃ����͎΂߈ړ�����
				m_x += m_speed;
			}
		}
		if (padBuf & PAD_INPUT_RIGHT || keyBuf[KEY_INPUT_RIGHT]) {	// �E�@�ȉ����Ɠ���
			if (!(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && !(padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) ||	// �������������Ă��Ȃ���
				(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN])) {	// �܂��͏������������Ă��鎞�A���ړ�����
				m_x -= m_speed_x;
			}
			else {
				m_x -= m_speed;
			}

		}

		// �ړ����E
		if (m_y < Y_MAX) {	//�� �������̈ʒu�ō����Ă邯�ǈꉞ���̒l
			m_y = Y_MAX;
		}
		if (m_y > (float)TEMP_SCREEN_HEIGHT - (float)m_height) {	// ��
			m_y = (float)TEMP_SCREEN_HEIGHT - (float)m_height;
		}
		if (m_x < TEMP_OFFSCREEN_WIDTH) {									// �E
			m_x = TEMP_OFFSCREEN_WIDTH;
		}
		if (m_x > (float)TEMP_SCREEN_WIDTH - (float)m_width) {		// ��
			m_x = (float)TEMP_SCREEN_WIDTH - (float)m_width;
		}
	}
	else {
		BrokenCalc(frameCount);
	}

	m_isHit = false;	// �q�b�g��񏉊���
}

// �`��֐�
void Player::Draw() {
	if (m_onActive) {
		if (m_isDraw) {
			DrawGraph((int)m_y, (int)m_x, m_image[0], TRUE);
		}
	}
	else {
		BrokenDraw();
	}
}

void Player::BrokenCalc(int frameCount) {
	if (m_frameCount != 0 && m_frameCount % 8 == 0) {
		m_brokenAnimID++;
	}

	if (frameCount % 4 == 0) {
		m_isFlip = !m_isFlip;
	}

	if (m_brokenAnimID == 7) {
		m_x = 0;
		m_y = 0;
	}

	m_frameCount++;
}

void Player::BrokenDraw() {
	if (m_isFlip) {
		DrawGraph(static_cast<int>(m_y) - 8, static_cast<int>(m_x) - 8, m_brokenImage[m_brokenAnimID], true);
	}
	else {
		DrawTurnGraph(static_cast<int>(m_y) - 8, static_cast<int>(m_x) - 8, m_brokenImage[m_brokenAnimID], true);
	}
}
