class cVideoWindow;
class cBitmapMulticolor;

class cCreep {

private:

	cVideoWindow		 *mWindow;
	cBitmapMulticolor	 *mBitmap;
	cVideoSurface<dword> *mSurface;

	cSprite				  mSprites[8];

	byte		*mDump,			*mLevel,		*m64CharRom;
	size_t		 mDumpSize;

	byte		 mFileListingNamePtr;
	
	bool		 mMenuIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	

	bool		 mQuit;

	byte		 byte_20DE, byte_24FD, RunRestorePressed;
	byte		 byte_2E02;

	byte		 byte_2E35, byte_2E36, byte_2232;					// Conveyor

	byte		 byte_3638;
	word		 word_41D3;

	word		 word_4A65;											// Keys
	byte		 byte_4A64;

	word		 word_498B;											// Mummys

	word		 word_4D5B;											// RayGuns	
	byte		 byte_4D5D, byte_4D5E, byte_4D5F;
	byte		 byte_574C, byte_4D60, byte_4D61;
	byte		 byte_4D62, byte_4D63, byte_4D64;
	byte		 byte_4D65, byte_4D66, byte_4D67;

	word		 word_5387;											// Trap Door
	byte		 byte_5389, byte_538A;

	word		 word_5748;											// Frankenstein
	byte		 byte_574A, byte_574D, byte_574E, byte_574F;


	word		 word_45DB;											// Lightning
	byte		 byte_44E5, byte_45DD, byte_45DE, byte_45DF;		// Lightning
	byte		 byte_474F;
	
	word		 word_564B;													// Conveyor
	byte		 byte_5642, byte_5643, byte_5644, byte_5645, byte_5646;		// Conveyor
	byte		 byte_5647, byte_5648, byte_5F58, byte_5F57;
	byte		 byte_3FD4;

	byte		 byte_840, byte_841, byte_883, byte_884, byte_885, byte_886, byte_887, byte_888, byte_889, byte_88A, byte_88B, byte_88C, byte_88D, byte_88E, byte_882, byte_D10, byte_D12;
	byte		 byte_839, byte_83E, byte_83F, byte_8C0, byte_8C1, byte_11C9, byte_5CE2;
	byte		 byte_B83, byte_603A;
	char		 byte_5FD5, byte_5FD6, byte_5FD7, byte_5FD8, byte_5F6A;
	
	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtX_0, mTxtY_0, mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_34, word_3C, word_3E, word_40, word_42, word_44;

public:

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress ) {
					if(mMenuIntro)
						return &mLevel[(pAddress - 0x9800) + 2];
					else
						return &mDump[pAddress];
				}
				
				inline byte charRom( word pAddress ) {
					return m64CharRom[ pAddress - 0xD000 ];
				}

		word	 lvlPtrCalculate( byte pCount );
		void	 ScreenClear();
		void	 roomLoad();
		void	 changeLevel( size_t pNumber );
		void	 DisplayText(  );
		void	 drawGraphics(  word pDecodeMode, word pGfxID, word pGfxPosX, word pGfxPosY, byte pTxtCurrentID );
		void	 handleEvents();
		void	 Game();
		void	 GameMain();
		void	 GameAction();
		void	 gameMenuDisplaySetup();
		void	 KeyboardJoystickMonitor( byte pA );
		void	 mainLoop();
		void	 MapDisplay();
		void	 ObjectActions();
		void	 ObjectHitsObject( byte pX );
		void	 objectFunction( byte pX );
		void	 optionsMenu();
		bool	 Menu();
		void	 SavePosition();
		void	 SpriteDraw();
		void	 SpriteMovement( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX );
		void	 start();	
		void	 sub_95F();
		void	 sub_1203();
		void	 sub_160A(); 
		void	 sub_21C8( char pA );
		void	 obj_PrepWalkway( );
		void	 obj_PrepSlidingPole( );
		void	 obj_PrepLadder();
		void	 sub_2973();
		void	 sub_29AE();
		void	 sub_29D0( byte pA, byte pY );
		void	 sub_2E37();
		void	 sub_2E79( );
		void	 sub_3026( byte pX );
		void	 sub_31F6( byte pX );
		void	 sub_359E( );
		void	 sub_3A7F( byte pX );
		void	 sub_3AEB( byte pX );
		void	 sub_3E87( );
		void	 sub_3F4F( );
		void	 obj_ExecLightning( byte pX );
		void	 sub_368A( byte &pY );
		void	 obj_ExecForcefield( byte pX );
		void	 sub_3757( );
		void	 sub_396A( byte pA, byte pX );
		void	 sub_3F14( byte &pX );
		void	 obj_PrepTeleport( );
		void	 obj_PrepRayGun( );
		void	 obj_ExecRayGun( byte pX );
		void	 obj_PrepKey( );
		void	 obj_PrepLock( );
		void	 obj_PrepDoors( );
		void	 obj_PrepDoorbell( );
		void	 obj_ExecLightningMachine( byte pX );
		void	 obj_PrepLightning( );
		void	 obj_PrepForcefield( );
		void	 obj_PrepMummy( );
		void	 obj_PrepTrapDoor();
		void	 obj_ExecConveyor( byte pX );
		void	 obj_PrepConveyor( );
		void	 obj_PrepFrankenstein();
		void	 sub_4DE9( byte pA );
		void	 sub_505C( byte pA, byte pX );
		void	 sub_526F( char &pA );
		bool	 sub_5750( byte &pX );
		void	 sub_57DF( byte pX );
		void	 hw_SpritePrepare( byte &pX );
		byte	 sub_5ED5( );
		void	 sub_5F6B( byte &pX );
		void	 sub_5FA3();
		void	 sub_6009( byte pA );
		void	 textDecode( );
		void	 roomPrepare( );
		void	 run();
};
