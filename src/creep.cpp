/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2016 Robert Crossfield
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
 *  Game
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "vic-ii/screen.h"
#include "vic-ii/sprite.h"
#include "playerInput.h"
#include "castle/castle.h"
#include "castle/objects/object.hpp"
#include "castleManager.h"
#include "creep.h"
#include "sound/sound.h"

#include "debug.h"

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif

const unsigned char cCreep::mRoomIntroData[] = {

	0x0A, 0x16, // eObjectIntroMultiDraw
	//	Count, Id,     X,    Y
		0x08, 0x06, 0x10, 0x48, 0x14, 0x00, 
	0x00,

	0x6D, 0x2A, // eObjectIntroText

	// X     Y  FONT  COLOR
	0x28, 0x20, 0x08, 0x21, 'T', 'H', 'E', ' ', 'C', 'A', 'S', 'T', 'L', 'E', 'S', ' ', 'O', 'F' | 0x80,
	0x30, 0x30, 0x0D, 0x22, 'D', 'O', 'C', 'T', 'O', 'R', ' ', 'C', 'R', 'E', 'E', 'P' | 0x80,
	0x34, 0x70, 0x07, 0x21, 'B', 'Y', ' ', 'E', 'D', ' ', 'H', 'O', 'B', 'B', 'S' | 0x80,
	0x10, 0xA0, 0x03, 0x21, 'T', 'U', 'T', 'O', 'R', 'I', 'A', 'L', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' | 0x80,
	0x10, 0xC0, 0x0C, 0x21, 'P', 'R', 'E', 'S', 'S', ' ', 'A', 'C', 'T', 'I', 'O', 'N', ' ', 'O', 'R', ' ', 'M', 'E', 'N', 'U' | 0x80,
	0x00,
	
	// Original
	//	0x10, 0xC0, 0x0C, 0x21, 'B', 'R', 'O', 'D', 'E', 'R', 'B', 'U', 'N', 'D', ' ', ' ', 'S', 'O', 'F', 'T', 'W', 'A', 'R', 'E' | 0x80,
	0x00, 0x00	// eObjectNone
};

sObjectData mObjectCollisionData[6] = {	{0x01, 0x00}, // Player
										{0x00, 0x04}, // Lightning
										{0x00, 0x03}, // Forcefield
										{0x01, 0x02}, // Mummy
										{0x00, 0x04}, // RayGun Laser
										{0x01, 0x00}  // Frankie
									};

cCreep::cCreep() {
	size_t romSize;
	mWindowTitle = "The Castles of Dr. Creep";

	mLevel = 0;
	mMenuMusicScore = 0xFF;
	mUnlimitedLives = 0;
	mQuit = false;
	mNoInput = false;

	// Prepare memory for use
	mMemorySize = 0x10000;
	mMemory = new byte[ mMemorySize ];

	memset( mMemory, 0, mMemorySize );

	mDebug = new cDebug();
	mCastleManager = new cCastleManager();
	mInput = new cPlayerInput();
	mScreen = new cScreen( mWindowTitle );
	mSound = 0;

	// Load the C64 Character Rom
	if( (m64CharRom = local_FileRead( "CHAR.ROM", "",  romSize, false )) == 0 ) {
		mDebug->error("File \"char.rom\" not found");
		return;
	}

	if( (mGameData = mCastleManager->fileLoad("OBJECT", romSize)) == 0 ) {
		mDebug->error("File \"OBJECT\" not found");
		return;
	}

    mGameData += 2;
	romSize -= 2;

	if(romSize)
		// Copy the game data into the memory buffer
		memcpy( &mMemory[ 0x800 ], mGameData, romSize );

	mDisableSoundEffects = 0;

	mRestorePressed = false;

	mMusicPlaying = 0x00;
	mPlayingSound = -1;
	mSaveGameLoaded = 0x00;
	mEngine_Ticks = 0xA0;

	mCurrentPlayer = 0xBA;

	mCastle = 0;
	mCastleChanged = 0;
	mJoyButtonState = 0xA0;

	mMusicCurrent = "MUSIC0";
	mMusicBuffer = 0;
	mMusicBufferStart = 0;
	mMusicBufferSize = 0;

	mMenuReturn = false;
	mTicksPrevious = SDL_GetTicks();
	mTimer = 0;

	mPlayerStatus[0] = mPlayerStatus[1] = false;
}

cCreep::~cCreep() {

	delete mSound;
	delete m64CharRom;
	delete mMemory;
	delete mCastleManager;
	delete mScreen;
	delete mInput;
	delete mDebug;
}

void cCreep::titleDisplay() {
	size_t size;
	byte *buffer, *introImage = buffer = mCastleManager->introLoad( size );

	if(!buffer) {
        mDebug->error( "File \"PIC A TITLE\" not found" );
		return;
	}

	// Skip load address
	buffer += 0x02;	

	mScreen->bitmapLoad( buffer,  buffer + 0x1F40, buffer + 0x2328, 1 );
	
	for (int i = 0; i < 0x15; ++i) {
		hw_IntSleep(1);
		eventProcess(true);
		mScreen->refresh();
	}
}

void cCreep::run( int pArgCount, char *pArgs[] ) {

	int	count = 0;
	int	playLevel = 0;
	bool	playLevelSet = false;
	bool	unlimited = false;

	// Output console message
	cout << "The Castles of Dr. Creep (" << VERSION << ")" << endl << endl;
	cout << "http://creep.sourceforge.net/" << endl << endl ;

	// Check command line parameters
	while( count < pArgCount ) {
		string arg = string( pArgs[count] );

		if( arg == "-u") {
			cout << " Unlimited Lives enabled." << endl;
			unlimited = true;
		}

		if( arg == "-l" )
			playLevelSet = true;

		++count;
	}

	// Level selection was requested
	if(playLevelSet) {
		mCastleManager->castleListDisplay();

		string lvl;
		cout << "\nPlease select a Castle: ";

		cin >> lvl;

		playLevel = atoi( lvl.c_str() );
	}

	// Level numbers begin at 1 in the list, but 0 in the actual game
	if(playLevel)
		--playLevel;

	mScreen->windowTitleUpdate();

	// Display the title screen
#ifndef _DEBUG
	DisplayControls();
	titleDisplay();
#endif

	mStartCastle = playLevel;

	// Start main loop
	start( playLevel, unlimited );
}

void cCreep::interruptWait( byte pCount) {
	// Screen Refresh occurs 50 times per second on a PAL C64
	// and 60 times on an NTSC
	// Meaning the interrupt should fire once every
	// 0.01666666666666666666666666666667 seconds on an NTSC C64
	// and 0.02 seconds on a PAL C64

	double diffMil = (SDL_GetTicks() - mTicksPrevious);
	double	sleepTime = 20;	// Milliseconds

	if( diffMil > sleepTime) 
		sleepTime -= diffMil;

	if( sleepTime < 0 ) {
		if(sleepTime < -20 )
			sleepTime = 0;
		else 
			sleepTime = -sleepTime;
	}

	Sleep( (dword) sleepTime * pCount );

	mTicksPrevious = SDL_GetTicks();
}

//08C2
void cCreep::start( int pStartLevel, bool pUnlimited ) {
	byte	byte_30, byte_31, count;

	mQuit = false;
	mStartCastle = pStartLevel;

	byte_30 = 0x40;
	byte_31 = 0xD7;
	
	if(pUnlimited)
		mUnlimitedLives = 0xFF;

	if(!mSound)
		mSound = new cSound( this );

	for( count = 0xC8; ;) {
		
		mMemory[0xBC00 + count] = byte_30;
		mMemory[0xBB00 + count] = byte_31;

		++count;
		if(count == 0xC8)
			break;

		if( (count & 7) ) {
			++byte_30;
			if(byte_30)
				continue;

			++byte_31;
			
		} else {
			if((byte_30 + 0x39) > 0xFF)
				++byte_31;

			byte_30 += 0x39;
			++byte_31;
		}
	}

	// 0x08F9
	byte_30 = byte_31 = 0;

	for( count = 0; count < 0x20; ++count ) {
		mMemory[ 0x5CE6 + count ] = byte_30;
		mMemory[ 0x5D06 + count ] = byte_31;

		if( (word) (byte_30 + 0x28) > 0xFF)
			++byte_31;

		byte_30 += 0x28;
	}

    /** Copy Protection
      *
	  * Removed: This is not necessary as the original copy protection 
      * affects the various element function addresses (which are not used/needed here, and wouldnt work even if i tried)
      */
	// 0x091B
	//copyProtection();
	optionsMenuPrepare();

	if( mMemory[ 0x839 ] != 1 ) {

		for( word_30 = 0x7572, word_32 = 0x77F7; word_30 < 0x7800; word_30 += 0x1, word_32 += 0x1)
			mMemory[ word_30 ] = mMemory[ word_32 ];
		
		if( pStartLevel > -1 )
			if( !ChangeLevel( 0x10 + (pStartLevel * 4) ) )
				return;

	} else {
		// 0x953

	}

	mainLoop();
}

void cCreep::DisplayControls() {
	static bool Started = false;

	int Count = 0x30;

	if (Started == false) {
		// First 6 launchs, display the controls for longer
		if (g_Steam.GetSingletonPtr()->IncreaseStat(eStat_TimesLaunched) < 6) {
			Count = 0x60;
		}
		Started = true;
	}

	cScreenSurface Keyboard("keyboard.bmp");

	for (int i = 0; i < Count; ++i) {
		g_Window.RenderShrunk(&Keyboard);
		g_Window.FrameEnd();

		hw_IntSleep(1);
		eventProcess(true);

		KeyboardJoystickMonitor(0);
		if (mJoyButtonState)
			return;

		KeyboardJoystickMonitor(1);
		if (mJoyButtonState)
			return;
	}

	cScreenSurface Joystick("joystick.bmp");	

	for (int i = 0; i < Count; ++i) {
		g_Window.RenderShrunk(&Joystick);
		g_Window.FrameEnd();
		hw_IntSleep(1);
		eventProcess(true);

		KeyboardJoystickMonitor(0);
		if (mJoyButtonState)
			break;

		KeyboardJoystickMonitor(1);
		if (mJoyButtonState)
			break;
	}
}

// 0x7572
void cCreep::optionsMenuPrepare() {
	word byte_30;

	byte_30 = 0x400;

	// Clear Screen
	for( signed char X = 3; X >= 0; --X ) {
		for( word Y = 0; Y <= 0xFF; ++Y )
			mMemory[ byte_30 + Y ] = 0x20;

		byte_30 += 0x100;
	}

	size_t Item = 0;

	mOptionsMenuCurrentItem = 0;

	//0x759C
	for(;;) {
		if(mMenu[Item].mX == -1)
			break;

		if(mMenu[Item].mAction != -1) {

			byte X = mOptionsMenuCurrentItem;

			mMemory[0xBA00 + X + 0] = (uint8) mMenu[Item].mX;
			mMemory[0xBA00 + X + 1] = (uint8) mMenu[Item].mY;
			mMemory[0xBA00 + X + 2] = (uint8) mMenu[Item].mAction;

			mOptionsMenuCurrentItem += 4;
		}

		// 75C0
		byte X = (byte) mMenu[Item].mY;
		byte_30 = mMemory[ 0x5CE6 + X ];
		
		//
		word A = mMemory[ 0x5D06 + X ] + 4;
		byte_30 = (A << 8) + (byte_30 & 0xFF);

		//
		A = (byte_30 & 0xFF) + mMenu[Item].mX;
		byte_30 = A + (byte_30 & 0xFF00);

		for (size_t Y = 0; mMenu[Item].mText[Y] != 0 ; ++Y) {


			mMemory[byte_30 + Y] = mMenu[Item].mText[Y] & 0x3F;
		}

		++Item;
	}

	// 760A
	byte x = mMemory[ 0xBA09 ];

	byte A = mMemory[ 0x5CE6 + x ];
	byte_30 = (A & 0xFF) | (byte_30 & 0xFF00);

	A = 4 + mMemory[ 0x5D06 + x];
	byte_30 = (A << 8) | (byte_30 & 0xFF);
	byte Y = mMemory[ 0xBA08 ];
	Y -= 2;

	mMemory[ byte_30 + Y ] = 0x3E;

	x = 7;
	A = mMemory[ 0x5CE6 + x ];
	byte_30 = A & 0xFF;

	// 762F
	byte_30 = ((4 + mMemory[ 0x5D06 + x ]) << 8) | (byte_30 & 0xFF);
	
	for( byte Y = 0x17; Y < 0x1A; ++Y ) {
		mMemory[ byte_30 + Y ] |= 0x80;
	}

	mMemory[ 0x775F ] = 0x0C;
	mMemory[ 0x775E ] = 0x03;

	// Now do available castle names
	for(int curCastle = 0; curCastle < 0x18; ++curCastle) {
		word X = mOptionsMenuCurrentItem;
		cCastleInfo *castle = mCastleManager->castleInfoGet( curCastle );

		if( !castle )
			break;

		mMemory[ 0xBA00 + X ] = mMemory[ 0x775E ];
		mMemory[ 0xBA01 + X ] = mMemory[ 0x775F ];

		mMemory[ 0xBA02 + X ] = 2;

        // 
		X = mMemory[ 0x775F ];

        //
		A = mMemory[ 0x5D06 + X ] + 4;

        word_32 = mMemory[ 0x5CE6 + X ];
		word_32 = (A << 8) + (word_32 & 0xFF);

        // 
		X = mMemory[ 0x775E ];
		
        // 
		X += word_32 & 0xFF;
        
        // Castle Name destination
		word_32 = X + (word_32 & 0xFF00);
		
        // Loop the castle name, copying it to 'game memory'
		for( size_t counter = 0; counter < castle->nameGet().length(); ++counter, ++word_32 )
			mMemory[ word_32 ] = toupper(castle->nameGet()[ counter ]) & 0x3F;

		mOptionsMenuCurrentItem += 4;

		if(mMemory[ 0x775E ] == 3)
			mMemory[ 0x775E ] = 0x16;
		else {
			mMemory[ 0x775E ] = 3;
			++mMemory[ 0x775F ];

			if(mMemory[ 0x775F ] >= 0x18)
				break;
		}
	}
	
	mMemory[ 0x239A ] = mOptionsMenuCurrentItem - 4;

	mOptionsMenuCurrentItem = 0x08;
	DisableSpritesAndStopSound();
}

// 268F: 
void cCreep::StringPrint_StringInput() {
	mRestorePressed = false;

	mTextColor = mMemory[ 0x278A ];
	mTextFont = mMemory[ 0x278B ] | 0x20;
	mTextXPos = mMemory[ 0x2788 ];
	mTextYPos = mMemory[ 0x2789 ];

	mMemory[ 0x27A2 ] = 0x2D;

	for( mStrLength = mStrLengthMax; mStrLength; --mStrLength ) {
		
		StringPrint_Character();
		mTextXPos += 0x08;
	}

	// 26D0
	for(;;) {
		hw_Update();

		if( mStrLength != mStrLengthMax ) {
			++mMemory[ 0x27A3 ];

			byte X = mMemory[ 0x27A3 ] & 3;
			mMemory[ 0x27A2 ] = mMemory[ 0x27A4 + X ];
			mTextXPos = (mStrLength << 3) + mMemory[ 0x2788 ];
			StringPrint_Character();
		}

		// 26F7
		byte A = StringInput_GetKey();
		if( A == 0x80 || mInput->restoreGet() == true ) {
			if( mRestorePressed == false &&  mInput->restoreGet() != true) {

				interruptWait( 3 );
				continue;
			} else {
				// 2712
				mStrLength = 0;
				
				return;
			}
		} else {

			// 2730: Backspace
			if( A == 8 ) {

				// Draw a '-' over the last character
				if( mStrLength != mStrLengthMax ) {
					mMemory[ 0x27A2 ] = 0x2D;
					StringPrint_Character();
				}

				// Backpeddle
				if (mStrLength)
					--mStrLength;

				continue;

			} else {
				// 274F
				// Enter pushed?
				if( A == 0x0D )
					return;
				
				// Max length?
				if( mStrLength == mStrLengthMax )
					continue;
				
				// Add keypress to string
				mMemory[ 0x278E + mStrLength ] = A;
				++mStrLength;

				mMemory[ 0x27A2 ] = A;
				StringPrint_Character();
			}
		}
	}
}

// 2772:
void cCreep::StringPrint_Character() {
	mMemory[ 0x27A2 ] |= 0x80;

	mObjectPtr = 0x27A2;
	StringPrint();
}

// 27A8
byte cCreep::StringInput_GetKey() {
	byte key = mInput->keyRawGet();
	
	if( key == 0 )
		return 0x80;

	return toupper( key );
}

// 95F: 
void cCreep::DisableSpritesAndStopSound() {
	byte A = 0x20;

	// Sprite??
	for( byte X = 0; X < 8; ++X ) {
		mMemory[ 0x26 + X ] = A;
		++A;
	}

	mMemory[ 0x21 ] = 0;
	mScreen->spriteDisable();

	interruptWait( 2 );

	mPlayingSound = -1;
}

// 0B84
void cCreep::mainLoop() {

	mScreen->bitmapLoad( &mMemory[ 0xE000 ], &mMemory[ 0xCC00 ], &mMemory[ 0xD800 ], 0 );
	
	// Main Intro/Game Loop
	while(!mQuit) {
	
		if( Intro() == true )
			continue;
		
		if(mQuit)
			break;

		Game();
	}
}

// 18E4
void cCreep::screenClear() {

	mScreen->clear(0);

	// Disable all sprites
	mScreen->spriteDisable();

	// Clear screen memory
	for( word word_30 = 0xFFFF; word_30 >= 0xE000; --word_30 )
		mMemory[ word_30 ] = 0;

	for( byte Y = 0; Y != MAX_SPRITES; ++Y )
		mRoomSprites[Y].state = SPR_UNUSED;

	for( word word_30 = 0xC000; word_30 < 0xC800; word_30++ ) 
		mMemory[word_30] = 0;
	
	mObjectCount = 0;
	mScreen->bitmapRedrawSet();
}

// 13F0
void cCreep::roomLoad() {
	
	screenClear();

	word_30 = 0xC000;

	while( word_30 < 0xC800)
		mMemory[word_30++] = 0;

	byte X, A;

	// 1411
	if( mPlayerStatus[0] != 1 )
		X = 1;
	else
		X = 0;

	if( mIntro )
		A = mMenuScreenCount;
	else
		A = mMemory[ 0x7809 + X ];

	roomPtrSet( A );
	
	// Room Color
	roomSetColours( mMemory[mRoomPtr] & 0xF );

	//14AC
	// Ptr to start of room data
	mObjectPtr = readLEWord( &mMemory[mRoomPtr + 6] );

	if(mIntro)
		mObjectPtr += 0x2000;

	// Function ptr
	roomPrepare( );
}

void cCreep::roomSetColours( byte pRoomColor ) {
	// Set floor colours
	// 1438
	mMemory[ 0x6481 ] = pRoomColor;
	pRoomColor <<= 4;
	pRoomColor |= mMemory[ 0x6481 ];

	mMemory[ 0x6481 ] = pRoomColor;
	mMemory[ 0x648E ] = pRoomColor;
	mMemory[ 0x649B ] = pRoomColor;
	mMemory[ 0x65CC ] = pRoomColor;
	mMemory[ 0x65CE ] = pRoomColor;
	mMemory[ 0x6EAE ] = pRoomColor;
	mMemory[ 0x6EAF ] = pRoomColor;
	mMemory[ 0x6EB0 ] = pRoomColor;
	mMemory[ 0x6EC6 ] = pRoomColor;
	mMemory[ 0x6EC7 ] = pRoomColor;
	mMemory[ 0x6EC8 ] = pRoomColor;
	mMemory[ 0x6EDB ] = pRoomColor;
	mMemory[ 0x6EDC ] = pRoomColor;
	mMemory[ 0x6EDD ] = pRoomColor;
	mMemory[ 0x6EED ] = pRoomColor;
	mMemory[ 0x6EEE ] = pRoomColor;
	mMemory[ 0x6EEF ] = pRoomColor;
	mMemory[ 0x6EFC ] = pRoomColor;
	mMemory[ 0x6EFD ] = pRoomColor;
	mMemory[ 0x6EFE ] = pRoomColor;
	mMemory[ 0x6F08 ] = pRoomColor;
	mMemory[ 0x6F09 ] = pRoomColor;
	mMemory[ 0x6F0A ] = pRoomColor;

	//1487

	for( signed char Y = 7; Y >= 0; --Y ) {
		mMemory[ 0x6FB2 + Y ] = pRoomColor;
		mMemory[ 0x6FF5 + Y ] = pRoomColor;
		mMemory[ 0x7038 + Y ] = pRoomColor;
		mMemory[ 0x707B + Y ] = pRoomColor;
	}
	
	pRoomColor &= 0x0F;
	pRoomColor |= 0x10;
	mMemory[ 0x6584 ] = pRoomColor;

	byte A = (mMemory[ 0x649B ] & 0xF0);
    A |= 0x01;

	mMemory[ 0x659B ] = A;
	mMemory[ 0x65CD ] = A;
}

// 15E0
void cCreep::roomPrepare( word pAddress ) {
	word func = 0x01;

	if(pAddress)
		mObjectPtr = pAddress;

	while(func) {

		func = readLEWord( &mMemory[ mObjectPtr ]);
		mObjectPtr  += 2;

		switch( func ) {
			case eObjectNone:			// Finished
				return;

			case eObjectDoor:				// Doors
				obj_Door_Prepare( );
				break;

			case eObjectWalkway:			// Walkway
				obj_Walkway_Prepare( );
				break;

			case eObjectSlidingPole:		// Sliding Pole
				obj_SlidingPole_Prepare( );
				break;

			case eObjectLadder:				// Ladder
				obj_Ladder_Prepare( );
				break;

			case eObjectDoorBell:			// Doorbell
				obj_Door_Button_Prepare( );
				break;

			case eObjectLightning:			// Lightning Machine
				obj_Lightning_Prepare( );
				break;

			case eObjectForcefield:			// Forcefield
				obj_Forcefield_Prepare( );
				break;

			case eObjectMummy:				// Mummy
				obj_Mummy_Prepare( );
				break;

			case eObjectKey:				// Key
				obj_Key_Prepare( );
				break;

			case eObjectLock:				// Lock
				obj_Door_Lock_Prepare( );
				break;

			case eObjectRayGun:				// Ray Gun
				obj_RayGun_Prepare( );
				break;

			case eObjectTeleport:			// Teleport
				obj_Teleport_Prepare( );
				break;

			case eObjectTrapDoor:			// Trap Door
				obj_TrapDoor_Prepare( );
				break;

			case eObjectConveyor:			// Conveyor
				obj_Conveyor_Prepare( );
				break;

			case eObjectFrankenstein:		// Frankenstein
				obj_Frankie_Prepare( );
				break;

			case eObjectText:		// String Print
			case eObjectIntroText:
				obj_stringPrint();
				break;

			case eObjectImage:
				obj_Image_Draw();
				break;

			case eObjectMultiDraw:
			case eObjectIntroMultiDraw:
				obj_MultiDraw( );
				break;

			default:
				cout << "roomPrepare: 0x";
				cout << std::hex << func << "\n";

				break;
		}

	}

}

// B8D
bool cCreep::Intro() {

	mCastleChanged = 0;
	mMenuMusicScore = 0;
	mMenuScreenTimer = 3;
	mMenuScreenCount = 0;
	mIntro = true;
	byte_D10 = 0;

    // The intro screen loop
	do {
		++mMenuScreenTimer;
		
		mMenuScreenTimer &= 3;

		if( mMenuScreenTimer != 0 ) {
			++mMenuScreenCount;
			roomPtrSet( mMenuScreenCount );

			// Final Room?
			if (((mMemory[mRoomPtr]) & 0x40)) {

				g_Steam.SetAchievement(eAchievement_IntroWatched);

				mMenuScreenCount = 0;
			}

			roomLoad();
		} else {
			mObjectPtr = 0x0D1A;
			memcpy( &mMemory[0x0D1A], mRoomIntroData, sizeof mRoomIntroData );

			string CastleName = mCastle->nameGet(true);

			size_t NameSize = mCastle->nameGet().size();
			if (NameSize > 20)
				NameSize = 20;
			
			if (NameSize < 20) {
				size_t Max = (20 / 2) - (mCastle->nameGet().size() / 2);

				while (NameSize < mCastle->nameGet().size() + Max) {

					CastleName.insert(CastleName.begin(), ' ');
					NameSize = CastleName.size();
				}
			}

			memcpy( &mMemory[0x0D1A + 64], CastleName.c_str(), NameSize);

			screenClear();
			roomPrepare( );
		}

		// 0BE1 
		if( mMusicPlaying != 1 ) {
		
			if( !mMenuScreenTimer ) {
				if( mMenuMusicScore == 0 )
					++mMenuMusicScore;
				else {
					g_Steam.SetAchievement(eAchievement_IntroMusicPlayed);

					// 0C49
					musicChange();
					mMusicPlaying = 1;
				}
			}
		}

		// Wait for input, or timeout (continues to next screen)
		for(byte byte_D12 = 0xC8; byte_D12; --byte_D12) {

			if( mMenuScreenTimer )
				events_Execute();
			else {
				// C0D
				interruptWait( 2 );
			}

			hw_Update();

			// C17
			KeyboardJoystickMonitor( byte_D10 );
			if( mJoyButtonState )
				break;

			// Quit the game if restore (Escape) is pressed
			if( mRestorePressed ) {
				mRestorePressed = false;
				mQuit = true;
				return false;
			}

			// Change which player controller is checked
			byte_D10 ^= 0x01;

			// Display Keymap
			if (mInput->f2Get()) {

				DisplayControls();
			}

			// Display Highscores on F3
			if( mInput->f3Get() ) {

				DisableSpritesAndStopSound();
				gameHighScores();

				mObjectPtr = 0x239C;
				obj_stringPrint();

				mStrLengthMax = 0;
				StringPrint_StringInput();

				return true;
			}

			if( mRunStopPressed == true ) {

				optionsMenu();

				if( mSaveGameLoaded == 1 ) {
					mJoyButtonState = 1;
					break;
				}

				if (mCastleChanged == 1) {
					mMenuScreenTimer = 3;
					mMenuScreenCount = 0;
					mJoyButtonState = 0;
					break;
				}

				return true;
			}
		}

	} while( !mJoyButtonState );

	// 0CDD
	mMusicPlaying = 0;
	mIntro = false;
	mMusicBuffer = 0;

	// Disable music playback
	mSound->playback( false );

	for(signed char X = 0x0E; X >= 0; X -= 0x07) {

		mMemory[ 0x20EF + X ] &= 0xFE;
		mMemory[ 0xD404 + X ] = mMemory[ 0x20EF + X ];
		mSound->sidWrite(0x04 + X, mMemory[ 0x20EF + X ]);
	}

	eventProcess( true );
	return false;
}

// 20A9 : 
void cCreep::musicPtrsSet() {
	byte X = mMemory[ 0x20CB ] & 3;
	
	mVoiceNum = X;

	X <<= 1;

	mVoice = readLEWord( &mMemory[ 0x20DF + X]);
	mVoiceTmp = readLEWord( &mMemory[ 0x20E5 + X]);
}

