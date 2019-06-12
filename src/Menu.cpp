#include "stdafx.h"

const sMenuItem mMenu[] = {
	{ 5,  3, eMenuAction_Default,			"PRESS MOVEMENT TO MOVE POINTER" },
	{ 5,  4, eMenuAction_Default,			"PRESS ACTION TO SELECT" },
	{ 3,  6, eMenuAction_ResumeGame,		"LOAD SAVED GAME" },
	{ 22, 6, eMenuAction_BestTimes,			"VIEW BEST TIMES" },
	{ 3,  7, eMenuAction_UnlimitedLives,	"UNLIMITED LIVES (ON/OFF)" },
	{ 3,  8, eMenuAction_ExitMenu,			"EXIT MENU" },
	{ 3, 10, eMenuAction_Default,			"LOAD CASTLE" },
	{ -1, -1, eMenuAction_Default,			"" }
};

