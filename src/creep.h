/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2015 Robert Crossfield
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

struct sObjectData {
	byte mFlashData;
	byte mHitData;
};

enum {
	OBJECT_TYPE_DOOR				= 0x00,
	OBJECT_TYPE_DOOR_BUTTON			= 0x01,
	OBJECT_TYPE_LIGHTNING_MACHINE	= 0x02,
	OBJECT_TYPE_LIGHTNING_CONTROL	= 0x03,
	OBJECT_TYPE_FORCEFIELD			= 0x04,
	OBJECT_TYPE_MUMMY				= 0x05,
	OBJECT_TYPE_KEY					= 0x06,
	OBJECT_TYPE_DOOR_LOCK			= 0x07,
	OBJECT_TYPE_RAYGUN_LASER		= 0x08,
	OBJECT_TYPE_RAYGUN_CONTROL		= 0x09,
	OBJECT_TYPE_TELEPORTER			= 0x0A,
	OBJECT_TYPE_TRAPDOOR_PANEL		= 0x0B,
	OBJECT_TYPE_TRAPDOOR_SWITCH		= 0x0C,
	OBJECT_TYPE_CONVEYOR_CONTROL	= 0x0D,
	OBJECT_TYPE_CONVEYOR_BELT		= 0x0E,
	OBJECT_TYPE_FRANKIE				= 0x0F
};

enum {
	SPRITE_TYPE_PLAYER		= 0x00,
	SPRITE_TYPE_LIGHTNING	= 0x01,
	SPRITE_TYPE_FORCEFIELD	= 0x02,
	SPRITE_TYPE_MUMMY		= 0x03,
	SPRITE_TYPE_LASER		= 0x04,
	SPRITE_TYPE_FRANKIE		= 0x05,
};

enum {
	SPR_ACTION_CREATED	= 0x80,	// Just Created
	SPR_ACTION_FLASH	= 0x40,	// Die
	SPR_ACTION_DIEING	= 0x20,	// Sprite Flash?
	SPR_ACTION_DESTROY	= 0x10,	// Disable ? Dead ? Destroy ?
	SPR_ACTION_FREE		= 0x08,	// Cause the object to be set free

	SPR_COLLIDE_BACKGROUND	= 0x04,
	SPR_COLLIDE_SPRITE		= 0x02,
	SPR_UNUSED				= 0x01
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
	RAYGUN_END_MARKER			= 0x80,
	RAYGUN_DONT_FIRE			= 0x40,
	RAYGUN_PLAYER_CONTROLLING	= 0x20,
	RAYGUN_TRACK_ONLY			= 0x10,
	RAYGUN_FIRED_BY_PLAYER		= 0x08,
	RAYGUN_MOVE_UP				= 0x04,
	RAYGUN_MOVE_DOWN			= 0x02,
	RAYGUN_TRACK_TO_LEFT		= 0x01
};

enum {
	LIGHTNING_IS_SWITCH		= 0x80,
	LIGHTNING_IS_ON			= 0x40,
	LIGHTNING_END_MARKER	= 0x20
};

enum {
	CONVEYOR_END_MARKER			= 0x80,
	CONVEYOR_PLAYER2_ENABLEDIT	= 0x20,		// Player2 turned it on
	CONVEYOR_PLAYER1_ENABLEDIT	= 0x10,		// Player1 turned it on
	CONVEYOR_PLAYER2_SWITCHED	= 0x08,		// Player2 just flicked the switch
	CONVEYOR_PLAYER1_SWITCHED	= 0x04,		// Player1 just flicked the switch
	CONVEYOR_MOVING_RIGHT		= 0x02,		// Conveyor is moving to the right
	CONVEYOR_TURNED_ON			= 0x01		// Conveyor is moving
};

enum {
	FRANKIE_END_MARKER		= 0x80,
	FRANKIE_DEATH			= 0x04,
	FRANKIE_AWAKE			= 0x02,
	FRANKIE_POINTING_LEFT	= 0x01
};

