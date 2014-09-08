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
 *  Castle
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "castle.h"
#include "castleManager.h"
#include "creep.h"
#include "vic-ii/screen.h"
#include "objects/object.hpp"
#include "room.hpp"

#ifdef _WII
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif

cCastle::cCastle( cCreep *pCreep, cCastleInfo *pCastleInfo ) {
	mStart_Room_Player1 = mStart_Room_Player2 = 0;
	mStart_Door_Player1 = mStart_Door_Player2 = 0;
	mLives_Player1 = mLives_Player2 = 3;
	
	mFinalRoom = 0;
	mFinalPtr = 0;
	mFinalScreen = 0;
	mCreep = pCreep;
	mCastleInfo = 0;
	mBuffer = 0;

	if(pCastleInfo) {

		mCastleInfo = pCastleInfo;
		mName = pCastleInfo->nameGet();
		mBuffer = pCastleInfo->bufferGet();
		mBuffer += 2;

		// Set the window title screen
		gCreep->screenGet()->levelNameSet( mName );

		// Copy the castle into game memory
		memcpy( gCreep->memory(0x9800), mBuffer, pCastleInfo->bufferSizeGet() - 2 );

		// Load the highscores for this castle
		if( mCastleInfo->managerGet()->scoresLoad( mName, gCreep->memory(0xB800) ) == false ) {
			
			// No highscores found, write a blank table

			// Size of table
			writeLEWord( gCreep->memory(0xB800), 0x007A );
			
			// Entries
			for( signed char Y = 0x77; Y >= 0; --Y )
				*gCreep->memory( 0xB802 + Y ) = 0xFF;

			gCreep->sub_2973();
		}
	} else {
		mBuffer = mCreep->memory( 0x7800 );
		mName = "Untitled";
		roomCreate( (cBuilder*) pCreep , 0 );
	}
}

cCastle::~cCastle() {

	roomCleanup();
}

void cCastle::castleLoad( cBuilder *pBuilder ) {
	byte *buffer = mBuffer;

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
		mFinalScreen = mBuffer + ( mFinalPtr - 0x7800 );

	buffer += 0x100;

	size_t count = 0;

	while( *buffer != 0xFF && *buffer != 0x40) {
		cRoom *room = roomCreate( pBuilder, count );
		mRooms.insert( make_pair( count++ , room) );

		room->roomLoad( &buffer );

		buffer += 2;
		room->mRoomDirPtr = mBuffer + (readLEWord(buffer) - 0x7800);
		buffer += 2;
	}

	buffer++;

	map< int, cRoom *>::iterator roomIT;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		cRoom *room = roomIT->second;

		buffer = room->mRoomDirPtr;

		room->roomLoadObjects( &buffer );

		buffer += 2;
	}

	if( mFinalScreen ) {
		mFinalRoom = roomCreate(pBuilder, -1);
		mFinalRoom->roomLoadObjects( &mFinalScreen );
	}
}	

void cCastle::castleSave( byte *pTarget ) {
	map< int, cRoom *>::iterator  roomIT;

	byte *buffer =  pTarget;

	for( word addr = 0x0; addr < 0x2000; ++addr )
		buffer[addr] = 0;

	*(buffer + 2) = 0x80;
	*(buffer + 3) = mStart_Room_Player1;
	*(buffer + 4) = mStart_Room_Player2;
	*(buffer + 5) = mStart_Door_Player1;
	*(buffer + 6) = mStart_Door_Player2;
	*(buffer + 7) = mLives_Player1;
	*(buffer + 8) = mLives_Player2;

	// Write the room directory
	buffer = pTarget + 0x100;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {	
		if( (char) roomIT->second->mNumber == -1 )
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
		buffer = (pTarget + 0x100) + (memDest - 0x7900);

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

	buffer = pTarget + (memDest - 0x7800);

	// Save final room ptr
	if(mFinalRoom) {
		writeLEWord( pTarget + 0x5F, memDest);

		// Save final room objects
		memDest += mFinalRoom->roomSaveObjects( &buffer );
	}

	writeLEWord( buffer, 0 );
	buffer += 2;

	memDest += 2;
	// Write size of castle to beginning to castle memory
	writeLEWord(  pTarget, (memDest - 0x7800) );
}

cRoom *cCastle::roomCreate( cBuilder *pBuilder, int pNumber ) {
	
	if( mRooms.find( pNumber ) != mRooms.end() )
		return mRooms.find( pNumber )->second;

	cRoom *room = new cRoom( pBuilder, this, pNumber );
	mRooms.insert( make_pair( pNumber, room ) );

	return room;
}

void cCastle::roomCleanup() {
	map< int, cRoom *>::iterator roomIT;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT ) {
		delete roomIT->second;
	}

	mRooms.clear();
}