// 1F29 : Play Music Buffer
bool cCreep::musicBufferFeed() {
	bool done = false;
	
	if( mMemory[ 0x20DC ] == 0 ) {
		if( mMemory[ 0x20DD ] == 0 )
			goto musicUpdate;
		
		mMemory[ 0x20DD ]--;
	}

	mMemory[ 0x20DC ]--;
	if( (mMemory[ 0x20DC ] | mMemory[ 0x20DD ]) )
		return true;
	
musicUpdate:;

	for(; !done ;) {
		if(!mMusicBuffer)
			return false;

		byte A = *mMusicBuffer;
		A >>= 2;

		byte X;

		X = mMemory[ 0x20D2 + A ];

		for( signed char Y = X - 1; Y >= 0; --Y )
			mMemory[ 0x20CB + Y ] = *(mMusicBuffer + Y);

		// 1F60

		mMusicBuffer += X;

		A = mMemory[ 0x20CB ];
		A >>= 2;
		switch( A ) {
			case 0:
				musicPtrsSet();
				
				X = mMemory[ 0x20CB ] & 3;

				X = mMemory[ 0x20CC ] + mMemory[ 0x2104 + X ];
				A = mMemory[ 0x2108 + X ];

				mSound->sidWrite( (mVoiceNum * 7), A );
				mMemory[ mVoiceTmp ] = A;

				A = mMemory[ 0x2168 + X ];
				mSound->sidWrite( (mVoiceNum * 7) + 1, A );
				mMemory[ mVoiceTmp + 1] = A;
			
				A = mMemory[ mVoiceTmp + 4 ];
				A |= 1;
				mSound->sidWrite( (mVoiceNum * 7) + 4, A );
				mMemory[ mVoiceTmp + 4 ] = A;

				continue;

			case 1:
				musicPtrsSet();
				
				A = mMemory[ mVoiceTmp + 4 ];
				A &= 0xFE;

				mMemory[ mVoiceTmp + 4 ] = A;
				mSound->sidWrite( (mVoiceNum * 7) + 4, A );
				
				continue;

			case 2:
				mMemory[ 0x20DC ] = mMemory[ 0x20CC ];
				return true;

			case 3:
				mMemory[ 0x20DD ] = mMemory[ 0x20CC ];
				return true;

			case 4:
				musicPtrsSet();
				
				for( signed char Y = 2; Y < 7; ++Y ) {
					if( Y != 4 ) {
						// 1FDD
						byte A = mMemory[ 0x20CA + Y ];
						mSound->sidWrite( (mVoiceNum * 7) + Y, A );
						mMemory[ mVoiceTmp + Y ] = A;

					} else {
						// 1FE7
						byte A = mMemory[ mVoiceTmp + Y ];
						A &= 1;
						A |= mMemory[ 0x20CA + Y ];
						mSound->sidWrite( (mVoiceNum * 7) + Y, A );
						mMemory[ mVoiceTmp + Y ] = A;
					}
				}

				continue;

			case 5:
				A = mMemory[0x20CC];
				mSound->sidWrite( 0x15, A);
				mMemory[0x2100] = A;

				A = mMemory[0x20CD];
				mSound->sidWrite( 0x16, A );
				mMemory[0x2101] = A;

				X = mMemory[0x20CB] & 3;
				A = 1 << X;
				A |= mMemory[0x20CE];
				mSound->sidWrite( 0x17, A );
				mMemory[0x2102] = A;

				A = mMemory[0x2103] & 0x0F;
				A |= mMemory[0x20CD];
				mMemory[0x2103] = A;
				mSound->sidWrite( 0x18, A );

				continue;

			case 6:
				X = mMemory[ 0x20CB ] & 3;
				mMemory[ 0x2104 + X ] = mMemory[ 0x20CC ];

				continue;

			case 7:
				A = (mMemory[0x2103] & 0xF0) | mMemory[0x20CC];
				mMemory[0x2103] = A;
				
				mSound->sidWrite( 0x18, A );
				continue;

			case 8:
				A = mMemory[0x20CC];
				mMemory[0x2107] = A;
				A <<= 2;
				A |= 3;

				mTimerSet( A );
				continue;

			default:
				done = true;
				break;
		}

	}	// for
	
	if( !mIntro ) {
		mPlayingSound = -1;
		mMusicBuffer = 0;
		
	} else {
		mMusicPlaying = 0;
		mMusicBuffer = mMusicBufferStart;
		mMemory[ 0x20DD ] = 0x02;
		
		byte A = (mMemory[ 0x2102 ] & 0xF0);
		mMemory[ 0x2102 ] = A;
		mSound->sidWrite( 0x17, A );
	}

	return true;
}

// C49 : Music Change
void cCreep::musicChange() {
	byte*						MusicPtr = 0;

	mMusicCurrent[5]++;
	
	mMusicBufferSize = 0;

	while (MusicPtr == 0) {
		MusicPtr = mCastleManager->musicLoad( mMusicCurrent, mMusicBufferSize );

		// Reset to MUSIC0
		if (MusicPtr == 0)
			mMusicCurrent[5] = '0';
	};

	mMusicBuffer = MusicPtr + 4;		// Skip PRG load address, and the first 2 bytes of the real-data
	mMusicBufferStart = mMusicBuffer;

	byte A = 0;

	//0C9A
	for( signed char X = 0x0E; X >= 0; X -= 7 ) {
		A = mMemory[ 0x20EF + X ];

		A &= 0xFE;
		mMemory[ 0x20EF + X ] = A;
		mSound->sidWrite( 0x04 + X, A );
	}

	A = mMemory[ 0x2102 ] & 0xF0;
	mSound->sidWrite( 0x17, A );

	mMemory[ 0x2102 ] = A;
	mMemory[ 0x20DC ] = 0;
	mMemory[ 0x20DD ] = 0;
	mMemory[ 0x2107 ] = 0x14;
	mMemory[ 0x20DE ] = 1;

	mTimerSet((0x14 << 2) | 3);

	mSound->playback( true );
}

// 0x2233 : Intro Menu
void cCreep::optionsMenu() {
s2238:
	for( ;; ) {
		mScreen->spriteDisable();

		// 0x2257: Set the color of all text positions to WHITE
		for(word_30 = 0xD800; word_30 < 0xDC00; ++word_30 ) {
			mMemory[ word_30 ] = 1;
		}
		
		// Decode the background as text
		mScreen->bitmapRedrawSet( false );
		mScreen->drawStandardText( &mMemory[ 0x400 ], 0x1000, &mMemory[ 0xD800 ]);

        // 
        // 0x226E: Options Menu Loop
		for(;;) {
		    
			// Check for input

			interruptWait(7);

			// Do hardware update/sleep
			hw_Update();

			KeyboardJoystickMonitor(0);

			// Leave menu if restore (escape) was pressed (or enter)
			if( mRestorePressed ) {
				mRestorePressed = false;

				DisableSpritesAndStopSound();
				return;
			}

            // No joystick button?
			if( !mJoyButtonState ) {
				
				// No Movement?
				if( mJoystickInput & 0xFB )
					continue;
				
				// 22F7
				// 
				byte X = mOptionsMenuCurrentItem;
				byte Y = mMemory[ 0xBA01 + X ];
				
				word_30 = mMemory[ 0x5CE6 + Y ];
				word_30 += (mMemory[ 0x5D06 + Y ] + 4) << 8;
				
				Y = mMemory[ 0xBA00 + X ] - 2;
				
				mMemory[ word_30 + Y ] = 0x20;

				// Up or down?
				byte A = mJoystickInput;
				if(A) {
					A = mOptionsMenuCurrentItem;

					if( mOptionsMenuCurrentItem == mMemory[ 0x239A ] )
						A = 0;
					else
						A += 4;

				} else {
					// 22B5
					A = mOptionsMenuCurrentItem;
					if( A )
						A -= 4;
					else
						A = mMemory[ 0x239A ];

				}

				// 22C3
				mOptionsMenuCurrentItem = A;
				X = A;

				Y = mMemory[ 0xBA01 + X ];
				A = mMemory[ 0x5CE6 + Y ];

				word_30 = A;
				word_30 |= ((mMemory[ 0x5D06 + Y ] + 4) << 8);
				
				Y = (mMemory[ 0xBA00 + X ] - 2);

				mMemory[ word_30 + Y ] = 0x3E;
				
			} else {
				// Button Press
				// 22E5
				byte X = mOptionsMenuCurrentItem;

                // Selected Menu Option
				byte A = mMemory[ 0xBA02 + X ];
                switch( A ) {

                case eMenuAction_UnlimitedLives: {

					g_Steam.SetAchievement(eAchievement_EnabledUnlimitedLives);

                    //  22ED: Unlimited Lives Toggle
					mUnlimitedLives ^= 0xFF;

					byte Y = mMemory[ 0xBA01 + X ];
					word_30 = mMemory[ 0x5CE6 + Y ];
					word_30 |= (mMemory[ 0x5D06 + Y ] + 4) << 8;

					A = mMemory[ 0xBA00 + X ];
					A += 0x11;
					Y = A;
					for(X = 0; X < 6; ++X) {
						if( X != 2 ) {
							mMemory[word_30 + Y] ^= 0x80;
						}
						++Y;
					}
                    break;
                        }

                case eMenuAction_ExitMenu:{
                default:
                   //inputWait();
				    DisableSpritesAndStopSound();
				    return;
                       }

                case eMenuAction_ChangeLevel:{
                    X = mOptionsMenuCurrentItem;
					ChangeLevel(X);
					mCastleChanged = 1;
                    return;
                       }

                case eMenuAction_ResumeGame:{

                    gamePositionLoad();
				
                    goto s2238;
                       }

                case eMenuAction_BestTimes:{
                    if( mMemory[ 0x2399 ] == 0xFF )
						continue;

					DisableSpritesAndStopSound();
					gameHighScores();

					// Draw 'Press Enter to exit'
					mObjectPtr = 0x239C;
					obj_stringPrint();

					mStrLengthMax = 0;
					StringPrint_StringInput();
						
					goto s2238;
                       }
                }

			}// Button Pressed
			
			// Decode the background as text
			mScreen->drawStandardText( &mMemory[ 0x400 ], 0x1000, &mMemory[ 0xD800 ]);
		}
	}
}

void cCreep::menuUpdate( size_t pCastleNumber ) {

	if(mMemory[ 0x2399 ] == pCastleNumber)
		return;

	byte Y = mMemory[ 0xBA01 + pCastleNumber ];
	word_30 = mMemory[ 0x5CE6 + Y ];
	word_30 |= ((mMemory[ 0x5D06 + Y ] | 4) << 8);

	word_30 += mMemory[ 0xBA00 + pCastleNumber ];

	mMemory[ 0x24A6 ] = (byte) pCastleNumber;

	// 2466

	byte X = mMemory[ 0x2399 ];
	
	Y = mMemory[ 0xBA01 + X ];
	word_32 = mMemory[ 0x5CE6 + Y ];
	word_32 |= ((mMemory[0x5D06 + Y ] | 4) << 8);

	word_32 += mMemory[ 0xBA00 + X ];

	// 2484
	X = mMemory[ 0x24A6 ];

	for( char Y = 0x0F; Y >= 0; --Y ) {

		if( mMemory[ 0x2399 ] != 0xFF )
			mMemory[ word_32 + Y] &= 0x7F;
			
		mMemory[ word_30 + Y] |= 0x80;
	}

	mMemory[ 0x2399 ] = X;
}

// 5EFC
void cCreep::KeyboardJoystickMonitor( byte pA ) {
	byte X = 0xFF, A = 0;
	if(mNoInput)
		return;

	mRunStopPressed = false;

	sPlayerInput *input = mInput->inputGet( pA );
	
	// Pause the game, or enter the options menu
	if( mInput->runStopGet() )
		mRunStopPressed = true;

	// Kill the player(s) if the restore key is pressed
	if( mInput->restoreGet() )
		mRestorePressed = true;

	if(input) {

		if( input->mButton )
			A = 1;

		if( input->mLeft )
			X ^= 0x04;
	
		if( input->mRight )
			X ^= 0x08;

		if( input->mUp )
			X ^= 0x01;

		if( input->mDown )
			X ^= 0x02;
	}

	X &= 0x0F;
	mJoystickInput = mMemory[ 0x5F59 + X ];
	mJoyButtonState = A;
}

void cCreep::events_Execute() {
	// 2E1D
	interruptWait( 2 );

	// Get collisions from the hardware, and set them in the objects
	Sprite_Collision_Set();

	// Check for any actions to execute, 
	// including collisions, movement, general checks
	Sprite_Execute();

	// Check for any other 'background' actions to execute
	object_Execute();

	++mEngine_Ticks;
}

// 29AE: 
void cCreep::convertTimerToTime( const sPlayerTime& pTime ) {
	
	convertTimeToNumber( pTime.Seconds(), 6 );

	convertTimeToNumber( pTime.Minutes(), 3 );
	
	convertTimeToNumber( pTime.Hours(), 0 );

}

// 29D0: 
void cCreep::convertTimeToNumber(size_t pA, byte pY ) {
	byte byte_2A6B = (byte) pA;
	
	byte byte_2A6C = 0;

	byte A = byte_2A6B >> 4;
	
	for(;;) {
		byte X = A << 3;
		for(;;) {
			
			A = X;
			A &= 7;
			if( A == 7 )
				break;
			
			mMemory[ 0x736C + pY ] = mMemory[ 0x2A1B + X ];
			pY += 0x08;
			++X;
		}

		// 29FE
		++byte_2A6C;
		if( byte_2A6C == 2 )
			break;
		
		pY -= 0x37;
		A = byte_2A6B & 0x0F;
	}
}

// 2E37: 
void cCreep::Sprite_Collision_Set() {
	byte gfxSpriteCollision = 0, gfxBackgroundCollision = 0;

	vector< sScreenPiece *>				*collisions = mScreen->collisionsGet();
	vector< sScreenPiece *>::iterator	 colIT;

	for( colIT = collisions->begin(); colIT != collisions->end(); ++colIT ) {
		sScreenPiece *piece = *colIT;

		if (piece->mSprite >= 0) {
			if (piece->mPriority == ePriority_Background || piece->mSprite2 == -1) {
				// Background collision

				if (!(mRoomSprites[piece->mSprite-1].state & SPR_UNUSED))
					mRoomSprites[piece->mSprite-1].state |= SPR_COLLIDE_BACKGROUND;

			}
			if (piece->mSprite2 >= 0) {
				// Sprite collision

				if (!(mRoomSprites[piece->mSprite-1].state & SPR_UNUSED))
					mRoomSprites[piece->mSprite-1].state |= SPR_COLLIDE_SPRITE;

				if (!(mRoomSprites[piece->mSprite2-1].state & SPR_UNUSED))
					mRoomSprites[piece->mSprite2-1].state |= SPR_COLLIDE_SPRITE;
			}
		}

	}
}

// 2E79: Execute any objects with actions / collisions, enable their sprites
void cCreep::Sprite_Execute( ) {
	byte  A;
	byte w30 = 0;

	for(byte spriteNumber = 0 ; spriteNumber < MAX_SPRITES; ++spriteNumber ) {

		A = mRoomSprites[spriteNumber].state;

		if(! (A & SPR_UNUSED) ) {
			// 2E8B
			if(! (A & SPR_ACTION_DESTROY) ) {
				
				if(! (A & SPR_ACTION_FLASH) ) {
					--mRoomSprites[spriteNumber].Sprite_field_5;

					if( mRoomSprites[spriteNumber].Sprite_field_5 != 0 ) {
						if(A & SPR_COLLIDE_SPRITE) {
							Sprite_Collision_Check(spriteNumber);

							A = mRoomSprites[spriteNumber].state;
							if((A & SPR_ACTION_FLASH))
								goto s2EF3;
						}
						
						continue;
					} else {
					    // 2EAD
						if(A & SPR_ACTION_DIEING) 
							goto s2EF3;

						if(A & SPR_COLLIDE_BACKGROUND) {
							Sprite_Object_Collision_Check( spriteNumber );
							if( mRoomSprites[spriteNumber].state & SPR_ACTION_FLASH)
								goto s2EF3;
						}
						// 2EC2
						if(!(mRoomSprites[spriteNumber].state & SPR_COLLIDE_SPRITE))
							goto s2ED5;

						Sprite_Collision_Check( spriteNumber );
						if( mRoomSprites[spriteNumber].state & SPR_ACTION_FLASH)
							goto s2EF3;

						goto s2ED5;
					}

				} else {
					// 2EF3
s2EF3:
					Sprite_FlashOnOff( spriteNumber );
				}

			} else {
				// 2ED5
s2ED5:
				Sprite_Execute_Action( spriteNumber );
				if( mRoomSprites[spriteNumber].state & SPR_ACTION_FLASH )
					goto s2EF3;
			}
			// 2EF6
			if( mRoomSprites[spriteNumber].state & SPR_ACTION_DESTROY )
				goto s2ED5;

			cSprite *sprite = mScreen->spriteGet( spriteNumber );

			if( mRoomSprites[spriteNumber].state & SPR_ACTION_FREE ) {
				mRoomSprites[spriteNumber].state = SPR_UNUSED;
				goto s2F51;
			} else {
				// 2F16
				word_30 = mRoomSprites[spriteNumber].mX;
				word_30 <<= 1;

				w30 = (word_30 & 0xFF00) >> 8;

				// Sprite X
				mMemory[ 0x10 + spriteNumber ] = (word_30 - 8);
				sprite->mX = (word_30 - 8);

				if((word_30 >= 0x100) && ((word_30 - 8) < 0x100))
					--w30;

				if( w30 & 0x80 ) {
s2F51:;
					// 2f51
					sprite->_rEnabled = false;

				} else {
					if( w30 ) {
						A = (mMemory[ 0x20 ] | mMemory[ 0x2F82 + spriteNumber ]);
					} else
						A = (mMemory[ 0x2F82 ] ^ 0xFF) & mMemory[ 0x20 ];

					// Sprites X Bit 8
					mMemory[ 0x20 ] = A;

					// 2F45
					if((A & mMemory[ 0x2F82 + spriteNumber ]) && (mMemory[ 0x10 + spriteNumber ] >= 0x58) && w30 ) {
						sprite->_rEnabled = false;
					} else {
						// 2F5B
						sprite->mY = mRoomSprites[spriteNumber].mY + 0x32;
						sprite->_rEnabled = true;
					}

				}

				// 2F69
				mRoomSprites[spriteNumber].Sprite_field_5 = mRoomSprites[spriteNumber].Sprite_field_6;

                mScreen->spriteRedrawSet();
			}
				
		}
		// 2F72
	}

}

// 311E
void cCreep::Sprite_Object_Collision_Check( byte pSpriteNumber ) {
	word SpriteX_Start, SpriteX_Finish;
	word SpriteY_Start, SpriteY_Finish;

	SpriteX_Start = mRoomSprites[pSpriteNumber].mX;
	SpriteX_Finish = SpriteX_Start + mRoomSprites[pSpriteNumber].mCollisionWidth;
	if( (mRoomSprites[pSpriteNumber].mX +  mRoomSprites[pSpriteNumber].mCollisionWidth) > 0xFF )
		SpriteX_Start = 0;

	SpriteY_Start = mRoomSprites[pSpriteNumber].mY;
	SpriteY_Finish = SpriteY_Start + mRoomSprites[pSpriteNumber].mCollisionHeight;
	if( (mRoomSprites[pSpriteNumber].mY +  mRoomSprites[pSpriteNumber].mCollisionHeight) > 0xFF )
		SpriteY_Start = 0;

	// 3149
	if( !mObjectCount )
		return;

	for( byte ObjectNumber = 0; ObjectNumber < mObjectCount; ++ObjectNumber ) {

		if( !(mRoomAnim[ObjectNumber].mFlags & ITM_DISABLE ))

			// SpriteX >= Room Object
			if( (SpriteX_Finish >= mRoomAnim[ObjectNumber].mX ))

				if( (mRoomAnim[ObjectNumber].mX + mRoomAnim[ObjectNumber].mWidth >= SpriteX_Start))

					if( (SpriteY_Finish >= mRoomAnim[ObjectNumber].mY) )

						if( (mRoomAnim[ObjectNumber].mY + mRoomAnim[ObjectNumber].mHeight >= SpriteY_Start) ) {
							//318C
							if( Sprite_Object_Collision( pSpriteNumber, ObjectNumber ) == true )
								mRoomSprites[pSpriteNumber].state |= SPR_ACTION_FLASH;
							

							Sprite_Object_Infront_Execute( pSpriteNumber, ObjectNumber );
						} 

	}
}

bool cCreep::Sprite_Object_Collision( byte pSpriteNumber, byte pObjectNumber ) {

	switch( mRoomSprites[pSpriteNumber].mSpriteType ) {
		case SPRITE_TYPE_LIGHTNING:
		case SPRITE_TYPE_FORCEFIELD:
			return false;

		case SPRITE_TYPE_PLAYER:
			return obj_Player_Collision( pSpriteNumber, pObjectNumber );

		case SPRITE_TYPE_MUMMY:
			return obj_Mummy_Collision( pSpriteNumber, pObjectNumber );

		case SPRITE_TYPE_LASER:
			return obj_RayGun_Laser_Collision( pSpriteNumber, pObjectNumber );

		case SPRITE_TYPE_FRANKIE:
			return obj_Frankie_Collision( pSpriteNumber, pObjectNumber );


		default:
			cout << "Sprite_Object_Collision: SpriteType 0x";
			cout << std::hex << mRoomSprites[pSpriteNumber].mSpriteType << "\n";
			break;

	}

	return true;
}

bool cCreep::Sprite_Object_Infront_Execute( byte pSpriteNumber, byte pObjectNumber ) {

	switch( mRoomAnim[pObjectNumber].mObjectType ) {
		default:
			return false;
		
		case OBJECT_TYPE_DOOR:		// In Front Door
			obj_Door_InFront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_DOOR_BUTTON:				// In Front Button
			obj_Door_Button_InFront( pSpriteNumber, pObjectNumber );
			break;
		
		case OBJECT_TYPE_LIGHTNING_CONTROL:		// In Front Lightning Switch
			obj_Lightning_Switch_InFront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_FORCEFIELD:				// In Front Forcefield Timer
			obj_Forcefield_Timer_InFront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_MUMMY:					// In Front Mummy Release
			obj_Mummy_Infront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_KEY:						// In Front Key
			obj_Key_Infront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_DOOR_LOCK:				// In Front Lock
			obj_Door_Lock_InFront( pSpriteNumber, pObjectNumber );
			break;
		
		case OBJECT_TYPE_RAYGUN_CONTROL:			// In Front RayGun Control
			obj_RayGun_Control_InFront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_TELEPORTER:				// In Front Teleport
			obj_Teleport_InFront( pSpriteNumber, pObjectNumber );
			break;
		
		case OBJECT_TYPE_CONVEYOR_BELT:			// In Front Conveyor
			obj_Conveyor_InFront( pSpriteNumber, pObjectNumber );
			break;

		case OBJECT_TYPE_CONVEYOR_CONTROL:		// In Front Conveyor Control
			obj_Conveyor_Control_InFront( pSpriteNumber, pObjectNumber );
			break;
	}

	return true;
}

// 30D9
void cCreep::Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 ) {
	
	if( mRoomSprites[pSpriteNumber].state & SPR_ACTION_DIEING )
		return;

	bool StartSpriteFlash = true;

	switch( mRoomSprites[pSpriteNumber].mSpriteType ) {

		case SPRITE_TYPE_LASER:
			break;

		case SPRITE_TYPE_LIGHTNING:
		case SPRITE_TYPE_FORCEFIELD:
			return;

		case SPRITE_TYPE_PLAYER:			//  Hit Player

			if (mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_LIGHTNING)
				g_Steam.SetAchievement(eAchievement_Death_Voltage);

			if (mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_LASER)
				g_Steam.SetAchievement(eAchievement_Death_Laser);

			if (mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_MUMMY)
				g_Steam.SetAchievement(eAchievement_Death_Mummy);

			if (mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_FRANKIE)
				g_Steam.SetAchievement(eAchievement_Death_Frankie);

			StartSpriteFlash = obj_Player_Sprite_Collision( pSpriteNumber, pSpriteNumber2 );
			break;

		case SPRITE_TYPE_MUMMY:				// Hit Mummy
			StartSpriteFlash = obj_Mummy_Sprite_Collision( pSpriteNumber, pSpriteNumber2 );
			break;

		case SPRITE_TYPE_FRANKIE:			//  Hit Frankie
			StartSpriteFlash = obj_Frankie_Sprite_Collision( pSpriteNumber, pSpriteNumber2 );
			break;

		default:
			cout << "Sprite_Collision: SpriteType 0x";
			cout << std::hex << mRoomSprites[pSpriteNumber].mSpriteType << "\n";
			break;
	}

	// 3104
	if( StartSpriteFlash == false )
		return;

	mRoomSprites[pSpriteNumber].state |= SPR_ACTION_FLASH;
}

// 3026
void cCreep::Sprite_Collision_Check( byte pSpriteNumber ) {
	word SpriteY_Bottom, SpriteX, SpriteX_Right;
	word SpriteY;

	byte HitData = mObjectCollisionData[mRoomSprites[pSpriteNumber].mSpriteType].mHitData;

	if(!(HitData & 0x80)) {
		SpriteX = mRoomSprites[pSpriteNumber].mX;
		SpriteX_Right = SpriteX + mRoomSprites[pSpriteNumber].mCollisionWidth;
		if( (SpriteX + mRoomSprites[pSpriteNumber].mCollisionWidth) > 0x100 )
			SpriteX = 0;

		SpriteY = mRoomSprites[pSpriteNumber].mY;
		SpriteY_Bottom = SpriteY + mRoomSprites[pSpriteNumber].mCollisionHeight;
		if( (SpriteY + mRoomSprites[pSpriteNumber].mCollisionHeight) > 0x100 )
			SpriteY = 0;

		for(byte SpriteNumber = 0; SpriteNumber < MAX_SPRITES; ++SpriteNumber ) {

			// 3068
			if( pSpriteNumber == SpriteNumber )
				continue;

			if( !(mRoomSprites[SpriteNumber].state & SPR_UNUSED) ) {
					
				if( (mRoomSprites[SpriteNumber].state & SPR_COLLIDE_SPRITE) ) {
					byte A = mMemory[ 0x895 + (mRoomSprites[SpriteNumber].mSpriteType << 3) ];

					if( !(A & 0x80 )) {
						if(! (A & HitData )) {

							// 308E
							if( SpriteX_Right >= mRoomSprites[SpriteNumber].mX ) {
								A = mRoomSprites[SpriteNumber].mX + mRoomSprites[SpriteNumber].mCollisionWidth;

								if( A >= SpriteX ) {
									// 30A5
									if( SpriteY_Bottom >= mRoomSprites[SpriteNumber].mY ) {
										if( (mRoomSprites[SpriteNumber].mY + mRoomSprites[SpriteNumber].mCollisionHeight) >= SpriteY ) {
												
											Sprite_Collision( pSpriteNumber, SpriteNumber);
											Sprite_Collision( SpriteNumber, pSpriteNumber );
										}
									}
								}
							}

						}
					}

				}	// COLLIDES
			}	// FREE
		}
	}
	// 30D5
}

//2F8A
void cCreep::Sprite_FlashOnOff( byte pSpriteNumber ) {
	byte state = mRoomSprites[pSpriteNumber].state;

	mScreen->spriteRedrawSet();
	cSprite *sprite = mScreen->spriteGet( pSpriteNumber );

	if( !(state & SPR_ACTION_FLASH) ) {
		
		// Flashing during death
		if( !(mRoomSprites[pSpriteNumber].Sprite_field_8 )) {
			state = mRoomSprites[pSpriteNumber].state ^ SPR_ACTION_DIEING;
		} else
			goto s2FE9;
	
	} else {
		// 2FA3

		// Start to die
		state ^= SPR_ACTION_FLASH;

		mRoomSprites[pSpriteNumber].state = state;

		// 
		if(!( mObjectCollisionData[mRoomSprites[pSpriteNumber].mSpriteType].mFlashData & SPRITE_FLASH_UNK))
			state = mRoomSprites[pSpriteNumber].state;
		else
			goto s2FC4;
	}

	// 2FBB
	mRoomSprites[pSpriteNumber].state = state | SPR_ACTION_DESTROY;
	return;

s2FC4:;
	mRoomSprites[pSpriteNumber].Sprite_field_8 = 8;

	// Sprite multicolor mode
	sprite->_rMultiColored = false;
	
	mRoomSprites[pSpriteNumber].state |= SPR_ACTION_DIEING;
	mRoomSprites[pSpriteNumber].Sprite_field_6 = 1;

s2FE9:;
	if(! (mEngine_Ticks & 1) ) {
		// Flash On
		// 2FF0
		sprite->_color = 1;

		--mRoomSprites[pSpriteNumber].Sprite_field_8;

		mMemory[ 0x760C ] = mRoomSprites[pSpriteNumber].Sprite_field_8 << 3;
		sound_PlayEffect(SOUND_SPRITE_FLASH);
		
	} else {
		// Flash Off
		// 3010
		sprite->_color = 0;
	}

	// 301C
	mRoomSprites[pSpriteNumber].Sprite_field_5 = mRoomSprites[pSpriteNumber].Sprite_field_6;
}

// Originally this was not a function, but its too big to bother
// implementing in the original location
void cCreep::Sprite_Execute_Action( byte pSpriteNumber ) {

	//2ED5
	switch( mRoomSprites[pSpriteNumber].mSpriteType ) {

		case SPRITE_TYPE_PLAYER:
			obj_Player_Execute( pSpriteNumber );
			break;

		case SPRITE_TYPE_LIGHTNING:
			obj_Lightning_Execute( pSpriteNumber );
			break;

		case SPRITE_TYPE_FORCEFIELD:
			obj_Forcefield_Execute( pSpriteNumber );
			break;
		
		case SPRITE_TYPE_MUMMY:
			obj_Mummy_Execute( pSpriteNumber );
			break;

		case SPRITE_TYPE_LASER:
			obj_RayGun_Laser_Execute( pSpriteNumber );
			break;

		case SPRITE_TYPE_FRANKIE:
			obj_Frankie_Execute( pSpriteNumber );
			break;

		default:
			cout << "Sprite_Execute_Action: 0x";
			cout << std::hex << mRoomSprites[pSpriteNumber].mSpriteType << "\n";
			break;
	}

}