enum {
	TRAPDOOR_END_MARKER = 0x80,
	TRAPDOOR_OPEN		= 0x01
};

enum {
	MAP_ROOM_STOP_DRAW	 = 0x40,
	MAP_ROOM_VISIBLE = 0x80
};

enum { 
	SOUND_LASER_FIRED			= 0x00,
	SOUND_TRAPDOOR_SWITCHED		= 0x01,
	SOUND_FORCEFIELD_TIMER		= 0x02,
	SOUND_DOOR_OPEN				= 0x03,
	SOUND_TELEPORT				= 0x04,
	SOUND_TELEPORT_CHANGE		= 0x05,
	SOUND_LIGHTNING_SWITCHED	= 0x06,
	SOUND_FRANKIE				= 0x07,
	SOUND_SPRITE_FLASH			= 0x08,
	SOUND_MAP_CLOSE				= 0x09,
	SOUND_CONVEYOR_SWITCH		= 0x0A,
	SOUND_MUMMY_RELEASE			= 0x0B,
	SOUND_KEY_PICKUP			= 0x0C
};

class sCreepSprite {
public:
	byte mSpriteType;				// 0
	byte mX;						// 1	
	byte mY;						// 2
	byte spriteImageID;				// 3
	byte state;						// 4
	byte Sprite_field_5;			// 5
	byte Sprite_field_6;			// 6
	byte Sprite_field_8;			// 8
	byte spriteFlags;				// 9
	byte mCollisionWidth;			// A
	byte mCollisionHeight;			// B
	byte mWidth;					// C
	byte mHeight;					// D
	byte Sprite_field_18;			// 18
	byte Sprite_field_19;			// 19
	byte Sprite_field_1A;			// 1A
	byte Sprite_field_1B;			// 1B
	byte playerNumber;				// 1C
	byte mButtonState;				// 1D
	byte Sprite_field_1E;			// 1E
	byte Sprite_field_1F;			// 1F

	sCreepSprite() {
		clear();
	}

	void clear() {
		mSpriteType = mX = mY = spriteImageID = 0;
		state = Sprite_field_5 = Sprite_field_6 = Sprite_field_8 = 0;
		spriteFlags = 0;
		mCollisionWidth = mCollisionHeight = 0;
		mHeight = mHeight = 0;
		Sprite_field_18 = Sprite_field_19 = Sprite_field_1A = Sprite_field_1B = 0;
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

	void clear() {
		objNumber = Object_field_1 = Object_field_2 = color = Object_field_4 = Object_field_5 = 0;
	}

};

struct sCreepAnim {		// 0xBF
	byte mObjectType;
	byte mX;
	byte mY;
	byte mGfxID;
	byte mFlags;
	byte mWidth;
	byte mHeight;

	void clear() {
		mObjectType = mX = mY = mGfxID = mFlags = mWidth = mHeight = 0;
	}
};

#define MAX_SPRITES 0x8
#define MAX_OBJECTS 0x20

class cCreep : public cSingleton<cCreep> {

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

	byte		 mMusicPlaying, mSaveGameLoaded;

	byte		 mEngine_Ticks;
	char		 mPlayingSound;					

	byte		 mPlayerExecutingSpriteNumber, mCurrentPlayer;

	word		 mRoomDoorPtr,mRoomKeyPtr,mRoomMummyPtr,mRoomRayGunPtr;		// Room Level Data Pointers

	byte		 mRaygunCount;
	byte		 mRaygunTmpVar;

	word		 mRoomTrapDoorPtr;											// Trap Door

	word		 mFrankiePtr;												// Frankenstein
	char		 mFrankieCount;

	word		 mRoomLightningPtr;											// Lightning
	
	word		 mConveyorPtr;												// Conveyor
	byte		 mJoyButtonState, byte_5F56;

	byte		 byte_D10;
	byte		 mDisableSoundEffects, mObjectCount;
	byte		 byte_5FD7;
	byte		 byte_5FD5, byte_5FD6, byte_5FD8;

