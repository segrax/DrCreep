enum eMenuAction {
	eMenuAction_Default			= -1,
	eMenuAction_UnlimitedLives	= 0,
	eMenuAction_ExitMenu		= 1,
	eMenuAction_ChangeLevel		= 2,
	eMenuAction_ResumeGame		= 3,
	eMenuAction_BestTimes		= 4,
};

struct sMenuItem {
	const int16_t		mX, mY;
	const eMenuAction	mAction;
	const char*			mText;
};

const extern sMenuItem mMenu[];
