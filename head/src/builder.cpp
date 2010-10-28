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
#include "builder.hpp"
#include "castle/objects/objectDoor.hpp"
#include "castle/objects/objectWalkway.hpp"
#include "castle/objects/objectSlidingPole.hpp"
#include "castle/objects/objectLadder.hpp"
#include "castle/objects/objectDoorBell.hpp"
#include "castle/objects/objectLightning.hpp"
#include "castle/objects/objectForcefield.hpp"
#include "castle/objects/objectMummy.hpp"
#include "castle/objects/objectKey.hpp"
#include "castle/objects/objectLock.hpp"
#include "castle/objects/objectRayGun.hpp"
#include "castle/objects/objectTeleport.hpp"
#include "castle/objects/objectTrapDoor.hpp"
#include "castle/objects/objectConveyor.hpp"
#include "castle/objects/objectFrankenstein.hpp"
#include "castle/objects/objectText.hpp"
#include "debug.h"

size_t cRoom::roomSaveObjects( byte **pBuffer ) {
	size_t size = 0;

	size = saveCount( pBuffer, eObjectDoor );
	size += saveObject( pBuffer, eObjectWalkway, 0x00 );
	size += saveObject( pBuffer, eObjectSlidingPole, 0x00 );
	size += saveObject( pBuffer, eObjectLadder, 0x00 );
	size += saveCount( pBuffer, eObjectDoorBell );
	size += saveObject( pBuffer, eObjectLightning, 0x20 );
	size += saveObject( pBuffer, eObjectForcefield, 0x00 );
	size += saveObject( pBuffer, eObjectMummy, 0x00 );
	size += saveObject( pBuffer, eObjectKey, 0x00 );
	size += saveObject( pBuffer, eObjectLock, 0x00 );
	size += saveObject( pBuffer, eObjectRayGun, 0x80 );
	size += saveObject( pBuffer, eObjectTeleport, 0x00 );
	size += saveObject( pBuffer, eObjectTrapDoor, 0x80 );
	size += saveObject( pBuffer, eObjectConveyor, 0x80 );
	size += saveObject( pBuffer, eObjectFrankenstein, 0x80 );
	size += saveObject( pBuffer, eObjectText, 0x00 );
	return size;
}

size_t cRoom::roomSave( byte **pBuffer ) {
	byte sizeFinal;
	
	mRoomDirPtr = *pBuffer;

	*(*pBuffer)++ = mColor;
	*(*pBuffer)++ = mMapX;
	*(*pBuffer)++ = mMapY;

	sizeFinal = mMapWidth;
	sizeFinal |= (mMapHeight << 3);

	*(*pBuffer)++ = sizeFinal;

	return 8;
}

vector< cObject* > cRoom::objectFind( eRoomObjects pType ) {
	vector< cObject* >			objects;
	vector< cObject* >::iterator	objectIT;

	for( objectIT = mObjects.begin(); objectIT != mObjects.end(); ++objectIT ) {

		if( (*objectIT)->objectTypeGet() == pType )
			objects.push_back( *objectIT );
	}	

	return objects;
}

size_t cRoom::saveCount( byte **pBuffer, eRoomObjects pObjectType ) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;
	size_t	size = 3;

	// No objects to save?
	if( objects.size() == 0 )
		return 0;

	// Write ID
	writeLEWord( *pBuffer, (word) pObjectType );
	*pBuffer += 2;

	// Number of objects
	*(*pBuffer)++ = objects.size();

	// Write each door
	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) 
		size += (*objectIT)->objectSave( pBuffer, 0 );

	return size;
}

size_t cRoom::saveObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker ) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;

	size_t	size = 3;

	// No poles to save?
	if( objects.size() == 0 )
		return 0;

	// Write ObjectID
	writeLEWord( *pBuffer, (word) pObjectType );
	*pBuffer += 2;

	// Write each Pole
	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) 
		size += (*objectIT)->objectSave( pBuffer, 0 );

	*(*pBuffer)++ = pEndMarker;

	return size;
}

cBuilder::cBuilder() {
	mCursorX = 0x10;
	mCursorY = 0;

	mRoomSelectedObject = 0;
	mCurrentObject = 0;
	mCurrentRoom = 0;

	mDragMode = false;

	mSelectedObject = eObjectsFinished;

	mStart_Door_Player1 = mStart_Door_Player2 = 0;
	mStart_Room_Player1 = mStart_Room_Player2 = 0;
	mLives_Player1 = mLives_Player2 = 3;
}

cBuilder::~cBuilder() {
	
}

