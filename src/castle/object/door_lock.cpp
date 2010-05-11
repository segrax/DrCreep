#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "door_lock.h"

cCastle_Object_Door_Lock::cCastle_Object_Door_Lock( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Door_Lock::graphicUpdate() {
	byte *gfxPtr = mRoom->castleGet()->creepGet()->gameData( 0x6C53 );

	for( char Y = 8; Y >= 0; --Y )
		*(gfxPtr + Y) = mColor;
}

void cCastle_Object_Door_Lock::prepare() {
	cCastle_Image *image = 0;

	image = imageCreate();
	image->mBF00.mField_0 = 7;

	mColor = *(mObjectBuffer);
	mColor |= (mColor << 4);

	image->update( 0x58, *(mObjectBuffer + 3), *(mObjectBuffer + 4 ));
	
}
