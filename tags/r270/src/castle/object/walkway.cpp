#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "walkway.h"
#include "castle/image.h"

cCastle_Object_Walkway::cCastle_Object_Walkway( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Walkway::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );

}

void cCastle_Object_Walkway::prepare() {
	
	cCastle_Object_Image *objectImage = 0;

	mLength = *(mObjectBuffer);
	
	byte gfxPosX = *(mObjectBuffer + 1);
	byte gfxPosY = *(mObjectBuffer + 2);

	byte byte_1744 = 1;
	byte *objectMap = mRoom->objectMapGet( ( gfxPosX >> 2 ) - 4, ( gfxPosY >> 3 ) );
	
	for(;;) {

		objectImage = new cCastle_Object_Image();
		mObjectImage.push_back( objectImage );

		if( byte_1744 != 1 ) {
			if( byte_1744 != mLength )
				objectImage->mID = 0x1C;
			else
				objectImage->mID = 0x1D;
		} else 
			objectImage->mID = 0x1B;

		objectImage->mPosX = gfxPosX;
		objectImage->mPosY = gfxPosY;

		objectImage->mGfxWidth = *mRoom->castleGet()->creepGet()->gfxPtr( objectImage->mID );
		objectImage->mGfxHeight = *(mRoom->castleGet()->creepGet()->gfxPtr( objectImage->mID ) + 1);
		
		byte byte_1745 = 1;
		byte A;

		// 16D1
		for(;;) {	
			
			if( byte_1744 != 1 ) {
				
				if( byte_1744 != mLength )
					A = 0x44;

				else {
					// 16EE
					if( byte_1745 == objectImage->mGfxWidth )
						A = 0x40;
					else
						A = 0x44;
				}

			} else {
				// 16E2
				if( byte_1745 == 1 )
					A = 0x04;
				else
					A = 0x44;
			}

			// 16F8
			(*objectMap) |= A;
			
			++byte_1745;
			objectMap += 2;

			if( byte_1745 > objectImage->mGfxWidth )
				break;
		}
		
		gfxPosX += (objectImage->mGfxWidth << 2);
		++byte_1744;

		if( byte_1744 > mLength )
			break;
	}
}
