/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2014 Robert Crossfield
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
class cBuilder;

enum {
	SPR_ACTION_CREATED	= 0x80,	// Just Created
	SPR_ACTION_FLASH	= 0x40,	// Die
	SPR_ACTION_DIEING	= 0x20,	// Sprite Flash?
	SPR_ACTION_DESTROY	= 0x10,	// Disable ? Dead ? Destroy ?
	SPR_ACTION_FREE		= 0x08,	// Cause the object to be set free

	SPR_FLAG_OVERLAPS	= 0x04,
	SPR_FLAG_COLLIDES	= 0x02,
	SPR_FLAG_FREE		= 0x01
};

enum {
	SPRITE_DOUBLEWIDTH	= 0x80,
	SPRITE_DOUBLEHEIGHT = 0x40,
	SPRITE_PRIORITY		= 0x20,
	SPRITE_MULTICOLOR	= 0x10,
	SPRITE_FLASH_UNK	= 0x01
};

enum {
	ITM_DISABLE	= 0x80,
	ITM_EXECUTE = 0x40,
	ITM_PICKED	= 0x20
};

enum {
	byte_45DD = 0x80,
	byte_45DE = 0x40,
	byte_45DF = 0x20
};

class sCreepSprite {
public:
	byte Sprite_field_0;			// 0
	byte mX;						// 1	
	byte mY;						// 2
	byte spriteImageID;				// 3
	byte state;						// 4
	byte Sprite_field_5;			// 5
	byte Sprite_field_6;			// 6
	byte Sprite_field_7;			// 7
	byte Sprite_field_8;			// 8
	byte spriteFlags;				// 9
	byte mCollisionWidth;			// A
	byte mCollisionHeight;			// B
	byte mWidth;					// C
	byte mHeight;					// D
	byte Sprite_field_E;			// E
	byte Sprite_field_F;			// F
	byte Sprite_field_10;			// 10
	byte Sprite_field_11;			// 11
	byte Sprite_field_12;			// 12
	byte Sprite_field_13;			// 13
	byte Sprite_field_14;			// 14
	byte Sprite_field_15;			// 15
	byte Sprite_field_16;			// 16
	byte Sprite_field_17;			// 17
	byte Sprite_field_18;			// 18
	byte Sprite_field_19;			// 19
	byte Sprite_field_1A;			// 1A
	byte Sprite_field_1B;			// 1B
	byte playerNumber;				// 1C
	byte mButtonState;			// 1D
	byte Sprite_field_1E;			// 1E
	byte Sprite_field_1F;			// 1F

	sCreepSprite() {
		clear();
	}

	void clear() {
		Sprite_field_0 = mX = mY = spriteImageID = 0;
		state = Sprite_field_5 = Sprite_field_6 = Sprite_field_7 = Sprite_field_8 = 0;
		spriteFlags = 0;
		mCollisionWidth = mCollisionHeight = 0;
		mHeight = mHeight = 0;
		Sprite_field_E = Sprite_field_F = Sprite_field_10 = Sprite_field_11 = Sprite_field_12 = Sprite_field_13 = Sprite_field_14 = Sprite_field_15 = Sprite_field_16 = 0;
		Sprite_field_17 = Sprite_field_18 = Sprite_field_19 = Sprite_field_1A = Sprite_field_1B = 0;
		playerNumber = 0;
		mButtonState = Sprite_field_1E = Sprite_field_1F = 0;
	}

};

struct sCreepObject {		// 0xBE
	byte objNumber;
	byte Object_field_1;
	byte Object_field_2;
	byte color;
	byte Object_field_4;
	byte Object_field_5;
	byte Object_field_6;
	byte Object_field_7;

	void clear() {
		objNumber = Object_field_1 = Object_field_2 = color = Object_field_4 = Object_field_5 = Object_field_6 = Object_field_7 = 0;
	}

};

struct sCreepAnim {		// 0xBF
	byte mFuncID;
	byte mX;
	byte mY;
	byte mGfxID;
	byte mFlags;
	byte mWidth;
	byte mHeight;
	byte Anim_field_7;

	void clear() {
		mFuncID = mX = mY = mGfxID = mFlags = mWidth = mHeight = Anim_field_7 = 0;
	}
};

#define MAX_SPRITES 0x8
#define MAX_OBJECTS 0x20

class cCreep {

protected:
	sCreepSprite	 mRoomSprites[ MAX_SPRITES ];	// BD00
	sCreepObject	 mRoomObjects[ MAX_OBJECTS ];	// BE00
	sCreepAnim		 mRoomAnim[ MAX_OBJECTS ];		// BF00

	byte			*mMemory,			*mGameData,		*mLevel,		*m64CharRom;
	size_t			 mMemorySize;

	cDebug			*mDebug;
	cCastle			*mCastle;
	cCastleManager	*mCastleManager;
	cScreen			*mScreen;
	cPlayerInput	*mInput;
	cSound			*mSound;
	cBuilder		*mBuilder;

