#include "stdafx.h"
#include "castle.h"
#include "creep.h"
#include "image.h"
#include "room.h"
#include "object/object.h"

cCastle_Image::cCastle_Image( cCastle_Object *pObject ) {
	mDecodeMode = mTxtCurrentID = mTxtX_0 = mTxtY_0 = 0;

	mBE00.mObject = pObject;
	mObject = pObject;
	
	mBF00.mField_4 = byte_83F;
}

void cCastle_Image::update( byte pID, byte pPosX, byte pPosY ) {

	if( !(mBF00.mField_4 & byte_83F) ) {

		mDecodeMode = 2;
		mTxtX_0 = mBF00.mPosX;
		mTxtY_0 = mBF00.mPosY;
		mTxtCurrentID = mBF00.mGfxID;

	} else {
		mDecodeMode = 0;
	}
	
	//57AE
	mBF00.mField_4	= ((byte_83F ^ 0xFF) & mBF00.mField_4);
	mBF00.mGfxID	= pID;
	mBF00.mPosX		= pPosX;
	mBF00.mPosY		= pPosY;
	
	word  gfxAddress  = (0x603B + (pID << 1));
	byte *gfxTablePtr = mObject->roomGet()->castleGet()->creepGet()->gameData( gfxAddress );
	byte *gfxPtr = mObject->roomGet()->castleGet()->creepGet()->gameData( *((word*) gfxTablePtr ));

	mBF00.mWidth	= *gfxPtr;
	mBF00.mHeight	= *(gfxPtr + 1);

	mBF00.mWidth <<= 2;
}

void cCastle_Image::draw() {
	
	mObject->roomGet()->castleGet()->creepGet()->mTxtX_0 = mTxtX_0;
	mObject->roomGet()->castleGet()->creepGet()->mTxtY_0 = mTxtY_0;

	mObject->roomGet()->screenDraw( mDecodeMode, mBF00.mGfxID, mBF00.mPosX, mBF00.mPosY, mTxtX_0, mTxtY_0, mTxtCurrentID );
}
