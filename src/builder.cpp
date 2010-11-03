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
 *  Castle Builder
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "graphics/screenSurface.h"
#include "vic-ii/screen.h"
#include "vic-ii/sprite.h"
#include "playerInput.h"
#include "castle/castle.h"
#include "castleManager.h"
#include "creep.h"
#include "sound/sound.h"
#include "castle/objects/object.hpp"
#include "castle/room.hpp"
#include "builder.hpp"
#include "debug.h"

cBuilder::cBuilder( cCreep *pParent ) {

	if(pParent) {
		mCreepParent = pParent;

		delete mScreen;
		mScreen = pParent->screenGet();
	}

	// Change window title
	stringstream windowTitle;
	windowTitle << "The Castles of Dr. Creep: Castle Builder";
	mScreen->windowTitleSet( windowTitle.str() );

	// Top Left of usable screen area
	mCursorX = 0x10;
	mCursorY = 0;

	mOriginalObject = 0;
	mRoomSelectedObject = 0;
	mCurrentObject = 0;
	mCurrentRoom = 0;
	mFinalRoom = 0;
	mFinalPtr = 0;
	mFinalScreen = 0;

	mDragMode = false;
	mLinkMode = false;
	mTest = false;

	mSelectedObject = eObjectNone;

	mStart_Door_Player1 = mStart_Door_Player2 = 0;
	mStart_Room_Player1 = mStart_Room_Player2 = 0;
	mLives_Player1 = mLives_Player2 = 3;

	mSearchObject = eObjectNone;
}

cBuilder::~cBuilder() {
	delete mCurrentObject;

	roomCleanup();
}

void cBuilder::castleCreate() {

	mCurrentRoom = roomCreate(0);
	mFinalRoom = roomCreate(-1);

	mStart_Room_Player1 = mStart_Room_Player2 = 0;
	mStart_Door_Player1 = mStart_Door_Player2 = 0;
	mLives_Player1 = mLives_Player2 = 3;
}

void cBuilder::objectStringsClear() {
	mStrings.clear();
}

void cBuilder::objectStringsPrint() {
	vector< sString >::iterator		stringIT;

	for( stringIT = mStrings.begin(); stringIT != mStrings.end(); ++stringIT ) {
		objectStringPrint( (*stringIT) );
	}
}

void cBuilder::objectStringAdd( string pMessage, byte pPosX, byte pPosY, byte pColor ) {
	mStrings.push_back( sString( pPosX, pPosY, pColor, pMessage ) );

	objectStringsPrint();
}

void cBuilder::objectStringPrint( sString pString ) {
	memcpy( &mMemory[ 0xBB04 ], pString.mString.c_str(), pString.mString.size() );

	mMemory[ 0xBB00 ] = pString.mPosX;
	mMemory[ 0xBB01 ] = pString.mPosY;
	mMemory[ 0xBB02 ] = pString.mColor;
	mMemory[ 0xB03 ] = 0x21;

	mMemory[ 0xBB03 +  pString.mString.size() ] |= 0x80;

	word_3E = 0xBB00;
	obj_stringPrint();

	obj_Actions();
	img_Actions();

	hw_Update();
}

void cBuilder::playerDraw() {

	mMemory[ 0x780B ] = mMemory[ 0x7805 ];	// Player1 Start Door
	mMemory[ 0x780C ] = mMemory[ 0x7806 ];	// Player2 Start Door

	if( mStart_Room_Player1 == mCurrentRoom->mNumber ) {
		byte_3638 = 0;
		obj_Player_Add();
	}

	if( mStart_Room_Player2 == mCurrentRoom->mNumber ) {
		byte_3638 = 1;
		obj_Player_Add();
	}
}

