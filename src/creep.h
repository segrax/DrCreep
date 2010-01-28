class cVideoWindow;
class cBitmapMulticolor;

class cCreep {

private:

	cVideoWindow		 *mWindow;
	cBitmapMulticolor	 *mBitmap;
	cVideoSurface<dword> *mSurface;

	byte		*mDump,			*mLevel,		*m64CharRom;
	size_t		 mDumpSize;

	byte		 mFileListingNamePtr;
	
	bool		 mMenuIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	

	bool		 mQuit;

	byte		 byte_20DE, byte_24FD, byte_2E36, RunRestorePressed;
	byte		 byte_2E35;

	word		 word_45DB;											// Lightning
	byte		 byte_44E5, byte_45DD, byte_45DE, byte_45DF;		// Lightning
	byte		 byte_474F;
	
	word		 word_564B;													// Conveyor
	byte		 byte_5642, byte_5643, byte_5644, byte_5645, byte_5646;		// Conveyor
	byte		 byte_5647, byte_5648, byte_5F58, byte_5F57;
	byte		 byte_3FD4;

	byte		 byte_840, byte_841, byte_883, byte_884, byte_885, byte_886, byte_887, byte_888, byte_889, byte_88A, byte_88B, byte_88C, byte_88D, byte_88E, byte_882, byte_D10, byte_D12;
	byte		 byte_83E, byte_83F, byte_11C9, byte_5CE2;
	char		 byte_5FD5, byte_5FD6, byte_5FD7, byte_5FD8;

	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtX_0, mTxtY_0, mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_34, word_3C, word_3E, word_40;

public:

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress ) {
					return &mLevel[(pAddress - 0x9800) + 2];
				}
				
				inline byte charRom( word pAddress ) {
					return m64CharRom[ pAddress - 0xD000 ];
				}

		word	 lvlPtrCalculate( byte pCount );
		void	 BlackScreen();
		void	 ClearScreen();
		void	 changeLevel( size_t pNumber );
		void	 DisplayText(  );
		void	 drawGraphics(  word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY, byte pTxtCurrentID );
		void	 handleEvents();
		void	 Game();
		void	 GameAction();
		void	 gameMenuDisplaySetup();
		void	 KeyboardJoystickMonitor( byte pA );
		void	 mainLoop();
		void	 ObjectActions();
		void	 ObjectHitsObject( byte pX );
		void	 objectFunction( byte pX );
		void	 optionsMenu();
		bool	 Menu();
		void	 SpriteDraw();
		void	 SpriteMovement( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX );
		void	 start();	
		void	 sub_95F();
		void	 sub_160A(); 
		void	 sub_166A( );
		void	 sub_1747( );
		void	 sub_17EE();
		void	 sub_2973();
		void	 sub_2E79( );
		void	 sub_3026( byte pX );
		void	 sub_31F6( byte pX );
		void	 sub_3F4F( );
		void	 sub_368A( byte &pY );
		void	 sub_36B3( byte pX );
		void	 sub_3757( );
		void	 sub_3F14( byte &pX );
		void	 sub_4AB6( );
		void	 sub_410C( );
		void	 sub_422A( );
		void	 sub_42AD( byte pX );
		void	 sub_43E4( );
		void	 sub_46AE( );
		void	 sub_538B( byte pX );
		void	 sub_5501( );
		bool	 sub_5750( byte &pX );
		void	 sub_57DF( byte pX );
		void	 sub_5D26( byte &pX );
		void	 sub_5F6B( byte &pX );
		void	 sub_5FA3();
		void	 textDecode( );
		void	 TextGraphicsDraw( );
		void	 run();
};