void cCreep::obj_Player_Execute( byte pSpriteNumber ) {
	byte A =  mRoomSprites[ pSpriteNumber ].state;

	// Player leaving room?
	if( A & SPR_ACTION_DESTROY ) {
		A ^= SPR_ACTION_DESTROY;
		A |= SPR_ACTION_FREE;
		mRoomSprites[ pSpriteNumber ].state = A;

		char Y = mRoomSprites[ pSpriteNumber ].playerNumber << 1;
		
		time_t diffSec;
		timeb timeNow;
		ftime(&timeNow);

		sPlayerTime *Time = 0;

		// Player 1/2 Time management
		if( Y == 0 ) {
			diffSec = timeNow.time - mPlayer1Time.time;
			diffSec -= mPlayer1SecondsPause;
			mPlayer1SecondsPause = 0;
			Time = &mPlayersTime[0];

			writeLEWord( &mMemory[0x7855], (uint16_t) (Time->mSeconds + diffSec));
		}
		if( Y == 2 ) {
			diffSec = timeNow.time - mPlayer2Time.time;
			diffSec -= mPlayer2SecondsPause;
			mPlayer2SecondsPause = 0;
			Time = &mPlayersTime[1];

			writeLEWord(&mMemory[0x7857], (uint16_t) (Time->mSeconds + diffSec));
		}

		Time->AddSeconds((size_t) diffSec);
		return;
	} 

	//322C
	// Player entering room?
	if( A & SPR_ACTION_CREATED ) {
		A ^= SPR_ACTION_CREATED;
		mRoomSprites[ pSpriteNumber ].state = A;

		// Current Player
		char Y = mRoomSprites[ pSpriteNumber ].playerNumber << 1;

		if( Y == 0 )
			ftime(&mPlayer1Time);

		if( Y == 2 )
			ftime(&mPlayer2Time);

		Y = mRoomSprites[ pSpriteNumber ].playerNumber;
		A = mMemory[ 0x780D + Y ];
		if( A != 6 ) {
			obj_Player_Color_Set( pSpriteNumber );
			goto s32DB;
		}

	} else {
		// 3269
		byte Y = mRoomSprites[ pSpriteNumber ].playerNumber;
		A = mMemory[ 0x780D + Y ];

		if( A == 5 ) {
			//3280
			mPlayerExecutingSpriteNumber = Y;
			Y = mRoomSprites[ pSpriteNumber ].Sprite_field_1B;
			A = mMemory[ 0x34A4 + Y ];

			if( A != 0xFF ) {
				Y = mPlayerExecutingSpriteNumber;
				mMemory[ 0x780D + Y ] = A;
				mRoomSprites[ pSpriteNumber ].Sprite_field_6 = 1;
				A = mMemory[ 0x780D + Y ];
				goto s32CB;

			} else {
				// 329E
				mRoomSprites[ pSpriteNumber ].Sprite_field_1B += 0x04;
				Y = mRoomSprites[ pSpriteNumber ].Sprite_field_1B;

				mRoomSprites[ pSpriteNumber ].mX += (int8_t) mMemory[ 0x34A1 + Y ];
				mRoomSprites[ pSpriteNumber ].mY += (int8_t) mMemory[ 0x34A2 + Y ];
			}

		} else if( A == 6 ) {
			mMemory[ 0x780D + Y ] = 5;
		} else
			goto s32CB;
	}
	// 32BC
	mRoomSprites[ pSpriteNumber ].spriteImageID = mMemory[ 0x34A3 + mRoomSprites[pSpriteNumber].Sprite_field_1B ];
	obj_Player_Color_Set( pSpriteNumber );
	return;
s32CB:;
	
	if( A != 0 ) {
		mRoomSprites[pSpriteNumber].state |= SPR_ACTION_DESTROY;
		return;
	}

s32DB:;
	A = mRoomSprites[pSpriteNumber].Sprite_field_1A;
	char a = A;
	if( A != 0xFF )
		if( A != mRoomSprites[pSpriteNumber].Sprite_field_19 ) {
			obj_TrapDoor_Switch_Check( a );
			A = a;
		}

	mRoomSprites[pSpriteNumber].Sprite_field_19 = A;
	mRoomSprites[pSpriteNumber].Sprite_field_1A = 0xFF;

	positionCalculate( pSpriteNumber );
	
	byte byte_34D5 = mMemory[ word_3C ] & mRoomSprites[pSpriteNumber].Sprite_field_18;
	//32FF
	mRoomSprites[pSpriteNumber].Sprite_field_18 = 0xFF;

	if( byte_5FD8 != 0 ) {
		// 3309
		if( !(byte_34D5 & 0x11) ) {
			byte_34D5 &= 0xBB;	
			if( byte_5FD8 >> 1 != byte_5FD7 )
				byte_34D5 &= 0x77;
			else
				byte_34D5 &= 0xDD;
		}

	} else {
		// 3337
		A = byte_5FD7;
		if( A == 3 ) {
			word_3C -= 0x4E;

			byte_34D5 &= 0x75;
			byte_34D5 |= mMemory[ word_3C ] & 2;

		} else {
			// 3360
			if( A == 0 ) {
				word_3C -= 0x52;

				byte_34D5 &= 0x5D;
				byte_34D5 |= mMemory[ word_3C ] & 0x80;

			} else {
				// 3386
				byte_34D5 &= 0x55;
			}
		}
	}
	// 338E
	KeyboardJoystickMonitor( mRoomSprites[pSpriteNumber].playerNumber );
	mRoomSprites[pSpriteNumber].mButtonState = mJoyButtonState;
	mRoomSprites[pSpriteNumber].Sprite_field_1E = mJoystickInput;
	
	byte Y = mJoystickInput;
	if( !(Y & 0x80 )) {

		if( mMemory[ 0x2F82 + Y ] & byte_34D5 ) {
			mRoomSprites[pSpriteNumber].Sprite_field_1F = Y;
			goto s33DE;

		} 

		// 33B2
		A = mRoomSprites[pSpriteNumber].Sprite_field_1F;

		if(!( A & 0x80 )) {
			A += 1;
			A &= 7;
			if( A != mJoystickInput ) {
				A -= 2;
				A &= 7;
				if( A != mJoystickInput )
					goto s33D6;
			}
			
			if( mMemory[ 0x2F82 + mRoomSprites[pSpriteNumber].Sprite_field_1F ] & byte_34D5 )
				goto s33DE;
		}
	}
s33D6:;
	// 33D6
	mRoomSprites[pSpriteNumber].Sprite_field_1F = 0x80;
	return;

	// Player Input
	// 33DE
s33DE:;
	A = (mRoomSprites[pSpriteNumber].Sprite_field_1F & 3);

	if( A == 2 ) {
		mRoomSprites[pSpriteNumber].mY -= (int8_t) byte_5FD8;

	} else {
		// 33F4
		if( A == 0 ) {
			mRoomSprites[pSpriteNumber].mX -= byte_5FD7;
			++mRoomSprites[pSpriteNumber].mX;
		}
	}
	// 3405
	Y = mRoomSprites[pSpriteNumber].Sprite_field_1F;
	mRoomSprites[pSpriteNumber].mX += (int8_t) mMemory[ 0x34D7 + Y ];
	mRoomSprites[pSpriteNumber].mY += (int8_t) mMemory[ 0x34DF + Y ];

	if( !(Y & 3) ) {
		// 3421
		if( byte_34D5 & 1 ) {
			if( !mRoomSprites[pSpriteNumber].Sprite_field_1F )
				++mRoomSprites[pSpriteNumber].spriteImageID;
			else 
				--mRoomSprites[pSpriteNumber].spriteImageID;
			
			// 3436
			// Ladder Movement 
			A = mRoomSprites[pSpriteNumber].spriteImageID;
			if( A >= 0x2E ) {
				// 3445
				// Moving Up Ladder
				if( A >= 0x32 )
					mRoomSprites[pSpriteNumber].spriteImageID = 0x2E;
			} else {
				// 343D
				// Moving Down Ladder
				mRoomSprites[pSpriteNumber].spriteImageID = 0x31;
			}

		} else {
			//3451
			// Down Pole
			mRoomSprites[pSpriteNumber].spriteImageID = 0x26;
		}

	} else {
		// 3459
		// Player Frame
		++mRoomSprites[pSpriteNumber].spriteImageID;

		if( mRoomSprites[pSpriteNumber].Sprite_field_1F < 4 ) {
			// 3463
			A = mRoomSprites[pSpriteNumber].spriteImageID;
			if( A >= 6 || A < 3 )
				mRoomSprites[pSpriteNumber].spriteImageID = 3;

		} else {
			// 3476
			// Max frame reached?
			if( mRoomSprites[pSpriteNumber].spriteImageID >= 3 )
				mRoomSprites[pSpriteNumber].spriteImageID = 0;
		}
	}

	// 3482
	obj_Player_Color_Set( pSpriteNumber );

}

// 3488: 
void cCreep::obj_Player_Color_Set( byte pSpriteNumber ) {
	hw_SpritePrepare( pSpriteNumber );

	mPlayerExecutingSpriteNumber = pSpriteNumber;

	cSprite *sprite = mScreen->spriteGet( pSpriteNumber );

	sprite->_color = mMemory[ 0x34D3 + mRoomSprites[pSpriteNumber].playerNumber ];
	mScreen->spriteRedrawSet();
}

// 3AEB: Frankie Movement
void cCreep::obj_Frankie_Execute( byte pSpriteNumber ) {
	byte Y;
	char A = mRoomSprites[pSpriteNumber].state;
	byte byte_3F0B, byte_3F12;
	char byte_3F0A, byte_3F10, byte_3F11;

	if( mNoInput )
		return;

	if( A & SPR_ACTION_DESTROY ) {
		mRoomSprites[pSpriteNumber].state = (A ^ SPR_ACTION_DESTROY) | SPR_ACTION_FREE;
		return;
	}

	if( A & SPR_ACTION_CREATED ) 
		mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_CREATED;

	word_40 = mFrankiePtr + mRoomSprites[pSpriteNumber].Sprite_field_1F;
	
	if( !(mRoomSprites[pSpriteNumber].Sprite_field_1E & FRANKIE_AWAKE) ) {
		if( mIntro )
			return;

		// 3B31
		for(byte_3F0A = 1; byte_3F0A >= 0; --byte_3F0A) {
			Y = byte_3F0A;

			if( mMemory[ 0x780D + Y ] != 0 )
				continue;

			Y = mMemory[ 0x34D1 + Y ];
			int16_t distanceY = mRoomSprites[pSpriteNumber].mY - mRoomSprites[Y].mY;

			// Within 4 on the Y axis, then frank can wake up
			if( distanceY >= 4 || distanceY <= -4)
				continue;

			// 3B4A
			int16_t distanceX = mRoomSprites[pSpriteNumber].mX - mRoomSprites[Y].mX;

			A = mRoomSprites[pSpriteNumber].Sprite_field_1E;

			if( !(distanceX < 0)) {
				// We are behind frank
				if( !(A & FRANKIE_POINTING_LEFT) )
					continue;
				else
					goto s3B6E;
			}

			// 3B5E
			if( !(A & FRANKIE_POINTING_LEFT)) {
s3B6E:
				A |= FRANKIE_AWAKE;
				mRoomSprites[pSpriteNumber].Sprite_field_1E = A;
				mMemory[ word_40 ] = A;
				mRoomSprites[pSpriteNumber].mButtonState = 0x80;

				sound_PlayEffect( SOUND_FRANKIE );
				break;
			}
		}
	
		if(byte_3F0A < 0 )
			return;
	}
	
	// 3B82
	A = mRoomSprites[pSpriteNumber].Sprite_field_1B;

	if( (byte) A != 0xFF )
		if( A != mRoomSprites[pSpriteNumber].Sprite_field_1A ) {
			obj_TrapDoor_Switch_Check( A );
		}

	mRoomSprites[pSpriteNumber].Sprite_field_1A = A;
	mRoomSprites[pSpriteNumber].Sprite_field_1B = 0xFF;
	positionCalculate( pSpriteNumber );

	//3B9C
	A = mMemory[ word_3C ] & mRoomSprites[pSpriteNumber].playerNumber;
	byte byte_3F13 = A;

	mRoomSprites[pSpriteNumber].playerNumber = 0xFF;
	A = byte_3F13;
	if(!A) {
		mRoomSprites[pSpriteNumber].mButtonState = 0x80;
		goto s3CB4;
	} else {
		byte_3F0A = 0;
		
		// 3BBF
		for(char Y = 6; Y >= 0;) {
			if( (mMemory[ 0x2F82 + Y ] & byte_3F13 )) {
				++byte_3F0A;
				byte_3F0B = Y;
			}

			Y -= 2;
		}
	
		// 3BD1
		if( byte_3F0A == 1 ) {
			mRoomSprites[pSpriteNumber].mButtonState = byte_3F0B;
			goto s3CB4;
		}
		if( byte_3F0A == 2 ) {
			byte Y = (byte_3F0B - 4) & 7;
			if( mMemory[ 0x2F82 + Y ] & byte_3F13 ) {
				Y = mRoomSprites[pSpriteNumber].mButtonState;
				if( !(Y & 0x80 ))
					if( mMemory[ 0x2F82 + Y ] & byte_3F13 )
						goto s3CB4;
			}
		}
		// 3C06
		for(char Y = 3; Y >= 0; --Y) {
			mMemory[ 0x3F0C + Y ] = 0xFF;
		}

		// 3C15
		for(byte_3F0A = 1; byte_3F0A>=0; --byte_3F0A) {

			byte Y;
			if( mMemory[ 0x780D + byte_3F0A ] == 0 ) {
				
				Y = mMemory[ 0x34D1 + byte_3F0A ];
				int16_t Pos = mRoomSprites[Y].mX - mRoomSprites[pSpriteNumber].mX;
				//3C2A
				if( Pos < 0 ) {
					Pos ^= 0xFF;
					++Pos;
					Y = 3;
				} else {
					Y = 1;
				}
				if(Pos < mMemory[ 0x3F0C +  Y ] )
					mMemory[ 0x3F0C + Y ] = (int8_t) Pos;

				Y = mMemory[ 0x34D1 + byte_3F0A ];
				Pos = mRoomSprites[Y].mY - mRoomSprites[pSpriteNumber].mY;

				if(Pos < 0 ) {
					Pos ^= 0xFF;
					++Pos;
					Y = 0;
				} else {
					Y = 2;
				}
				if(Pos < mMemory[ 0x3F0C + Y ] )
					mMemory[ 0x3F0C + Y ] = (int8_t) Pos;
			}
			// 3C62
		}

		// 3C67
		byte_3F10 = -1;
		for(;;) {
			byte_3F11 = 0x00;
			byte_3F12 = 0xFF;
			
			for( signed char Y = 3; Y >= 0; --Y ) {
				A = mMemory[ 0x3F0C + Y ];
				if( (byte) A >= (byte) byte_3F10 )
					continue;
				if( (byte) A < (byte) byte_3F11 )
					continue;

				byte_3F11 = A;
				byte_3F12 = Y;
			}
		
			//3C8E
			A = byte_3F12;
			if( A == -1 ) {
				mRoomSprites[pSpriteNumber].mButtonState = 0x80;
				goto s3CB4;
			}

			A = mMemory[ 0x2F82 + (byte_3F12 << 1) ];
			if( A & byte_3F13 ) {
				// 3CB0
				mRoomSprites[pSpriteNumber].mButtonState = (byte_3F12 << 1);
				break;
			}

			byte_3F10 = byte_3F11;
		}

	}

	// 3CB4
s3CB4:;
	if( mRoomSprites[pSpriteNumber].mButtonState & 2 ) {
		mRoomSprites[pSpriteNumber].mY -= (int8_t) byte_5FD8;

		++mRoomSprites[pSpriteNumber].spriteImageID;
		if( mRoomSprites[pSpriteNumber].mButtonState != 2 ) {
			// 3ccf
			--mRoomSprites[pSpriteNumber].mX;
			if( mRoomSprites[pSpriteNumber].spriteImageID >= 0x87 )
				if( mRoomSprites[pSpriteNumber].spriteImageID < 0x8A )
					goto s3D4C;
			
			mRoomSprites[pSpriteNumber].spriteImageID = 0x87;
			
		} else {
			// 3ce5
			++mRoomSprites[pSpriteNumber].mX;
			if( mRoomSprites[pSpriteNumber].spriteImageID >= 0x84 )
				if( mRoomSprites[pSpriteNumber].spriteImageID < 0x87 )
					goto s3D4C;

			mRoomSprites[pSpriteNumber].spriteImageID = 0x84;

			goto s3D4C;
		}
	} else {
		// 3CFB
		A = mRoomSprites[pSpriteNumber].mButtonState;
		if(A & 0x80)
			goto s3D4F;

		mRoomSprites[pSpriteNumber].mX -= byte_5FD7;
		++mRoomSprites[pSpriteNumber].mX;
		if( !(mMemory[ word_3C ] & 1) ) {
			// 3d15
			mRoomSprites[pSpriteNumber].spriteImageID = 0x8A;
			mRoomSprites[pSpriteNumber].mY += 2;
			goto s3D4C;
		} else {
			// 3d26
			if( !(mRoomSprites[pSpriteNumber].mButtonState) )
				mRoomSprites[pSpriteNumber].mY -= 2;
			else
				mRoomSprites[pSpriteNumber].mY += 2;

			// 3d40
			mRoomSprites[pSpriteNumber].spriteImageID = ((mRoomSprites[pSpriteNumber].mY & 0x06) >> 1) + 0x8B;
		}
	}
s3D4C:;
	// 3D4C
	hw_SpritePrepare( pSpriteNumber );

s3D4F:;
	mMemory[ word_40 + 6 ] = mRoomSprites[pSpriteNumber].mButtonState;
	mMemory[ word_40 + 3 ] = mRoomSprites[pSpriteNumber].mX;
	mMemory[ word_40 + 4 ] = (uint8) mRoomSprites[pSpriteNumber].mY;
	mMemory[ word_40 + 5 ] = mRoomSprites[pSpriteNumber].spriteImageID;
}

// 3D6E: Frankie?
bool cCreep::obj_Frankie_Collision( byte pSpriteNumber, byte pObjectNumber ) {

	uint8 A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	A -= mRoomAnim[pObjectNumber].mX;

	if( A >= 4 )
		return false;
	
	// 3d85
	if( mRoomAnim[pObjectNumber].mObjectType != OBJECT_TYPE_TRAPDOOR_PANEL ) {
		if( mRoomAnim[pObjectNumber].mObjectType != OBJECT_TYPE_TRAPDOOR_SWITCH )
			return false;

		mRoomSprites[pSpriteNumber].Sprite_field_1B = mRoomObjects[pObjectNumber].objNumber;
		return false;

	} else {
		// 3DA1
		word_40 = mRoomTrapDoorPtr + mRoomObjects[pObjectNumber].objNumber;

		if( !(mMemory[ word_40 ] & TRAPDOOR_OPEN) )
			return false;
		
		word_40 = mFrankiePtr + mRoomSprites[pSpriteNumber].Sprite_field_1F;

		A = (FRANKIE_AWAKE ^ 0xFF) & mMemory[ word_40 ];
		A |= FRANKIE_DEATH;
		mMemory[ word_40 ] = A;
		mRoomSprites[pSpriteNumber].Sprite_field_1E = A;
	}

	return true;
}

// 3DDE: Franky Hit 
bool cCreep::obj_Frankie_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 ) {
	if( mRoomSprites[pSpriteNumber].Sprite_field_1E & FRANKIE_AWAKE ) {
		byte A = mRoomSprites[pSpriteNumber2].mSpriteType;

		if( A != SPRITE_TYPE_PLAYER && A != SPRITE_TYPE_FORCEFIELD && A != SPRITE_TYPE_MUMMY ) {

			// 3DF3
			// If Frankie hits Lightning, or a laser.. he dies
			if( A != SPRITE_TYPE_FRANKIE ) {
			
				word_40 = mFrankiePtr + mRoomSprites[pSpriteNumber].Sprite_field_1F;
				mMemory[ word_40 ] = ((FRANKIE_AWAKE ^ 0xFF) & mMemory[ word_40 ]) | FRANKIE_DEATH;
				return true;

			} else {
				// 3E18
				A = mRoomSprites[pSpriteNumber].spriteImageID;
				if( A >= 0x8A && A < 0x8F ) {
					// 3E23
					A = mRoomSprites[pSpriteNumber2].spriteImageID;
					if( A < 0x8A || A >= 0x8F ) {
						return false;
					}
					// 3E2E
					if( mRoomSprites[pSpriteNumber].mY == mRoomSprites[pSpriteNumber2].mY ) {
						return false;
					}
					if( mRoomSprites[pSpriteNumber].mY < mRoomSprites[pSpriteNumber2].mY ) {
						mRoomSprites[pSpriteNumber].playerNumber &= 0xEF;
						return false;
					}
					// 3E43
					mRoomSprites[pSpriteNumber].playerNumber &= 0xFE;
					return false;
				}
				// 3E4E
				byte A =  mRoomSprites[pSpriteNumber].spriteImageID;
				byte B = mRoomSprites[pSpriteNumber2].spriteImageID;

				// 3E51
				if( A < 0x84 || A >= 0x8A || B < 0x84 || B >= 0x8A ) {
					return false;
				}
				if( mRoomSprites[pSpriteNumber].mX < mRoomSprites[pSpriteNumber2].mX ) {
					mRoomSprites[pSpriteNumber].playerNumber &= 0xFB;
					return false;
				}
				// 3E77
				mRoomSprites[pSpriteNumber].playerNumber &= 0xBF;
			}
		} 	
	}

	// 3E4B
	return false;
}			

// 3E87: 
void cCreep::obj_Frankie_Sprite_Create() {
	if( mMemory[ mObjectPtr ] & FRANKIE_DEATH )
		return;

	byte X = Sprite_CreepFindFree();

	mRoomSprites[X].mSpriteType = SPRITE_TYPE_FRANKIE;
	mRoomSprites[X].Sprite_field_1F = mFrankieCount;
	mRoomSprites[X].Sprite_field_1E = mMemory[ mObjectPtr ];

	if( !(mMemory[ mObjectPtr ] & FRANKIE_AWAKE) ) {
		mRoomSprites[X].mX= mMemory[ mObjectPtr + 1 ];
		mRoomSprites[X].mY= mMemory[ mObjectPtr + 2 ] + 7;
		mRoomSprites[X].spriteImageID= 0x8F;
	} else {
		// 3EC8
		mRoomSprites[X].mX= mMemory[ mObjectPtr + 3 ];
		mRoomSprites[X].mY= mMemory[ mObjectPtr + 4 ];
		mRoomSprites[X].spriteImageID= mMemory[ mObjectPtr + 5 ];
		mRoomSprites[X].mButtonState = mMemory[ mObjectPtr + 6 ];
	}
	// 3ee4
	mRoomSprites[X].mWidth = 3;
	mRoomSprites[X].mHeight = 0x11;
	hw_SpritePrepare(X);
	mRoomSprites[X].playerNumber = 0xFF;
	mRoomSprites[X].Sprite_field_1A = 0xFF;
	mRoomSprites[X].Sprite_field_1B = 0xFF;
	mRoomSprites[X].Sprite_field_6 = 2;
	mRoomSprites[X].Sprite_field_5 = 2;
}

void cCreep::object_Execute() {

	for(byte X = 0; X < mObjectCount; ++X ) {
		
		byte A = mRoomAnim[X].mFlags;
		if(A & ITM_EXECUTE) {

			switch( mRoomAnim[X].mObjectType ) {
				default:
					mRoomAnim[X].mFlags ^= ITM_EXECUTE;
					break;

				case OBJECT_TYPE_DOOR:
					obj_Door_Execute( X );
					break;

				case OBJECT_TYPE_LIGHTNING_MACHINE:
					obj_Lightning_Pole_Execute( X );
					break;

				case OBJECT_TYPE_FORCEFIELD:	
					obj_Forcefield_Timer_Execute( X );
					break;

				case OBJECT_TYPE_MUMMY:
					obj_Mummy_Tomb_Execute( X );
					break;

				case OBJECT_TYPE_RAYGUN_LASER:
					obj_RayGun_Execute( X );
					break;

				case OBJECT_TYPE_TELEPORTER:
					obj_Teleport_Execute( X );
					break;

				case OBJECT_TYPE_TRAPDOOR_PANEL:
					obj_TrapDoor_Switch_Execute( X );
					break;

				case OBJECT_TYPE_CONVEYOR_BELT:
					obj_Conveyor_Execute( X );
					break;
			}

			// 3F93
			A = mRoomAnim[X].mFlags;
		}
	
		// 3F96
		// Key picked up
		if( A & ITM_PICKED ) {
			roomAnim_Disable( X );

			// Decrease object count
			--mObjectCount;

			// Last object? then nothing to do
			if( X == mObjectCount )
				break;

			mRoomAnim[X] = mRoomAnim[mObjectCount];
			mRoomObjects[X] = mRoomObjects[mObjectCount];
		}

		// 3FC7
	}

	//
	
}

// 3639: 
void cCreep::obj_Lightning_Execute( byte pSpriteNumber ) {
	byte A = mRoomSprites[pSpriteNumber].state;
	if( A & SPR_ACTION_DESTROY ) {
		mRoomSprites[pSpriteNumber].state = (A ^ SPR_ACTION_DESTROY) | SPR_ACTION_FREE;
		return;
	}

	if( A & SPR_ACTION_CREATED ) {
		A ^= SPR_ACTION_CREATED;
		mRoomSprites[pSpriteNumber].state = A;
	}

	A = seedGet();
	A &= 0x03;
	mRoomSprites[pSpriteNumber].Sprite_field_6 = A;
	++mRoomSprites[pSpriteNumber].Sprite_field_6;

	A = seedGet();
	A &= 3;

	A += 0x39;
	if( A == mRoomSprites[pSpriteNumber].spriteImageID ) {
		A += 0x01;
		if( A >= 0x3D )
			A = 0x39;
	}
	// 3679
	mRoomSprites[pSpriteNumber].spriteImageID = A;
	hw_SpritePrepare( pSpriteNumber );
}

// 368A
void cCreep::obj_Lightning_Sprite_Create( byte pObjectNumber  ) {
	sCreepSprite *sprite = Sprite_CreepGetFree();

	sprite->mSpriteType = SPRITE_TYPE_LIGHTNING;

	sprite->mX = mRoomAnim[pObjectNumber].mX;
	sprite->mY = mRoomAnim[pObjectNumber].mY + 8;
	
	sprite->Sprite_field_1F = mRoomObjects[pObjectNumber].objNumber;
}

// 36B3: Forcefield
void cCreep::obj_Forcefield_Execute( byte pSpriteNumber ) {

	if(mRoomSprites[pSpriteNumber].state & SPR_ACTION_DESTROY ) {
		mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_DESTROY;

		mRoomSprites[pSpriteNumber].state |= SPR_ACTION_FREE;
		mRoomSprites[pSpriteNumber].state = mRoomSprites[pSpriteNumber].state;
		return;
	}

	if(mRoomSprites[pSpriteNumber].state & SPR_ACTION_CREATED ) 
		mRoomSprites[pSpriteNumber].state = (mRoomSprites[pSpriteNumber].state ^ SPR_ACTION_CREATED);

	if( mMemory[ 0x4750 + mRoomSprites[pSpriteNumber].Sprite_field_1F ] == 1 ) {

		if( mRoomSprites[pSpriteNumber].Sprite_field_1E != 1 ) {
			mRoomSprites[pSpriteNumber].Sprite_field_1E = 1;
			positionCalculate( pSpriteNumber );
			
			word_3C -= 2;
			mMemory[ word_3C + 0 ] = mMemory[ word_3C + 0 ] & 0xFB;
			mMemory[ word_3C + 4 ] = mMemory[ word_3C + 4 ] & 0xBF;

			mRoomSprites[pSpriteNumber].spriteImageID = 0x35;
		} else {
			// 3709
			if( mRoomSprites[pSpriteNumber].spriteImageID == 0x35 )
				mRoomSprites[pSpriteNumber].spriteImageID = 0x3D;
			else
				mRoomSprites[pSpriteNumber].spriteImageID = 0x35;
		}

	} else {
	// 371A
		if( mRoomSprites[pSpriteNumber].Sprite_field_1E != 1 )
			return;

		mRoomSprites[pSpriteNumber].Sprite_field_1E = 0;
		positionCalculate( pSpriteNumber );

		word_3C -= 2;
		mMemory[ word_3C + 0 ] |= 4;
		mMemory[ word_3C + 4 ] |= 0x40;
		mRoomSprites[pSpriteNumber].spriteImageID = 0x41;
	}

	// 3746
	// Draw the forcefield
	hw_SpritePrepare( pSpriteNumber );
}

