/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2010 Robert Crossfield
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  ------------------------------------------
 *  Game Header
 *  ------------------------------------------
 */

class cCastle;
class cCastleManager;
class cScreen;
class cPlayerInput;
class cSound;
class cDebug;

class cCreep {

private:

	byte			*mMemory,			*mGameData,		*mLevel,		*m64CharRom;
	size_t			 mMemorySize;

	cDebug			*mDebug;
	cCastle			*mCastle;
	cCastleManager	*mCastleManager;
	cScreen			*mScreen;
	cPlayerInput	*mInput;
	cSound			*mSound;

	string			 mMusicCurrent;
	byte			*mMusicBuffer, *mMusicBufferStart;
	size_t			 mMusicBufferSize;

	byte		 mFileListingNamePtr;
	
	bool		 mIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	byte		 mUnlimitedLives;
	timeb		 mTimePrevious, mPlayer1Time, mPlayer2Time;
	size_t		 mPlayer1Seconds, mPlayer2Seconds;

	word		 mVoice, mVoiceTmp;
	byte		 mVoiceNum;

	bool		 mQuit;

	byte		 mStrLength;

	byte		 byte_20DE, byte_24FD, mRunStopPressed;
	byte		 byte_2E02;

	byte		 mInterruptCounter, byte_2E36, byte_2232;					// Conveyor

	byte		 byte_311D;

	byte		 byte_34D6, byte_3638;
	word		 word_41D3;

	word		 word_4A65;											// Keys
	byte		 byte_4A64;

	word		 word_498B;											// Mummys

	word		 word_4D5B;											// RayGuns	
	byte		 byte_4D5D;
	byte		 byte_4D5E, byte_4D5F;
	byte		 byte_574C, byte_4D60, byte_4D61;
	byte		 byte_4D62, byte_4D63, byte_4D64;
	byte		 byte_4D65, byte_4D66, byte_4D67;

	word		 word_5387;											// Trap Door
	byte		 byte_5389, byte_538A;

	word		 word_5748;											// Frankenstein
	byte		 byte_574A, byte_574D, byte_574E, byte_574F;
	
	byte		 byte_31EF, byte_31F0, byte_31F1, byte_31F2;
	byte		 byte_31F3, byte_31F4, byte_31F5;

	word		 word_45DB;											// Lightning
	byte		 byte_44E5, byte_45DD, byte_45DE, byte_45DF;		// Lightning
	byte		 byte_474F;
	
	word		 word_564B;													// Conveyor
	byte		 byte_5642, byte_5643, byte_5644, byte_5645, byte_5646;		// Conveyor
	byte		 byte_5647, byte_5648, byte_5F58, mJoyButtonState, byte_5F56;
	byte		 byte_3FD4;

	byte		 byte_883, byte_884, byte_885, byte_886, byte_887, byte_888, byte_889, byte_88A, byte_88B, byte_88C, byte_88D, byte_88E, byte_882, byte_D10, byte_D12;
	byte		 byte_839, mImageCount, byte_8C0, byte_8C1, byte_5CE2;
	byte		 byte_B83, byte_603A, byte_5FD7;
	byte		 byte_5FD5, byte_5FD6, byte_5FD8;
	
	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_34, word_3C, word_3E, word_40, word_42, word_44;

	bool		 mMenuReturn;

public:
	byte		 mTxtX_0, mTxtY_0;

				 cCreep();
				~cCreep();

				inline byte	*level( word pAddress ) {
					if(mIntro)
						return &mMemory[pAddress];
					else
						return &mMemory[pAddress];
				}
				
				inline byte charRom( word pAddress ) {
					return m64CharRom[ pAddress - 0xD000 ];
				}

				inline byte *memory( word pAddress ) {
					return &mMemory[ pAddress ];
				}
				
				inline byte *gameData( word pAddress ) {
					return &mGameData[ pAddress - 0x800 ];
				}

				inline byte *gfxPtr( word pID ) {
					word  gfxAddress  = (0x603B + (pID << 1));
					byte *gfxTablePtr = gameData( gfxAddress );
					byte *gfxPtr = gameData( readLEWord( gfxTablePtr ));
						
					return gfxPtr;
				}
			
				inline byte *musicBufferGet() {
					return mMusicBuffer;
				}

		inline cPlayerInput		*inputGet()		{ return mInput; }
		inline cScreen			*screenGet()	{ return mScreen; }

		void	 start( size_t pStartLevel, bool pUnlimited );			// Game Entry Point
		void	 run( int pArgCount, char *pArgs[] );					// Executed from main()

		void	 lvlPtrCalculate( byte pCount );

		bool	 ChangeLevel( size_t pMenuItem );
		void	 Game();
		void	 roomMain();

		void	 events_Execute();
		void	 KeyboardJoystickMonitor( byte pA );

		void	 gameEscapeCastle();
		void	 gameHighScores( );
		void	 gameMenuDisplaySetup();

		void	 gamePositionLoad();
		void	 gamePositionSave();
		void	 gamePositionFilenameGet( bool pLoading );

		void	 graphicRoomColorsSet( byte pRoomColor );

		void	 hw_Update();							// 
		void	 hw_IntSleep( byte pA );				// hardware interrupt wait loop
		void	 hw_SpritePrepare( byte &pX );			// prepare a sprite 

		bool	 Intro();								// Intro Loop
		void	 interruptWait( byte pCount );			// Wait 'pCount' amount of VIC-II interrupt executions

		void	 mainLoop();							// Main Intro/Game Loop