void cBuilder::mainLoop() {
	mQuit = false;
	mIntro = false;
	mNoInput = true;

	if( mStartCastle != -2 && mStartCastle > -1 ) {
		// Load Castle
		mCastle = mCreepParent->castleGet();
		castleLoad();	

	} else {
	
		if(mStartCastle != -2) {
			// New Castle
			mScreen->levelNameSet("Untitled");
			castleCreate();
			castleSave( false );
		}
	}
	
	// Set the screen ptrs
	mScreen->bitmapLoad( &mMemory[ 0xE000 ], &mMemory[ 0xCC00 ], &mMemory[ 0xD800 ], 0 );

	// Prep Cursor
	mScreen->cursorSet( mCursorX, mCursorY );
	mScreen->cursorEnabled(true);

	// Change to first room
	roomChange(mMemory[ 0x7803 ]);

	// 
	while(!mQuit) {
		byte key = tolower( mInput->keyGet() );
		
		// Check keyboard input
		switch(key) {
			
			case 0x02:	// '1' Set player one starting door
				if( mCurrentObject && mCurrentObject->objectTypeGet() == eObjectDoor ) {
					mStart_Room_Player1 = mCurrentRoom->mNumber;
					mStart_Door_Player1 = findItemIndex( mCurrentObject );
					castlePrepare();
				}
				break;

			case 0x03:	// '2' Set player two starting door
				if( mCurrentObject && mCurrentObject->objectTypeGet() == eObjectDoor ) {
					mStart_Room_Player2 = mCurrentRoom->mNumber;
					mStart_Door_Player2 = findItemIndex( mCurrentObject );
					castlePrepare();
				}
				break;

			case 0x10:	// 'q' Change Selected Placement Object 'Up'
				selectedObjectChange( true );
				break;

			case 0x1E:	// 'a' Change Selected Placement Object 'Down'
				selectedObjectChange( false );
				break;

			case 0x26:	// 'l' Link Objects
				selectedObjectLink();
				break;

			case 0x1F:	// 's' Object State
				if(mCurrentObject) {
					mCurrentObject->stateChange();
					castlePrepare();
				}
				break;

			//	castleSaveToDisk();
			//	break;

			case 0x1A:	// '[' Select a placed object
				selectPlacedObject( false );
				break;

			case 0x1B:	// ']' Select a placed object
				selectPlacedObject( true );
				break;

			case 0x0C:	{// '-' Previous Room
				int newRoom = ((char) mCurrentRoom->mNumber) - 1;
				if(newRoom < 0)
					newRoom = 0;
				roomChange( newRoom );
				break;
						}

			case 0x0D:	{// '=' Next Room
				int newRoom = ((char) mCurrentRoom->mNumber) + 1;
				//if( newRoom >= (int) mRooms.size() )
				//	newRoom = mRooms.size() - 1;
				roomChange( newRoom );
				break;
						}

			case 0x21:	// 'f' edit final room
				if( mFinalRoom == mCurrentRoom )
					roomChange(0);
				else
					roomChange(-1);
				
				break;

			case 0x2E:	// 'c' color decrease
				if(mCurrentObject)
					mCurrentObject->colorDecrease();
				castlePrepare();
				break;

			case 0x2F:	// 'v' color increase
				if(mCurrentObject)
					mCurrentObject->colorIncrease();
				castlePrepare();
				break;

			case 0x73:	// 'Delete' Delete selected object
				selectedObjectDelete();
				break;

			
			default:
				/*if(key) {
					cout << "0x";
					cout << hex << (int) key << endl;
				}*/
				break;
		}

		// Check 'joystick' input
		parseInput();

		interruptWait( 2 );

		// Draw
		obj_Actions();
		img_Actions();

		// Redraw screen
		hw_Update();
	}

	mScreen->cursorEnabled(false);
	if( mTest )
		castleSave( true );
}

void cBuilder::roomCleanup() {
	map< int, cRoom *>::iterator roomIT;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		delete roomIT->second;
	}

	mRooms.clear();
}

void cBuilder::roomChange( int pNumber ) {

	if( mCurrentObject && mCurrentObject->partGet(0)->mPlaced == false ) {
		mCurrentRoom->objectDelete( mCurrentObject );
		delete mCurrentObject;
		mCurrentObject = 0;
	}

	if( mLinkMode ) {
		mOriginalRoom = mCurrentRoom;
		mOriginalObject = mCurrentObject;
	}

	castleSave( false );		
	mCurrentRoom = roomCreate( pNumber );
	
	// Set the room number in the window title
	mScreen->roomNumberSet( pNumber );

	if( pNumber == -1) {

		screenClear();
		word_3E = readLEWord( &mMemory[ 0x785F ] );
		roomPrepare();

	} else {
		mMemory[ 0x7809 ] = pNumber;
		mMemory[ 0x780A ] = pNumber;
		roomLoad();
	}

	playerDraw();
}


void cBuilder::messagePrint( string pMessage ) {

	objectStringAdd(pMessage, 0x10, 0xC0, 2);
}

