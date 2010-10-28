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
 *  Object
 *  ------------------------------------------
 */

class cRoom;

class cObjectRayGun : public cObject {
private:
	byte	mDirection;

protected:
	
	void partSetPosition( byte pPosX, byte pPosY, byte pPart ) {

		cObject::partSetPosition( pPosX, pPosY, pPart );
		// Set raygun position
		if( pPart == 0 )
			mParts[2].mY = pPosY;

		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 1;

		mParts[1].mCursorWidth = 2;
		mParts[1].mCursorHeight = 3;

	}
public:
	cObjectRayGun( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectRayGun;

		mParts[0].mDrags = true;
		mParts[0].mDragDirection = eDirectionDown;
		mParts[0].mLength = 1;

		mPartCount = 3;
		mDirection = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = mDirection;
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		
		*(*pBuffer)++ = mParts[0].mLength;
		*(*pBuffer)++ = mParts[2].mY;

		strSize += cObject::objectSave( pBuffer, 1 );

		return (strSize + 3);
	}

};