		bool	 mapDisplay();							// Map Screen
		void	 mapRoomDraw();							// Draw the rooms on the map
		
		void	 menuUpdate( size_t pCastleNumber );
		void     musicChange();
		bool	 musicBufferFeed();
		void	 musicPtrsSet();

		void	 optionsMenu();							// In-Game Options Menu

		void	 screenClear();							// Clear the screen
		void	 screenDraw(  word pDecodeMode, word pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID );
		
		byte	 seedGet( );

		void	 sprite_FlashOnOff( byte pX );			// Flash a sprite on and off
		void	 stringDraw( );							// Draw a string

		void	 textShow();
		void	 titleDisplay();						// Display 'PIC A TITLE'

		void	 roomLoad();
		void	 roomPrepare( );
		
		void	 sub_95F();
		void	 obj_Image_Draw();
		void	 gameHighScoresHandle();
		void	 sub_21C8( char pA );
		
		void	 textPrintCharacter();
		byte	 textGetKeyFromUser();
		void	 sub_2973();
		void	 convertTimerToTime();
		void	 convertTimeToNumber( byte pA, byte pY );
		void	 sub_3940( byte pX, byte pY );
		void	 sub_3A60( byte pX, byte pY );
		void	 obj_Teleport_unk( byte pA, byte pX );
		void	 sub_5171( byte pA );
		void	 sub_526F( char &pA );
		void	 sub_57DF( byte pX );
		bool	 sub_5E8E( byte pA, byte pX, byte pY );
		void	 sub_5F6B( byte &pX );
		void	 sub_5FA3();
		void	 sub_6009( byte pA );
		
		// Image Handling Functions
		void	 img_Actions( );
		bool	 img_FindFree( byte &pX );
		void	 img_Update( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX );

		// object Handling Functions
		void	 obj_Actions( );
		bool	 obj_Actions_Collision( byte pX, byte pY );
		bool	 obj_Actions_InFront( byte pX, byte pY );
		void	 obj_Actions_Hit( byte pX, byte pY );
		void	 obj_Actions_Execute( byte pX );
		void	 obj_CheckCollisions( byte pX );
		void	 obj_CollisionSet();
		void	 obj_FindFree( byte &pX );
		void	 obj_OverlapCheck( byte pX );

		void	 obj_MultiDraw();					// Draw multiple objects
		void	 obj_stringPrint(  );				// Draw a string

		// Object Functions
		void	 obj_Conveyor_Prepare( );
		void	 obj_Conveyor_InFront( byte pX, byte pY );
		void	 obj_Conveyor_Control_InFront( byte pX, byte pY );
		void	 obj_Conveyor_Img_Execute( byte pX );

		void	 obj_Door_Prepare( );
		void	 obj_Door_InFront( byte pX, byte pY );
		void	 obj_Door_Img_Execute( byte pX );

		void	 obj_Door_Button_Prepare( );
		void	 obj_Door_Button_InFront( byte pX, byte pY );

		void	 obj_Door_Lock_InFront( byte pX, byte pY );
		void	 obj_Door_Lock_Prepare( );
		
		void	 obj_Forcefield_Prepare( );
		void	 obj_Forcefield_Execute( byte pX );
		void	 obj_Forcefield_Create( );
		void	 obj_Forcefield_Img_Timer_Execute( byte pX );
		void	 obj_Forcefield_Timer_InFront( byte pX, byte pY );
		
		void	 obj_Frankie_Add( );
		void	 obj_Frankie_Collision( byte pX, byte pY );
		void	 obj_Frankie_Load();
		void	 obj_Frankie_Hit( byte pX, byte pY );
		void	 obj_Frankie_Execute( byte pX );

		void	 obj_Key_Infront( byte pX, byte pY );
		void	 obj_Key_Load( );

		void	 obj_Ladder_Prepare();

		void	 obj_Lightning_Prepare( );
		void	 obj_Lightning_Add( byte &pY );
		void	 obj_Lightning_Execute( byte pX );
		void	 obj_Lightning_Img_Execute( byte pX );
		void	 obj_Lightning_Switch_InFront( byte pX, byte pY );
		
		void	 obj_Mummy_Prepare( );
		void	 obj_Mummy_Add( byte pA, byte pX );
		void	 obj_Mummy_Collision( byte pX, byte pY );
		void	 obj_Mummy_Execute( byte pX );
		void	 obj_Mummy_Infront( byte pX, byte pY );
		void	 obj_Mummy_Img_Execute( byte pX );

		void	 obj_Player_Add( );
		void	 obj_Player_Collision( byte pX, byte pY );
		void	 obj_Player_Hit( byte pX, byte pY );
		void	 obj_Player_Execute( byte pX );
		void	 obj_Player_Unk( byte pX );
		
		void	 obj_SlidingPole_Prepare( );

		void	 obj_Teleport_Prepare( );
		void	 obj_Teleport_Img_Execute( byte pX );
		void	 obj_Teleport_InFront( byte pX, byte pY );

		void	 obj_TrapDoor_Prepare();
		void	 obj_TrapDoor_Switch_Img_Execute( byte pX );

		void	 obj_RayGun_Prepare( );
		void	 obj_RayGun_Laser_Add( byte pX );
		void	 obj_RayGun_Laser_Execute( byte pX );
		void	 obj_RayGun_Img_Execute( byte pX );
		void	 obj_RayGun_Control_InFront( byte pX, byte pY );
		void	 obj_RayGun_Control_Update( byte pA );

		void	 obj_Walkway_Prepare( );
};