	bool		 mRestorePressed;

	byte		 mGfxEdgeOfScreenX;
	byte		 mTextXPos, mTextYPos, mTextColor, mTextFont, mTextFontt;
	byte		 mTxtPosLowerY, mTxtDestXLeft, mTxtDestX, mTxtEdgeScreenX;
	byte		 mTxtDestXRight, mTxtWidth, mTxtHeight;
	byte		 mGfxWidth, mGfxHeight;
	byte		 mCount;
	 
	word		 word_30, word_32, word_3C, mObjectPtr, word_40, mRoomPtr;

	bool		 mMenuReturn, mNoInput;

public:
	std::vector<cEvent>		mEvents;

	int			 mStartCastle;

	byte		 mTxtX_0, mTxtY_0;

				 cCreep();
				~cCreep();
				
	void		eventProcess( bool pResetKeys );

				inline byte charRom( word pAddress ) {
					return m64CharRom[ pAddress - 0xD000 ];
				}

				inline byte *memory( word pAddress ) {
					return &mMemory[ pAddress ];
				}
				
				inline byte *musicBufferGet() {
					return mMusicBuffer;
				}

		bool			EventAdd( cEvent pEvent );

		inline cPlayerInput		*inputGet()		{ return mInput; }
		inline cScreen			*screenGet()	{ return mScreen; }
		inline void				 screenSet( cScreen *pScreen ) { mScreen = pScreen; }

		inline byte	mStrLengthGet() { return mStrLength; }

		void	 builderStart( int pStartCastle );

		cCastle	*castleGet() { return mCastle; }

		void	 start( int pStartLevel, bool pUnlimited );			// Game Entry Point
		void	 run( int pArgCount, char *pArgs[] );					// Executed from main()

		void	 roomPtrSet( byte pRoomNumber );

		bool	 ChangeLevel( size_t pMenuItem );
		void	 Game();
		void	 roomMain();

		void	 events_Execute();
		void	 KeyboardJoystickMonitor( byte pA );

		void	 gameEscapeCastle();
		void	 gameHighScores( );
		void	 optionsMenuPrepare();

		void	 gamePositionLoad();
		void	 gamePositionSave( bool pCastleSave );
		void	 gameFilenameGet( bool pLoading, bool pCastleSave );

		void	 stringSet( byte pPosX, byte pPosY, byte pColor, string pMessage );

		void	 roomSetColours( byte pRoomColor );

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

		void	 Sprite_FlashOnOff( byte pSpriteNumber );// Flash a sprite on and off
		void	 stringDraw( );							// Draw a string

		void	 textShow();
		void	 titleDisplay();						// Display 'PIC A TITLE'

		void	 roomLoad();
		void	 roomPrepare( word pAddress = 0);
		
		void	 DisableSpritesAndStopSound();

		void	 gameHighScoresHandle();
		void	 sound_PlayEffect( char pA );
		
		void	 textPrintCharacter();
		byte	 textGetKeyFromUser();

		void	 convertTimerToTime();
		void	 convertTimeToNumber( byte pA, byte pY );

		void	 obj_TrapDoor_Switch_Check( byte pA );
		void	 roomAnim_Disable( byte pSpriteNumber );			// Redraw floor piece?

		void	 positionCalculate( byte pSpriteNumber );
		void	 word_3C_Calculate();
		byte	 sub_6009( byte pA );
		
		// Image Handling Functions
		void	 object_Execute( );
		void	 Draw_RoomAnimObject( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pObjectNumber );
		
		bool	 object_Create( byte &pX );

		// object Handling Functions
		void	 Sprite_Execute( );
		void	 Sprite_Execute_Action( byte pSpriteNumber );
		void	 Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 );
		void	 Sprite_Collision_Check( byte pSpriteNumber );
		void	 Sprite_Collision_Set();
		bool	 Sprite_Object_Collision( byte pSpriteNumber, byte pObjectNumber );
		bool	 Sprite_Object_Infront_Execute( byte pSpriteNumber, byte pObjectNumber );
		
