
class cCreep {

private:

	byte		*mDump,			*mLevel;
	size_t		 mDumpSize;

	byte		 mFileListingNamePtr;
	
	bool		 mMenuIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	

	bool		 mQuit;

public:

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress) {
					return &mLevel[(pAddress - 0x9800) + 2];
				}

		word	 lvlPtrCalculate( byte pCount );
		void	 ClearScreen();
		void	 changeLevel( size_t pNumber );
		void	 Game();
		void	 gameMenuDisplaySetup();
		void	 mainLoop();
		void	 Menu();
		void	 start();											// 
		void	 run();
};