void cBuilder::cursorObjectUpdate() {

	// Cursor object has changed? or Room has changed?
	if( mCurrentObject && !mCurrentObject->isSelected() && mCurrentObject->objectTypeGet() != mSelectedObject) {

		// Dont delete if its placed
		if( mCurrentObject->partGet(0)->mPlaced == false ) {
			mCurrentRoom->objectDelete( mCurrentObject );
			delete mCurrentObject;
		} else
			if( mCurrentObject->partGet()->mPlaced == false )
				mCurrentObject->partDel();
		mCurrentObject = 0;
	}

	// No Current object? create a new one
	if( !mCurrentObject )
		mCurrentObject = objectCreate( mCurrentRoom, mSelectedObject, mCursorX, mCursorY );
	else {	
		if( mCurrentObject->mRoomGet() == mCurrentRoom )
			mCurrentObject->partSetPosition( mCursorX, mCursorY );
	}

	// If the object is placed, and not selected by the user
	// Clear the current object
	if( mCurrentObject && mCurrentObject->isPlaced() && !mCurrentObject->isSelected()) {
		mCurrentObject = 0;
	}

	cursorUpdate();
}

size_t cBuilder::findItemIndex( cObject *pObject ) {
	vector< cObject* > objects = mCurrentRoom->objectFind( pObject->objectTypeGet() );
	vector< cObject* >::iterator	objIT;

	size_t count = 0;

	// Find the 'index' of an item, of type 'pObject'
	for(objIT = objects.begin(); objIT != objects.end(); ++objIT ) {
		/*if( pObject->objectTypeGet() == eObjectLightning ) {
			cObjectLightning *light = (cObjectLightning*) (*objIT);

			if( !light->mMachineMode )
				continue;
		}*/

		if( (*objIT) == pObject )
			return count;

		++count;	
	}

	return 0;
}

void cBuilder::cursorUpdate() {

	// Set cursor position
	mScreen->cursorSet( mCursorX, mCursorY );

	// Set cursor size
	if(mCurrentObject && mCurrentObject->partGet() )
		mScreen->cursorSize( mCurrentObject->partGet()->mCursorWidth, mCurrentObject->partGet()->mCursorHeight );
	else
		mScreen->cursorSize( 1, 1 );

	castlePrepare( );
}

void cBuilder::castlePrepare( ) {
	castleSave( false );

	// Final Room?
	if( (char) mCurrentRoom->mNumber == -1) {
		screenClear();
		word_3E = readLEWord( &mMemory[ 0x785F ] );
		roomPrepare();

	} else {
		mMemory[ 0x7809 ] = mCurrentRoom->mNumber;
		mMemory[ 0x780A ] = mCurrentRoom->mNumber;
		roomLoad();
	}

	// Print any strings
	objectStringsPrint();

	// Force draw of sprites
	obj_Actions();
	img_Actions();

	playerDraw();
}

void cBuilder::castleSaveToDisk() {

	// Remove cursor object for now
	mCurrentRoom->objectDelete( mCurrentObject );
	castlePrepare( );

	mScreen->cursorEnabled( false );
	

	gameDataSave( true );
	
	// Re-add cursor object
	mCurrentRoom->objectAdd( mCurrentObject );

	mScreen->cursorEnabled( true );
	castlePrepare( );
}

void cBuilder::mapRoomsDraw( size_t pArrowRoom ) {
	map< int, cRoom *>::iterator	roomIT;
		
	screenClear();

	// Loop through the data for each room, marking it as visible
	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		
		// Skip Final Room
		if( roomIT->second->mNumber == -1 )
			continue;

		castleRoomData( roomIT->second->mNumber );

		mMemory[ word_42 ] |= byte_8C0;

		// Draw the room
		mapRoomDraw();

		if( roomIT->second->mNumber == pArrowRoom ) {
			castleRoomData( roomIT->second->mNumber );
			mapArrowDraw( 0 );
		}
	}

	// Set the sprite color to white
	mScreen->spriteGet( 0 )->_color = 1;
}