// 5FD9
void cCreep::roomPtrSet( byte pRoomNumber ) {

	mRoomPtr = pRoomNumber << 3;
	mRoomPtr += 0x7900;

	if( mIntro )
		mRoomPtr += 0x2000;
}

bool cCreep::ChangeLevel( size_t pMenuItem ) {
	size_t castleNumber = (pMenuItem - 0x10);
	castleNumber /= 4;

	if( (mCastle = mCastleManager->castleLoad( castleNumber )) == 0)
			return false;

	menuUpdate(pMenuItem); 

	return true;
}

// 0D71: 
void cCreep::Game() {

	mMenuReturn = false;
	mPlayer1SecondsPause = 0;
	mPlayer2SecondsPause = 0;

	if( mSaveGameLoaded == 1 ) {
		// D7D
		mSaveGameLoaded = 0;
		mMemory[ 0x7802 ] |= 1;
	} else {

		// D8D
		word_30 = 0x9800;
		word_32 = 0x7800;

		// 
		memcpy( &mMemory[ word_32 ], &mMemory[ word_30 ], readLEWord( &mMemory[ 0x9800 ] ) );

		// DC6
		// Which joystick was selected when button press was detected (in intro)
		mMemory[ 0x7812 ] = byte_D10;

		// Clear Stored CIA Timers (Original Player Time)
		for( signed char Y = 7; Y >= 0; --Y )
			mMemory[ 0x7855 + Y ] = 0;

		mPlayersTime[0].Reset();
		mPlayersTime[1].Reset();

		mMemory[ 0x785D ] = mMemory[ 0x785E ] = 0;
		mMemory[ 0x7809 ] = mMemory[ 0x7803 ];	// Player1 Start Room
		mMemory[ 0x780A ] = mMemory[ 0x7804 ];	// Player2 Start Room
		mMemory[ 0x780B ] = mMemory[ 0x7805 ];	// Player1 Start Door
		mMemory[ 0x780C ] = mMemory[ 0x7806 ];	// Player2 Start Door
		mMemory[ 0x7811 ] = 0;
		mMemory[ 0x780F ] = 1;

		// E00
		if( mMemory[ 0x7812 ] != 1 ) {
			mMemory[ 0x7810 ] = 0;
			mMemory[ 0x780E ] = 4;
		} else
			mMemory[ 0x7810 ] = 1;
	}


	g_Steam.FindLeaderboard(mCastle->nameGet().c_str());

	// E19
	for(;;) {
		if( mMemory[ 0x780F ] != 1 )
			if( mMemory[ 0x7810 ] != 1 )
				break;
		// E2A
		if( mMemory[ 0x780F ] == 1 && mMemory[ 0x7810 ] == 1 ) {

			// Player1 and 2 in same room?
			if( mMemory[ 0x7809 ] != mMemory[ 0x780A ] ) {

				// No
				byte X = mMemory[ 0x7811 ];

				mPlayerStatus[X] = false;
				X ^= 0x01;
				mPlayerStatus[X] = true;

			} else {
				mPlayerStatus[0] = mPlayerStatus[1] = true;
			}

		} else {
			// E5F
			if( mMemory[ 0x780F ] != 1 ) {
				mPlayerStatus[1] = true;
				mPlayerStatus[0] = false;
			} else {
				// E73
				mPlayerStatus[0] = true;
				mPlayerStatus[1] = false;
			}
		}

		// E7D
		while( mapDisplay() );

		if(mMenuReturn) {
			mInput->inputClear();
			hw_IntSleep(1);
			return;
		}

		roomMain();
		screenClear();
		
		mMemory[ 0xF62 ] = 0;

		// E8D
		for( byte X = 0; X < 2; ++X ) {

			if( mPlayerStatus[X] == 1 ) {
				if( mMemory[ 0x780D + X ] != 2 ) {
					if( mMemory[ 0x785D + X ] != 1 )
						continue;

					mMemory[ 0x1AB2 ] = X;
					gameEscapeCastle();

					if(! (mMemory[ 0x7802 ] & 1 )) {
						
						if( mUnlimitedLives != 0xFF ) {

							mMemory[0x1CF9 + 3] = (uint8) mPlayersTime[X].Seconds();
							mMemory[0x1CF9 + 2] = (uint8) mPlayersTime[X].Minutes();
							mMemory[0x1CF9 + 1] = (uint8) mPlayersTime[X].Hours();
							mMemory[0x1CF9 + 0] = 0;
							
							mMemory[ 0x1CFD ] = X;
							gameHighScoresHandle(X);
						}
					}
					// EFC
sEFC:;
					mMemory[ 0x780F + X ] = 0;
					mMemory[ 0xF62 ] = 1;
				} else {
				// EDE
					// Player Died
					int32_t Count = g_Steam.IncreaseStat( eStat_GameOverCount );

					if (Count > 0)
						g_Steam.SetAchievement( eAchievement_DeathCount_1 );

					if (Count > 10)
						g_Steam.SetAchievement( eAchievement_DeathCount_10 );

					if (Count > 50)
						g_Steam.SetAchievement( eAchievement_DeathCount_50 );

					if (Count > 100)
						g_Steam.SetAchievement( eAchievement_DeathCount_100 );

					if( mUnlimitedLives != 0xFF ) {
						--mMemory[ 0x7807 + X ];
						byte A = mMemory[ 0x7807 + X ];
						if(A == 0)
							goto sEFC;
					}
					// Set player1/2 start room
					mMemory[ 0x7809 + X ] = mMemory[ 0x7803 + X ];
					mMemory[ 0x780B + X ] = mMemory[ 0x7805 + X ];
				}
			}
			// F06

		}

		// Game Over Check
		// F0B
		if( mMemory[ 0xF62 ] == 1 ) {
			screenClear();
			if ( mMemory[0x7807] == 0 || mMemory[0x7808] == 0 ) {

				// either player lost all lives?
				if ((mMemory[0x780F] == 0 && mMemory[0x7807] == 0) || (mMemory[0x7810] == 0 && mMemory[0x7808] == 0))
					g_Steam.SetAchievement(eAchievement_DeathCount_1Player);

				// Both players dead?
				if ((mMemory[0x780F] == 0 && mMemory[0x7807] == 0) && (mMemory[0x7810] == 0 && mMemory[0x7808] == 0))
					g_Steam.SetAchievement(eAchievement_DeathCount_2Player);
			}

			mObjectPtr = 0x0F64;		// Game Over
			obj_stringPrint();

			if( mMemory[ 0x7812 ] != 0 ) {
				if( mMemory[ 0x780F ] != 1 ) {
					mObjectPtr = 0x0F72;	// For Player
					obj_stringPrint();
				}

				// F39
				if( mMemory[ 0x7810 ] != 1 ) {
					mObjectPtr = 0x0F83;	// For Player
					obj_stringPrint();
				}

			}

			hw_Update();

			// F4B
			hw_IntSleep( 0x23 );
		}

	}
	// F5B
}

void cCreep::mapArrowDraw( byte pPlayer ) {
	sub_6009( mMemory[ 0x780B + pPlayer ] );
			
	mMemory[ 0x11D9 ] = mMemory[ word_40 + 2 ] & 3;
	
	// Object Number (saved as sprite number)
	pPlayer = Sprite_CreepFindFree( );
	mMemory[ 0x11D8 ] = pPlayer;

	cSprite *sprite = mScreen->spriteGet( pPlayer );
	
	// Calculate X
	byte A = mMemory[ mRoomPtr + 1 ];	// X
	A += mMemory[ word_40 + 5 ];

	byte Y = mMemory[ 0x11D9 ];
	A += mMemory[ 0x11DA + Y ];

	word posX = A;
	posX -= 4;
	posX <<= 1;

	// Sprite X
	sprite->mX = posX;
	mMemory[ 0x10 + Y ] = A;

	// Calculate Y
	// 100D
	A = mMemory[ mRoomPtr + 2 ];		// Y
	A += mMemory[ word_40 + 6 ];
	A += mMemory[ 0x11DE + mMemory[ 0x11D9 ] ];
	A += 0x32;

	// Sprite Y
	sprite->mY = A;
	mRoomSprites[pPlayer].spriteImageID = mMemory[ 0x11E2 + mMemory[0x11D9] ];
	
	// Enable the Arrow sprite
	hw_SpritePrepare( pPlayer );
	
	// Sprites Enabled
	sprite->_rEnabled = true;
}

// F94: Display the map/time taken screen
bool cCreep::mapDisplay() {
	byte gfxPosX, gfxPosY;

	screenClear();
	
	Sleep(300);
	eventProcess( true );

	// Draw both players Name/Time/Arrows
	// FA9
	for(mMemory[ 0x11D7 ] = 0 ; mMemory[ 0x11D7 ] != 2; ++mMemory[ 0x11D7 ] ) {

		byte X = mMemory[ 0x11D7 ];

		if( mPlayerStatus[X] == 1 ) {
			// FB6

			// Mark Room as visible on map
			byte A = mMemory[ 0x7809 + X ];
			roomPtrSet( A );
			mMemory[ mRoomPtr ] |= MAP_ROOM_VISIBLE;
			
			// 
			mapArrowDraw( X );

			// 103C
			byte Y = mMemory[ 0x11D7 ];
			X = mMemory[ 0x7807 + Y ];
			A = mMemory[ 0x11E5 + X ];

			mMemory[ 0x11EF ] = mMemory[ 0x11FA ] = A;
			X = Y << 1;
			
			// Player (One Up / Two Up)
			mObjectPtr = mMemory[ 0x11E9 + X ];
			mObjectPtr += (mMemory[ 0x11EA + X ] << 8);

			obj_stringPrint();

			// 1058
			convertTimerToTime( mPlayersTime[ Y ] );

			gfxPosX = mMemory[ 0x11D3 + Y ] + 8;
			gfxPosY = 0x10;

			// Draw Time ' : : '
			screenDraw(0, 0x93, gfxPosX, gfxPosY, 0);
		} 
		
		// 1087
	}
	
	// 1094
	mapRoomDraw();

	mMemory[ 0x11CB ] = 0;
	mMemory[ 0x11CD ] = mMemory[ 0x11CC ] = 1;
	if( mPlayerStatus[0] != 1 ) {
		if( mPlayerStatus[1] == 1 )
			mMemory[ 0x11CD ] = 0;

	} else {
		// 10BA
		mMemory[ 0x11CC ] = 0;
		if( mPlayerStatus[1] == 1 ) {
			mMemory[ 0x11CD ] = 0;
			if( mMemory[ 0x780B ] == mMemory[ 0x780C ] ) {
				mMemory[ 0x11CB ] = 1;

				mScreen->spriteGet( 0 )->_color = 1;
				mScreen->spriteGet( 1 )->_color = 1;
				goto s10EB;
			}
		}
	}

	// 10E3
	mScreen->spriteGet( 0 )->_color = 0;
	mScreen->spriteGet( 1 )->_color = 0;

s10EB:;
	mMemory[ 0x11D0 ] = 1;
	mRestorePressed = false;
	mMemory[ 0x11CE ] = mMemory[ 0x11D1 ];
	mMemory[ 0x11CF ] = mMemory[ 0x11D2 ];
	mMemory[ 0x11D7 ] = 0;
	

	// 110E
	for( ;; ) {

		byte X = mMemory[ 0x11D7 ];

		// Player Arrow Flash
		if( mMemory[ 0x11CB ] != 1 ) {
			--mMemory[ 0x11CE + X ];
			if(mMemory[ 0x11CE + X ] == 0 ) {
				// 1122
				mMemory[ 0x11CE + X ] = mMemory[ 0x11D1 + X ];
				if( X == 0 || mPlayerStatus[0] != 1 ) {
					// 1133
					mScreen->spriteGet( 0 )->_color ^= 0x01;
				} else {
					// 113E
					mScreen->spriteGet( 1 )->_color ^= 0x01;
				}
				
				mScreen->spriteRedrawSet();
			}
		}

		// 115A
		KeyboardJoystickMonitor( X );

		// 1146
		if( mRestorePressed == true ) {
			mRestorePressed = false;
			mMemory[ 0x11D0 ] = 0;
			mMenuReturn = true;
			eventProcess( true );
			return false;
		}

		// Check if run/stop was pressed
		if( mRunStopPressed == true ) {

			// Save the game
			gamePositionSave( false );

			return true;
		}

		if( mInput->f2Get() ) {

			// Load a game
			gamePositionLoad();

			return true;
		}

		// 117D
		// Button Press Check
		if( mJoyButtonState )
			mMemory[ 0x11CC + X ] = 1;

		if( mMemory[ 0x11CC ] == 1 )
			if( mMemory[ 0x11CD ] == 1 )
				break;
		
		// 1195
		mMemory[ 0x11D7 ] ^= 0x01;

		interruptWait( 1 );

		hw_Update();
	}
	// 11A5
	do {
		hw_Update();

		KeyboardJoystickMonitor(0);
		if( mJoyButtonState )
			continue;

		KeyboardJoystickMonitor(1);

	} while( mJoyButtonState );
		

	sound_PlayEffect( SOUND_MAP_CLOSE );
	mMemory[ 0x11D0 ] = 0;
	return false;
}

// 34EF
bool cCreep::obj_Player_Collision( byte pSpriteNumber, byte pObjectNumber ) {
	byte A;
	if( mRoomAnim[pObjectNumber].mObjectType == OBJECT_TYPE_TRAPDOOR_PANEL ) {
		
		A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
		A -= mRoomAnim[pObjectNumber].mX;

		if( A < 4 ) {
			mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] = 2;

			g_Steam.SetAchievement(eAchievement_Death_TrapDoor);

			return true;
		}

	} 
	// 3505
	if( mRoomAnim[pObjectNumber].mObjectType != OBJECT_TYPE_TRAPDOOR_SWITCH ) 
		return false;

	A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	A -= mRoomAnim[pObjectNumber].mX;

	if( A >= 4 )
		return false;

	mRoomSprites[pSpriteNumber].Sprite_field_1A = mRoomObjects[pObjectNumber].objNumber;
	return false;
}

// 3534: Hit Player
bool cCreep::obj_Player_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 ) {
	byte A = mRoomSprites[pSpriteNumber2].mSpriteType;

	if( A == SPRITE_TYPE_FORCEFIELD )
		return false;

	if( A != SPRITE_TYPE_PLAYER ) {
		// 358C
		if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0 ) {
			return false;
		}

		mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] = 2;
		return true;
	}
	// 353F

	A = mRoomSprites[pSpriteNumber2].spriteImageID;

	if( A == 0x2E || A == 0x2F || A == 0x30 || A == 0x31 || A == 0x26 ) {
		return false;
	} 
	
	if( mRoomSprites[pSpriteNumber2].mY == mRoomSprites[pSpriteNumber].mY ) {
		return false;
	}
	if( mRoomSprites[pSpriteNumber2].mY >= mRoomSprites[pSpriteNumber].mY ) {
		mRoomSprites[pSpriteNumber].Sprite_field_18 = 0xEF;
		return false;
	}

	mRoomSprites[pSpriteNumber].Sprite_field_18 = 0xFE;
	return false;
}

// 359E
void cCreep::obj_Player_Add( ) {
	byte spriteNumber = Sprite_CreepFindFree();
	
	byte Y = mCurrentPlayer;
	mMemory[ 0x34D1 + Y ] = spriteNumber;		// Set player object number
	
	byte A = mMemory[ 0x780B + Y ];
	A <<= 3;
	word_40 = A + mRoomDoorPtr;
	
	// 35C0
	if( mMemory[ word_40 + 2 ] & 0x80 ) {
		mMemory[ 0x780D + mCurrentPlayer ] = 6;
		mRoomSprites[spriteNumber].mX= mMemory[ word_40 ] + 0x0B;
		mRoomSprites[spriteNumber].mY= mMemory[ word_40 + 1 ] + 0x0C;
		mRoomSprites[spriteNumber].Sprite_field_1B = 0x18;
		mRoomSprites[spriteNumber].Sprite_field_6 = 0x03;

	} else {
		// 35F1
		mMemory[ 0x780D + mCurrentPlayer ] = 0;
		mRoomSprites[spriteNumber].mX= mMemory[ word_40 ] + 6;
		mRoomSprites[spriteNumber].mY= mMemory[ word_40 + 1 ] + 0x0F;
	}

	// 360D
	mRoomSprites[spriteNumber].mSpriteType = SPRITE_TYPE_PLAYER;
	mRoomSprites[spriteNumber].mWidth = 3;
	mRoomSprites[spriteNumber].mHeight = 0x11;
	mRoomSprites[spriteNumber].Sprite_field_1F = 0x80;
	mRoomSprites[spriteNumber].playerNumber = mCurrentPlayer;
	mRoomSprites[spriteNumber].spriteImageID= 0;
	mRoomSprites[spriteNumber].Sprite_field_19 = mRoomSprites[spriteNumber].Sprite_field_1A = mRoomSprites[spriteNumber].Sprite_field_18 = 0xFF;
}

void cCreep::roomMain() {

	roomLoad();
	eventProcess( true );
	mInput->inputClear();

	for(byte X = 0; X < 2; ++X ) {
		
		if( mPlayerStatus[X] == 1 ) {
			mCurrentPlayer = X;
			obj_Player_Add();
		}
	}

	//14EA
	mMemory[ 0x15D7 ] = 1;
	mRestorePressed = false;

	for(;;) {

		events_Execute();
		hw_Update();

		// Do pause?
		if( mRunStopPressed == true ) {
			timeb timeNow;
			ftime(&timeNow);

			time_t TimePause = timeNow.time;

			//150E
			do {
				interruptWait( 3 );

				hw_Update();
				KeyboardJoystickMonitor( 0 );						

			} while( !mRunStopPressed );


			ftime(&timeNow);
			time_t diff = timeNow.time - TimePause;

			if (mMemory[0x780D + 0] == 0)
				mPlayer1SecondsPause += diff;

			if (mMemory[0x780D + 1] == 0)
				mPlayer2SecondsPause += diff;

		}
		// 156B
		if( mRestorePressed == true ) {
			mRestorePressed = false;

			for(signed char X=1; X>=0 ; --X) {
				if( mMemory[ 0x780D + X ] == 0 ) {
					mMemory[ 0x780D + X ] = 2;
					byte Y = mMemory[ 0x34D1 + X ];
					mRoomSprites[Y].state |= SPR_ACTION_FLASH;
				}
			}
		}
		// 1594
		if( mMemory[ 0x780D ] == 0 ) {
			mMemory[ 0x7811 ] = 0;
			continue;
		}
		// 15A3
		if( mMemory[ 0x780E ] == 0 ) {
			mMemory[ 0x7811 ] = 1;
			continue;
		}
		
		byte X = 0;
s15B4:;
		if( (mMemory[ 0x780D + X ] == 5) || 
			(mMemory[ 0x780D + X ] == 6) )
			continue;

		++X;
		if( X < 2 )
			goto s15B4;

		break;
	}

	// Leaving Room
	// 15C4
	mMemory[ 0x15D7] = 0;

	for( byte X = 0x1E; X; --X ) {
		events_Execute();
		hw_Update();
	}
}

byte cCreep::sub_6009( byte pA ) {
	word_40 = readLEWord( &mMemory[ mRoomPtr + 4] );
	
	byte A = mMemory[ word_40 ];
	
	word_40 += (pA << 3) + 1;

	return A;	
}

// 2AA9
void cCreep::StringPrint() {
	byte gfxPosX, gfxPosY;

	gfxPosX = mTxtX_0 = mTextXPos;
	gfxPosY = mTxtY_0 = mTextYPos;
	
	if( mTextFont == 0 )
		mTextFontt = 1;
	else
		mTextFontt = mTextFont & 0x03;

	mMemory[0x73B5] = mTextFontt << 3;
	mMemory[0x73E8] = mMemory[0x73B5];

	word_30 = ((mMemory[0x73E8] << 1) + 0xEA);
	word_30 += 0x7300;

	for( signed char Y = 5; Y >= 0; --Y)
		mMemory[ word_30 + Y] = (mTextColor << 4);
	
	//2AFE
	for(;;) {
		byte X = mTextFont & 0x30;
		X >>= 3;

		word_30 = (mMemory[ mObjectPtr ] & 0x7F) << 3;
		word_30 += mMemory[ 0x2BE8 + X ];
		word_30 += (mMemory[ 0x2BE9 + X ] << 8);

		// Copy from Char ROM
		for( signed char count = 7; count >= 0; --count )
			mMemory[ 0x2BF0 + count ] = charRom( word_30 + count);

		word_30 = 0x73EA;

		// 2B50
		for(X = 0; X < 8; ++X) {

			byte Y = mMemory[ 0x2BF0 + X ];
			Y >>= 4;
			Y &= 0x0F;

			word tmp =  mMemory[ 0x2BF8 + Y ];
			Y = mMemory[ 0x2BF0 + X ] & 0xF;
			tmp += (mMemory[ 0x2BF8 + Y] << 8);
			
			writeLEWord(&mMemory[ word_30 ], tmp);

			byte A;
			
			if( mTextFontt < 2 ) {
				A = 2;

			} else {
				if( mTextFontt == 2 ) {
					mMemory[ word_30 + 2 ] = mMemory[ word_30 ];
					mMemory[ word_30 + 3 ] = mMemory[ word_30 + 1 ];

					A = 4;

				} else {
					mMemory[ word_30 + 2 ] = mMemory[ word_30 ];
					mMemory[ word_30 + 4 ] = mMemory[ word_30 ];
					mMemory[ word_30 + 3 ] = mMemory[ word_30 + 1 ];
					mMemory[ word_30 + 5 ] = mMemory[ word_30 + 1 ];

					A = 6;
				}
			}
			
			//2BAb
			word_30 += A;
		}

		screenDraw( 2, 0x95, gfxPosX, gfxPosY, 0x94 );

		if( ((char) mMemory[ mObjectPtr ]) < 0 )
			break;

		++mObjectPtr;
		gfxPosX += 8;
		mTxtX_0 = gfxPosX;
	}

	// 2BD7
	++mObjectPtr;
}

//2A6D
void cCreep::obj_stringPrint( ) {
	size_t count = 0;

	while( (mTextXPos = mMemory[ mObjectPtr ]) ) {
		mTextYPos = mMemory[ mObjectPtr + 1 ];
		mTextColor = mMemory[ mObjectPtr + 2 ];
		mTextFont = mMemory[ mObjectPtr + 3 ];

		mObjectPtr += 0x04;

		StringPrint( );
	}

	++mObjectPtr;
}

