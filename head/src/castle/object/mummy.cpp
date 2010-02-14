#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "mummy.h"

cCastle_Object_Mummy::cCastle_Object_Mummy( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Mummy::graphicUpdate() {
	
	for( char Y = 5; Y >= 0; --Y )
		*mRoom->castleGet()->creepGet()->gameData( 0x68F0 + Y ) = 0x66;
}

void cCastle_Object_Mummy::prepare() {
	cCastle_Image *image = 0;
		
	image = imageCreate();
	
	image->mBF00.mField_0 = 5;
	image->mBE00.mObject = this;
	image->mBE00.mField_2 = 0x66;
	
	image->update( 0x44, *(mObjectBuffer + 1), *(mObjectBuffer + 2) );
	
	byte gfxPosX, gfxPosY = *( mObjectBuffer + 4 );

	for( byte byte_498E = 3; byte_498E; --byte_498E ) {
		
		gfxPosX = *( mObjectBuffer + 3 );

		for( byte byte_498F = 5; byte_498F; --byte_498F ) {
			image = imageCreate();
			image->mBF00.mGfxID = 0x42;
			image->mBF00.mPosX = gfxPosX;
			image->mBF00.mPosY = gfxPosY;
			gfxPosX += 0x04;
		}

		gfxPosY += 0x08;
	}

	if( *mObjectBuffer != 1 ) {
		
		byte txtX_0 = *(mRoom->castleGet()->creepGet()->gameData( *(mObjectBuffer + 3))) + 0x04;
		byte txtY_0 = *(mRoom->castleGet()->creepGet()->gameData( *(mObjectBuffer + 4))) + 0x08;
		
		for( byte byte_498E = 3; byte_498E; --byte_498E ) {
			image = imageCreate();
			image->mTxtX_0 = txtX_0;
			image->mTxtY_0 = txtY_0;

			txtX_0 += 0x04;
		}
		
		image = imageCreate();
		image->mBF00.mPosX = txtX_0 - 0x0C;
		image->mBF00.mPosY = txtY_0;
		image->mBF00.mGfxID = 0x43;
		image->mTxtCurrentID = 0x42;
		image->mDecodeMode = 0;
		
		if( *mObjectBuffer == 2 )
			add( 0xFF );
	}

}

void cCastle_Object_Mummy::add( byte pA ) {
	
	mBD00.mField_0 = 3;
	mBD00.mField_1B = 0xFF;
	mBD00.mPlayerNumber = 0xFF;

	// BD1D
	mBD00.mImage = imageGet(0)->mBE00.mObject;
	
	mBD00.mSpriteXAdd = 5;
	mBD00.mSpriteYAdd = 0x11;
	mBD00.mSpriteImageID = 0xFF;

	if( pA == 0 ) {
		mBD00.mField_1E = 0;
		mBD00.mField_1F = 0xFF;
		mBD00.mField_6 = 4;

		mBD00.mSpriteX = imageGet(1)->mBF00.mPosX + 0x0D;
		mBD00.mSpriteY = imageGet(1)->mBF00.mPosY + 0x08;

	} else {
		mBD00.mField_1E = 1;
		mBD00.mSpriteX = imageGet(2)->mBF00.mPosX + 0x0D;
		mBD00.mSpriteY = imageGet(2)->mBF00.mPosY + 0x08;
		mBD00.mField_6 = 2;
	}

}
