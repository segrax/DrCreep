#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "ladder.h"

cCastle_Object_Ladder::cCastle_Object_Ladder( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Ladder::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );

}

void cCastle_Object_Ladder::prepare() {
	cCastle_Image *image = 0;
	cCastle_Object_Image *objectImage = 0;

	mLength = *(mObjectBuffer);
	
	byte gfxPosX = *(mObjectBuffer + 1);
	byte gfxPosY = *(mObjectBuffer + 2);

	byte *objectMap = mRoom->objectMapGet( ( gfxPosX >> 2 ) - 4, ( gfxPosY >> 3 ) );
	
	for( word count = mLength; ; ) {

		// Is there walkway near here?
		if( ((*objectMap) & 0x44) )  {
			image = imageCreate();

			if( count == 1 ) {
				
				image->mTxtX_0 = gfxPosX;
				image->mTxtY_0 = gfxPosY;
				image->mDecodeMode = 2;
				image->mBF00.mPosX = gfxPosX;
				image->mBF00.mPosY = gfxPosY;
				image->mBF00.mGfxID = 0x29;
				image->mTxtCurrentID = 0x2A;
			} else {
				image->mTxtX_0 = gfxPosX - 4;
				image->mTxtY_0 = gfxPosY;
				image->mDecodeMode = 2;
				image->mBF00.mPosX = gfxPosX - 4;
				image->mBF00.mPosY = gfxPosY;
				image->mBF00.mGfxID = 0x2C;
				image->mTxtCurrentID = 0x2D;
			}

		} else {

			objectImage = new cCastle_Object_Image();

			objectImage->mPosX = gfxPosX;
			objectImage->mPosY = gfxPosY;
			if( count == 1 )
				objectImage->mID = 0x2B;
			else
				objectImage->mID = 0x28;

			mObjectImage.push_back( objectImage );
		}

		//189C
		if( count != mLength )
			(*objectMap) |= 1;

		--count;
		if( !count )
			return;

		(*objectMap) |= 0x10;
		gfxPosY += 0x08;
		objectMap += 0x50;
	}
}