// 580D
void cCreep::screenDraw( word pDecodeMode, word pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID = 0 ) {
	byte gfxPosTopY;
	byte gfxHeight_0;

	byte gfxPosBottomY, gfxBottomY;
	byte gfxDestX;
	byte gfxDestX2;
	byte gfxPosRightX;
	byte gfxDestY;
	word gfxCurPos = 0;

	byte videoPtr0, videoPtr1;
	byte Counter2;
	byte drawingFirst = 0;
	byte byte_5CE2;

	mScreen->bitmapRedrawSet();

	if( pDecodeMode	!= 0 ) {
		// Draw Text
		word word_38 = pTxtCurrentID;
		
		word_38 <<= 1;
		word_38 += 0x603B;

		word_30 = mMemory[ word_38 ];
		word_30 += (mMemory[ word_38 + 1 ] << 8);
		
		mTxtWidth = mMemory[ word_30 ];
		mTxtHeight = mMemory[ word_30 + 1];
		mCount = mMemory[ word_30 + 1 ];

		mTxtPosLowerY = mTxtY_0 + mTxtHeight;
		--mTxtPosLowerY;

		if( mTxtX_0 < 0x10 ) {
			mTxtDestX = 0xFF;
			mTxtEdgeScreenX = 0xFF;
		} else {
			mTxtDestX = mTxtX_0 - 0x10;
			mTxtEdgeScreenX = 0;
		}		
		
		mTxtDestXLeft = mTxtDestX >> 2;
		mTxtDestXLeft |= (mTxtEdgeScreenX & 0xC0);
		
		mTxtEdgeScreenX <<= 1;
		if( mTxtDestX & 0x80 )
			mTxtEdgeScreenX |= 0x01;

		mTxtDestX <<= 1;
		mTxtDestXRight = mTxtDestXLeft + mTxtWidth;
		--mTxtDestXRight;
		drawingFirst = false;

		word_30 += 0x03;
	} 

	if( pDecodeMode != 1 ) {
		word byte_38;

		//58B7
		// Draw Graphics
		byte_38 = pGfxID << 1;
		byte_38 += 0x603B;
		
		word_32 = readLEWord( &mMemory[ byte_38 ] );
		
		mGfxWidth = mMemory[ word_32 ];
		mGfxHeight = mMemory[ word_32 + 1 ];
		gfxHeight_0 = mGfxHeight;

		//58ED
		gfxPosBottomY = pGfxPosY + mGfxHeight;
		--gfxPosBottomY;

		gfxDestX = (pGfxPosX - 0x10);

		if( pGfxPosX < 0x10 )
			mGfxEdgeOfScreenX = 0xFF;
		else
			mGfxEdgeOfScreenX = 0;

		gfxDestX2 = gfxDestX >> 2;
		gfxDestX2 |= (mGfxEdgeOfScreenX & 0xC0);
		
		mGfxEdgeOfScreenX <<= 1;
		if(gfxDestX & 0x80)
			mGfxEdgeOfScreenX |= 0x1;

		gfxDestX <<= 1;
		
		//592C
		gfxPosRightX = gfxDestX2 + mGfxWidth;
		--gfxPosRightX;
		
		Counter2 = 0;

		word_32 += 3;
		gfxDestY = pGfxPosY;

		if( pGfxPosY < 0xDC )
			byte_38 = 0;
		else
			byte_38 = 0xFF;

		// 595B
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;
		gfxDestY >>= 1;
		if(byte_38 & 0x1) {
			gfxDestY |= 0x80;
		}
		byte_38 >>= 1;

		// 596A
		byte_5CE2 = mGfxHeight;
		--byte_5CE2;
		byte_5CE2 >>= 3;
		byte_5CE2 += gfxDestY;

		byte A = gfxDestY;

		if(gfxDestY >= 0x80) {
			A = 0;
			A -= gfxDestY;
		}
		
		videoPtr0 = mMemory[0x5CE6 + A];
		videoPtr1 = mMemory[0x5D06 + A];
		
		if( gfxDestY > 0x80 ) {
			signed char a = 0 - videoPtr0;
			videoPtr0 = a;
			
			a = 0 - videoPtr1;
			videoPtr1 = a;
		}

		// 59A8

		byte_38 = pGfxPosX - 0x10;
		if( pGfxPosX < 0x10 )
			A = 0xFF;
		else
			A = 0;

		byte byte38 = byte_38 & 0xFF, byte39 = ((byte_38 & 0xFF00) >> 8);

		byte39 = A;

		byte38 >>= 1;
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}
		byte39 >>= 1;
		byte38 >>= 1;
		
		if( byte39 & 0x01 ) {
			byte38 |= 0x80;
		}

		byte39 = A;

		// 59C6
		if((videoPtr0 + byte38) > 0xFF)
			++videoPtr1;

		videoPtr0 += byte38;
		videoPtr1 += byte39;

		if( pDecodeMode == 0 ) {
			gfxPosTopY = pGfxPosY;
			gfxBottomY = gfxPosBottomY;
		}
			
	}

	// 59F0
	if( pDecodeMode == 1 ) {

		gfxPosTopY = mTxtY_0;
		gfxBottomY = mTxtPosLowerY;

	} else if( pDecodeMode == 2 ) {
		byte A = 0;

		if( pGfxPosY != mTxtY_0 ) {
			if( pGfxPosY >= mTxtY_0 ) {
				
				if( pGfxPosY >= 0xDC || mTxtY_0 < 0xDC ) {
					 A = mTxtY_0;
				}  
			} else {
				// 5a17
				if( mTxtY_0 >= 0xDC && pGfxPosY < 0xDC ) {
					A = mTxtY_0;
				}
			}
		}

		// 5A2E
		gfxPosTopY = A;

		A = 0;

		if( gfxPosBottomY != mTxtPosLowerY ) {

			if( gfxPosBottomY >= mTxtPosLowerY ) {

				if( gfxPosBottomY >= 0xDC && mTxtPosLowerY < 0xDC ) {
					gfxBottomY = mTxtPosLowerY;
				}  else
					gfxBottomY = gfxPosBottomY;

			} else {

				// 5A49
				if( mTxtPosLowerY >= 0xDC && gfxPosBottomY < 0xDC ) {
					gfxBottomY = gfxPosBottomY;
				} else
					gfxBottomY = mTxtPosLowerY;
			}

		} else {
			//5a60
			gfxBottomY = mTxtPosLowerY;
		}
	}

	// 5A66
	byte gfxCurrentPosY = gfxPosTopY;
	word byte_36 = 0;
	word byte_34 = mMemory[ 0xBC00 + gfxCurrentPosY ];

	word A = mMemory[ 0xBB00 + gfxCurrentPosY ];

	byte_34 |= (A << 8);

	// 5A77
	for(;;) {
		
        // Bitmap: Overlay
		if( pDecodeMode != 0 && mCount != 0 ) {
			
			if( drawingFirst != 1 ) {
				if( mTxtY_0 == gfxCurrentPosY ) 
					drawingFirst = 1;
				else
					goto s5AED;
			}
			//5A97
			--mCount;

			if( gfxCurrentPosY < 0xC8 ) {
				
				gfxCurPos = mTxtDestXLeft;

				word word_36 = (byte_34 + mTxtDestX) + (mTxtEdgeScreenX << 8);


				// 5AB8
				for( byte Y = 0;; ++Y ) {
					if( (int8_t) gfxCurPos < 0x28 ) {
						
						byte A = mMemory[ word_30 + Y ];
						mMemory[ word_36 + Y] &= (A ^ 0xFF );
					} 
					// 5AC7
					if( (byte) gfxCurPos == mTxtDestXRight )
						break;
					
					word_36 += 0x7;
					++gfxCurPos;

				}
			}
			// 5AE1
			word_30 += mTxtWidth;
		}
s5AED:;

		//5aed
		if( pDecodeMode != 1 && gfxHeight_0 != 0) {
			if( Counter2 != 1 ) {
				if( gfxCurrentPosY == pGfxPosY ) {
					Counter2 = 1;
				} else {
					goto noCarry2;
				}
			}

			--gfxHeight_0;
			if( gfxCurrentPosY < 0xC8 ) {
				// 5b17
				gfxCurPos = gfxDestX2;
				byte_36 = byte_34 +  (mGfxEdgeOfScreenX << 8);
				byte_36 += gfxDestX;

				for( byte Y = 0; ; ++Y ) {
					// 5B2E
					if((byte) gfxCurPos < 0x28 )
						mMemory[ byte_36 + Y ] |= mMemory[ word_32 + Y ];

					if((byte) gfxCurPos == gfxPosRightX )
						break;

					//5B43
					byte_36 += 7;
					++gfxCurPos;
				}
			}

			//5B55
			word_32 += mGfxWidth;
		}
noCarry2:;
		//5B61
		if( gfxCurrentPosY == gfxBottomY ) 
			break;

		++gfxCurrentPosY;
		if(gfxCurrentPosY & 0x7 ) {
			++byte_34;
			continue;
		}

		byte_34 += 0x0139;
	}
	// 5B8C

	if(pDecodeMode == 1)
		return;

    //
    // Color 1
    //
	if( pGfxPosY & 0x7)
		mMemory[0x5CE5] = 1;
	else
		mMemory[0x5CE5] = 0;

	// 5BA4
	gfxCurrentPosY = gfxDestY;
	word_30 = videoPtr0;
	word_30 += (0xCC + videoPtr1) << 8;

	byte Y = 0;

	// 5BBC
	for( ;; ) {

		if( gfxCurrentPosY < 0x19 ) {
			
			for( Y = 0, gfxCurPos = gfxDestX2; (int8_t)gfxCurPos <= (int8_t)gfxPosRightX; ++gfxCurPos, ++Y ) {
			
				if((byte)gfxCurPos < 0x28 )
					mMemory[ word_30 + Y ] = mMemory[ word_32 + Y ];
			}
		}

		// 5BE5
		if( gfxCurrentPosY != byte_5CE2 ) {
			++gfxCurrentPosY;
			
			word_32 += mGfxWidth;

		} else {
			// 5BFF
			if( mMemory[ 0x5CE5 ] != 1 )
				break;

			mMemory[ 0x5CE5 ] = 0;

			if( gfxCurrentPosY != 0xFF )
				if( gfxCurrentPosY >= 0x18 )
					break;
		}				
		// 5C16
		word_30 += 0x28;
	}
	
	// 5C24
    // Color 2
	word_32 += mGfxWidth;
	if( pGfxPosY & 0x7 )
		mMemory[ 0x5CE5 ] = 1;
	else
		mMemory[ 0x5CE5 ] = 0;

	gfxCurrentPosY = gfxDestY;
	word_30 = videoPtr0;
	word_30 += (0xD8 + videoPtr1) << 8;
	
	// 5C56
	for( ;; ) {
		
		if( gfxCurrentPosY < 0x19 ) {

			for(Y = 0, gfxCurPos = gfxDestX2; (int8_t)gfxCurPos <= (int8_t)gfxPosRightX; ++gfxCurPos, ++Y) {

				if((byte) gfxCurPos < 0x28 )
					mMemory[ word_30 + Y ] = mMemory[ word_32 + Y ];
			}
		}
		//5C7F
		if( gfxCurrentPosY != byte_5CE2 ) {
			++gfxCurrentPosY;
			word_32 += mGfxWidth;
		} else {
		// 5C99
			if( mMemory[ 0x5CE5 ] != 1 )
				return;

			mMemory[ 0x5CE5 ] = 0;
			if( gfxCurrentPosY != 0xFF )
				if( gfxCurrentPosY >= 0x18 )
					return;
		}
		// 5CB0
		word_30 += 0x28;
	}
}

// 1203: 
void cCreep::mapRoomDraw() {
	byte byte_13EA, roomX;
	byte roomY, roomHeight, roomWidth;

	mRoomPtr = 0x7900;
	
	byte gfxPosX;
	byte gfxPosY;

	//1210
	for(;;) {
		
		if( mMemory[ mRoomPtr ] & MAP_ROOM_STOP_DRAW )
			return;
		
		if( mMemory[ mRoomPtr ] & MAP_ROOM_VISIBLE ) {
			//1224
			
			mMemory[ 0x63E7 ] = mMemory[ mRoomPtr ] & 0xF;		// color
			roomX		= mMemory[ mRoomPtr + 1 ];				// top left x
			roomY		= mMemory[ mRoomPtr + 2 ];				// top left y
			roomWidth	= mMemory[ mRoomPtr + 3 ] & 7;			// width
			roomHeight	= (mMemory[ mRoomPtr + 3 ] >> 3) & 7;	// height

			gfxPosY = roomY;
			
			// Draw Room Floor Square
			// 1260
			for( byte CurrentX = roomWidth; CurrentX ; --CurrentX) {
				
				gfxPosX = roomX;
				
				for(byte_13EA = roomHeight; byte_13EA; --byte_13EA) {
					screenDraw( 0, 0x0A, gfxPosX, gfxPosY, 0 );
					gfxPosX += 0x04;
				}

				gfxPosY += 0x08;
			}

			// 128B
			// Top edge of room
			mTxtX_0 = roomX;
			mTxtY_0 = roomY;
			

			for( byte_13EA = roomHeight; byte_13EA; --byte_13EA) {
				screenDraw(1, 0, 0, 0, 0x0B );
				mTxtX_0 += 0x04;
			}

			// 12B8
			// Bottom edge of room
			mTxtX_0 = roomX;
			mTxtY_0 = ((roomWidth << 3) + roomY) - 3;

			for( byte_13EA = roomHeight; byte_13EA; --byte_13EA) {
				screenDraw(1, 0, 0, 0, 0x0B );
				mTxtX_0 += 0x04;
			}

			//12E5
			// Draw Left Edge
			mTxtX_0 = roomX;
			mTxtY_0 = roomY;

			for( byte_13EA = roomWidth; byte_13EA; --byte_13EA) {
				screenDraw(1, 0, 0, 0, 0x0C );
				mTxtY_0 += 0x08;
			}

			//130D
			// Draw Right Edge
			mTxtX_0 = ((roomHeight << 2) + roomX) - 4;
			mTxtY_0 = roomY;

			for( byte_13EA = roomWidth; byte_13EA; --byte_13EA) {
				screenDraw(1, 0, 0, 0, 0x0D );
				mTxtY_0 += 0x08;
			}

			// 133E
			byte_13EA = sub_6009( 0 );
s1349:;
			if( byte_13EA )
				break;
		} 

		//134E
		mRoomPtr += 0x08;
	}

	//135C
	byte A = mMemory[ word_40 + 2 ];
	A &= 3;

	if( !( A & 3) ) {
		mTxtY_0 = roomY;
	} else {
		// 136D
		if( A == 2 ) {
			// 136F
			mTxtY_0 = (roomWidth << 3) + roomY;
			mTxtY_0 -= 3;

		} else {
			// 13A0
			mTxtY_0 = roomY + mMemory[ word_40 + 6 ];
			
			if( A != 3 ) {
				mTxtX_0 = ((roomHeight << 2) + roomX) - 4;
				A = 0x11;
			} else {
				// 13C5
				mTxtX_0 = roomX;
				A = 0x10;
			}
			
			goto s13CD;
		}
	}

	// 1381
	mTxtX_0 = A = roomX + mMemory[ word_40 + 5 ];

	A &= 2;
	
	if( A ) {
		A ^= mTxtX_0;
		mTxtX_0 = A;
		A = 0x0F;
	} else 
		A = 0x0E;
		
	// Draw Doors in sides
s13CD:;
	screenDraw( 1, 0, 0, 0, A );

	word_40 += 0x08;
	--byte_13EA;
	goto s1349;

}

void cCreep::obj_Image_Draw() {
	writeLEWord(&mMemory[ 0x6067 ], mObjectPtr);
	
	byte gfxDecodeMode = 0, gfxCurrentID = 0x16;

	word_32 = (mMemory[ mObjectPtr + 1 ] - 1) >> 3;
	++word_32;

	word_30 = 0;
	
	//1B18
	for( byte X =  mMemory[ mObjectPtr ]; X; --X) {
		word_30 += word_32;
	}

	// 1B2D
	word_30 <<= 1;

	for( byte X = mMemory[ mObjectPtr + 1 ]; X; --X) {
		word_30 += mMemory[ mObjectPtr ];
	}

	// 1B4D
	word_30 += 3;

	mObjectPtr += word_30;

	//1B67
	byte A;
	while( (A = mMemory[ mObjectPtr ]) ) {
		// 1B7D
		screenDraw( gfxDecodeMode, gfxCurrentID, A, mMemory[ mObjectPtr + 1 ], 0 );
			
		mObjectPtr += 2;
	}

	++mObjectPtr;
}

// 160A: Draw multiples of an object
void cCreep::obj_MultiDraw() {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	signed char gfxRepeat;

	while( (gfxRepeat = mMemory[ mObjectPtr ]) != 0 ) {

		gfxCurrentID = mMemory[ mObjectPtr + 1 ];
		gfxPosX = mMemory[ mObjectPtr + 2 ];
		gfxPosY = mMemory[ mObjectPtr + 3 ];

		--gfxRepeat;

		// Draw an object a number of times,starting at a certain X/Y and repeating every X/Y
		for( ; gfxRepeat >= 0; --gfxRepeat ) {
		
			screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY );
		
			gfxPosX += mMemory[ mObjectPtr + 4 ];
			gfxPosY += mMemory[ mObjectPtr + 5 ];
		}

		mObjectPtr += 0x06;
	}
	
	++mObjectPtr;
}

void cCreep::eventProcess( bool pResetKeys ) {
	g_Window.EventCheck();

	for (std::vector<cEvent>::iterator EventIT = mEvents.begin(); EventIT != mEvents.end(); ++EventIT) {

		switch (EventIT->mType) {
			case eEvent_KeyUp:
			case eEvent_KeyDown:
				mInput->inputCheck( pResetKeys, *EventIT );
				break;

			case eEvent_JoyButtonDown:
			case eEvent_JoyButtonUp:
			case eEvent_JoyMovement:
			case eEvent_JoyStickDisconnect:
				mInput->inputCheck( pResetKeys, *EventIT );
				break;

			case eEvent_MouseLeftDown:
				break;

			case eEvent_MouseRightDown:
				break;

			case eEvent_MouseLeftUp:
				break;

			case eEvent_MouseRightUp:
				break;

			case eEvent_MouseMove:
				break;

			case eEvent_Quit:
				mQuit = true;
#ifdef STEAM_BUILD
				SteamAPI_Shutdown();
#endif
				exit( 0 );
				break;

			case eEvent_Redraw:
				mScreen->bitmapRedrawSet();
				break;
		}

	}

	mEvents.clear();
}

void cCreep::hw_Update() {

	mScreen->refresh();
	eventProcess( false );

#ifdef STEAM_BUILD
	SteamAPI_RunCallbacks();
#endif
}

// 1935: Sleep for X amount of interrupts
void cCreep::hw_IntSleep( byte pA ) {

	for( byte X = 6; X > 0; --X ) {
		interruptWait( pA );
	}

}

// 1950: Player Escapes from the Castle
void cCreep::gameEscapeCastle() {
	
	// Steam
	g_Steam.SetAchievement( (eAchievements) (eAchievement_Castle + mCastle->infoGet()->castleNumberGet()));

	int32_t Count = g_Steam.IncreaseStat(eStat_CastlesCompleted);

	if (Count > 1)
		g_Steam.SetAchievement(eAchievement_EscapeCount_1);

	if (Count > 5)
		g_Steam.SetAchievement(eAchievement_EscapeCount_5);

	if (Count >= 14)
		g_Steam.SetAchievement(eAchievement_EscapeCount_14);

	// Still got all lives?
	if ((mMemory[0x780F] == 1 && mMemory[0x7807] == 3) || (mMemory[0x7810] == 1 && mMemory[0x7808] == 3) )
		g_Steam.SetAchievement(eAchievement_EscapeCount_1Player);

	// Still got all lives?
	if (mMemory[0x780F] == 1 && mMemory[0x7810] == 1)
		if (mMemory[0x7807] == 3 && mMemory[0x7808] == 3)
			g_Steam.SetAchievement(eAchievement_EscapeCount_2Player);

	// Both players alive?
	if (mMemory[0x780F] == 1 && mMemory[0x7810] == 1)
		g_Steam.SetAchievement(eAchievement_EscapeCount_BothAlive);

	screenClear();
	mMemory[ 0x0B72 ] = 6;
	if( mMemory[ 0x7802 ] & 0x80 ) {
		mObjectPtr = readLEWord( &mMemory[ 0x785F ] );
		roomPrepare();
	}

	byte A = mMemory[ 0x1AB2 ];
	A += 0x31;

	// Set player number in string 'Player Escapes'
	mMemory[ 0x1ABE ] = A;

	mObjectPtr = 0x1AB3;
	obj_stringPrint();
	
	// Draw Players Escape time
	convertTimerToTime( mPlayersTime[mMemory[0x1AB2]] );
	screenDraw(0, 0x93, 0x68, 0x18, 0 );
	
	// 19AF
	byte Y = mMemory[ 0x1AB2 ];
	byte X = mMemory[ 0x34D1 + Y ];
	mRoomSprites[X].mY= 0x87;
	mRoomSprites[X].mX= 0x08;

	A =	seedGet() & 0x0E;
	if( A != 0 )
		A = 0;
	else
		A = 8;

	mMemory[ 0x1AE5 ] = A;
	mMemory[ 0x1AE4 ] = 0;
	
	// Set the colors to draw the road
	std::vector<sBackgroundColor>* background = mScreen->backgroundColorsGet();
	background->push_back({ 0x00, 0xA2, 0x00 });
	background->push_back({ 0xA2, 0xCA, 0x0B });
	background->push_back({ 0xCA, 0xD2, 0x09 });
	background->push_back({ 0xD2, 0xFF, 0x0B });

	// 19DF
	for(;;) {
		A = mMemory[ 0x1AE4 ];

		if( A == 0 )  {
			byte Y = mMemory[ 0x1AE5 ];
			A = mMemory[ 0x1AD1 + Y ];
			if( A == 0 )
				break;

			// 19EF
			mMemory[ 0x1AE4 ] = A;
			mMemory[ 0x1AE3 ] = mMemory[ 0x1AD2 + Y ];
			mMemory[ 0x1AE5 ] += 0x02;
		}
		// 1A01
		if( mMemory[ 0x1AE3 ] >= 1 ) {
			if( mMemory[ 0x1AE3 ] != 1 ) {
				// 1A0A
				++mRoomSprites[X].spriteImageID;
				A = mRoomSprites[X].spriteImageID;
				if( A >= 0x9B || A < 0x97 ) {
					A = 0x97;
				}
			} else {
				// 1A33
				--mRoomSprites[X].mX;
				++mRoomSprites[X].spriteImageID;
				A = mRoomSprites[X].spriteImageID;
				if( A >= 3 )
					A = 0;
			}

		} else {
			// 1A1D
			++mRoomSprites[X].mX;
			++mRoomSprites[X].spriteImageID;
			A = mRoomSprites[X].spriteImageID;
			if( A >= 6 || A < 3 )
				A = 0x03;
		}

		// 1A42
		mRoomSprites[X].spriteImageID= A;
		Y = X;

		cSprite *sprite = mScreen->spriteGet( Y );

		// 1A4B
		mMemory[ 0x10 + Y ] = ((mRoomSprites[X].mX - 0x10) << 1) + 0x18;
		sprite->mX = ((mRoomSprites[X].mX - 0x10) << 1) + 0x18;
		
		// 1A72
		sprite->mY = mRoomSprites[X].mY + 0x32;

		hw_SpritePrepare( X );

		sprite->_rEnabled = true;

		if( mMemory[ 0x1AB2 ] )
			A = mMemory[ 0x34D4 ];
		else
			A = mMemory[ 0x34D3 ];

		sprite->_color = A;
		--mMemory[ 0x1AE4 ];

		// 1A95
		interruptWait(2);
		hw_Update();
	}

	// 1AA7
	//hw_IntSleep(0xA);
	background->clear();
}

// 1B9F
void cCreep::gameHighScoresHandle( const size_t pPlayerNumber ) {

	eventProcess(false);
	mInput->keyRawGet();

	// Player1 or 2?
	if(pPlayerNumber)
		word_30 = 0xB840;
	else
		word_30 = 0xB804;

	// 1BBE
	signed int HighScorePosition = 0x0A;
	
	for(; HighScorePosition>=0; --HighScorePosition) {

		if (mHighScores[pPlayerNumber][HighScorePosition].mTime.mSeconds == 0 || mPlayersTime[pPlayerNumber].mSeconds < mHighScores[pPlayerNumber][HighScorePosition].mTime.mSeconds) {

			--HighScorePosition;
			continue;
		}

		if (mPlayersTime[pPlayerNumber].mSeconds > mHighScores[pPlayerNumber][HighScorePosition].mTime.mSeconds) {
			++HighScorePosition;
			break;
		}
	}

	if (HighScorePosition < 0)
		HighScorePosition = 0;

	// 1BE7
	byte Y;

	if(pPlayerNumber) {
		Y = 0x73;
		mMemory[ 0x2788 ] = 0x68;
	} else {
		// 1BF8
		Y = 0x37;
		mMemory[ 0x2788 ] = 0x18;
	}

	// Y Position to input
	byte A = HighScorePosition;
	A <<= 3;
	A += 0x38;
	mMemory[ 0x2789 ] = A;

	// Color
	byte X = HighScorePosition;
	mMemory[ 0x278A ] = mMemory[ 0x1E85 + X ];

	writeLEWord(&mMemory[ 0x1D03 ], (word_30 - 2));
	
	for( int HSP = 0x09; HSP >= HighScorePosition; --HSP) {

		mHighScores[pPlayerNumber][HSP + 1] = mHighScores[pPlayerNumber][HSP];
		
		//1C40
	}

	// 1C43
	mHighScores[pPlayerNumber][HighScorePosition].mTime = mPlayersTime[pPlayerNumber];

	// Update Leaderboard for Player 1
	if(pPlayerNumber == 0 && mPlayersTime[pPlayerNumber].mSeconds)
		g_Steam.LeaderboardUploadScore(mPlayersTime[pPlayerNumber].mSeconds);

	gameHighScores();

	// 1C60

	mObjectPtr = 0x1D05;
	X = mMemory[ 0x1CFD ];

	A = mMemory[ 0x1D01 + X ];
	mMemory[ 0x1D10 ] = A;
	obj_stringPrint();

	mMemory[ 0x278B ] = 1;

	mStrLengthMax = sizeof(mHighScores[pPlayerNumber][HighScorePosition].mName);
	StringPrint_StringInput();

	// Copy the name
	for(Y = 0; Y < sizeof(mHighScores[pPlayerNumber][HighScorePosition].mName); ++Y) {

		mHighScores[pPlayerNumber][HighScorePosition].mName[Y] = mMemory[0x278E + Y];
	}
	
	// 1CA9
	X = mMemory[ 0x2399 ];
	Y = mMemory[ 0xBA01 + X ];

	A = mMemory[ 0x5CE6 + Y ];
	A += mMemory[ 0xBA00 + X ];

	word_30 = A + ((mMemory[ 0x5D06 + Y ] | 4) << 8);
	mMemory[ 0x28D6 ] = 0x59;

	for( signed char Y = 0x0E; Y >= 0; --Y ) {
		A = mMemory[ word_30 + Y ];

		if( (A & 0x7F) < 0x20 )
			A |= 0x40;

		mMemory[ 0x28D7 + Y ] = A;
	}

	// 1CD8
	mMemory[ 0x28D2 ] = mMemory[ 0xBA03 + X ];
	mMemory[ 0x28D1 ] = 2;
	
	// Save highscores
	mCastleManager->scoresSave( mCastle->nameGet(), sizeof(mHighScores), (uint8*) &mHighScores[0] );

	DisableSpritesAndStopSound();
}

// 1D42: Display Highscores for this Castle
void cCreep::gameHighScores( ) {
	screenClear();

	g_Steam.SetAchievement(eAchievement_LookAtHighScores);

	byte X = (byte) (mCastle->infoGet()->castleNumberGet() * 4) + 0x10;
	mMemory[ 0x2399 ] = X;

	byte Y = mMemory[ 0xBA01 + X ];
	byte A = mMemory[ 0x5CE6 + Y ];

	word_30 = (A + mMemory[ 0xBA00 + X ]) + ((mMemory[ 0x5D06 + Y] | 0x04) << 8);

	mMemory[ 0xBA03 + X ] = (byte) mCastle->nameGet().size() + 1;

	Y =  mMemory[ 0xBA03 + X ] - 2;

	// 1D67
	mMemory[ 0xB87A + Y ] = mMemory[ word_30 + Y ];

	// Convert name
	for( ; (char) Y >= 0; --Y )
		mMemory[ 0xB87A + Y ] = toupper(mCastle->nameGet()[Y]) & 0x3F;

	// Mark end of name
	mMemory[ 0xB87A + (mCastle->nameGet().size()-1) ] |= 0x80;

	mObjectPtr = 0xB87A;

	A = 0x15 - mMemory[ 0xBA03 + X ];
	A <<= 2;
	A += 0x10;
	
	mTextXPos = A;
	mTextYPos = 0x10;
	mTextColor = 0x01;
	mTextFont = 0x02;

	// Draw castle name
	StringPrint();

	X = 0;
	mTextXPos = 0x18;
	mTextFont = 0x21;

	// 1DAD
	for(;;) {
		mTextYPos = 0x38;

		for(size_t HighScorePosition = 0; HighScorePosition < 0x0A; ++HighScorePosition ) {
			
			mTextColor = mMemory[ 0x1E85 + HighScorePosition ];
			A = mHighScores[X][HighScorePosition].mName[0];

			if(mHighScores[X][HighScorePosition].mName[0] != 0xFF ) {
				mMemory[ 0xB87A ] = mHighScores[X][HighScorePosition].mName[0];
				mMemory[ 0xB87B ] = mHighScores[X][HighScorePosition].mName[1];
				A = mHighScores[X][HighScorePosition].mName[2];

			} else {
				// 1DD6

				mMemory[ 0xB87A ] = mMemory[ 0xB87B ] = A = 0x2E; 
			}

			// 1DDE
			mMemory[ 0xB87C ] = A | 0x80;

			mObjectPtr = 0xB87A; 
			StringPrint();

			if(mHighScores[X][HighScorePosition].mName[0] != 0xFF ) {
				// 1DF5
				convertTimerToTime( mHighScores[X][HighScorePosition].mTime );
				screenDraw( 0, 0x93, mTextXPos + 0x20, mTextYPos, 0x94 );
			}
				
			// 1E20
			mTextYPos += 0x08;
		}
		
		// 1E3B
		if( mTextXPos != 0x18 )
			break;

		mTextXPos = 0x68;
		++X;
	}

	// Draw BEST TIMES
	// 1E4A
	mObjectPtr = 0x1E5B;
	obj_stringPrint();
}

// 21C8
void cCreep::sound_PlayEffect( char pA ) {

	if( mIntro )
		return;

	if( mDisableSoundEffects == 1 )
		return;

	if( mPlayingSound >= 0 )
		return;

	mPlayingSound = pA;

	byte Y = mPlayingSound << 1;

	mSound->sidWrite( 0x04, 0 );
	mSound->sidWrite( 0x0B, 0 );
	mSound->sidWrite( 0x12, 0 );
	mSound->sidWrite( 0x17, 0 );

	mMusicBuffer = &mMemory[ readLEWord( &mMemory[ 0x7572 + Y ]) ];

	mSound->sidWrite( 0x18, 0x0F );

	mMemory[ 0xD404 ] = mMemory[ 0xD40B ] = mMemory[ 0xD412 ] = mMemory[ 0xD417 ] = 0;
	mMemory[ 0x20DC ] = mMemory[ 0x20DD ] = 0;
	mMemory[ 0xD418 ] = 0x0F;
	mMemory[ 0x2104 ] = 0x18;
	mMemory[ 0x2105 ] = 0x18;
	mMemory[ 0x2106 ] = 0x18;
	mMemory[ 0x2107 ] = 0x14;
	mTimerSet((mMemory[ 0x2107 ] << 2 ) | 3);
	mMemory[ 0xDC0D ] = 0x81;
	mMemory[ 0xDC0E ] = 0x01;

	mSound->playback(true);
}

void cCreep::obj_Walkway_Prepare() {
	byte CurrentX, Width, Length;
	byte gfxCurrentID, gfxPosX, gfxPosY;

	while( (Length = mMemory[ mObjectPtr ]) ) {
		
		gfxPosX = mMemory[ mObjectPtr + 1 ];
		gfxPosY = mMemory[ mObjectPtr + 2 ];
		
		byte_5FD5 = (gfxPosX >> 2);
		byte_5FD5 -= 4;

		byte_5FD6 = (gfxPosY >> 3);
		word_3C_Calculate();

		// 16A9
		
		for( CurrentX = 1; CurrentX <= Length; ++CurrentX ) {
			byte A;

			if( CurrentX != 1 ) {
				if( CurrentX != Length )
					A = 0x1C;
				else
					A = 0x1D;
			} else 
				A = 0x1B;

			// 16C1
			gfxCurrentID = A;
			screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY );
			
			// 16D1
			for( Width = 1; Width <= mGfxWidth; ++Width ) {
				
				if( CurrentX != 1 ) {
					
					if( CurrentX != Length )
						A = 0x44;

					else {
						// 16EE
						if( Width == mGfxWidth )
							A = 0x40;
						else
							A = 0x44;
					}

				} else {
					// 16E2
					if( Width == 1 )
						A = 0x04;
					else
						A = 0x44;
				}

				// 16F8
				A |= mMemory[ word_3C ];
				mMemory[ word_3C ] = A;
				
				
				word_3C += 2;
			}
			
			gfxPosX += (mGfxWidth << 2);
		}
		// 1732

		mObjectPtr += 3;
	}

	++mObjectPtr;
}

