#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "lightning.h"

cCastle_Object_Lightning::cCastle_Object_Lightning( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Lightning::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );

}

void cCastle_Object_Lightning::prepare() {
	byte byte_44E6;
	
	cCastle_Image *image = 0;
	//cCastle_Object_Image *objectImage = 0;
	
	byte gfxPosX = *(mObjectBuffer + 1);
	byte gfxPosY = *(mObjectBuffer + 2);

	//byte *objectMap = mRoom->objectMapGet( ( gfxPosX >> 2 ) - 4, ( gfxPosY >> 3 ) );
	
	image = imageCreate();
	
	image->mBE00.mObject = this;
	
	if( *(mObjectBuffer) & byte_45DD ) {
		image->mDecodeMode = 0;
		image->mBF00.mGfxID = 0x36;
		image->mBF00.mPosX = gfxPosX;
		image->mBF00.mPosY = gfxPosY;

		image = imageCreate();
		gfxPosX += 0x04;
		gfxPosY += 0x08;
		image->mBF00.mPosX = gfxPosX;
		image->mBF00.mPosY = gfxPosY;

		image->mBF00.mField_0 = 3;

		byte A;
		if( *(mObjectBuffer) & byte_45DE )
			A = 0x37;
		else
			A = 0x38;

		image->update( A, gfxPosX, gfxPosY );

	} else {
		image->mBF00.mField_0 = 2;

		byte_44E6 = *(mObjectBuffer + 3);

		for(;;) {
			if( !byte_44E6 )
				break;
			cCastle_Image *image = imageCreate();

			image->mBE00.mColor = *(mObjectBuffer + 3);
			image->mBF00.mGfxID = 0x32;
			image->mBF00.mPosX = gfxPosX;
			image->mBF00.mPosY = gfxPosY;
			
			gfxPosY += 0x08;
			--byte_44E6;
		}

		gfxPosX -= 0x04;

		image->update( 0x33, gfxPosX, gfxPosY );
		if( *(mObjectBuffer) & byte_45DE )
			image->mBF00.mField_4 |= byte_840;

	}
		
}