void cBuilder::castleCreate() {

	cRoom *room0 = roomCreate(0);

	room0->mColor = 3;
	room0->mMapHeight = 3;
	room0->mMapWidth = 3;
	room0->mMapX = 3;
	room0->mMapY = 3;

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
	memcpy( &mMemory[ 0x8004 ], pString.mString.c_str(), pString.mString.size() );

	mMemory[ 0x8000 ] = pString.mPosX;
	mMemory[ 0x8001 ] = pString.mPosY;
	mMemory[ 0x8002 ] = pString.mColor;
	mMemory[ 0x8003 ] = 0x21;

	mMemory[ 0x8003 +  pString.mString.size() ] |= 0x80;

	word_3E = 0x8000;
	obj_stringPrint();

	obj_Actions();
	img_Actions();

	hw_Update();
}

void cBuilder::mainLoop() {

	mIntro = false;

	mScreen->bitmapLoad( &mMemory[ 0xE000 ], &mMemory[ 0xCC00 ], &mMemory[ 0xD800 ], 0 );
	mScreen->cursorSet( mCursorX, mCursorY );

	// TODO: Check if we load a castle instead of make a new one
	castleCreate();
	mCurrentRoom = roomCreate(0);
	castleSave();
	
	mScreen->cursorEnabled(true);

	// Set player1 start room
	mMemory[ 0x7809 ] = mMemory[ 0x7803 ];
	mMemory[ 0x780B ] = mMemory[ 0x7805 ];

	roomLoad();
	objectStringsPrint();

	while(!mQuit) {
		byte key = tolower( mInput->keyGet() );
		
		// Check keyboard input
		switch(key) {

			case 'q':	// Change Selected Placement Object 'Up'
				selectedObjectChange( true );
				break;

			case 'a':	// Change Selected Placement Object 'Down'
				selectedObjectChange( false );
				break;

			case '[':	// Select a placed object
				selectPlacedObject( false );
				break;

			case ']':	// Select a placed object
				selectPlacedObject( true );
				break;

			case 0x7F:	// Delete Key
				selectedObjectDelete();
				break;

			default:
				//if(key) {
				//	cout << "0x";
				//	cout << hex << (int) key << endl;
				//}
				break;
		}

		// Check 'joystick' input
		parseInput();

		// Force draw of sprites
		interruptWait( 1 );
		obj_Actions();

		// Redraw screen
		hw_Update();
	}

}

void cBuilder::messagePrint( string pMessage ) {

	objectStringAdd(pMessage, 0x10, 0xC0, 2);
}

void cBuilder::cursorObjectUpdate() {

	// Cursor object has changed?
	if( mCurrentObject && mCurrentObject->objectTypeGet() != mSelectedObject ) {

		// Dont delete if its placed
		if( mCurrentObject->partGet(0)->mPlaced == false ) {
			mCurrentRoom->objectDelete( mCurrentObject );
			delete mCurrentObject;
		} else
			if( mCurrentObject->partGet()->mPlaced == false )
				mCurrentObject->partDel();

		mCurrentObject = 0;
	}

	if( !mCurrentObject )
		mCurrentObject = objectCreate( mSelectedObject, mCursorX, mCursorY );
	else
		mCurrentObject->partSetPosition( mCursorX, mCursorY );

	if( mCurrentObject && mCurrentObject->isPlaced() && !mCurrentObject->isSelected())
		mCurrentObject = 0;

	cursorUpdate();
}

void cBuilder::cursorUpdate() {

	// Set cursor position
	mScreen->cursorSet( mCursorX, mCursorY );

	// Set cursor size
	if(mCurrentObject && mCurrentObject->partGet() )
		mScreen->cursorSize( mCurrentObject->partGet()->mCursorWidth, mCurrentObject->partGet()->mCursorHeight );
	else
		mScreen->cursorSize( 1, 1 );

	castlePrepare();
}

void cBuilder::castlePrepare() {
	castleSave();
	roomLoad();

	objectStringsPrint();

	// Force draw of sprites
	obj_Actions();
	img_Actions();
}

void cBuilder::parseInput() {
	bool			 update = false;
	sPlayerInput	*input = mInput->inputGet(0);

	sObjectPart		*part = 0;
	
	// Get the current selected 'part' of the object
	if(mCurrentObject)
		part = mCurrentObject->partGet();


	if(input->mLeft) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionLeft:
					part->mLength++;
					break;
				case eDirectionRight:
					part->mLength--;
					break;
			}
		} else
			mCursorX -= 4;
		update = true;
	}

	if(input->mRight) {

		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionLeft:
					part->mLength--;
					break;
				case eDirectionRight:
					part->mLength++;
					break;
			}
		} else
			mCursorX += 4;
		update = true;
	}

	if(input->mDown) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionDown:
					part->mLength++;
					break;
				case eDirectionUp:
					part->mLength--;
					break;
			}
		} else
			mCursorY += 8;
		update = true;
	}

	if(input->mUp) {
		if( part && mDragMode ) {

			switch( part->mDragDirection ) {
				case eDirectionDown:
					part->mLength--;
					break;
				case eDirectionUp:
					part->mLength++;
					break;
			}
		} else
			mCursorY -= 8;
		update = true;
	}

	if(input->mButton) {
		mInput->inputCheck( true );

		if( part && part->mDrags == true ) {
			if( mDragMode == false ) 
				mDragMode = true;
			else {
				mDragMode = false;
				if(mCurrentObject)
					mCurrentObject->partPlace();
			}

		} else {
			mDragMode = false;
			if(mCurrentObject)
				mCurrentObject->partPlace();
		}

		
		update = true;
	}

	if(update) 
		cursorObjectUpdate();
}