void cCreep::roomAnim_Disable( byte pSpriteNumber ) {

	if( !(mRoomAnim[pSpriteNumber ].mFlags & ITM_DISABLE) ) {

		mTxtX_0 = mRoomAnim[pSpriteNumber ].mX;
		mTxtY_0 = mRoomAnim[pSpriteNumber ].mY;
		
		screenDraw( 1, 0, 0, 0, mRoomAnim[pSpriteNumber ].mGfxID );
		
		mRoomAnim[pSpriteNumber ].mFlags |= ITM_DISABLE;
	}


}

byte cCreep::seedGet() {
	static byte byte_5EFB = 0x57, byte_5EFA = 0xC6, byte_5EF9 = 0xA0;
	
	byte A = byte_5EFB;
	bool ocf = false, cf = false;

	if( A & 0x01 )
		cf = true;
	
	A >>= 1;
	if(ocf)
		A |= 0x80;
	
	ocf = cf;
	cf = false;
	A = byte_5EFA;
	if( A & 0x01 )
		cf = true;
	A >>= 1;
	if(ocf)
		A |= 0x80;

	byte_5EF9 = A;
	A = 0;
	if(cf)
		A = 0x01;

	A ^= byte_5EFA;
	byte_5EFA = A;

	A = byte_5EF9;
	A ^= byte_5EFB;
	byte_5EFB = A;
	
	A ^= byte_5EFA;
	byte_5EFA = A;

	return A;
}

// 5D26: Prepare sprites
void cCreep::hw_SpritePrepare( byte pSpriteNumber ) {
	byte tmpHeight, tmpWidth;
	byte A;

	word word_38 = mRoomSprites[pSpriteNumber].spriteImageID;
	word_38 <<= 1;
	word_38 += 0x603B;
	
	// read sprite pointer
	word_30 = readLEWord( &mMemory[ word_38 ] );
	
	mRoomSprites[pSpriteNumber].spriteFlags = mMemory[ word_30 + 2 ];
	
	tmpWidth = mMemory[ word_30 ];
	mRoomSprites[pSpriteNumber].mCollisionWidth = tmpWidth << 2;
	mRoomSprites[pSpriteNumber].mCollisionHeight = mMemory[ word_30 + 1 ];
	
	// 5D72
	
	word_32 = mMemory[ 0x26 + pSpriteNumber ] ^ 8;
	word_32 <<= 6;
	word_32 += 0xC000;

	word_30 += 0x03;
	tmpHeight = 0;
	
	// 5DB2
	for(;;) {

		for(byte Y = 0; Y < 3; ++Y ) {
			
			if( Y < tmpWidth )
				A = mMemory[ word_30 + Y ];
			else
				A = 0;

			mMemory[ word_32 + Y ] = A;
		}

		++tmpHeight;
		if( tmpHeight == 0x15 )
			break;

		if( tmpHeight < mRoomSprites[pSpriteNumber].mCollisionHeight ) 
			word_30 += tmpWidth;
		else 
			word_30 = 0x5E89;
		
		// 5DED
		word_32 += 0x03;
	}

	// 5DFB
	cSprite *sprite = mScreen->spriteGet(pSpriteNumber);

	word dataSrc = mMemory[ 0x26 + pSpriteNumber ] ^ 8;
	dataSrc <<= 6;
	dataSrc += 0xC000;

	mMemory[ 0x26 + pSpriteNumber ] = mMemory[ 0x26 + pSpriteNumber ] ^ 8;

	// Sprite Color
	sprite->_color = mRoomSprites[pSpriteNumber].spriteFlags & 0x0F;

	if( !(mRoomSprites[pSpriteNumber].spriteFlags & SPRITE_DOUBLEWIDTH )) {
		sprite->_rDoubleWidth = false;
	} else {
		mRoomSprites[pSpriteNumber].mCollisionWidth <<= 1;
		sprite->_rDoubleWidth = true;
	}

	// 5E2D
	if( !(mRoomSprites[pSpriteNumber].spriteFlags & SPRITE_DOUBLEHEIGHT )) {
		sprite->_rDoubleHeight = false;
	} else {
		mRoomSprites[pSpriteNumber].mCollisionHeight <<= 1;
		sprite->_rDoubleHeight = true;
	}

	// 5E4C
	if( !(mRoomSprites[pSpriteNumber].spriteFlags & SPRITE_PRIORITY )) {
		sprite->_rPriority = true;
	} else {
		sprite->_rPriority = false;
	}


	// 5E68
	if(! (mRoomSprites[pSpriteNumber].spriteFlags & SPRITE_MULTICOLOR )) {
		sprite->_rMultiColored = true;
	} else {
		sprite->_rMultiColored = false;
	}

	sprite->streamLoad( &mMemory[ dataSrc ] );
	mScreen->spriteRedrawSet();
}

void cCreep::stringSet( byte pPosX, byte pPosY, byte pColor, string pMessage ) {
	memcpy( &mMemory[ 0xB906 ], pMessage.c_str(), pMessage.size() );
 
	mMemory[ 0xB900 ] = 0x6D;
    mMemory[ 0xB901 ] = 0x2A;
    mMemory[ 0xB902 ] = pPosX;
	mMemory[ 0xB903 ] = pPosY;
	mMemory[ 0xB904 ] = pColor;
    mMemory[ 0xB905 ] = 0x22;

    mMemory[ 0xB905 + pMessage.size() ] |= 0x80;
	mMemory[ 0xB906 + pMessage.size() ] = 0;
	mMemory[ 0xB907 + pMessage.size() ] = 0;
}

// 25B8
void cCreep::gameFilenameGet( bool pLoading, bool pCastleSave ) {
	
	screenClear();
	eventProcess( true );

	mObjectPtr = 0x2633;
	roomPrepare();
	
	if( pLoading )
		mObjectPtr = 0x261F;
	else {

		if(!pCastleSave)
			mObjectPtr = 0x2609;
		else {
			stringSet( 0x34, 0x00, 0x01, "SAVE CASTLE" );
			mObjectPtr = 0xB900;
		}
	}

	roomPrepare();
	mMemory[ 0x2788 ] = 0x20;
	mMemory[ 0x2789 ] = 0x48;
	mStrLengthMax = 0x10;
	mMemory[ 0x278A ] = 0x01;
	mMemory[ 0x278B ] = 0x02;

	StringPrint_StringInput();
}

// 24A7
void cCreep::gamePositionLoad() {
	
	gameFilenameGet( true, false );
	if(!mStrLength)
		return;

	string filename = string( (char*) &mMemory[ 0x278E ], mStrLength );
	
	if (mCastleManager->positionLoad(filename, &mMemory[0x7800]) == true) {

		g_Steam.SetAchievement(eAchievement_RestoredGame);

		mSaveGameLoaded = 1;

		mPlayersTime[0].mSeconds = readLEWord(&mMemory[0x7855]);
		mPlayersTime[1].mSeconds = readLEWord(&mMemory[0x7857]);
	}

	DisableSpritesAndStopSound();
}

// 24FF: Save a game, or a castle
void cCreep::gamePositionSave( bool pCastleSave ) {
	
	gameFilenameGet( false, pCastleSave );
	if(!mStrLength)
		return;

	string filename = string( (char*) &mMemory[ 0x278E ], mStrLength );
	// Save from 0x7800

	word saveSize = readLEWord( &mMemory[ 0x7800 ] );
	
	// Castles save with a Z
	if( pCastleSave )
		filename.insert(0, "Z" );

	bool result = false;

	if( pCastleSave )
		result = mCastleManager->castleSave( filename, saveSize, &mMemory[ 0x7800 ] );
	else
		result = mCastleManager->positionSave( filename, saveSize, &mMemory[ 0x7800 ] );

	if( result == false) {

		mObjectPtr = 0x25AA;	// IO ERROR
		screenClear();
		obj_stringPrint();

		hw_Update();
		hw_IntSleep(0x23);
	}
	else {
		DisableSpritesAndStopSound();

		g_Steam.SetAchievement(eAchievement_SavedGame);
	}

}

void cCreep::positionCalculate( byte pSpriteNumber ) {

	byte_5FD5 = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	
	byte_5FD7 = byte_5FD5 & 3;
	byte_5FD5 = (byte_5FD5 >> 2) - 4;

	byte_5FD6 = mRoomSprites[pSpriteNumber].mY + mRoomSprites[pSpriteNumber].mHeight;
	byte_5FD8 = byte_5FD6 & 7;
	byte_5FD6 >>= 3;
	
	word_3C_Calculate();
}

void cCreep::word_3C_Calculate() {
	// 5fa6
	word_3C = mMemory[ 0x5CE6 + byte_5FD6 ];
	word_3C += mMemory[ 0x5D06 + byte_5FD6 ] << 8;

	word_3C <<= 1;
	word_3C += 0xC000;

	word_3C += (byte_5FD5 << 1);
}

void cCreep::obj_SlidingPole_Prepare() {
	byte Height;
	byte A, gfxPosX, gfxPosY;

	while( (Height = mMemory[ mObjectPtr ]) ) {

		gfxPosX = mMemory[ mObjectPtr + 1 ];
		gfxPosY = mMemory[ mObjectPtr + 2 ];
		
		byte_5FD5 = (gfxPosX >> 2) - 0x04;
		byte_5FD6 = (gfxPosY >> 3);

		word_3C_Calculate();

		//1781
		for( ; Height; --Height, gfxPosY += 0x08, word_3C += 0x50) {
			if (mMemory[ word_3C ]  & 0x44) {
				mTxtX_0 = gfxPosX - 4;
				mTxtY_0 = gfxPosY;
				
				screenDraw( 2, 0x27, gfxPosX, gfxPosY, 0x25 );
			} else {
				// 17AA
				screenDraw( 0, 0x24, gfxPosX, gfxPosY );
			}

			A = mMemory[ word_3C ];
			A |= 0x10;
			mMemory[ word_3C ] = A;
		}

		mObjectPtr += 0x03;
	}

	++mObjectPtr;
}

void cCreep::obj_Ladder_Prepare() {
	byte Height, gfxPosX, gfxPosY;
	
	while( (Height = mMemory[ mObjectPtr ]) ) {

		// 1800
		gfxPosX = mMemory[ mObjectPtr + 1 ];
		gfxPosY = mMemory[ mObjectPtr + 2 ];

		byte A = gfxPosX >> 2;
		A -= 0x04;

		byte_5FD5 = A;
		A = (gfxPosY >> 3);
		byte_5FD6 = A;

		word_3C_Calculate();

		// 1828
		for(;;) {
			if( (mMemory[ word_3C ] & 0x44) == 0 ) {
				byte  gfxCurrentID;

				if( Height != 1 ) 
					gfxCurrentID = 0x28;
				else
					gfxCurrentID = 0x2B;

				screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );

			} else {
				// 184C
				if( Height == 1 ) {
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;

					screenDraw( 2, 0x29, gfxPosX, gfxPosY, 0x2A );
				} else {
				// 1874
					gfxPosX -= 4;
					mTxtX_0 = gfxPosX;
					mTxtY_0 = gfxPosY;
					screenDraw( 2, 0x2C, gfxPosX, gfxPosY, 0x2D );
					gfxPosX += 4;
				}
			}
			// 189C
			if( Height != mMemory[ mObjectPtr ] )
				mMemory[ word_3C ] = ( mMemory[ word_3C ] | 1);
			
			--Height;
			if( !Height ) { 
				mObjectPtr += 3;
				break;
			}

			mMemory[ word_3C ] |= 0x10;
			gfxPosY += 8;
		
			word_3C += 0x50;
		}
	}

	++mObjectPtr;
}

// 3FD5: Door Opening
void cCreep::obj_Door_Execute( byte pObjectNumber ) {

	if( mRoomObjects[pObjectNumber].Object_field_1 == 0 ) {
		mRoomObjects[pObjectNumber].Object_field_1 = 1;
		mRoomObjects[pObjectNumber].Object_field_2 = 0x0E;

		word_40 = (mRoomObjects[pObjectNumber].objNumber << 3) + mRoomDoorPtr;
		
		mMemory[ word_40 + 2 ] |= 0x80;
		byte A = mMemory[ word_40 + 4 ];

		roomPtrSet( mMemory[ word_40 + 3 ] );
		sub_6009( A );
		mMemory[ word_40 + 2 ] |= 0x80;
	}

	// 4017
	mMemory[ 0x75B7 ] = 0x10 - mRoomObjects[pObjectNumber].Object_field_2;

	sound_PlayEffect( SOUND_DOOR_OPEN );
	byte A = mRoomObjects[pObjectNumber].Object_field_2;

	if( A ) {
		--mRoomObjects[pObjectNumber].Object_field_2;
		A += mRoomAnim[pObjectNumber].mY; 
		mTxtY_0 = A;
		mTxtX_0 = mRoomAnim[pObjectNumber].mX;
		screenDraw( 1, 0, 0, 0, 0x7C );
		return;
	}
	mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
	for(char Y = 5; Y >= 0; --Y ) 
		mMemory[ 0x6390 + Y ] = mRoomObjects[pObjectNumber].color;

	Draw_RoomAnimObject( 0x08, mRoomAnim[pObjectNumber].mX, mRoomAnim[pObjectNumber].mY, 0, pObjectNumber );
}

// 4075: In Front Door
void cCreep::obj_Door_InFront( byte pSpriteNumber, byte pObjectNumber ) {
	if( mRoomObjects[pObjectNumber].Object_field_1 == 0 )
		return;

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	// 4085
	if( mRoomSprites[pSpriteNumber].Sprite_field_1E != 1 )
		return;
	
	byte playerNumber = mRoomSprites[pSpriteNumber].playerNumber;

	if( mMemory[ 0x780D + playerNumber ] != 0 )
		return;

	// Enter the door
	mMemory[ 0x780D + playerNumber ] = 6;
	mRoomSprites[pSpriteNumber].Sprite_field_1B = 0;
	mRoomSprites[pSpriteNumber].Sprite_field_6 = 3;
	
	byte A = mRoomObjects[pObjectNumber].objNumber;
	A <<= 3;

	word_40 = mRoomDoorPtr + A;

	// 40BB

	mRoomSprites[pSpriteNumber].mY = (uint8) mMemory[ word_40 + 1 ] + 0x0F;
	mRoomSprites[pSpriteNumber].mX = mMemory[ word_40 ] + 0x06;
	if( mMemory[ word_40 + 7 ] != 0 )
		mMemory[ 0x785D + mRoomSprites[pSpriteNumber].playerNumber ] = 1;

	//40DD
	word word_41D6 = readLEWord( &mMemory[ word_40 + 3 ] );
	roomPtrSet( (word_41D6 & 0xFF) );
	
	mMemory[ mRoomPtr ] |= MAP_ROOM_VISIBLE;

	playerNumber = mRoomSprites[pSpriteNumber].playerNumber;

	// Set player room / door
	mMemory[ 0x7809 + playerNumber ] = word_41D6 & 0xFF;
	mMemory[ 0x780B + playerNumber ] = (word_41D6 & 0xFF00) >> 8;
}

// 4F5C: Load the rooms' Teleports
void cCreep::obj_Teleport_Prepare() {

	mTxtX_0 = mMemory[ mObjectPtr ];
	mTxtY_0 = mMemory[ mObjectPtr + 1 ] + 0x18;

	for(byte byte_50D0 = 3; byte_50D0 > 0; --byte_50D0) {
		screenDraw( 1, 0, 0, 0, 0x1C );
		
		mTxtX_0 += 0x04;
	}

	byte gfxPosX = mMemory[ mObjectPtr ];
	byte gfxPosY = mMemory[ mObjectPtr + 1 ];

	// Draw the teleport unit
	screenDraw( 0, 0x6F, gfxPosX, gfxPosY, 0 );

	//4fad
	gfxPosX += 0x0C;
	gfxPosY += 0x18;
	screenDraw( 0, 0x1C, gfxPosX, gfxPosY, 0 );

	byte X;
	object_Create( X );
	
	mRoomAnim[X].mObjectType = OBJECT_TYPE_TELEPORTER;
	gfxPosX = mMemory[ mObjectPtr ] + 4;
	gfxPosY = mMemory[ mObjectPtr + 1 ] + 0x18;

	mRoomObjects[X].objNumber = (mObjectPtr & 0xFF);
	mRoomObjects[X].Object_field_1 = (mObjectPtr & 0xFF00) >> 8;
	
	Draw_RoomAnimObject( 0x70, gfxPosX, gfxPosY, 0, X );

	obj_Teleport_SetColour( (mMemory[ mObjectPtr + 2 ] + 2), X );
	
	byte A = 0x20;

    // Draw the Teleport Sources
	while( mMemory[ mObjectPtr + 3 ] ) {
		mMemory[ 0x6E95 ] = mMemory[ 0x6E96 ] = mMemory[ 0x6E97 ] = mMemory[ 0x6E98 ] = A;
		gfxPosX = mMemory[ mObjectPtr + 3 ];
		gfxPosY = mMemory[ mObjectPtr + 4 ];

		screenDraw( 0, 0x72, gfxPosX, gfxPosY, 0 );
		
		mObjectPtr += 2;

		A += 0x10;
	}

	mObjectPtr += 0x04;
}

// 475E: Mummy Releasing
void cCreep::obj_Mummy_Tomb_Execute( byte pObjectNumber ) {
	if( mEngine_Ticks & 3 )
		return;

	byte A;

	--mRoomObjects[pObjectNumber].Object_field_1;
	if( mRoomObjects[pObjectNumber].Object_field_1 == 0 ) {
		mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
		A = 0x66;
	} else {
		// 4776
		if( mRoomObjects[pObjectNumber].Object_field_2 == 0x66 )
			A = 0x11;
		else
			A = 0x66;
	}
	
	// 4784
	for(char Y = 5; Y >= 0; --Y)
		mMemory[ 0x68F0 + Y ] = A;

	// 478C
	mRoomObjects[pObjectNumber].Object_field_2 = A;
	Draw_RoomAnimObject( mRoomAnim[pObjectNumber].mGfxID, mRoomAnim[pObjectNumber].mX, mRoomAnim[pObjectNumber].mY, 0, pObjectNumber );
}

// 4B1A: 
void cCreep::obj_RayGun_Execute( byte pObjectNumber ) {
	byte gfxPosX = 0;
	byte gfxPosY = 0;
	byte Y = 0;

	if( mEngine_Ticks & 3 )
		return;

	word_40 = mRoomRayGunPtr + mRoomObjects[pObjectNumber].objNumber;

	byte A = mRoomAnim[pObjectNumber].mFlags;
	if(!( A & ITM_DISABLE )) {
		if( mIntro || mNoInput )
			return;

		// 4B46
		if(!(mMemory[ word_40 ] & RAYGUN_PLAYER_CONTROLLING) ) {
			mRaygunCount = 0xFF;
			mRaygunTmpVar = 0x00;
			
			for(byte byte_4D5F = 0x01; ((char) byte_4D5F) >= 0; --byte_4D5F) {

				if( mMemory[ 0x780D + byte_4D5F ] == 0 ) {
					byte Y = mMemory[ 0x34D1 + byte_4D5F ];
					char A = (char) mRoomSprites[Y].mY;
					A -= mRoomAnim[pObjectNumber].mY;
					if( A < 0 )
						A = (A ^ 0xFF) + 1;

					if( A < mRaygunCount ) {
						mRaygunCount = A;
						byte A = (uint8) mRoomSprites[Y].mY;

						if( A >= 0xC8 || A < mRoomAnim[pObjectNumber].mY ) {
							mRaygunTmpVar = RAYGUN_MOVE_UP;	// Will Move Up
						} else {
							mRaygunTmpVar = RAYGUN_MOVE_DOWN;	// Will Move Down
						}
					}
				}
				//4B9C
			}
			// 4BA1
			A = 0xFF;
			A ^= RAYGUN_MOVE_UP;
			A ^= RAYGUN_MOVE_DOWN;

			A &= mMemory[ word_40 ];
			A |= mRaygunTmpVar;
			mMemory[ word_40 ] = A;
		}
		//4BB2
		A = mMemory[ word_40 ];

		if( A & RAYGUN_MOVE_UP ) {

			A = mMemory[ word_40 + 4 ];

			// Can RayGun Move Up
			if( A != mMemory[ word_40 + 2 ] ) {
				mMemory[ word_40 + 4 ] = A - 1;
				obj_RayGun_Control_Update( 0x5C );
			} else
				goto s4BD9;

		} else {
			// 4BD4
			if( !(A & RAYGUN_MOVE_DOWN) ) {
s4BD9:;
				obj_RayGun_Control_Update( 0xCC );
				goto s4C27;
			}
			// 4BE1
			A = mMemory[ word_40 + 4 ];

			// Can Raygun Move Down
			if( A >= mRoomObjects[pObjectNumber].Object_field_1 )
				goto s4BD9;

			mMemory[ word_40 + 4 ] = A + 1;
			obj_RayGun_Control_Update( 0xC2 );
		}	
	}
	// 4BF4
	gfxPosX = mRoomAnim[pObjectNumber].mX;
	gfxPosY = mMemory[ word_40 + 4 ];

	A = mMemory[ word_40 ];
	if( A & RAYGUN_TRACK_TO_LEFT )
		A = 4;
	else
		A = 0;

	mRaygunTmpVar = A;
	Y = mMemory[ word_40 + 4 ] & 3;
	Y |= mRaygunTmpVar;

	// Draw the ray gun
	Draw_RoomAnimObject( mMemory[ 0x4D68 + Y ], gfxPosX, gfxPosY, 0, pObjectNumber );

s4C27:;
	A = mMemory[ word_40 ];

	if( A & RAYGUN_PLAYER_CONTROLLING ) {
		A ^= RAYGUN_PLAYER_CONTROLLING;
		mMemory[ word_40 ] = A;

		if( !(A & RAYGUN_FIRED_BY_PLAYER ))
			return;
	} else {
		// 4C3D
		if( mRaygunCount >= 5 )
			return;
	}

	// Fire Weapon

	// 4C44
	A = mMemory[ word_40 ];
	if( (A & RAYGUN_DONT_FIRE) )
		return;

	obj_RayGun_Laser_Sprite_Create( pObjectNumber );

	A |= RAYGUN_DONT_FIRE;
	mMemory[ word_40 ] = A;
}

// 4E32: Teleport?
void cCreep::obj_Teleport_Execute( byte pObjectNumber ) {
	if( mEngine_Ticks & 1 )
		return;

	byte A = seedGet();
	A &= 0x3F;
	
	mMemory[ 0x75CD + 2 ] = A;
	sound_PlayEffect( SOUND_TELEPORT );
	if( mEngine_Ticks & 3  )
		A = 1;
	else
		A = mRoomObjects[pObjectNumber].Object_field_2;

	A <<= 4;
	mMemory[ 0x6E95 ] = mMemory[ 0x6E96 ] = mMemory[ 0x6E97 ] = mMemory[ 0x6E98 ] = A;
	byte gfxPosX = mRoomObjects[pObjectNumber].Object_field_4;
	byte gfxPosY = mRoomObjects[pObjectNumber].Object_field_5;
	screenDraw( 0, 0x72, gfxPosX, gfxPosY, 0 );

	if( mEngine_Ticks & 3 ) 
		A = 0;
	else
		A = mRoomObjects[pObjectNumber].Object_field_2;

	obj_Teleport_SetColour(A, pObjectNumber);
	if( mEngine_Ticks & 3 )
		return;
	
	--mRoomObjects[pObjectNumber].color;
	if( mRoomObjects[pObjectNumber].color )
		return;

	mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
}

// 4C58: Load the rooms' Ray Guns
void cCreep::obj_RayGun_Prepare() {

	mRoomRayGunPtr = mObjectPtr;
	mRaygunCount = 0;

	do {
		// 4C7E
		mMemory[ mObjectPtr ] &=( 0xFF ^ RAYGUN_DONT_FIRE);
		byte gfxPosX = mMemory[ mObjectPtr + 1 ];
		byte gfxPosY = mMemory[ mObjectPtr + 2 ];
		byte gfxCurrentID;

		if( mMemory[ mObjectPtr ] & RAYGUN_TRACK_TO_LEFT )
			gfxCurrentID = 0x5F;
		else
			gfxCurrentID = 0x60;

		// Length
		for( mRaygunTmpVar = mMemory[ mObjectPtr + 3 ]; mRaygunTmpVar; --mRaygunTmpVar ) {
			
			screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );
			gfxPosY += 0x08;
		}

		// 4CCB
		if(!( mMemory[ mObjectPtr ] & RAYGUN_TRACK_ONLY )) {
			byte X;

			object_Create( X );
			
			mRoomAnim[X].mObjectType = OBJECT_TYPE_RAYGUN_LASER;
			mRoomObjects[X].objNumber = mRaygunCount;
			mRoomAnim[X].mFlags |= ITM_EXECUTE;
			
			byte A = mMemory[ mObjectPtr + 3 ];
			A <<= 3;
			A += mMemory[ mObjectPtr + 2 ];
			A -= 0x0B;
			mRoomObjects[X].Object_field_1 = A;
			// 4D01
			if( !(mMemory[ mObjectPtr ] & RAYGUN_TRACK_TO_LEFT) ) {
				A = mMemory[ mObjectPtr + 1 ];
				A += 4;
			} else {
				A = mMemory[ mObjectPtr + 1 ];
				A -= 8;
			}
			mRoomAnim[X].mX = A;
		} 
		
		// 4D1A
		byte X;

		object_Create( X );
		mRoomAnim[X].mObjectType = OBJECT_TYPE_RAYGUN_CONTROL;
		gfxPosX = mMemory[ mObjectPtr + 5 ];
		gfxPosY = mMemory[ mObjectPtr + 6 ];

		Draw_RoomAnimObject( 0x6D, gfxPosX, gfxPosY, 0, X );

		mRoomObjects[X].objNumber = mRaygunCount;

		mObjectPtr += 0x07;
		mRaygunCount += 0x07;

	} while( !(mMemory[ mObjectPtr ] & RAYGUN_END_MARKER) );

	++mObjectPtr;
}

// 49F8: Load the rooms' Keys
void cCreep::obj_Key_Prepare() {
	mRoomKeyPtr = mObjectPtr;

	byte KeyID = 0;
	
	do {

		if( mMemory[ mObjectPtr + 1 ] != 0 ) {
			byte X;
			
			object_Create( X );
			mRoomAnim[X].mObjectType = OBJECT_TYPE_KEY;

			byte gfxPosX = mMemory[ mObjectPtr + 2 ];
			byte gfxPosY = mMemory[ mObjectPtr + 3 ];
			byte gfxCID = mMemory[ mObjectPtr + 1 ];

			mRoomObjects[X].objNumber = KeyID;

			Draw_RoomAnimObject( gfxCID, gfxPosX, gfxPosY, 0, X );
		}

		// 4A47
		KeyID += 0x04;
		mObjectPtr += 0x04;

	} while( mMemory[ mObjectPtr ] != 0 );

	++mObjectPtr;
}

void cCreep::obj_Door_Lock_Prepare() {
	
	byte X, gfxPosX, gfxPosY;

	for( ; mMemory[ mObjectPtr ]; mObjectPtr += 0x05) {
		
		object_Create( X );
		
		mRoomAnim[X].mObjectType = OBJECT_TYPE_DOOR_LOCK;
		gfxPosX = mMemory[ mObjectPtr + 3 ];
		gfxPosY = mMemory[ mObjectPtr + 4 ];
		
		byte A = (mMemory[ mObjectPtr ] << 4);
		A |= mMemory[ mObjectPtr ];

		for( signed char Y = 8; Y >= 0; --Y )
			mMemory[ 0x6C53 + Y ] = A;

		mRoomObjects[X].objNumber = mMemory[ mObjectPtr ];
		mRoomObjects[X].Object_field_1 = mMemory[ mObjectPtr + 2 ];
		Draw_RoomAnimObject( 0x58, gfxPosX, gfxPosY, 0, X );
	}

	++mObjectPtr;
}

