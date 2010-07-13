#include "stdafx.h"
#include "creep.h"
#include "castle/castle.h"
#include "castle/room.h"
#include "object.h"
#include "doorButton.h"
#include "castle/image.h"

cCastle_Object_Door_Button::cCastle_Object_Door_Button( cCastle_Room *pRoom, byte *&pObjectBuffer ) : cCastle_Object( pRoom, pObjectBuffer ) {
	mDoorImage = 0;

	prepare( );
}

void cCastle_Object_Door_Button::graphicUpdate() {
	mColor = mDoorImage->mBE00.mColor;
	
	byte color = mColor;
	
	for( signed char Y = 8; Y >= 0; --Y )
		*(mRoom->castleGet()->creepGet()->gameData( 0x63D2 + Y ) ) = color;

	// color
	color >>= 4;
	color |= 0x10;

	*(mRoom->castleGet()->creepGet()->gameData( 0x63D6 ) ) = color;
}

void cCastle_Object_Door_Button::prepare() {
	cCastle_Image *image = imageCreate();
	image->mBF00.mField_0 = 1;
	
	image->mBF00.mPosX = *(mObjectBuffer );
	image->mBF00.mPosY = *(mObjectBuffer + 1);
	image->mBF00.mGfxID = 0x09;
	image->mBE00.mObject = (cCastle_Object*) mRoom->objectDoorGet( *(mObjectBuffer + 2) );
	
	vector< cCastle_Object * >				*objects = mRoom->objectsGet();
	vector< cCastle_Object * >::iterator	 objectIT;
	
	vector< cCastle_Image * >				*images;
	vector< cCastle_Image * >::iterator		 imageIT;

	mColor = 0;
	mDoorImage = 0;

	for( objectIT = objects->begin(); objectIT != objects->end(); ++objectIT ) {
		images = (*objectIT)->imagesGet();

		for( imageIT = images->begin(); imageIT != images->end(); ++imageIT ) {
			
			if( (*imageIT)->mBF00.mField_0 == 0 ) {
			
				if( (*imageIT)->mBE00.mObject == image->mBE00.mObject ) {
					mDoorImage = (*imageIT);
					break;
				}
			}
		}
		
		if(mDoorImage)
			break;
	}

	image->update( 0x09, image->mBF00.mPosX, image->mBF00.mPosY );
}
