#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "castle/image.h"
#include "key.h"

cCastle_Object_Key::cCastle_Object_Key( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {

	prepare( );
}

void cCastle_Object_Key::graphicUpdate() {
	
	mRoom->castleGet()->creepGet()->graphicRoomColorsSet( mRoom->colorGet() );

}

void cCastle_Object_Key::prepare() {
	cCastle_Image *image = 0;

	image = imageCreate();
	
	image->mBE00.mObject = this;
	image->mBF00.mField_0 = 6;

	image->mBE00.mObject = this;

	image->update( *(mObjectBuffer + 1), *(mObjectBuffer + 2), *(mObjectBuffer + 3 ));
}