void cCreep::obj_Door_Prepare() {
	byte DoorCount = mMemory[mObjectPtr++];
	mRoomDoorPtr = mObjectPtr;
	
	byte X, gfxCurrentID, gfxPosX, gfxPosY;

	for( byte count = 0; count != DoorCount; ++count) {
		X = mMemory[mObjectPtr + 7];
		
		gfxCurrentID  = mMemory[ 0x41D1 + X ];
		gfxPosX = mMemory[ mObjectPtr + 0 ];
		gfxPosY = mMemory[ mObjectPtr + 1 ];

		screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY );
		// 4159

		object_Create( X );
		
		gfxPosX += 0x04;
		gfxPosY += 0x10;

		mRoomObjects[X].objNumber = count;
		mRoomAnim[X].mObjectType = OBJECT_TYPE_DOOR;

		roomPtrSet( mMemory[ mObjectPtr + 3 ] );
		
		byte A = mMemory[ mRoomPtr ] & 0xF;

		mRoomObjects[X].color = A;
		A <<= 4;

		A |= mRoomObjects[X].color;
		mRoomObjects[X].color = A;
		
		A = mMemory[ mObjectPtr + 2 ];
		if(A & 0x80) {
			mRoomObjects[X].Object_field_1 = 1;
			A = mRoomObjects[X].color;

			for(byte Y = 5; Y; --Y ) 
				mMemory[ 0x6390 + Y ] = A;

			A = 0x08;
		} else
			A = 0x07;

		// 41B2
		gfxCurrentID = A;
		Draw_RoomAnimObject( gfxCurrentID, gfxPosX, gfxPosY, 0, X );

		mObjectPtr += 0x08;
	}

}

// 42AD: Lightning Machine pole movement
void cCreep::obj_Lightning_Pole_Execute( byte pObjectNumber ) {
	byte gfxPosX, gfxPosY;

	byte byte_43E2, byte_43E3;
	word_40 = mRoomLightningPtr + mRoomObjects[pObjectNumber].objNumber;
	byte Y = 0;

	if( mRoomObjects[pObjectNumber].Object_field_1 != 1 ) {
		mRoomObjects[pObjectNumber].Object_field_1 = 1;

		obj_Lightning_Sprite_Create( pObjectNumber );

	} else {
		// 42CF
		if( !(mMemory[ word_40 + Y ] & LIGHTNING_IS_ON )) {
			// 42D8
			mRoomObjects[pObjectNumber].Object_field_1 = 0;
			mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
			mMemory[ 0x66E6 ] = mMemory[ 0x66E7 ] = 0x55;

			gfxPosX = mMemory[ word_40 + 1 ];
			gfxPosY = mMemory[ word_40 + 2 ];
			
			for( byte_43E2 = mMemory[ word_40 + 3 ]; byte_43E2; --byte_43E2 ) {

				screenDraw( 0, 0x34, gfxPosX, gfxPosY, 0 );
				gfxPosY += 0x08;
			}

			// 4326
			for( Y = 0; ;Y++ ) {

				if( mRoomSprites[Y].mSpriteType == SPRITE_TYPE_LIGHTNING ) {
					if( !(mRoomSprites[Y].state & SPR_UNUSED) )
						if( mRoomSprites[Y].Sprite_field_1F == mRoomObjects[pObjectNumber].objNumber )
							break;
				}
			}

			// 4345
			mRoomSprites[Y].state |= SPR_ACTION_DESTROY;			// Turning Off 
            mScreen->clear(0);
            mScreen->refresh();
			return;

		} else {
			// 4351
			if( mEngine_Ticks & 3 )
				return;
		}
	}
	// 435B
	++mRoomObjects[pObjectNumber].Object_field_2;

	if( mRoomObjects[pObjectNumber].Object_field_2 >= 3 )
		mRoomObjects[pObjectNumber].Object_field_2 = 0;
	
	byte_43E3 = mRoomObjects[pObjectNumber].Object_field_2;
	
	gfxPosX = mMemory[ word_40 + 1 ];
	gfxPosY = mMemory[ word_40 + 2 ];

	for(byte_43E2 = mMemory[ word_40 + 3 ]; byte_43E2; --byte_43E2 ) {

		if( byte_43E3 ) {
			if( byte_43E3 != 1 ) {
				mMemory[ 0x66E6 ] = 0x66;
				mMemory[ 0x66E7 ] = 0x01;

			} else {
			// 43B4
				mMemory[ 0x66E6 ] = 0x61;
				mMemory[ 0x66E7 ] = 0x06;
			}
		} else {
			// 43A7
			mMemory[ 0x66E6 ] = 0x16;
			mMemory[ 0x66E7 ] = 0x06;
		}

		// 43BE
		screenDraw( 0, 0x34, gfxPosX, gfxPosY, 0 );
		++byte_43E3;
		if( byte_43E3 >= 3 )
			byte_43E3 = 0;

		gfxPosY += 0x08;
	}

}

void cCreep::obj_Door_Button_Prepare() {
	byte gfxCurrentID, gfxPosX, gfxPosY;
	byte X = 0;

	for( byte ButtonCount = mMemory[ mObjectPtr++ ]; ButtonCount; --ButtonCount) {

		object_Create( X );
		mRoomAnim[X].mObjectType = OBJECT_TYPE_DOOR_BUTTON;

		gfxPosX = mMemory[mObjectPtr];
		gfxPosY = mMemory[mObjectPtr+1];
		gfxCurrentID = 0x09;
		mRoomObjects[X].objNumber = mMemory[ mObjectPtr+2 ];
		
		byte A = 0;

		// Find the colour of the door this button connects to
		for( unsigned char Y = 0; Y < MAX_OBJECTS; ++Y ) {
			if( mRoomAnim[Y].mObjectType != OBJECT_TYPE_DOOR ) 
				continue;

			if( mRoomObjects[Y].objNumber == mRoomObjects[X].objNumber ) {
				A = mRoomObjects[Y].color;
				break;
			}	
		}

		// Set the palette color
		for( signed char Y = 8; Y >= 0; --Y )
			mMemory[ 0x63D2 + Y ] = A;

		A >>= 4;
		A |= 0x10;

		mMemory[ 0x63D6 ] = A;
		Draw_RoomAnimObject( gfxCurrentID, gfxPosX, gfxPosY, 0, X);
		mObjectPtr += 0x03;
	}

}

// 44E7: Lightning Switch
void cCreep::obj_Lightning_Switch_InFront( byte pSpriteNumber, byte pObjectNumber ) {
	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	if( (mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth) - mRoomAnim[pObjectNumber].mX >= 4 )
		return;

	if( mRoomSprites[pSpriteNumber].Sprite_field_1E != 0 && mRoomSprites[pSpriteNumber].Sprite_field_1E != 4 )
		return;

	// 4507
	
	word_30 = mRoomLightningPtr + mRoomObjects[pObjectNumber].objNumber;

	if( !(mMemory[ word_30 ] & LIGHTNING_IS_ON )) {
		if( mRoomSprites[pSpriteNumber].Sprite_field_1E )
			return;
	} else {
		if( !(mRoomSprites[pSpriteNumber].Sprite_field_1E) )
			return;
	}
	
	// 4535
	mMemory[ word_30 ] ^= LIGHTNING_IS_ON;
	for(byte byte_45D7 = 0; byte_45D7 < 4; ++byte_45D7) {

		byte A = mMemory[ word_30 + (byte_45D7 + 4) ];
		if( A == 0xFF )
			break;

		// 4553
		word_32 = mRoomLightningPtr + A;
		
		mMemory[ word_32 ] ^= LIGHTNING_IS_ON;
		byte Y;

		for( Y = 0; Y < MAX_OBJECTS; ++Y ) {
			
			if( mRoomAnim[Y].mObjectType != OBJECT_TYPE_LIGHTNING_MACHINE )
				continue;

			if( mRoomObjects[Y].objNumber == A )
				break;
		}

		// 4585
		mRoomAnim[Y].mFlags |= ITM_EXECUTE;
	}

	byte A;

	// 4594
	if( !(mMemory[ word_30 ] & LIGHTNING_IS_ON )) {
		mMemory[ 0x75E7 ] = 0x2F;
		A = 0x38;
	} else {
		mMemory[ 0x75E7 ] = 0x23;
		A = 0x37;
	}

	Draw_RoomAnimObject( A, mRoomAnim[pObjectNumber].mX, mRoomAnim[pObjectNumber].mY, 0, pObjectNumber );

	sound_PlayEffect( SOUND_LIGHTNING_SWITCHED );
}

// 45E0: Forcefield Timer
void cCreep::obj_Forcefield_Timer_Execute( byte pObjectNumber ) {

	if( --mRoomObjects[pObjectNumber].Object_field_1 != 0 )
		return;

	--mRoomObjects[pObjectNumber].Object_field_2;
	byte A = mMemory[ 0x4756 + mRoomObjects[pObjectNumber].Object_field_2 ];

	mMemory[ 0x75AB ] = A;
	sound_PlayEffect( SOUND_FORCEFIELD_TIMER );

	for( byte Y = 0; Y < 8; ++Y ) {
		if( Y >= mRoomObjects[pObjectNumber].Object_field_2 )
			A = 0x55;
		else
			A = 0;

		mMemory[ 0x6889 + Y ] = A;
	}

	screenDraw( 0, 0x40, mRoomAnim[pObjectNumber].mX, mRoomAnim[pObjectNumber].mY, 0 );
	if( mRoomObjects[pObjectNumber].Object_field_2 != 0 ) {
		mRoomObjects[pObjectNumber].Object_field_1 = 0x1E;
		return;
	} 

	// 4633
	mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
	mMemory[ 0x4750 + mRoomObjects[pObjectNumber].objNumber ] = 1;
}

// Lightning Machine Setup
void cCreep::obj_Lightning_Prepare() {
	byte	LightningCount, gfxPosX, gfxPosY;

	mRoomLightningPtr = mObjectPtr;
	LightningCount = 0;

	byte X = 0, A;

	do {

		object_Create( X );

		mRoomObjects[X].objNumber = LightningCount;

		if( mMemory[ mObjectPtr ] & LIGHTNING_IS_SWITCH ) {
			// 441C
			gfxPosX = mMemory[ mObjectPtr + 1 ];
			gfxPosY = mMemory[ mObjectPtr + 2 ];
			screenDraw( 0, 0x36, gfxPosX, gfxPosY, 0 );
			
			gfxPosX += 0x04;
			gfxPosY += 0x08;

			mRoomAnim[X].mObjectType = OBJECT_TYPE_LIGHTNING_CONTROL;
			if( mMemory[ mObjectPtr ] & LIGHTNING_IS_ON )
				A = 0x37;
			else
				A = 0x38;

			Draw_RoomAnimObject( A, gfxPosX, gfxPosY, 0, X );

		} else {
			// 4467
			mRoomAnim[X].mObjectType = OBJECT_TYPE_LIGHTNING_MACHINE;
			gfxPosX = mMemory[ mObjectPtr + 1 ];
			gfxPosY = mMemory[ mObjectPtr + 2 ];
			
			mRoomObjects[X].color = mMemory[ mObjectPtr + 3 ];

			for( byte Height = mMemory[ mObjectPtr + 3 ]; Height; --Height) {

				screenDraw( 0, 0x32, gfxPosX, gfxPosY, 0 );
				gfxPosY += 0x08;
			}

			gfxPosX -= 0x04;

			Draw_RoomAnimObject( 0x33, gfxPosX, gfxPosY, 0, X );
			if( mMemory[ mObjectPtr ] & LIGHTNING_IS_ON )
				mRoomAnim[X].mFlags |= ITM_EXECUTE;
		}

		// 44C8
		LightningCount += 0x08;
		mObjectPtr += 0x08;

	} while ( !(mMemory[ mObjectPtr ] & LIGHTNING_END_MARKER) );

	++mObjectPtr;
}

// 46AE: Forcefield
void cCreep::obj_Forcefield_Prepare() {
	byte X = 0;
	byte gfxPosX, gfxPosY;

	byte ForcefieldCount = 0;
	
	do {

		object_Create( X );

		mRoomAnim[X].mObjectType = OBJECT_TYPE_FORCEFIELD;

		gfxPosX = mMemory[ mObjectPtr ];
		gfxPosY = mMemory[ mObjectPtr + 1 ];

		// Draw outside of timer
		screenDraw( 0, 0x3F, gfxPosX, gfxPosY, 0 );

		// 46EA
		gfxPosX += 4;
		gfxPosY += 8;

		for( signed char Y = 7; Y >= 0; --Y ) 
			mMemory[ 0x6889 + Y ] = 0x55;

		// Draw inside of timer
		Draw_RoomAnimObject( 0x40, gfxPosX, gfxPosY, 0, X );

		mRoomObjects[X].objNumber = ForcefieldCount;
		mMemory[ 0x4750 + ForcefieldCount ] = 1;

		obj_Forcefield_Create( ForcefieldCount );

		gfxPosX = mMemory[ mObjectPtr + 2 ];
		gfxPosY = mMemory[ mObjectPtr + 3 ];

		// Draw top of forcefield
		screenDraw( 0, 0x3E, gfxPosX, gfxPosY, 0 );
		
		++ForcefieldCount;
		mObjectPtr += 0x04;
	} while( mMemory[ mObjectPtr ] );

	++mObjectPtr;
}

// 4872 : Load the rooms' Mummys
void cCreep::obj_Mummy_Prepare( ) {
	byte	ObjectNumber = 0, YCount, XCount;
	byte	X;
	byte	gfxCurrentID;

	mRoomMummyPtr = mObjectPtr;

	do {
		object_Create( X );
		
		mRoomAnim[X].mObjectType = OBJECT_TYPE_MUMMY;

		byte gfxPosX = mMemory[ mObjectPtr + 1 ];
		byte gfxPosY = mMemory[ mObjectPtr + 2 ];
		gfxCurrentID = 0x44;

		mRoomObjects[X].objNumber = ObjectNumber;
		mRoomObjects[X].Object_field_2 = 0x66;
		for( signed char Y = 5; Y >= 0; --Y )
			mMemory[ 0x68F0 + Y ] = 0x66;

		Draw_RoomAnimObject( gfxCurrentID, gfxPosX, gfxPosY, 0, X );
		
		gfxPosY = mMemory[ mObjectPtr + 4 ];
		gfxCurrentID = 0x42;

		for( YCount = 3; YCount; --YCount ) {
			
			gfxPosX = mMemory[ mObjectPtr + 3 ];

			for( XCount = 5; XCount; --XCount ) {
				screenDraw( 0, gfxCurrentID, gfxPosX, gfxPosY, 0 );
				gfxPosX += 4;
			}

			gfxPosY += 8;
		}

		if( mMemory[ mObjectPtr ] != 1 ) {
			// 4911
			mTxtX_0 = mMemory[ mObjectPtr + 3 ] + 4;
			mTxtY_0 = mMemory[ mObjectPtr + 4 ] + 8;
			

			for( YCount = 3; YCount; --YCount ) {
				screenDraw( 1, gfxCurrentID, gfxPosX, gfxPosY, 0x42 );
				mTxtX_0 += 4;
			}
			
			gfxPosX = mTxtX_0 - 0x0C;
			gfxPosY = mTxtY_0;
			screenDraw( 0, 0x43, gfxPosX, gfxPosY, 0x42 );
			
			if( mMemory[ mObjectPtr ] == 2 )
				obj_Mummy_Sprite_Create(0xFF, X);
		}
		// 496E
		mObjectPtr += 0x07;
		ObjectNumber += 0x07;

	} while( mMemory[ mObjectPtr ] );

	++mObjectPtr;
}

// 517F : Load the rooms' Trapdoors
void cCreep::obj_TrapDoor_Prepare( ) {
	byte	byte_5381;
	mRoomTrapDoorPtr = mObjectPtr;

	byte X;

	byte_5381 = 0;
	do {
		
		object_Create( X );
		mRoomAnim[X].mObjectType = OBJECT_TYPE_TRAPDOOR_PANEL;
		mRoomObjects[X].objNumber = byte_5381;

		if( !(mMemory[ mObjectPtr ] & TRAPDOOR_OPEN) ) {
			// 51BC
			mMemory[ 0x6F2E ] = 0xC0;
			mMemory[ 0x6F30 ] = 0x55;
		} else {
			// 51c9
			mTxtX_0 = mMemory[ mObjectPtr + 1 ];
			mTxtY_0 = mMemory[ mObjectPtr + 2 ];
			screenDraw( 1, 0, 0, 0, 0x7B );
			
			byte gfxPosX, gfxPosY;

			gfxPosX = mTxtX_0 + 4;
			gfxPosY = mTxtY_0;

			Draw_RoomAnimObject( 0x79, gfxPosX, gfxPosY, 0x7B, X );

			mMemory[ 0x6F2E ] = 0x20;
			mMemory[ 0x6F30 ] = 0xCC;
			byte_5FD5 = mMemory[ mObjectPtr + 1 ] >> 2;
			byte_5FD5 -= 4;
			
			byte_5FD6 = mMemory[ mObjectPtr + 2 ] >> 3;
			word_3C_Calculate();

			mMemory[ word_3C ] = mMemory[ word_3C ] & 0xFB;
			mMemory[ word_3C + 4 ] = mMemory[ word_3C + 4 ] & 0xBF;
		}

		// 522E
		object_Create( X );
		mRoomAnim[X].mObjectType = OBJECT_TYPE_TRAPDOOR_SWITCH;
		
		byte gfxPosX = mMemory[ mObjectPtr + 3 ];
		byte gfxPosY = mMemory[ mObjectPtr + 4 ];
		
		mRoomObjects[X].objNumber = byte_5381;
		Draw_RoomAnimObject( 0x7A, gfxPosX, gfxPosY, 0, X );
		
		byte_5381 += 0x05;
		mObjectPtr += 0x05;

	} while( !(mMemory[ mObjectPtr ] & TRAPDOOR_END_MARKER) );

	++mObjectPtr;
}

// 50D2: Floor Switch
void cCreep::obj_TrapDoor_Switch_Execute( byte pObjectNumber ) {
	
	word_40 = mRoomTrapDoorPtr + mRoomObjects[pObjectNumber].objNumber;
	if( mRoomObjects[pObjectNumber].Object_field_1 ) {
		mTxtX_0 = mMemory[ word_40 + 1 ];
		mTxtY_0 = mMemory[ word_40 + 2 ];

		byte A = mRoomObjects[pObjectNumber].Object_field_2;

		obj_TrapDoor_PlaySound( A );
		screenDraw( 1, 0, 0, 0, A );
		if( mRoomObjects[pObjectNumber].Object_field_2 != 0x78 ) {
			// 515F
			++mRoomObjects[pObjectNumber].Object_field_2;
			return;
		}
		
		Draw_RoomAnimObject( 0x79, mMemory[ word_40 + 1 ] + 4, mMemory[ word_40 + 2 ], 0, pObjectNumber );
		
	} else {
		// 5129
		if( mRoomObjects[pObjectNumber].Object_field_2 == 0x78 )
			roomAnim_Disable( pObjectNumber );
		
		byte A = mRoomObjects[pObjectNumber].Object_field_2;

		obj_TrapDoor_PlaySound( A );
		screenDraw( 0, A, mMemory[ word_40 + 1 ], mMemory[ word_40 + 2 ], 0 );
		if( mRoomObjects[pObjectNumber].Object_field_2 != 0x73 ) {
			--mRoomObjects[pObjectNumber].Object_field_2;
			return;
		}
	}
	// 5165
	mRoomAnim[pObjectNumber].mFlags ^= ITM_EXECUTE;
}

// 538B: Conveyor
void cCreep::obj_Conveyor_Execute( byte pObjectNumber ) {
	
	word_40 = mConveyorPtr + mRoomObjects[pObjectNumber].objNumber;
	byte A = mMemory[ word_40 ];

	// 539F
	if( (A & CONVEYOR_PLAYER1_SWITCHED) && !(A & CONVEYOR_PLAYER1_ENABLEDIT) ||
		(A & CONVEYOR_PLAYER2_SWITCHED) && !(A & CONVEYOR_PLAYER2_ENABLEDIT) ) {

			if( A & CONVEYOR_TURNED_ON ) {
				
				A ^= CONVEYOR_TURNED_ON;
				A ^= CONVEYOR_MOVING_RIGHT;

				mMemory[ word_40 ] = A;
				mMemory[ 0x70A8 ] = mMemory[ 0x70A6 ] = 0xC0;
				mMemory[ 0x7624 ] = 0x18;

			} else {
				// 53D0
				A |= CONVEYOR_TURNED_ON;
				mMemory[ word_40 ] = A;
				if( A & CONVEYOR_MOVING_RIGHT ) {
					mMemory[ 0x70A6 ] = 0x50;
					mMemory[ 0x70A8 ] = 0xC0;
					mMemory[ 0x7624 ] = 0x18;
				} else {
					// 53EC
					mMemory[ 0x70A6 ] = 0xC0;
					mMemory[ 0x70A8 ] = 0x20;
					mMemory[ 0x7024 ] = 0x18;
				}
			}

			// 53FB
			byte gfxPosX = mMemory[ word_40 + 3 ];
			byte gfxPosY = mMemory[ word_40 + 4 ];
			screenDraw( 0, 0x82, gfxPosX, gfxPosY, 0 );

			sound_PlayEffect( SOUND_CONVEYOR_SWITCH );
	}

	// 541B
	A = 0xFF;

	A ^= CONVEYOR_PLAYER1_ENABLEDIT;
	A ^= CONVEYOR_PLAYER2_ENABLEDIT;
	A &= mMemory[ word_40 ];
	// 5427
	if( A & CONVEYOR_PLAYER1_SWITCHED ) {
		A |= CONVEYOR_PLAYER1_ENABLEDIT;
		A ^= CONVEYOR_PLAYER1_SWITCHED;
	}
	if( A & CONVEYOR_PLAYER2_SWITCHED ) {
		A |= CONVEYOR_PLAYER2_ENABLEDIT;
		A ^= CONVEYOR_PLAYER2_SWITCHED;
	}

	mMemory[ word_40 ] = A;
	// 543F
	if( A & CONVEYOR_TURNED_ON ) {

		if( mEngine_Ticks & 1 )
			return;

		byte gfxCurrentID = mRoomAnim[pObjectNumber].mGfxID;

		if( !(mMemory[ word_40 ] & CONVEYOR_MOVING_RIGHT) ) {
			// 5458
			++gfxCurrentID;
			if( gfxCurrentID >= 0x82 )
				gfxCurrentID = 0x7E;

		} else {
			//546a
			--gfxCurrentID;
			if( gfxCurrentID < 0x7E )
				gfxCurrentID = 0x81;
		}
		// 5479
		byte gfxPosX = mRoomAnim[pObjectNumber].mX;
		byte gfxPosY = mRoomAnim[pObjectNumber].mY;
		
		Draw_RoomAnimObject( gfxCurrentID, gfxPosX, gfxPosY, 0, pObjectNumber );
	}
}

// 47A7: In Front Mummy Release
void cCreep::obj_Mummy_Infront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	byte A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	A -= mRoomAnim[pObjectNumber].mX;

	if( A >= 8 )
		return;

	word_40 = mRoomMummyPtr + mRoomObjects[pObjectNumber].objNumber;

	// 47D1
	if( mMemory[ word_40 ] != 1 )
		return;

	mMemory[ word_40 ] = 2;
	mMemory[ word_40 + 5 ] = mMemory[ word_40 + 3 ] + 4;
	// 47ED
	mMemory[ word_40 + 6 ] = mMemory[ word_40 + 4 ] + 7;
	
	// 47FB
	mRoomAnim[pObjectNumber].mFlags |= ITM_EXECUTE;

	mRoomObjects[pObjectNumber].Object_field_1 = 8;
	mRoomObjects[pObjectNumber].Object_field_2 = 0x66;
	mTxtX_0 = mMemory[ word_40 + 3 ] + 4;
	mTxtY_0 = mMemory[ word_40 + 4 ] + 8;
	
	for( byte byte_4871 = 3; byte_4871 ; --byte_4871) {
		screenDraw( 1, 0, 0, 0, 0x42 );
		mTxtX_0 += 4;
	}

	// 4842
	byte gfxPosX = mTxtX_0 - 0x0C;
	byte gfxPosY = mTxtY_0;

	screenDraw( 0, 0x43, gfxPosX, gfxPosY, 0 );
	obj_Mummy_Sprite_Create(0, pObjectNumber );
}

// 564E: Load the rooms' frankensteins
void cCreep::obj_Frankie_Prepare() {
	mFrankiePtr = mObjectPtr;

	mFrankieCount = 0;

	do {

		mTxtX_0 = mMemory[ mObjectPtr + 1 ];
		mTxtY_0 = mMemory[ mObjectPtr + 2 ] + 0x18;
		screenDraw( 1, 0, 0, 0, 0x92 );

		byte_5FD5 = (mTxtX_0 >> 2) - 4;
		byte_5FD6 = (mTxtY_0 >> 3);

		word_3C_Calculate();
		byte A;

		if( ( mMemory[ mObjectPtr ] & FRANKIE_POINTING_LEFT )) {
			word_3C -= 2;
			A = 0xFB;
		} else
			A = 0xBF;

		// 56C4
		for( signed char Y = 4; Y >= 0; Y -= 2 ) {
			mMemory[ word_3C + Y ] &= A;
		}
		byte X;
		object_Create( X );
		mRoomAnim[X].mObjectType = OBJECT_TYPE_FRANKIE;
		byte gfxPosX = mMemory[ mObjectPtr + 1 ];
		byte gfxPosY = mMemory[ mObjectPtr + 2 ];
		if( !(mMemory[ mObjectPtr ] & FRANKIE_POINTING_LEFT ))
			A = 0x90;
		else
			A = 0x91;

		Draw_RoomAnimObject( A, gfxPosX, gfxPosY, 0, X );

		//5700
		if(!( mMemory[ mObjectPtr ]  & FRANKIE_POINTING_LEFT )) {
			gfxPosX += 4;
			gfxPosY += 0x18;
			screenDraw( 0, 0x1C, gfxPosX, gfxPosY, 0 );
		}
		obj_Frankie_Sprite_Create();

		mObjectPtr += 0x07;
		mFrankieCount += 0x07;

	} while( !(mMemory[ mObjectPtr ] & FRANKIE_END_MARKER) );

	++mObjectPtr;
}

// 5501: Load the rooms' Conveyors
void cCreep::obj_Conveyor_Prepare() {
	mConveyorPtr = mObjectPtr;

	byte byte_5649 = 0, gfxPosX = 0, gfxPosY = 0;
	
	byte A, X;

	do {
		
		//5527
		A = 0xFF;

		A ^= CONVEYOR_PLAYER1_SWITCHED;
		A ^= CONVEYOR_PLAYER2_SWITCHED;
		A ^= CONVEYOR_PLAYER1_ENABLEDIT;
		A ^= CONVEYOR_PLAYER2_ENABLEDIT;

		A &= mMemory[ mObjectPtr ];
		mMemory[ mObjectPtr ] = A;

		object_Create( X );
		
		mRoomAnim[X].mObjectType = OBJECT_TYPE_CONVEYOR_BELT;
		mRoomObjects[X].objNumber = byte_5649;

		mRoomAnim[X].mFlags = (mRoomAnim[X].mFlags | ITM_EXECUTE);
		
		mTxtX_0 = mMemory[ mObjectPtr + 1 ];
		mTxtY_0 = mMemory[ mObjectPtr + 2 ];

		screenDraw( 1, 0x7D, gfxPosX, gfxPosY, 0x7D );
		gfxPosX = mTxtX_0;
		gfxPosY = mTxtY_0;
		
		Draw_RoomAnimObject( 0x7E, gfxPosX, gfxPosY, 0x7D, X );
		object_Create( X );

		mRoomAnim[X].mObjectType = OBJECT_TYPE_CONVEYOR_CONTROL;
		mRoomObjects[X].objNumber = byte_5649;
		
		gfxPosX = mMemory[ mObjectPtr + 3 ];
		gfxPosY = mMemory[ mObjectPtr + 4 ];
		byte gfxCurrentID = 0x82;
		byte gfxDecodeMode = 0;

		if( (mMemory[ mObjectPtr ] & CONVEYOR_TURNED_ON) == 0 ) {
			mMemory[ 0x70A6 ] = 0xC0;
			mMemory[ 0x70A8 ] = 0xC0;
		} else {
			if( (mMemory[ mObjectPtr ] & CONVEYOR_MOVING_RIGHT) == 0 ) {
				// 55BE
				mMemory[ 0x70A6 ] = 0xC0;
				mMemory[ 0x70A8 ] = 0x20;
			} else {
				// 55CB
				mMemory[ 0x70A6 ] = 0x50;
				mMemory[ 0x70A8 ] = 0xC0;
			}
		}

		screenDraw( gfxDecodeMode, gfxCurrentID, gfxPosX, gfxPosY, 0 );
		gfxPosX = mMemory[ mObjectPtr + 3] + 0x04;
		gfxPosY = mMemory[ mObjectPtr + 4] + 0x08;

		Draw_RoomAnimObject( 0x83, gfxPosX, gfxPosY, 0, X );
		byte_5649 += 0x05;
		mObjectPtr += 0x05;

	} while( !(mMemory[ mObjectPtr ] & CONVEYOR_END_MARKER) );

	++mObjectPtr;
}

