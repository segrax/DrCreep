#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "forcefield.h"

cCastle_Object_Forcefield::cCastle_Object_Forcefield( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Forcefield::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );

	for( char Y = 7; Y >= 0; --Y )
		*mRoom->castleGet()->creepGet()->gameData( 0x6889 + Y ) = 0x55;
}

void cCastle_Object_Forcefield::prepare() {
	
	cCastle_Object_Image *objectImage = 0;
	cCastle_Image		 *image = 0;
	byte				  gfxPosX;
	byte				  gfxPosY;
	
	// Outside of Timer
	image = imageCreate();
	image->mBF00.mField_0 = 4;

	gfxPosX = *(mObjectBuffer);
	gfxPosY = *(mObjectBuffer + 1);

	image->mBF00.mPosX = gfxPosX;
	image->mBF00.mPosY = gfxPosY;
	image->mBF00.mGfxID = 0x3F;
	image->mDecodeMode = 0;
	
	// Inside of timer
	// 46EA
	gfxPosX += 0x04;
	gfxPosY += 0x08;

	image = imageCreate();
	image->update( 0x40, gfxPosX, gfxPosY ); 
	image->mBE00.mObject = this;

	mActive = true;

	create();

	gfxPosX = *(mObjectBuffer + 2);
	gfxPosY = *(mObjectBuffer + 3);

	image = imageCreate();

	image->mBF00.mGfxID = 0x3E;
	image->mBF00.mPosX = gfxPosX;
	image->mBF00.mPosY = gfxPosY;

}

void cCastle_Object_Forcefield::create() {
	
	mBD00.mField_0 = 2;
	mBD00.mSpriteX = *(mObjectBuffer + 2);
	mBD00.mSpriteY = (*(mObjectBuffer + 3)) + 2;
	mBD00.mSpriteImageID = 0x35;

	mBD00.mObject = this;
	mBD00.mField_1E = 0;
	mBD00.mField_6= 4;
	mBD00.mSpriteXAdd = 2;
	mBD00.mSpriteYAdd = 0x19;
}
