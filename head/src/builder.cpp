#include "stdafx.h"
#include "graphics/screenSurface.h"
#include "vic-ii/screen.h"
#include "vic-ii/sprite.h"
#include "playerInput.h"
#include "castle/castle.h"
#include "castleManager.h"
#include "creep.h"
#include "sound/sound.h"
#include "builder.hpp"

#include "debug.h"

cBuilder::cBuilder() {
	mCursorX = mCursorY = 1;

	mSelectedItem = 0;
	mSelectedRoom = 0;
}

cBuilder::~cBuilder() {
	
}

void cBuilder::mainLoop() {

	mScreen->bitmapLoad( &mMemory[ 0xE000 ], &mMemory[ 0xCC00 ], &mMemory[ 0xD800 ], 0 );

	word_30 = 0xC000;

	while( word_30 < 0xC800) {
		mMemory[word_30++] = 0;
	}

	while(!mQuit) {
		byte key = mInput->keyGet();
		
		switch(key) {

			case 'q':
				break;
		}

		hw_Update();

	}

}

sRoom *cBuilder::roomCreate( size_t pNumber ) {
	sRoom *room = new sRoom();

	room->mNumber = pNumber;

	if( mRooms.find( pNumber ) != mRooms.end() )
		return mRooms.find( pNumber )->second;

	mRooms.insert( make_pair( pNumber, room ) );

	return room;
}