void cCreep::obj_Forcefield_Create( byte pObjectNumber ) {
	sCreepSprite *sprite = Sprite_CreepGetFree();

	sprite->mSpriteType = SPRITE_TYPE_FORCEFIELD;
	sprite->mX = mMemory[ mObjectPtr + 2 ];
	sprite->mY = mMemory[ mObjectPtr + 3 ] + 2;
	sprite->spriteImageID= 0x35;

	sprite->Sprite_field_1F = pObjectNumber;
	sprite->Sprite_field_1E = 0;
	sprite->Sprite_field_6 = 4;
	sprite->mWidth = 2;
	sprite->mHeight = 0x19;
}

// 38CE: Mummy ?
bool cCreep::obj_Mummy_Collision( byte pSpriteNumber, byte pObjectNumber ) {
	if( mRoomAnim[pObjectNumber].mObjectType == OBJECT_TYPE_TRAPDOOR_PANEL ) {
		
		char A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
		A -= mRoomAnim[pObjectNumber].mX;
		if( A < 4 ) {
			
			word_40 = mRoomTrapDoorPtr + mRoomObjects[pObjectNumber].objNumber;
			// 38F7
			if( mMemory[ word_40 ] & TRAPDOOR_OPEN ) {
				// 3900
				word_40 = mRoomMummyPtr + mRoomSprites[pSpriteNumber].mButtonState;
				mMemory[ word_40 ] = 3;
				return true;
			}
		}
	} 

	// 3919
	if( mRoomAnim[pObjectNumber].mObjectType != OBJECT_TYPE_TRAPDOOR_SWITCH )
		return false;

	char A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	A -= mRoomAnim[pObjectNumber].mX;
	if( A >= 4 )
		return false;

	mRoomSprites[pSpriteNumber].playerNumber = mRoomObjects[pObjectNumber].objNumber;
	return false;
}

// 3940: 
bool cCreep::obj_Mummy_Sprite_Collision( byte pSpriteNumber, byte pSpriteNumber2 ) {

	if( mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_PLAYER 
	 || mRoomSprites[pSpriteNumber2].mSpriteType == SPRITE_TYPE_FRANKIE ) {
		return false;
	}

	word_40 = mRoomMummyPtr + mRoomSprites[pSpriteNumber].mButtonState;

	mMemory[ word_40 ] = 3;
	return true;
}

// 3A60:  
bool cCreep::obj_RayGun_Laser_Collision( byte pSpriteNumber, byte pObjectNumber ) {
	if( mRoomAnim[pObjectNumber].mObjectType == OBJECT_TYPE_LIGHTNING_MACHINE )
		return true;

	if( mRoomAnim[pObjectNumber].mObjectType == OBJECT_TYPE_FRANKIE )
		return true;

	if( mRoomAnim[pObjectNumber].mObjectType == OBJECT_TYPE_RAYGUN_LASER ) {
		if( mRoomSprites[pSpriteNumber].Sprite_field_1E != mRoomObjects[pObjectNumber].objNumber )
			return true;
	}

	return false;
}

void cCreep::obj_RayGun_Laser_Sprite_Create( byte pObjectNumber ) {
	byte A = mRoomObjects[pObjectNumber].objNumber + 0x07;
	A |= 0xF8;
	A >>= 1;
	A += 0x2C;
	
	mMemory[ 0x7591 + 2 ] = A;

	sound_PlayEffect( SOUND_LASER_FIRED );

	byte X = Sprite_CreepFindFree( );

	mRoomSprites[X].mSpriteType = SPRITE_TYPE_LASER;
	mRoomSprites[X].mX= mRoomAnim[pObjectNumber].mX;
	mRoomSprites[X].mY= mRoomAnim[pObjectNumber].mY + 0x05;
	mRoomSprites[X].spriteImageID= 0x6C;
	mRoomSprites[X].Sprite_field_1E = mRoomObjects[pObjectNumber].objNumber;

	if( mMemory[ word_40 ] & RAYGUN_TRACK_TO_LEFT ) {
		mRoomSprites[X].mX-= 0x08;
		mRoomSprites[X].Sprite_field_1F = 0xFC;
	} else {
		// 3AD4	
		mRoomSprites[X].mX+= 0x08;
		mRoomSprites[X].Sprite_field_1F = 4;
	}

	hw_SpritePrepare( X );
}

// 396A: 
void cCreep::obj_Mummy_Sprite_Create( byte pA, byte pObjectNumber ) {
	byte sprite = Sprite_CreepFindFree( );
	
	mRoomSprites[ sprite ].mSpriteType = SPRITE_TYPE_MUMMY;
	mRoomSprites[ sprite ].Sprite_field_1B = 0xFF;
	mRoomSprites[ sprite ].playerNumber = 0xFF;
	mRoomSprites[ sprite ].mButtonState = mRoomObjects[pObjectNumber].objNumber;
	
	word_40 = mRoomMummyPtr + mRoomSprites[ sprite ].mButtonState;
	//3998

	mRoomSprites[ sprite ].mWidth = 5;
	mRoomSprites[ sprite ].mHeight = 0x11;
	mRoomSprites[ sprite ].spriteImageID= 0xFF;
	if( pA == 0 ) {
		mRoomSprites[ sprite ].Sprite_field_1E = 0;
		mRoomSprites[ sprite ].Sprite_field_1F = 0xFF;
		mRoomSprites[ sprite ].Sprite_field_6 = 4;
		
		mRoomSprites[ sprite ].mX= mMemory[ word_40 + 3 ] + 0x0D;
		mRoomSprites[ sprite ].mY= mMemory[ word_40 + 4 ] + 0x08;
	} else {
		// 39D0
		mRoomSprites[ sprite ].Sprite_field_1E = 1;
		mRoomSprites[ sprite ].mX= mMemory[ word_40 + 5 ];
		mRoomSprites[ sprite ].mY= mMemory[ word_40 + 6 ];
		mRoomSprites[ sprite ].Sprite_field_6 = 2;
	}

	// 39E8
}

// 379A: Mummy
void cCreep::obj_Mummy_Execute( byte pSpriteNumber ) {

	if( mRoomSprites[pSpriteNumber].state & SPR_ACTION_DESTROY ) {
		mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_DESTROY;
		mRoomSprites[pSpriteNumber].state |= SPR_ACTION_FREE;
		return;
	}

	if( mRoomSprites[pSpriteNumber].state & SPR_ACTION_CREATED ) {

		mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_CREATED;

		if( mRoomSprites[pSpriteNumber].Sprite_field_1E ) {
			mRoomSprites[pSpriteNumber].spriteImageID = 0x4B;
			hw_SpritePrepare( pSpriteNumber );
		}
	}
	// 37C6
	char AA = mRoomSprites[pSpriteNumber].playerNumber;
	if( AA != -1 ) {
		if( AA != mRoomSprites[pSpriteNumber].Sprite_field_1B )
			obj_TrapDoor_Switch_Check( AA );
	}
	// 37D5
	mRoomSprites[pSpriteNumber].Sprite_field_1B = AA;
	mRoomSprites[pSpriteNumber].playerNumber = 0xFF;
	word_40 = mRoomMummyPtr + mRoomSprites[pSpriteNumber].mButtonState;
	if( mRoomSprites[pSpriteNumber].Sprite_field_1E == 0 ) {
		++mRoomSprites[pSpriteNumber].Sprite_field_1F;
		byte Y = mRoomSprites[pSpriteNumber].Sprite_field_1F;
		byte A = mMemory[ 0x39EF + Y ];

		if( A != 0xFF ) {
			mRoomSprites[pSpriteNumber].spriteImageID = A;
			mRoomSprites[pSpriteNumber].mX += mMemory[ 0x39F7 + Y ];

			mRoomSprites[pSpriteNumber].mY += (int8_t) mMemory[ 0x39FF + Y ];
			
			mMemory[ 0x7630 ] = (mRoomSprites[pSpriteNumber].Sprite_field_1F << 2) + 0x24;
			sound_PlayEffect( SOUND_MUMMY_RELEASE );
			hw_SpritePrepare( pSpriteNumber );
			return;
		}
		// 3828
		mRoomSprites[pSpriteNumber].Sprite_field_1E = 0x01;
		mRoomSprites[pSpriteNumber].mX = mMemory[ word_40 + 3 ] + 4;
		mRoomSprites[pSpriteNumber].mY = mMemory[ word_40 + 4 ] + 7;
		mRoomSprites[pSpriteNumber].Sprite_field_6 = 2;
	}
	byte Y;

	// 3846
	if( mMemory[ 0x780D ] != 0 ) {
		if( mMemory[ 0x780E ] != 0 )
			return;
		else
			Y = 1;

	} else {
	// 3857
		Y = 0;
	}

	// 385E
	Y = mMemory[ 0x34D1 + Y ];
	positionCalculate( pSpriteNumber );

	AA = mRoomSprites[pSpriteNumber].mX;
	AA -= mRoomSprites[Y].mX;
	if( AA < 0 ) {
		AA ^= 0xFF;
		++AA;
	}
	// 3872
	if( AA < 2 )
		return;

	// Frame
	++mRoomSprites[pSpriteNumber].spriteImageID;
	if( mRoomSprites[pSpriteNumber].mX < mRoomSprites[Y].mX ) {
		// Walking Right
		// 3881
		if( !(mMemory[ word_3C ] & 0x04) )
			return;
		
		// 3889
		++mRoomSprites[pSpriteNumber].mX;
		if( mRoomSprites[pSpriteNumber].spriteImageID < 0x4E || mRoomSprites[pSpriteNumber].spriteImageID >= 0x51 )
				mRoomSprites[pSpriteNumber].spriteImageID = 0x4E;
	
	} else {
		// Walking Left
		// 389F
		if( !(mMemory[ word_3C ] & 0x40) )
			return;

		--mRoomSprites[pSpriteNumber].mX;
		if( mRoomSprites[pSpriteNumber].spriteImageID < 0x4B || mRoomSprites[pSpriteNumber].spriteImageID >= 0x4E)
			mRoomSprites[pSpriteNumber].spriteImageID = 0x4B;
	}

	// 38BA
	mMemory[ word_40 + 5 ] = mRoomSprites[pSpriteNumber].mX;
	mMemory[ word_40 + 6 ] = (uint8) mRoomSprites[pSpriteNumber].mY;

	hw_SpritePrepare( pSpriteNumber );
}

// 3A08
void cCreep::obj_RayGun_Laser_Execute( byte pSpriteNumber ) {
	if( mNoInput )
		return;
		
	mScreen->spriteRedrawSet();

	if( mRoomSprites[pSpriteNumber].state & SPR_ACTION_DESTROY ) {

		mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_DESTROY;
		mRoomSprites[pSpriteNumber].state |= SPR_ACTION_FREE;
		
		word_40 = mRoomRayGunPtr + mRoomSprites[pSpriteNumber].Sprite_field_1E;
		mMemory[ word_40 ] = (0xFF ^ RAYGUN_DONT_FIRE) & mMemory[ word_40 ];

	} else {
		if( mRoomSprites[pSpriteNumber].state & SPR_ACTION_CREATED )
			mRoomSprites[pSpriteNumber].state ^= SPR_ACTION_CREATED;

		// 3A42
		byte A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].Sprite_field_1F;
		mRoomSprites[pSpriteNumber].mX = A;

		// Edge of screen?
		if( A < 0xB0 && A >= 8 )
			return;
		
		// Reached edge
		mRoomSprites[pSpriteNumber].state |= SPR_ACTION_DESTROY;
	}
}

// 3F14: Find a free object position, and clear it
int cCreep::Sprite_CreepFindFree( ) {

	for( int number = 0 ; number < MAX_SPRITES; ++number ) {
		sCreepSprite *sprite = &mRoomSprites[number];

		if( sprite->state & SPR_UNUSED ) {

			sprite->clear();

			sprite->state = SPR_ACTION_CREATED;

			sprite->Sprite_field_5 = 1;
			sprite->Sprite_field_6 = 1;

			return number;
		}
	}

	return -1;
}

sCreepSprite *cCreep::Sprite_CreepGetFree( ) {
	int number = Sprite_CreepFindFree();
	sCreepSprite *sprite = 0;

	if( number != -1 )
		sprite = &mRoomSprites[ number ];

	return sprite;
}

void cCreep::Draw_RoomAnimObject( byte pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtCurrentID, byte pObjectNumber ) {
	//5783
	byte gfxDecodeMode;

	if( !(mRoomAnim[pObjectNumber].mFlags & ITM_DISABLE) ) {
		gfxDecodeMode = 2;
		mTxtX_0 = mRoomAnim[pObjectNumber].mX;
		mTxtY_0 = mRoomAnim[pObjectNumber].mY;
		pTxtCurrentID = mRoomAnim[pObjectNumber].mGfxID;

	} else {
		gfxDecodeMode = 0;
	}

	screenDraw( gfxDecodeMode, pGfxID, pGfxPosX, pGfxPosY, pTxtCurrentID );
	//57AE
	mRoomAnim[pObjectNumber].mFlags = ((ITM_DISABLE ^ 0xFF) & mRoomAnim[pObjectNumber].mFlags);
	mRoomAnim[pObjectNumber].mGfxID = pGfxID;
	mRoomAnim[pObjectNumber].mX = pGfxPosX;
	mRoomAnim[pObjectNumber].mY = pGfxPosY;
	mRoomAnim[pObjectNumber].mWidth = mGfxWidth;
	mRoomAnim[pObjectNumber].mHeight = mGfxHeight;

	mRoomAnim[pObjectNumber].mWidth <<= 2;
}

// 41D8: In Front Button?
void cCreep::obj_Door_Button_InFront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	if( mRoomSprites[pSpriteNumber].mButtonState == 0 )
		return;

	byte A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;

	A -= mRoomAnim[pObjectNumber].mX;
	if( A >= 0x0C )
		return;

	if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0 )
		return;
	
	byte DoorID;

	for(DoorID = 0;; ++DoorID ) {
		if( mRoomAnim[DoorID].mObjectType != OBJECT_TYPE_DOOR )
			continue;
		
		if( mRoomObjects[DoorID].objNumber == mRoomObjects[pObjectNumber].objNumber )
			break;
	}
	//4216
	if( mRoomObjects[DoorID].Object_field_1 )
		return;

	mRoomAnim[DoorID].mFlags |= ITM_EXECUTE;

	g_Steam.SetAchievement(eAchievement_OpenDoor);
}

// 4647: In Front Forcefield Timer
void cCreep::obj_Forcefield_Timer_InFront( byte pSpriteNumber, byte pObjectNumber ) {
	if(mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	if(!mRoomSprites[pSpriteNumber].mButtonState)
		return;

	mMemory[ 0x75AB ] = 0x0C;
	sound_PlayEffect( SOUND_FORCEFIELD_TIMER );
	
	mRoomAnim[pObjectNumber].mFlags |= ITM_EXECUTE;
	mRoomObjects[pObjectNumber].Object_field_1 = 0x1E;
	mRoomObjects[pObjectNumber].Object_field_2 = 0x08;
	mMemory[ 0x6889 ] = mMemory[ 0x688A ] = mMemory[ 0x688B ] = 0x55;
	mMemory[ 0x688C ] = mMemory[ 0x688D ] = mMemory[ 0x688E ] = 0x55;
	mMemory[ 0x688F ] = mMemory[ 0x6890 ] = 0x55;

	mTxtX_0 = mRoomAnim[pObjectNumber].mX;
	mTxtY_0 = mRoomAnim[pObjectNumber].mY;
	screenDraw( 1, 0, 0, 0, mRoomAnim[pObjectNumber].mGfxID);

	mMemory[ 0x4750 + mRoomObjects[pObjectNumber].objNumber ] = 0;
}

// 4990: In front of key
void cCreep::obj_Key_Infront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0 )
		return;
	
	if( mRoomSprites[pSpriteNumber].mButtonState == 0 )
		return;

	sound_PlayEffect( SOUND_KEY_PICKUP );
	mRoomAnim[pObjectNumber].mFlags |= ITM_PICKED;

	word_40 = mRoomKeyPtr + mRoomObjects[pObjectNumber].objNumber;

	mMemory[ word_40 + 1 ] = 0;
	byte KeyID = mMemory[ word_40 ];

	if( mRoomSprites[pSpriteNumber].playerNumber ) {
		// 49DA
		mMemory[ 0x7835 + mMemory[ 0x7814 ] ] = KeyID;
		++mMemory[ 0x7814 ];
		
	} else {
		mMemory[ 0x7815 + mMemory[ 0x7813 ] ] = KeyID;
		++mMemory[ 0x7813 ];
	}
}

// 4A68: In Front Lock
void cCreep::obj_Door_Lock_InFront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0 )
		return;

	if( mRoomSprites[pSpriteNumber].mButtonState == 0 ) 
		return;

	if( obj_Key_NotFound( mRoomObjects[pObjectNumber].objNumber, pSpriteNumber ) == true )
		return;

	// This loop expects to find the object, if it doesnt, its meant to loop forever
	for( byte X = 0; X < MAX_OBJECTS; ++X ) {

		if( mRoomAnim[X].mObjectType != OBJECT_TYPE_DOOR )
			continue;

		if( mRoomObjects[X].objNumber == mRoomObjects[pObjectNumber].Object_field_1 ) {

			// 4AA2
			if( mRoomObjects[X].Object_field_1 )
				return;

			mRoomAnim[X].mFlags |= ITM_EXECUTE;
			return;
		}
	}
	// Therefor, we should probably crash or something if this point is ever reached
}

bool cCreep::obj_Key_NotFound( byte pObjectNumber, byte pSpriteNumber ) {
	byte KeyCount;
	
	if( mRoomSprites[pSpriteNumber].playerNumber != 0 ) {
		KeyCount = mMemory[ 0x7814 ];
		word_30 = 0x7835;
	} else {
		// 5EAA
		KeyCount = mMemory[ 0x7813 ];
		word_30 = 0x7815;
	}

	//5EB8
	for( byte pY = 0;; ++pY ) {
		if( pY == KeyCount )
			return true;

		if( mMemory[ word_30 + pY ] == pObjectNumber )
			return false;
	}
}

// 4D70: In Front RayGun Control
void cCreep::obj_RayGun_Control_InFront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	byte A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	A -= mRoomAnim[pObjectNumber].mX;

	if( A >= 8 )
		return;

	if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0)
		return;

	word_40 = mRoomRayGunPtr + mRoomObjects[pObjectNumber].objNumber;
	A = 0xFF;
	A ^= RAYGUN_MOVE_UP;
	A ^= RAYGUN_MOVE_DOWN;
	A &= mMemory[ word_40 ];

	if( !mRoomSprites[pSpriteNumber].Sprite_field_1E ) 
		A |= RAYGUN_MOVE_UP;
	else {
		if( mRoomSprites[pSpriteNumber].Sprite_field_1E == 4 )
			A |= RAYGUN_MOVE_DOWN;
		else
			if( mRoomSprites[pSpriteNumber].Sprite_field_1E != 0x80 )
				return;
	}

	// 4DC9
	A |= RAYGUN_PLAYER_CONTROLLING;

	mMemory[ word_40 ] = A;
	if( mRoomSprites[pSpriteNumber].mButtonState )
		A = mMemory[ word_40 ] | RAYGUN_FIRED_BY_PLAYER;
	else
		A = (0xFF ^ RAYGUN_FIRED_BY_PLAYER) & mMemory[ word_40 ];

	mMemory[ word_40 ] = A;
}


// 4EA8: Teleport?
void cCreep::obj_Teleport_InFront( byte pSpriteNumber, byte pObjectNumber ) {

	if( mRoomAnim[pObjectNumber].mFlags & ITM_EXECUTE )
		return;

	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER )
		return;

	// 4EB5
	if( mMemory[ 0x780D + mRoomSprites[pSpriteNumber].playerNumber ] != 0 )
		return;

	// 4EC5
	word_40 = readLEWord( &mRoomObjects[pObjectNumber].objNumber );
	if(! (mRoomSprites[pSpriteNumber].mButtonState) ) {
		// 4ED4
		if( (mRoomSprites[pSpriteNumber].Sprite_field_1E) )
			return;

		if( mEngine_Ticks & 0x0F )
			return;
		
		// Change teleport destination
		byte A = mMemory[ word_40 + 2 ];
		A += 1;
		mMemory[ word_40 + 2 ] = A;
		A <<= 1;
		A += 3;

		if( !(mMemory[ word_40 + A ]) )
			mMemory[ word_40 + 2 ] = 0;

		// 4EF7
		mMemory[ 0x75DB ] = mMemory[ word_40 + 2 ] + 0x32;

		sound_PlayEffect( SOUND_TELEPORT_CHANGE );
		A = mMemory[ word_40 + 2 ] + 2;

		obj_Teleport_SetColour( A, pObjectNumber );
	} else {

		g_Steam.SetAchievement(eAchievement_UseTeleport);

		// 4F1A
		// Use Teleport
		mRoomAnim[pObjectNumber].mFlags |= ITM_EXECUTE;
		mRoomObjects[pObjectNumber].color = 8;
		
		byte A = mMemory[ word_40 + 2 ] + 0x02;
		// 4F35
		mRoomObjects[pObjectNumber].Object_field_2 = A;

		A = mMemory[ word_40 + 2 ] << 1;
		A += 0x03;

		byte A2 = mMemory[ word_40 + A ];
		++A;
		//4F44
		mRoomObjects[pObjectNumber].Object_field_5 = mMemory[ word_40 + A ];

		// Set player new X/Y
		mRoomSprites[pSpriteNumber].mY = mRoomObjects[pObjectNumber].Object_field_5 + 0x07;
		mRoomObjects[pObjectNumber].Object_field_4 = mRoomSprites[pSpriteNumber].mX = A2;
	}

}

// 4DE9: 
void cCreep::obj_RayGun_Control_Update( byte pA ) {

	mMemory[ 0x6DBF ] = pA;
	mMemory[ 0x6DC0 ] = pA;

	byte gfxPosX = mMemory[ word_40 + 5 ];
	byte gfxPosY = mMemory[ word_40 + 6 ];

	screenDraw( 0, 0x6E, gfxPosX, gfxPosY, 0 );
	mMemory[ 0x6DBF ] = pA << 4;
	mMemory[ 0x6DC0 ] = pA << 4;

	gfxPosY += 0x10;
	screenDraw( 0, 0x6E, gfxPosX, gfxPosY, 0 );
}

// 505C: 
void cCreep::obj_Teleport_SetColour( byte pColor, byte pObjectNumber ) {

	byte A =  (pColor << 4) | 0x0A;

	mMemory[ 0x6E70 ] = mMemory[ 0x6E71 ] = mMemory[ 0x6E72 ] = A;
	mMemory[ 0x6E73 ] = mMemory[ 0x6E74 ] = mMemory[ 0x6E75 ] = 0x0F;

	word_40 = readLEWord( &mRoomObjects[pObjectNumber].objNumber );

	byte gfxPosX = mMemory[ word_40 ] + 4;
	byte gfxPosY = mMemory[ word_40 + 1 ] ;
	screenDraw( 0, 0x71, gfxPosX, gfxPosY, 0 );

	gfxPosY += 0x08;

	mMemory[ 0x6E73 ] = mMemory[ 0x6E74 ] = mMemory[ 0x6E75 ] = 1;
	screenDraw( 0, 0x71, gfxPosX, gfxPosY, 0 );

	gfxPosY += 0x08;
	screenDraw( 0, 0x71, gfxPosX, gfxPosY, 0 );
}

// 5171: 
void cCreep::obj_TrapDoor_PlaySound( byte pA ) {
	
	mMemory[ 0x759F ] = pA - 0x48;
	sound_PlayEffect(SOUND_TRAPDOOR_SWITCHED);
}

// 526F: 
void cCreep::obj_TrapDoor_Switch_Check( byte pA ) {
	word SavedWord40, SavedWord3C;
	byte X;

	SavedWord40 = word_40;
	SavedWord3C = word_3C;

	word_40 = mRoomTrapDoorPtr + pA;
	// 529B
	mMemory[ word_40 ] ^= TRAPDOOR_OPEN;

	for( X = 0 ;;++X) {
		if( mRoomAnim[X].mObjectType != OBJECT_TYPE_TRAPDOOR_PANEL )
			continue;
		if( mRoomObjects[X].objNumber == pA )
			break;
	}

	//52bd
	mRoomAnim[X].mFlags |= ITM_EXECUTE;
	
	if( !(mMemory[ word_40 ] & TRAPDOOR_OPEN) ) {
		// 52cf
		mRoomObjects[X].Object_field_1 = 0;
		mRoomObjects[X].Object_field_2 = 0x78;
		mMemory[ 0x6F2E ] = 0xC0;
		mMemory[ 0x6F30 ] = 0x55;

		byte_5FD5 = (mMemory[ word_40 + 1 ] >> 2) - 4;
		byte_5FD6 = mMemory[ word_40 + 2 ] >> 3;

		word_3C_Calculate();

		mMemory[ word_3C ] |= 0x04;
		mMemory[ word_3C + 4 ] |= 0x40;

	} else {
		// 530F
		mRoomObjects[X].Object_field_1 = 1;
		mRoomObjects[X].Object_field_2 = 0x73;
		mMemory[ 0x6F2E ] = 0x20;
		mMemory[ 0x6F30 ] = 0xCC;
		
		byte_5FD5 = (mMemory[ word_40 + 1 ] >> 2) - 4;
		byte_5FD6 = mMemory[ word_40 + 2 ] >> 3;

		word_3C_Calculate();

		mMemory[ word_3C ] &= 0xFB;
		mMemory[ word_3C + 4 ] &= 0xBF;
	}

	// 534c
	byte gfxPosX = mMemory[ word_40 + 3 ];
	byte gfxPosY = mMemory[ word_40 + 4 ];

	screenDraw( 0, 0x7A, gfxPosX, gfxPosY, 0 );

	word_40 = SavedWord40;
	word_3C = SavedWord3C;
}

// 548B: In Front Conveyor
void cCreep::obj_Conveyor_InFront( byte pSpriteNumber, byte pObjectNumber ) {

	word_40 = mConveyorPtr + mRoomObjects[pObjectNumber].objNumber;

	if( !(mMemory[ word_40 ] & CONVEYOR_TURNED_ON ))
		return;
	
	byte A = mRoomSprites[pSpriteNumber].mSpriteType;

	if( A == SPRITE_TYPE_PLAYER ) {
		// 54B7
		if( mRoomSprites[pSpriteNumber].spriteImageID >= 6 )
			return;

	} else
		if( A != SPRITE_TYPE_MUMMY && A != SPRITE_TYPE_FRANKIE ) 
			return;
	
	// 54BE
	A = mRoomSprites[pSpriteNumber].mX + mRoomSprites[pSpriteNumber].mWidth;
	if( (char) A >= 0 && ((char) (A - mRoomAnim[pObjectNumber].mX)) < 0 )
		return;

	A -= mRoomAnim[pObjectNumber].mX;
	if( A >= 0x20 )
		return;

	if( mMemory[ word_40 ] & CONVEYOR_MOVING_RIGHT )
		A = 0xFF;
	else
		A = 0x01;

	// 54E2
	if( !(mRoomSprites[pSpriteNumber].mSpriteType == SPRITE_TYPE_PLAYER && (mEngine_Ticks & 7) ))
		A <<= 1;

	mRoomSprites[pSpriteNumber].mX += A;
}

// 5611: In Front Conveyor Control
void cCreep::obj_Conveyor_Control_InFront( byte pSpriteNumber, byte pObjectNumber ) {
	if( mRoomSprites[pSpriteNumber].mSpriteType != SPRITE_TYPE_PLAYER)
		return;

	if( !mRoomSprites[pSpriteNumber].mButtonState )
		return;

	word_40 = mConveyorPtr + mRoomObjects[pObjectNumber].objNumber;
	byte A;

	if( mRoomSprites[pSpriteNumber].playerNumber )
		A = CONVEYOR_PLAYER2_SWITCHED;
	else
		A = CONVEYOR_PLAYER1_SWITCHED;

	mMemory[ word_40 ] |= A;
}

bool cCreep::object_Create( byte &pX ) {

	if( mObjectCount == MAX_OBJECTS )
		return false;

	pX = mObjectCount++;

	mRoomAnim[pX].clear();
	mRoomObjects[pX].clear();

	mRoomAnim[pX].mFlags = ITM_DISABLE;

	return true;
}

bool cCreep::EventAdd( cEvent pEvent ) {
	
	mEvents.push_back( pEvent );

	return true;
}
