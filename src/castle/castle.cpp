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
 *  Castle
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "castle.h"
#include "castleManager.h"
#include "creep.h"
#include "vic-ii/screen.h"
#include "room.h"
#include "castle/object/object.h"
#include "castle/object/player.h"

cCastle::cCastle( cCreep *pCreep, cCastleInfo *pCastleInfo ) {
	mCastleInfo = pCastleInfo;
	mCreep = pCreep;
	
	mName = pCastleInfo->nameGet();
	mBuffer = pCastleInfo->bufferGet();
	mBuffer += 2;

	// Set the window title screen
	gCreep->screenGet()->levelNameSet( mName );

	// Copy it into the memory region
	memcpy( gCreep->memory(0x9800), mBuffer, pCastleInfo->bufferSizeGet() - 2 );

	// Load the highscores for this castle
	if( mCastleInfo->managerGet()->scoresLoad( mName, gCreep->memory(0xB800) ) == false ) {
		
		writeWord( gCreep->memory(0xB800), 0x007A );
		
		for( char Y = 0x77; Y >= 0; --Y )
			*gCreep->memory( 0xB802 + Y ) = 0xFF;

		gCreep->sub_2973();
	}

	//loadHeader();
}

cCastle::~cCastle() {
	map< size_t, cCastle_Room* >::iterator	roomIT;

	for( roomIT = mRooms.begin(); roomIT != mRooms.end(); ++roomIT )
		delete (*roomIT).second;
}

byte *cCastle::bufferGet(size_t &pBufferSize) {
	pBufferSize = mCastleInfo->bufferSizeGet();
	return mBuffer;
}

void cCastle::castleStart( size_t pPlayers ) {

	for( byte count = 0; count < pPlayers; ++count ) {
		mPlayer[count] = new cCastle_Object_Player( mStartRoom[count], mStartDoor[count] );
	}

	draw();
	
	mCreep->screenGet()->refresh();
}

void cCastle::draw() {
	cCastle_Room *room = mPlayer[ 0 ] ->roomGet();

	room->draw();
}

cCastle_Room *cCastle::roomGet( size_t pRoomNumber ) {
	map< size_t, cCastle_Room * >::iterator		roomIT;

	roomIT = mRooms.find( pRoomNumber );
	if(roomIT == mRooms.end())
		return 0;

	return roomIT->second;
}

void cCastle::loadHeader() {
	byte		 *roomDirectoryBuffer = 0;
	cCastle_Room *room = 0;

	for( size_t X = 0; ; ++X ) {
		roomDirectoryBuffer = roomDirPtrGet( X );

		room = new cCastle_Room( this, roomDirectoryBuffer );
		
		mRooms.insert( make_pair( X, room ) );

		roomDirectoryBuffer += 0x08;

		if(*roomDirectoryBuffer == 0xFF || *roomDirectoryBuffer == 0x40 )
			break;
	}

	mStartRoom[0] = roomGet( *(mBuffer + 3) );
	mStartRoom[1] = roomGet( *(mBuffer + 4) );

	mStartDoor[0] = mStartRoom[0]->objectDoorGet( *(mBuffer + 5) );
	mStartDoor[1] = mStartRoom[1]->objectDoorGet( *(mBuffer + 6) );

}