void cBuilder::mapBuilder() {
	bool mapRedraw = true, saveCastle = false;

	while(!mQuit) {
		byte key = tolower( mInput->keyGet() );

		if( mInput->restoreGet() )
			mQuit = true;

		switch(key) {

			case 0x0C:	{// '-' Previous Room
				int newRoom = ((char) mCurrentRoom->mNumber) - 1;
				if(newRoom < 0)
					newRoom = 0;
				roomChange( newRoom );
				mapRedraw = true;
				break;
						}

			case 0x0D:	{// '=' Next Room
				int newRoom = ((char) mCurrentRoom->mNumber) + 1;
				//if( newRoom >= (int) mRooms.size() )
				//	newRoom = mRooms.size() - 1;
				roomChange( newRoom );
				mapRedraw = true;
				break;
						}

			case 0x1A:	// '[' Increase Width
				if(mCurrentRoom->mMapWidth < 7)
					mCurrentRoom->mMapWidth ++;

				saveCastle = true;
				break;

			case 0x1B:	// ']' Decrease Width
				if(mCurrentRoom->mMapWidth > 1)
					mCurrentRoom->mMapWidth --;

				saveCastle = true;
				break;
	
			case 0x27:	// ';' Increase Height
			if(mCurrentRoom->mMapHeight < 7)
					mCurrentRoom->mMapHeight ++;

				saveCastle = true;
				break;

			case 0x28:	// ''' Decrease Height
				if(mCurrentRoom->mMapHeight > 1)
					mCurrentRoom->mMapHeight --;

				saveCastle = true;
				break;

			case 0x2E:
				if(mCurrentRoom->mColor < 0x15)
					mCurrentRoom->mColor++;
				else
					mCurrentRoom->mColor = 0;

				saveCastle = true;
				break;

			default:
				/*if(key) {
					cout << "0x";
					cout << hex << (int) key << endl;
				}*/
				break;
		}

		sPlayerInput *input = mInput->inputGet(0);
		if(input->mLeft) {
			if(mCurrentRoom->mMapX > 3)
				mCurrentRoom->mMapX -= 4;
			saveCastle = true;
		}
		if(input->mRight) {
			if(mCurrentRoom->mMapX < 0x255)
				mCurrentRoom->mMapX += 4;
			saveCastle = true;
		}
		if(input->mUp) {
			if(mCurrentRoom->mMapY > 7)
				mCurrentRoom->mMapY -= 8;
			saveCastle = true;
		}
		if(input->mDown) {
			if(mCurrentRoom->mMapY < 0x255)
				mCurrentRoom->mMapY += 8;
			saveCastle = true;
		}

		if( saveCastle ) {
			saveCastle = false;
			
			castleSave(false);
			mapRedraw = true;
		}

		if( mapRedraw ) {
			mapRedraw = false;

			mapRoomsDraw( mCurrentRoom->mNumber );
		}

		hw_IntSleep(1);
		hw_Update();	
	}

	mQuit = false;
}

void cBuilder::parseInput() {
	sObjectPart		*part	= 0;
	sPlayerInput	*input	= mInput->inputGet(0);
	bool			 update = false;
	
	// Get the current selected 'part' of the object
	if(mCurrentObject)
		part = mCurrentObject->partGet();

	// Left
	if(input->mLeft) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionLeft:
					part->mLength++;
					break;
				case eDirectionRight:
					part->mLength--;
					break;
				default:
					break;
			}
		} else
			mCursorX -= 4;
		update = true;
	}

	// Right
	if(input->mRight) {

		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionLeft:
					part->mLength--;
					break;
				case eDirectionRight:
					part->mLength++;
					break;
				default:
					break;
			}
		} else
			mCursorX += 4;
		update = true;
	}

	// Down
	if(input->mDown) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionDown:
					part->mLength++;
					break;
				case eDirectionUp:
					part->mLength--;
					break;
				default:
					break;
			}
		} else
			mCursorY += 8;
		update = true;
	}

	// Up
	if(input->mUp) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionDown:
					part->mLength--;
					break;
				case eDirectionUp:
					part->mLength++;
					break;
				default:
					break;
			}
		} else
			mCursorY -= 8;
		update = true;
	}

	//
	if(part && ((int) part->mLength) < 1)
		part->mLength = 1;

	// Button
	if(input->mButton) {
		mInput->inputCheck( true );

		if( part && part->mDrags == true ) {
			if( mDragMode == false ) 
				mDragMode = true;
			else {
				mDragMode = false;
				if(mLinkMode) {
					if(mOriginalObject) {
						mOriginalObject->mLinkedSet( findItemIndex( mCurrentObject ) );
						mOriginalObject->mLinked2Set( mCurrentRoom->mNumber );
					}
					mSearchObject = eObjectNone;
					mLinkMode = false;
					mCurrentObject = 0;

				} else if(mCurrentObject) {
					mCurrentObject->partPlace();
				}
			}

		} else {
			mDragMode = false;
			if(mLinkMode) {
				if(mOriginalObject) {
					mOriginalObject->mLinkedSet( findItemIndex( mCurrentObject ) );
					mOriginalObject->mLinked2Set( mCurrentRoom->mNumber );
				}
				mSearchObject = eObjectNone;
				mLinkMode = false;
				mCurrentObject = 0;

			} else if(mCurrentObject) {
				mCurrentObject->partPlace();
			}
		}

		
		update = true;
	}

	// ESC: Quit builder
	if( mInput->restoreGet() ) {
		mTest = false;
		mQuit = true;
		return;
	}

	// F1: Save Castle To Disk
	if( mInput->runStopGet() )
		castleSaveToDisk();
	
	if( mInput->f3Get() ) {
		mapBuilder();
		update = true;
	}

	// F4: Quit builder in test castle mode
	if( mInput->f4Get() ) {
		mTest = true;
		mQuit = true;
	}

	// Check cursor position to see if its outside screen range
	int downWidth = 1;
	int downHeight = 1;

	if(mCurrentObject) {
		downHeight= mCurrentObject->partGet()->mCursorHeight;
		downWidth = mCurrentObject->partGet()->mCursorWidth;
	}

	downHeight *= 8;
	downWidth *= 4;

	#ifndef _WII
		// Cursor minimum and maximums
		if( (mCursorX + downWidth) > 0xB0)
			mCursorX = 0xB0 - downWidth;

		if( mCursorX < 0x10 )
			mCursorX = 0x10;

		if( mCursorY > 0xF0 )
			mCursorY = 0;

		if( (mCursorY + downHeight) > 0xC8 )
			mCursorY = 0xC8 - downHeight;