		int		 Sprite_CreepFindFree( );
		sCreepSprite *Sprite_CreepGetFree( );

		void	 Sprite_Object_Collision_Check( byte pSpriteNumber );

		void	 obj_MultiDraw();					// Draw multiple objects
		void	 obj_stringPrint(  );				// Draw a string

		// Object Functions
		void	 obj_Conveyor_Prepare( );
		void	 obj_Conveyor_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Conveyor_Control_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Conveyor_Execute( byte pObjectNumber );

		void	 obj_Door_Prepare( );
		void	 obj_Door_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Door_Execute( byte pObjectNumber );

		void	 obj_Door_Button_Prepare( );
		void	 obj_Door_Button_InFront( byte pSpriteNumber, byte pObjectNumber );

		void	 obj_Door_Lock_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Door_Lock_Prepare( );
		
		void	 obj_Forcefield_Prepare( );
		void	 obj_Forcefield_Execute( byte pSpriteNumber );
		void	 obj_Forcefield_Create( byte pObjectNumber );
		void	 obj_Forcefield_Timer_Execute( byte pObjectNumber );
		void	 obj_Forcefield_Timer_InFront( byte pSpriteNumber, byte pObjectNumber );
		
		void	 obj_Frankie_Sprite_Create( );
		bool	 obj_Frankie_Collision( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Frankie_Prepare();
		bool	 obj_Frankie_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 );
		void	 obj_Frankie_Execute( byte pSpriteNumber );

		void	 obj_Image_Draw();

		void	 obj_Key_Infront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Key_Prepare( );
		bool	 obj_Key_NotFound( byte pObjectNumber, byte pSpriteNumber );

		void	 obj_Ladder_Prepare();

		void	 obj_Lightning_Prepare( );
		void	 obj_Lightning_Sprite_Create( byte pObjectNumber );
		void	 obj_Lightning_Execute( byte pObjectNumber );
		void	 obj_Lightning_Pole_Execute( byte pObjectNumber );
		void	 obj_Lightning_Switch_InFront( byte pSpriteNumber, byte pObjectNumber );
		
		void	 obj_Mummy_Prepare( );
		void	 obj_Mummy_Sprite_Create( byte pA, byte pObjectNumber );
		bool	 obj_Mummy_Collision( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Mummy_Execute( byte pObjectNumber );
		void	 obj_Mummy_Infront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Mummy_Tomb_Execute( byte pObjectNumber );
		bool	 obj_Mummy_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 );

		void	 obj_Player_Add( );
		bool	 obj_Player_Collision( byte pSpriteNumber, byte pObjectNumber );
		bool	 obj_Player_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 );
		void	 obj_Player_Execute( byte pObjectNumber );
		void	 obj_Player_Color_Set( byte pSpriteNumber );
		
		void	 obj_SlidingPole_Prepare( );

		void	 obj_Teleport_Prepare( );
		void	 obj_Teleport_Execute( byte pObjectNumber );
		void	 obj_Teleport_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_Teleport_SetColour( byte pColor, byte pObjectNumber );
		
		void	 obj_TrapDoor_PlaySound( byte pA );
		void	 obj_TrapDoor_Prepare();
		void	 obj_TrapDoor_Switch_Execute( byte pObjectNumber );

		void	 obj_RayGun_Prepare( );
		void	 obj_RayGun_Laser_Sprite_Create( byte pObjectNumber );
		bool	 obj_RayGun_Laser_Collision( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_RayGun_Laser_Execute( byte pObjectNumber );
		void	 obj_RayGun_Execute( byte pObjectNumber );
		void	 obj_RayGun_Control_InFront( byte pSpriteNumber, byte pObjectNumber );
		void	 obj_RayGun_Control_Update( byte pA );

		void	 obj_Walkway_Prepare( );
};

#define g_Creep cCreep::GetSingleton()
