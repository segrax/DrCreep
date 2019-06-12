#include "stdafx.h"

#define EnumToStr(x) #x

const char* cSteam::mAchievements[] = {
	EnumToStr(eAchievement_IntroWatched),
	EnumToStr(eAchievement_IntroMusicPlayed),

	/** Menu Actions **/
	EnumToStr(eAchievement_RestoredGame),
	EnumToStr(eAchievement_SavedGame),
	EnumToStr(eAchievement_EnabledUnlimitedLives),
	EnumToStr(eAchievement_LookAtHighScores),

	/** Use Accounts **/
	EnumToStr(eAchievement_OpenDoor),
	EnumToStr(eAchievement_UseTeleport),

	/** Death Types **/
	EnumToStr(eAchievement_Death_TrapDoor),
	EnumToStr(eAchievement_Death_Voltage),
	EnumToStr(eAchievement_Death_Laser),
	EnumToStr(eAchievement_Death_Mummy),
	EnumToStr(eAchievement_Death_Frankie),

	/** Death Count **/
	EnumToStr(eAchievement_DeathCount_1),
	EnumToStr(eAchievement_DeathCount_10),
	EnumToStr(eAchievement_DeathCount_50),
	EnumToStr(eAchievement_DeathCount_100),
	EnumToStr(eAchievement_DeathCount_1Player),
	EnumToStr(eAchievement_DeathCount_2Player),

	/** Escape Count **/
	EnumToStr(eAchievement_EscapeCount_1),
	EnumToStr(eAchievement_EscapeCount_5),
	EnumToStr(eAchievement_EscapeCount_14),
	EnumToStr(eAchievement_EscapeCount_1Player),
	EnumToStr(eAchievement_EscapeCount_2Player),
	EnumToStr(eAchievement_EscapeCount_BothAlive),

	/** Castle Names **/
	EnumToStr(eAchievement_Castle_Alternation),
	EnumToStr(eAchievement_Castle_Baskerville),
	EnumToStr(eAchievement_Castle_Callenwolde),
	EnumToStr(eAchievement_Castle_Carpathia),
	EnumToStr(eAchievement_Castle_Doublecross),
	EnumToStr(eAchievement_Castle_Freedonia),
	EnumToStr(eAchievement_Castle_LoveCraft),
	EnumToStr(eAchievement_Castle_Pathenia),
	EnumToStr(eAchievement_Castle_Rittenhouse),
	EnumToStr(eAchievement_Castle_Romania),
	EnumToStr(eAchievement_Castle_Sylvania),
	EnumToStr(eAchievement_Castle_Tannenbaum),
	EnumToStr(eAchievement_Castle_Teasdale),
	EnumToStr(eAchievement_Castle_Tutorial)
};

const char* cSteam::mStats[] = {
	EnumToStr(eStat_GameOverCount),
	EnumToStr(eStat_CastlesCompleted),
	EnumToStr(eStat_TimesLaunched),
};

cSteam::cSteam() {

	
}

cSteam::~cSteam() {


}

void cSteam::SetAchievement(eAchievements pAchievement) {
	bool result = false;

}

int32_t cSteam::IncreaseStat(eStats pStat) {

	return 0;
}

void cSteam::FindLeaderboard(const char *pchLeaderboardName)
{

}


bool cSteam::LeaderboardUploadScore(int score)
{

	return true;
}

