class cVideoWindow;
class cBitmapMulticolor;

class cCreep {

private:

	cVideoWindow		 *mWindow;
	cBitmapMulticolor	 *mBitmap;

	byte		*mDump,			*mLevel,		*m64CharRom;
	size_t		 mDumpSize;

	byte		 mFileListingNamePtr;
	
	bool		 mMenuIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	

	bool		 mQuit;

	byte		 byte_83E, byte_83F, byte_11C9, byte_5CE2;
	char		 byte_5FD5, byte_5FD6;

	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtX_0, mTxtY_0, mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_34, word_3C;

public:

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress ) {
					return &mLevel[(pAddress - 0x9800) + 2];
				}
				
				inline word charRom( word pAddress ) {
					return m64CharRom[ pAddress - 0xD000 ];
				}

		word	 lvlPtrCalculate( byte pCount );
		void	 BlackScreen();
		void	 ClearScreen();
		void	 changeLevel( size_t pNumber );
		void	 DisplayText( word &pData );
		void	 drawGraphics( word &pData, word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY, byte pTxtCurrentID );
		void	 Game();
		void	 gameMenuDisplaySetup();
		void	 mainLoop();
		void	 Menu();
		void	 SpriteMovement( word &pData, byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX );
		void	 start();	
		void	 sub_160A( word &pData ); 
		void	 sub_166A( word &pData );
		void	 sub_1747( word &pData );
		void	 sub_17EE( word &pData );
		void	 sub_410C( word &pData );
		bool	 sub_5750();
		void	 sub_5FA3();
		void	 textDecode( word &pData );
		void	 TextGraphicsDraw( word &pData );
		void	 run();
};
