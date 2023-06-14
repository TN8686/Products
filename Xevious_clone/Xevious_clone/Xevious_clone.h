#pragma once

#include <random>
#include "DxLib.h"

static constexpr int TEMP_SCREEN_WIDTH = 240;	// ����ʂ����Ɍ������̕��i�܂���ۂ͍����j
static constexpr int TEMP_SCREEN_HEIGHT = 320;	// ����ʂ����Ɍ������̍����i�܂���ۂ͕��j

static constexpr int TEMP_OFFSCREEN_WIDTH = 16;	// ����ʂ����Ɍ������̕\���O�̈�͈̔͂̕��i�܂���ۂ͍����j
static constexpr int TEMP_OFFSCREEN_HEIGHT = 32;	// ����ʂ����Ɍ������̂̕\���O�̈�͈̔͂̍����i�܂���ۂ͕��j

static constexpr int WINDOW_WIDTH = 448;	// ���ۂɍ쐬����E�C���h�E�̕�
static constexpr int WINDOW_HEIGHT = 576;	// ���ۂɍ쐬����E�C���h�E�̍���

static constexpr float PI = 3.14159265f;	// �~����

static constexpr int CHARACTER_SIZE = 8;	// 1�L�����N�^�̃h�b�g��

static constexpr int ZAPPER_NUM = 3;		// �U�b�p�[�̐�

enum class ModeId : int {
	TITLE,
	MAIN_GAME,
	NAME_ENTRY
};