	string			 mMusicCurrent;
	string			 mWindowTitle;
	byte			*mMusicBuffer, *mMusicBufferStart;
	size_t			 mMusicBufferSize;

	byte		 mFileListingNamePtr;
	
	bool		 mIntro;
	byte		 mMenuMusicScore, mMenuScreenCount, mMenuScreenTimer;
	byte		 mUnlimitedLives;
	dword		 mTicksPrevious;
	timeb		 mPlayer1Time, mPlayer2Time;
	size_t		 mPlayer1Seconds, mPlayer2Seconds;

	bool		 mPlayerStatus[2];
	word		 mVoice, mVoiceTmp;
	byte		 mVoiceNum;

	bool		 mQuit;
	bool		 mRunStopPressed;

	byte		 mStrLength;

	byte		 byte_20DE, byte_24FD;
	byte		 byte_2E02;

	byte		 mEngine_Ticks;
	char		 byte_2232;					// Conveyor

	byte		 byte_311D;

	byte		 mPlayerExecutingSpriteNumber, byte_3638;
	word		 mRoomDoorDataPtr;

	word		 word_4A65;											// Keys
	byte		 mCurrentKeyID;

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
	char		 byte_574A, byte_574D, byte_574E, byte_574F;
	
	byte		 byte_31EF, byte_31F0, byte_31F1, byte_31F2;
	byte		 byte_31F3, byte_31F4, byte_31F5;

	word		 word_45DB;											// Lightning
	byte		 byte_44E5;											// Lightning
	byte		 byte_474F;
	
	word		 word_564B;													// Conveyor
	byte		 byte_5642, byte_5643, byte_5644, byte_5645, byte_5646;		// Conveyor
	byte		 byte_5647, byte_5648, byte_5F58, mJoyButtonState, byte_5F56;

	byte		 byte_D10, byte_D12;
	byte		 byte_839, mObjectCount, byte_8C0, byte_8C1, byte_5CE2;
	byte		 byte_603A, byte_5FD7;
	byte		 byte_5FD5, byte_5FD6, byte_5FD8;

	bool		 mRestorePressed;

	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_34, word_3C, word_3E, word_40, word_42, word_44;

	bool		 mMenuReturn, mNoInput;

	// HW Sprite Registers
	byte		 mSprite_Multicolor_Enable, mSprite_DataPriority, mSprite_Y_Expansion, mSprite_X_Expansion;

public:
	int			 mStartCastle;

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
		inline void				 screenSet( cScreen *pScreen ) { mScreen = pScreen; }

		inline byte	mStrLengthGet() { return mStrLength; }

		void	 builderStart( int pStartCastle );

		cCastle	*castleGet() { return mCastle; }

		void	 start( int pStartLevel, bool pUnlimited );			// Game Entry Point
		void	 run( int pArgCount, char *pArgs[] );					// Executed from main()

		void	 castleRoomData( byte pRoomNumber );

		bool	 ChangeLevel( size_t pMenuItem );
		void	 Game();
		void	 roomMain();

		void	 events_Execute();
		void	 KeyboardJoystickMonitor( byte pA );

		void	 gameEscapeCastle();
		void	 gameHighScores( );
		void	 optionMenuPrepare();

		void	 gamePositionLoad();
		void	 gamePositionSave( bool pCastleSave );
		void	 gameFilenameGet( bool pLoading, bool pCastleSave );

		void	 stringSet( byte pPosX, byte pPosY, byte pColor, string pMessage );

		void	 graphicRoomColorsSet( byte pRoomColor );

		void	 hw_Update();							// 
		void	 hw_IntSleep( byte pA );				// hardware interrupt wait loop
		void	 hw_SpritePrepare( byte pSpriteNumber );// prepare a sprite 

		bool	 Intro();								// Intro Loop
		void	 interruptWait( byte pCount );			// Wait 'pCount' amount of VIC-II interrupt executions

		virtual void	 mainLoop();							// Main Intro/Game Loop

		void	 mapArrowDraw( byte pPlayer );
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

		void	 sprite_FlashOnOff( byte pSpriteNumber );// Flash a sprite on and off
		void	 stringDraw( );							// Draw a string

		void	 textShow();
		void	 titleDisplay();						// Display 'PIC A TITLE'

		void	 roomLoad();
		void	 roomPrepare( word pAddress = 0);
		
		void	 sub_95F();
		void	 obj_Image_Draw();
		void	 gameHighScoresHandle();
		void	 sound_PlayEffect( char pA );
		
		void	 textPrintCharacter();
		byte	 textGetKeyFromUser();

