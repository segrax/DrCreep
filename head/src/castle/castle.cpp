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

#ifdef _WII
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif

cCastle::cCastle( cCreep *pCreep, cCastleInfo *pCastleInfo ) {
	mCastleInfo = pCastleInfo;
	mCreep = pCreep;
	
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

	//loadHeader();
}

cCastle::~cCastle() {

}
