#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "door.h"
#include "castle/image.h"

byte	byte_41D1[ 2 ] = { 0x06, 0x96 };

cCastle_Object_Door::cCastle_Object_Door( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	mOpen = false;

	prepare( );
}

void cCastle_Object_Door::graphicUpdate() {
	cCastle_Room *roomTarget = 0;
	byte *gfxPtr = mRoom->castleGet()->creepGet()->gameData( 0x6390 );

	roomTarget = mRoom->castleGet()->roomGet( *(mObjectBuffer + 3) );
	byte A = roomTarget->colorGet() & 0xF;

	mImages[0]->mBE00.mColor = (A | ( A << 4 ));

	// Change color for inside of draw
	if( mOpen ) {
		byte Y = 5;
		while(Y) {
			*(gfxPtr + Y) = mImages[0]->mBE00.mColor;
			--Y;
		}
	}
}

void cCastle_Object_Door::prepare() {

	//word_41D3 = pObjectBuffer;

	cCastle_Object_Image *objectImage = new cCastle_Object_Image();

	objectImage->mID = byte_41D1[ *(mObjectBuffer + 7) ];
	objectImage->mPosX = *( mObjectBuffer + 0 );
	objectImage->mPosY = *( mObjectBuffer + 1 );

	mObjectImage.push_back( objectImage );

	// 4159
	cCastle_Image *image = imageCreate( );
	
	mImages.push_back( image );

	//image->mBE00.mObjNumber = count;
	image->mBF00.mField_0	= 0;

	byte A;

	if( *( mObjectBuffer + 2 ) & 0x80) {
		// Door Open
		image->mBE00.mField_1 = 1;
		A = 0x08;
		mOpen = true;
	} else {
		// Door Closed
		A = 0x07;
		mOpen = false;
	}

	// 41B2
	image->update( A, objectImage->mPosX + 0x04, objectImage->mPosY + 0x10 );
}