		void	 sub_2973();
		void	 convertTimerToTime();
		void	 convertTimeToNumber( byte pA, byte pY );
		void	 obj_Mummy_Hit_Player( byte pX, byte pY );
		void	 obj_Laser_Collision( byte pX, byte pY );
		void	 obj_Teleport_unk( byte pA, byte pX );
		void	 obj_TrapDoor_PlaySound( byte pA );
		void	 sub_526F( char &pA );
		void	 roomAnim_Disable( byte pSpriteNumber );			// Redraw floor piece?
		bool	 obj_Key_NotFound( byte pA, byte pX, byte pY );
		void	 positionCalculate( byte pSpriteNumber );
		void	 word_3C_Calculate();
		void	 sub_6009( byte pA );
		
		// Image Handling Functions
		void	 anim_Execute( );
		void	 anim_Update( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pX );
		
		bool	 object_Create( byte &pX );

		// object Handling Functions
		void	 obj_Actions( );
		bool	 obj_Actions_Collision( byte pSpriteNumber, byte pY );
		bool	 obj_Actions_InFront( byte pSpriteNumber, byte pY );
		void	 obj_Actions_Hit( byte pSpriteNumber, byte pY );
		void	 obj_Actions_Execute( byte pSpriteNumber );
		void	 obj_CheckCollisions( byte pSpriteNumber );
		void	 obj_CollisionSet();
		
		int		 sprite_CreepFindFree( );
		sCreepSprite *sprite_CreepGetFree( );

		void	 obj_OverlapCheck( byte pSpriteNumber );

		void	 obj_MultiDraw();					// Draw multiple objects
		void	 obj_stringPrint(  );				// Draw a string

		// Object Functions
		void	 obj_Conveyor_Prepare( );
		void	 obj_Conveyor_InFront( byte pSpriteNumber, byte pY );
		void	 obj_Conveyor_Control_InFront( byte pSpriteNumber, byte pY );
		void	 obj_Conveyor_Img_Execute( byte pX );

		void	 obj_Door_Prepare( );
		void	 obj_Door_InFront( byte pSpriteNumber, byte pY );
		void	 obj_Door_Img_Execute( byte pSpriteNumber );

		void	 obj_Door_Button_Prepare( );
		void	 obj_Door_Button_InFront( byte pSpriteNumber, byte pY );

		void	 obj_Door_Lock_InFront( byte pSpriteNumber, byte pY );
		void	 obj_Door_Lock_Prepare( );
		
		void	 obj_Forcefield_Prepare( );
		void	 obj_Forcefield_Execute( byte pSpriteNumber );
		void	 obj_Forcefield_Create( );
		void	 obj_Forcefield_Img_Timer_Execute( byte pSpriteNumber );
		void	 obj_Forcefield_Timer_InFront( byte pSpriteNumber, byte pObjectNumber );
		
		void	 obj_Frankie_Add( );
		void	 obj_Frankie_Collision( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Frankie_Load();
		void	 obj_Frankie_Hit( byte pSpriteNumber, byte pY );
		void	 obj_Frankie_Execute( byte pSpriteNumber );

		void	 obj_Key_Infront( byte pSpriteNumber, byte pY );
		void	 obj_Key_Load( );

		void	 obj_Ladder_Prepare();

		void	 obj_Lightning_Prepare( );
		void	 obj_Lightning_Create_Sprite( byte &pY );
		void	 obj_Lightning_Execute( byte pX );
		void	 obj_Lightning_Img_Execute( byte pX );
		void	 obj_Lightning_Switch_InFront( byte pX, byte pY );
		
		void	 obj_Mummy_Prepare( );
		void	 obj_Mummy_Add( byte pA, byte pX );
		void	 obj_Mummy_Collision( byte pSpriteNumber, byte pY );
		void	 obj_Mummy_Execute( byte pSpriteNumber );
		void	 obj_Mummy_Infront( byte pSpriteNumber, byte pY );
		void	 obj_Mummy_Img_Execute( byte pX );

		void	 obj_Player_Add( );
		void	 obj_Player_Collision( byte pSpriteNumber, byte pY );
		void	 obj_Player_Hit( byte pSpriteNumber, byte pY );
		void	 obj_Player_Execute( byte pSpriteNumber );
		void	 obj_Player_Color_Set( byte pSpriteNumber );
		
		void	 obj_SlidingPole_Prepare( );

		void	 obj_Teleport_Prepare( );
		void	 obj_Teleport_Img_Execute( byte pX );
		void	 obj_Teleport_InFront( byte pSpriteNumber, byte pY );

		void	 obj_TrapDoor_Prepare();
		void	 obj_TrapDoor_Switch_Img_Execute( byte pX );

		void	 obj_RayGun_Prepare( );
		void	 obj_RayGun_Laser_Add( byte pX );
		void	 obj_RayGun_Laser_Execute( byte pSpriteNumber );
		void	 obj_RayGun_Img_Execute( byte pX );
		void	 obj_RayGun_Control_InFront( byte pSpriteNumber, byte pY );
		void	 obj_RayGun_Control_Update( byte pA );

		void	 obj_Walkway_Prepare( );
};