#else
		if( (mCursorX = downWidth ) > 0xA0 )
			mCursorX = 0xA0 - downWidth;

		if( mCursorX < 0x00 )
			mCursorX = 0x00;

		if( mCursorY > 0xF0 )
			mCursorY = 0;

		if( (mCursorY + downHeight) > 0xC8 )
			mCursorY = 0xC8 - downHeight;
#endif

	// Does the cursor actually need updating
	if(update) 
		cursorObjectUpdate();
}

cRoom *cBuilder::roomCreate( int pNumber ) {
	
	if( mRooms.find( pNumber ) != mRooms.end() )
		return mRooms.find( pNumber )->second;

	cRoom *room = new cRoom( this, pNumber );
	mRooms.insert( make_pair( pNumber, room ) );

	return room;
}

void cBuilder::castleLoad( ) {
	size_t size = 0;
	byte *bufferStart = mCastle->bufferGet( size );
	byte *buffer = bufferStart;

	if( *(buffer + 2) != 0x80 )
		return;

	mStart_Room_Player1 = *(buffer + 3);
	mStart_Room_Player2 = *(buffer + 4);
	mStart_Door_Player1 = *(buffer + 5);
	mStart_Door_Player2 = *(buffer + 6);
	mLives_Player1 = *(buffer + 7);
	mLives_Player2 = *(buffer + 8);

	mFinalPtr = readLEWord(buffer + 0x5F);

	if(mFinalPtr)
		mFinalScreen = bufferStart + ( mFinalPtr - 0x7800 );

	buffer += 0x100;

	size_t count = 0;

	while( *buffer != 0xFF && *buffer != 0x40) {
		cRoom *room = roomCreate( count );
		mRooms.insert( make_pair( count++ , room) );

		room->roomLoad( &buffer );

		buffer += 2;
		room->mRoomDirPtr = bufferStart + (readLEWord(buffer) - 0x7800);
		buffer += 2;
	}

	buffer++;

	map< int, cRoom *>::iterator roomIT;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		cRoom *room = roomIT->second;

		buffer = room->mRoomDirPtr;

		mCurrentRoom = room;
		room->roomLoadObjects( &buffer );

		buffer += 2;
	}

	if( mFinalScreen ) {
		mFinalRoom = roomCreate(-1);
		mFinalRoom->roomLoadObjects( &mFinalScreen );
	}
}	

