class cVideoWindow;

class cCreep {

private:

	cVideoWindow		 *mWindow;

	byte		*mDump,			*mLevel;
	size_t		 mDumpSize;

	byte		 mFileListingNamePtr;
	
	bool		 mMenuIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	

	bool		 mQuit;

	byte		 byte_11C9;

	byte		 mTxtY_0, mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth;

public:

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress) {
					return &mLevel[(pAddress - 0x9800) + 2];
				}

		word	 lvlPtrCalculate( byte pCount );
		void	 ClearScreen();
		void	 changeLevel( size_t pNumber );
		void	 drawGraphics( word &pData, word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY );
		void	 Game();
		void	 gameMenuDisplaySetup();
		void	 mainLoop();
		void	 Menu();
		void	 start();											// 
		void	 sub_410C( word &pData );
		void	 TextGraphicsDraw( word pData );
		void	 run();
};
