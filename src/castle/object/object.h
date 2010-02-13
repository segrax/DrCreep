/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2010 Robert Crossfield
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  ------------------------------------------
 *  Castle Object
 *  ------------------------------------------
 */

class cCastle_Room;
class cCastle_Image;

class cBD00 {
public:
	byte			mField_0;		// BD00
	byte			mSpriteX;		// BD01
	byte			mSpriteY;		// BD02
	byte			mSpriteImageID; // BD03
	byte			mState;			// BD04
	byte			mField_5, mField_6, mField_7, mField_8;
	byte			mSpriteFlags;	// BD09
	byte			mField_A, mField_B;
	byte			mSpriteXAdd;	// BD0C
	byte			mSpriteYAdd;	// BD0D
	byte			mField_E, mField_F, mField_10, mField_11;
	byte			mField_12, mField_13, mField_14, mField_15;
	byte			mField_16, mField_17, mField_18, mField_19;
	byte			mField_1A, mField_1B;
	byte			mPlayerNumber;	// BD1C
	byte			mField_1D, mField_1E, mField_1F;
};

class cCastle_Object_Image {
public:
	byte			mID, mPosX, mPosY;
	byte			mGfxWidth, mGfxHeight;

	cCastle_Object_Image() {
		mID = mPosX = mPosY = mGfxWidth = mGfxHeight = 0;
	}
};

class cCastle_Object {
	
protected:
	vector<cCastle_Image*>			mImages;
	vector<cCastle_Object_Image*>	mObjectImage;

	cBD00			 mBD00;
	cCastle_Room	*mRoom;						// Players current room
	
	byte			*mObjectBuffer;


public:

					 cCastle_Object( cCastle_Room *pRoom );
					 cCastle_Object( cCastle_Room *pRoom, byte *&pObjectBuffer );
					~cCastle_Object();

	void			 draw(); 

	cCastle_Image					*imageCreate( );
	vector<cCastle_Image*>			*imagesGet() { return &mImages; }

	cCastle_Object_Image			*objectImageCreate();
	vector<cCastle_Object_Image*>	*objectImageGet() { return &mObjectImage; }

	virtual void	 graphicUpdate() = 0;


	inline cCastle_Room	*roomGet() { return mRoom; }
};