void cBuilder::castleSave( bool pRemoveCursor ) {
	map< int, cRoom *>::iterator  roomIT;
	
	if(mCurrentRoom && pRemoveCursor )
		mCurrentRoom->objectDelete( mCurrentObject );

	byte *buffer =  &mMemory[ 0x7800 ];

	for( word addr = 0x7800; addr < 0x9800; ++addr )
		mMemory[ addr ] = 0;

	*(buffer + 2) = 0x80;
	*(buffer + 3) = mStart_Room_Player1;
	*(buffer + 4) = mStart_Room_Player2;
	*(buffer + 5) = mStart_Door_Player1;
	*(buffer + 6) = mStart_Door_Player2;
	*(buffer + 7) = mLives_Player1;
	*(buffer + 8) = mLives_Player2;

	// Write the room directory
	buffer = &mMemory[ 0x7900 ];

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {	
		if( roomIT->second == mFinalRoom )
			continue;

		roomIT->second->roomSave( &buffer );

		// Skip the pointers to room objects for now
		buffer += 4;
	}

	// Room Directory Terminator
	*(buffer) = 0xFF;

	word	memDest = 0x7900 + (mRooms.size() * 8) + 1;
	size_t	size = 0;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		if( (char) roomIT->second->mNumber == -1 )
			continue;
		buffer = &mMemory[ memDest ];

		// Write pointer to room objects
		byte *dirBuffer = roomIT->second->mRoomDirPtr + 4;
		writeLEWord( dirBuffer, memDest + 2);
		writeLEWord( dirBuffer + 2, memDest );

		// Save all objects, and add up memory size
		memDest += roomIT->second->roomSaveObjects( &buffer );

		// End room marker
		*buffer++ = 0x00;
		*buffer++ = 0x00;

		memDest += 2;
	}

	buffer = &mMemory[ memDest ];

	// Save final room ptr
	if(mFinalRoom) {
		writeLEWord(&mMemory[ 0x785F ], memDest);

		// Save final room objects
		memDest += mFinalRoom->roomSaveObjects( &buffer );
	}

	writeLEWord( buffer, 0 );
	buffer += 2;

	memDest += 2;
	// Write size of castle to beginning to castle memory
	writeLEWord(  &mMemory[ 0x7800 ], (memDest - 0x7800) );

	if(mCurrentRoom && pRemoveCursor )
		mCurrentRoom->objectAdd( mCurrentObject );
}

cObject *cBuilder::objectCreate( cRoom *pRoom, eRoomObjects pObject, byte pPosX, byte pPosY ) {
	cObject *obj = 0;
	
	if( pRoom == mFinalRoom && pObject != eObjectImage )
		return 0;

	switch( pObject ) {
			case eObjectNone:				// Finished
				return 0;

			case eObjectDoor:				// Doors
				obj = obj_Door_Create( pPosX, pPosY );
				break;

			case eObjectWalkway:			// Walkway
				obj = obj_Walkway_Create( pPosX, pPosY );
				break;

			case eObjectSlidingPole:		// Sliding Pole
				obj = obj_SlidingPole_Create( pPosX, pPosY );
				break;

			case eObjectLadder:				// Ladder
				obj = obj_Ladder_Create( pPosX, pPosY );
				break;

			case eObjectDoorBell:			// Doorbell
				obj = obj_Door_Button_Create( pPosX, pPosY );
				break;

			case eObjectLightning:			// Lightning Machine
				obj = obj_Lightning_Create( pPosX, pPosY );
				break;

			case eObjectForcefield:			// Forcefield
				obj = obj_Forcefield_Create( pPosX, pPosY );
				break;

			case eObjectMummy:				// Mummy
				obj = obj_Mummy_Create( pPosX, pPosY );
				break;

			case eObjectKey:				// Key
				obj = obj_Key_Create( pPosX, pPosY );
				break;

			case eObjectLock:				// Lock
				obj = obj_Door_Lock_Create( pPosX, pPosY );
				break;

			case eObjectRayGun:				// Ray Gun
				obj = obj_RayGun_Create( pPosX, pPosY );
				break;

			case eObjectTeleport:			// Teleport
				obj = obj_Teleport_Create( pPosX, pPosY );
				break;

			case eObjectTrapDoor:			// Trap Door
				obj = obj_TrapDoor_Create( pPosX, pPosY );
				break;

			case eObjectConveyor:			// Conveyor
				obj = obj_Conveyor_Create( pPosX, pPosY );
				break;

			case eObjectFrankenstein:		// Frankenstein
				obj = obj_Frankie_Create( pPosX, pPosY );
				break;

			case eObjectText:		// String Print
			case 0x2A6D:
				obj = obj_string_Create( pPosX, pPosY );
				break;

			case eObjectImage:
				obj = obj_Image_Create( pPosX, pPosY );
				break;

			case eObjectMultiDraw:
			case 0x160A:				// Intro
				obj = obj_Multi_Create( pPosX, pPosY );
				break;

			default:
				cout << "roomPrepare: 0x";
				cout << std::hex << pObject << "\n";

				break;
	}

	if(pRoom)
		pRoom->objectAdd( obj );

	return obj;
}

