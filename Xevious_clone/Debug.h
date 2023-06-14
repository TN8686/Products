#pragma once

#include "Xevious_clone.h"
#include "Player.h"
#include "ScoreManager.h"
#include "AreaManager.h"

class Debug {
private:
	bool m_IsDebugMode;
	bool m_IsStop;
	bool m_IsStopKeyFirst;
	bool m_IsAdvanceKeyFirst;
	bool m_IsFrameAdvance;

	void SetDebugFont();
public:
	Debug(bool isDebugMode);

	void DrawPlayerValue(Player* playerP, int score, AreaManager* areaManagerP, int level);
	void DrawDebugWindow(int TempScreen, Player* playerP, int score, AreaManager* areaManagerP, int level);
	bool CalcStop(char keyBuf[], int padBuf);

	bool GetIsDebugMode();
};