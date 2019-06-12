enum eAchievements {
	/** Intro **/
	eAchievement_IntroWatched,
	eAchievement_IntroMusicPlayed,

	/** Menu Actions **/
	eAchievement_RestoredGame,
	eAchievement_SavedGame,
	eAchievement_EnabledUnlimitedLives,
	eAchievement_LookAtHighScores,

	/** Use Accounts **/
	eAchievement_OpenDoor,
	eAchievement_UseTeleport,

	/** Death Types **/
	eAchievement_Death_TrapDoor,
	eAchievement_Death_Voltage,
	eAchievement_Death_Laser,
	eAchievement_Death_Mummy,
	eAchievement_Death_Frankie,

	/** Death Count **/
	eAchievement_DeathCount_1,
	eAchievement_DeathCount_10,
	eAchievement_DeathCount_50,
	eAchievement_DeathCount_100,
	eAchievement_DeathCount_1Player,
	eAchievement_DeathCount_2Player,

	/** Escape Count **/
	eAchievement_EscapeCount_1,
	eAchievement_EscapeCount_5,
	eAchievement_EscapeCount_14,
	eAchievement_EscapeCount_1Player,
	eAchievement_EscapeCount_2Player,
	eAchievement_EscapeCount_BothAlive,

	/** Castle Names **/
	eAchievement_Castle,
	eAchievement_Castle_Alternation		= eAchievement_Castle,
	eAchievement_Castle_Baskerville,
	eAchievement_Castle_Callenwolde,
	eAchievement_Castle_Carpathia,
	eAchievement_Castle_Doublecross,
	eAchievement_Castle_Freedonia,
	eAchievement_Castle_LoveCraft,
	eAchievement_Castle_Pathenia,
	eAchievement_Castle_Rittenhouse,
	eAchievement_Castle_Romania,
	eAchievement_Castle_Sylvania,
	eAchievement_Castle_Tannenbaum,
	eAchievement_Castle_Teasdale,
	eAchievement_Castle_Tutorial,

	eAchievement_Last
};

enum eStats {
	eStat_GameOverCount,
	eStat_CastlesCompleted,
	eStat_TimesLaunched,

};

class cSteam : public cSingleton<cSteam> {
	static const char*		mAchievements[];
	static const char*		mStats[];
	//SteamLeaderboard_t		m_CurrentLeaderboard;

	public:
							cSteam();
							~cSteam();

	void					SetAchievement(eAchievements pAchievement);
	int32_t					IncreaseStat(eStats pStat);
	
	void					FindLeaderboard( const char *pchLeaderboardName );

	bool					LeaderboardUploadScore(int score);
};