cObject *cBuilder::obj_Door_Create( byte pPosX, byte pPosY ) {
	cObjectDoor	*object = new cObjectDoor( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Walkway_Create( byte pPosX, byte pPosY ) {
	cObjectWalkway *object = new cObjectWalkway( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_SlidingPole_Create( byte pPosX, byte pPosY ) {
	cObjectSlidingPole	*object = new cObjectSlidingPole( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Ladder_Create( byte pPosX, byte pPosY ) {
	cObjectLadder *object = new cObjectLadder( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Door_Button_Create( byte pPosX, byte pPosY ) {
	cObjectDoorBell *object = new cObjectDoorBell( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Lightning_Create( byte pPosX, byte pPosY ) {
	cObjectLightning *object = new cObjectLightning( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Forcefield_Create( byte pPosX, byte pPosY ) {
	cObjectForcefield *object = new cObjectForcefield( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Mummy_Create( byte pPosX, byte pPosY ) {
	cObjectMummy *object = new cObjectMummy( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Key_Create( byte pPosX, byte pPosY ) {
	cObjectKey *object = new cObjectKey( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Door_Lock_Create( byte pPosX, byte pPosY ) {
	cObjectLock *object = new cObjectLock( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_RayGun_Create( byte pPosX, byte pPosY ) {
	cObjectRayGun *object = new cObjectRayGun( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Teleport_Create( byte pPosX, byte pPosY ) {
	vector< cObject*> objects = mCurrentRoom->objectFind( eObjectTeleport );
	cObjectTeleport *object = 0;

	// Already got a teleport on this screen?
	if(objects.size() == 0 ) {
		object = new cObjectTeleport( mCurrentRoom, pPosX, pPosY );

	} else {
		object = (cObjectTeleport*) objects[0];

		if(object->partGet()->mPlaced) {
			object->partAdd();
			object->mPartAdd();
		}
	}

	return object;
}

cObject *cBuilder::obj_TrapDoor_Create( byte pPosX, byte pPosY ) {
	cObjectTrapDoor *object = new cObjectTrapDoor( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Conveyor_Create( byte pPosX, byte pPosY ) {
	cObjectConveyor *object = new cObjectConveyor( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Frankie_Create( byte pPosX, byte pPosY ) {
	cObjectFrankenstein *object = new cObjectFrankenstein( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_string_Create( byte pPosX, byte pPosY ) {
	cObjectText *object = new cObjectText( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Image_Create( byte pPosX, byte pPosY ) {
	cObjectImage *object = new cObjectImage( mCurrentRoom, pPosX, pPosY );

	return object;
}

cObject *cBuilder::obj_Multi_Create( byte pPosX, byte pPosY ) {

	return 0;
}

void cBuilder::selectedObjectDelete() {

	// Delete a selected object from the room
	if( mCurrentObject) {
		mCurrentRoom->objectDelete( mCurrentObject );
		delete mCurrentObject;
		mCurrentObject = 0;
	}

	cursorUpdate();
}

void cBuilder::selectedObjectLink() {
	if( !mCurrentObject )
		return;

	if( mCurrentObject->mLinkObjectGet() == eObjectNone )
		return;

	mOriginalObject = mCurrentObject;
	mOriginalRoom = mCurrentRoom;
	mSearchObject = mCurrentObject->mLinkObjectGet();

	mLinkMode = true;
}

void cBuilder::selectPlacedObject( bool pChangeUp ) {
	bool changeObject = false;

	// If the cursor currently has an unplaced object on it,
	// Delete it
	if( mCurrentObject && mCurrentObject->isPlaced() == false ) {
		mCurrentRoom->objectDelete( mCurrentObject );
		delete mCurrentObject; 
		mCurrentObject = 0;
	} 

	// Check if a selected object has parts, and rotate through them
	if( mCurrentObject ) {

		// Change up a part
		if( mCurrentObject->mPartGet() < mCurrentObject->mPartCountGet() && pChangeUp )
			mCurrentObject->partSet( mCurrentObject->mPartGet() + 1 );
		
		// Change down a part
		else if( mCurrentObject->mPartGet() > 0 && !pChangeUp )
			mCurrentObject->partSet( mCurrentObject->mPartGet() - 1 );
			
		else
			changeObject = true;

		// Last Part?
		if( mCurrentObject->mPartGet() == mCurrentObject->mPartCountGet() )
			changeObject = true;

	} else
		changeObject = true;

	// Change to a new room object
	if( changeObject ) {
		
		if(mCurrentObject)
			mCurrentObject->isSelected(false);

		do {
			if( pChangeUp )
				++mRoomSelectedObject;
			else
				--mRoomSelectedObject;

			// Get the next object
			mCurrentObject = mCurrentRoom->objectGet( mRoomSelectedObject );
			if( mCurrentObject == 0 ) {
				mRoomSelectedObject = 0;
				mCurrentObject = mCurrentRoom->objectGet( mRoomSelectedObject );
				break;
			}
		} while ((mLinkMode && mSearchObject != mCurrentObject->objectTypeGet()) );

		if(mCurrentObject)
			mCurrentObject->partSet(0);
	}

	// Update the cursor
	if(mCurrentObject) {
		mCursorX = mCurrentObject->partGet()->mX;
		mCursorY = mCurrentObject->partGet()->mY;

		mCurrentObject->isSelected( true );
	}

	cursorUpdate();
}

void cBuilder::selectedObjectChange( bool pChangeUp ) {

	switch( mSelectedObject ) {
			case eObjectNone:			// No object selected
				if( pChangeUp )
					mSelectedObject = eObjectText;
				else
					mSelectedObject = eObjectDoor;
				break;

			case eObjectDoor:			
				if( pChangeUp )
					mSelectedObject = eObjectNone;
				else
					mSelectedObject = eObjectWalkway;
				break;

			case eObjectWalkway:
				if( pChangeUp )
					mSelectedObject = eObjectDoor;
				else
					mSelectedObject = eObjectSlidingPole;
				break;

			case eObjectSlidingPole:
				if( pChangeUp )
					mSelectedObject = eObjectWalkway;
				else
					mSelectedObject = eObjectLadder;
				break;

			case eObjectLadder:
				if( pChangeUp )
					mSelectedObject = eObjectSlidingPole;
				else
					mSelectedObject = eObjectDoorBell;
				break;

			case eObjectDoorBell:	
				if( pChangeUp )
					mSelectedObject = eObjectLadder;
				else
					mSelectedObject = eObjectLightning;
				break;

			case eObjectLightning:
				if( pChangeUp )
					mSelectedObject = eObjectDoorBell;
				else
					mSelectedObject = eObjectForcefield;
				break;

			case eObjectForcefield:	
				if( pChangeUp )
					mSelectedObject = eObjectLightning;
				else
					mSelectedObject = eObjectMummy;
				break;

			case eObjectMummy:	
				if( pChangeUp )
					mSelectedObject = eObjectForcefield;
				else
					mSelectedObject = eObjectKey;
				break;

			case eObjectKey:	
				if( pChangeUp )
					mSelectedObject = eObjectMummy;
				else
					mSelectedObject = eObjectLock;
				break;

			case eObjectLock:
				if( pChangeUp )
					mSelectedObject = eObjectKey;
				else
					mSelectedObject = eObjectRayGun;
				break;

			case eObjectRayGun:		
				if( pChangeUp )
					mSelectedObject = eObjectLock;
				else
					mSelectedObject = eObjectTeleport;
				break;

			case eObjectTeleport:	
				if( pChangeUp )
					mSelectedObject = eObjectRayGun;
				else
					mSelectedObject = eObjectTrapDoor;
				break;

			case eObjectTrapDoor:	
				if( pChangeUp )
					mSelectedObject = eObjectTeleport;
				else
					mSelectedObject = eObjectConveyor;
				break;

			case eObjectConveyor:
				if( pChangeUp )
					mSelectedObject = eObjectTrapDoor;
				else
					mSelectedObject = eObjectFrankenstein;
				break;

			case eObjectFrankenstein:
				if( pChangeUp )
					mSelectedObject = eObjectConveyor;
				else
					mSelectedObject = eObjectText;
				break;

			case eObjectText:		// String Print
			case 0x2A6D:
				if( pChangeUp )
					mSelectedObject = eObjectFrankenstein;
				else
					mSelectedObject = eObjectNone;
				break;

			/*case eObjectImage:
				if( pChangeUp )
					mSelectedObject = eObjectText;
				else
					mSelectedObject = eObjectNone;
				break;

			/*case eObjectMultiDraw:
			case 0x160A:				// Intro
				if( pChangeUp )
					mSelectedObject = eObjectImage;
				else
					mSelectedObject = eObjectNone;
				break;
*/
			default:
				cout << "selectedObjectChange: 0x";
				cout << std::hex << mSelectedObject << "\n";

				break;
	}

	cursorObjectUpdate();
}
