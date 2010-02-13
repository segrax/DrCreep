#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "slidingpole.h"

cCastle_Object_SlidingPole::cCastle_Object_SlidingPole( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_SlidingPole::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );
}

void cCastle_Object_SlidingPole::prepare() {
	
	cCastle_Object_Image *objectImage = 0;
	cCastle_Image		 *image = 0;

	mLength = *(mObjectBuffer);
	
	byte gfxPosX = *(mObjectBuffer + 1);
	byte gfxPosY = *(mObjectBuffer + 2);

	byte *objectMap = mRoom->objectMapGet( ( gfxPosX >> 2 ) - 4, ( gfxPosY >> 3 ) );
	
	for( word count = mLength; ; ) {

		// Is there walkway near here?
		if( ((*objectMap) & 0x44) )  {
			image = imageCreate();

			image->mTxtX_0 = gfxPosX - 4;
			image->mTxtY_0 = gfxPosY;
			image->mDecodeMode = 2;
			image->mBF00.mPosX = gfxPosX;
			image->mBF00.mPosY = gfxPosY;
			image->mBF00.mGfxID = 0x27;
			image->mTxtCurrentID = 0x25;

		} else {
			objectImage = new cCastle_Object_Image();

			objectImage->mPosX = gfxPosX;
			objectImage->mPosY = gfxPosY;
			objectImage->mID = 0x24;

			mObjectImage.push_back( objectImage );
		}

		(*objectMap) |= 0x10;

		--count;
		if( !count )
			return;

		gfxPosY += 0x08;
		objectMap += 0x50;
	}
	
}
