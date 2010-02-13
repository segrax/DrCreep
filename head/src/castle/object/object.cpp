#include "stdafx.h"
#include "creep.h"
#include "object.h"
#include "castle/room.h"
#include "castle/castle.h"
#include "castle/image.h"

cCastle_Object::cCastle_Object( cCastle_Room *pRoom, byte *&pObjectBuffer ) {
	
	mObjectBuffer = pObjectBuffer;
	mRoom = pRoom;

}

cCastle_Object::cCastle_Object( cCastle_Room *pRoom ) {

	mObjectBuffer = 0;
	mRoom = pRoom;

}

cCastle_Object::~cCastle_Object() {
	vector<cCastle_Image*>::iterator		imageIT;
	vector<cCastle_Object_Image*>::iterator objectImageIT;

	for( imageIT = mImages.begin(); imageIT != mImages.end(); ++imageIT )
		delete (*imageIT);

	for( objectImageIT = mObjectImage.begin(); objectImageIT != mObjectImage.end(); ++objectImageIT )
		delete (*objectImageIT);
}

void cCastle_Object::draw() {
	vector<cCastle_Image*>::iterator			imageIT;
	vector<cCastle_Object_Image*>::iterator		objectImageIT;
	
	graphicUpdate();

	for( objectImageIT = mObjectImage.begin(); objectImageIT != mObjectImage.end(); ++objectImageIT ) {
		mRoom->screenDraw( 0, (*objectImageIT)->mID, (*objectImageIT)->mPosX, (*objectImageIT)->mPosY );
	}

	for( imageIT = mImages.begin(); imageIT != mImages.end(); ++imageIT )
		(*imageIT)->draw();
}

cCastle_Image *cCastle_Object::imageCreate( ) {
	cCastle_Image *image = new cCastle_Image( this );

	mImages.push_back( image );

	return image;
}

cCastle_Object_Image *cCastle_Object::objectImageCreate() {
	cCastle_Object_Image *objectImage = new cCastle_Object_Image( );

	mObjectImage.push_back( objectImage );

	return objectImage;
}