cRoom *cBuilder::roomCreate( size_t pNumber ) {
	cRoom *room = new cRoom( pNumber );

	if( mRooms.find( pNumber ) != mRooms.end() )
		return mRooms.find( pNumber )->second;

	mRooms.insert( make_pair( pNumber, room ) );

	return room;
}

void cBuilder::castleSave( ) {
	map< size_t, cRoom *>::iterator  roomIT;

	byte *buffer =  &mMemory[ 0x7800 ];

	for( word addr = 0x7800; addr < 0x8200; ++addr )
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
		roomIT->second->roomSave( &buffer );

		// Skip the pointers to room objects for now
		buffer += 4;
	}

	// Room Directory Terminator
	*(buffer + 5) = 0xFF;

	word	memDest = 0x7900 + (mRooms.size() * 8) + 1;
	size_t	size = 0;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		buffer = &mMemory[ memDest ];

		// Write pointer to room objects
		byte *dirBuffer = roomIT->second->mRoomDirPtr + 4;
		writeLEWord( dirBuffer, memDest + 2);
		writeLEWord( dirBuffer + 2, memDest );

		// Save all objects, and add up memory size
		memDest += roomIT->second->roomSaveObjects( &buffer );
	}
}

cObject *cBuilder::objectCreate( eRoomObjects pObject, byte pPosX, byte pPosY ) {
	cObject *obj = 0;

	switch( pObject ) {
			case eObjectsFinished:			// Finished
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

			case eObjectImageDraw:
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

	if(mCurrentRoom)
		mCurrentRoom->objectAdd( obj );

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

	return 0;
}

cObject *cBuilder::obj_Multi_Create( byte pPosX, byte pPosY ) {

	return 0;
}

void cBuilder::selectedObjectDelete() {
	if( mCurrentObject) {
		mCurrentRoom->objectDelete( mCurrentObject );
		delete mCurrentObject;
		mCurrentObject = 0;
	}

	cursorUpdate();
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

	if( mCurrentObject ) {
		if( mCurrentObject->mPartGet() < mCurrentObject->mPartCountGet() && pChangeUp )
			mCurrentObject->partSet( mCurrentObject->mPartGet() + 1 );
		
		else if( mCurrentObject->mPartGet() > 0 && !pChangeUp )
			mCurrentObject->partSet( mCurrentObject->mPartGet() - 1 );

		else
			changeObject = true;

		// Last Part?
		if( mCurrentObject->mPartGet() == mCurrentObject->mPartCountGet() )
			changeObject = true;

	} else
		changeObject = true;

	if( changeObject ) {

		if(mCurrentObject)
			mCurrentObject->isSelected(false);

		if( pChangeUp )
			++mRoomSelectedObject;
		else
			--mRoomSelectedObject;

		// Get the next object
		mCurrentObject = mCurrentRoom->objectGet( mRoomSelectedObject );
		if( mCurrentObject == 0 ) {
			mRoomSelectedObject = 0;
			mCurrentObject = mCurrentRoom->objectGet( mRoomSelectedObject );
		}
		
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
			case eObjectsFinished:			// Finished
				if( pChangeUp )
					mSelectedObject = eObjectMultiDraw;
				else
					mSelectedObject = eObjectDoor;
				break;

			case eObjectDoor:			
				if( pChangeUp )
					mSelectedObject = eObjectsFinished;
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
					mSelectedObject = eObjectImageDraw;
				break;

			case eObjectImageDraw:
				if( pChangeUp )
					mSelectedObject = eObjectText;
				else
					mSelectedObject = eObjectMultiDraw;
				break;

			case eObjectMultiDraw:
			case 0x160A:				// Intro
				if( pChangeUp )
					mSelectedObject = eObjectImageDraw;
				else
					mSelectedObject = eObjectsFinished;
				break;

			default:
				cout << "selectedObjectChange: 0x";
				cout << std::hex << mSelectedObject << "\n";

				break;
	}

	cursorObjectUpdate();
}
