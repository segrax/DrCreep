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

class cObjectLock : public cObject {
public:
	byte	mLockColor, mDoorNumber;

public:
	cObjectLock( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLock;
		mLockColor = 1;
		mDoorNumber = 0;
		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = mLockColor;
		*(*pBuffer)++ = mLockColor + 0x57;
		*(*pBuffer)++ = mDoorNumber;

		size_t strSize = cObject::objectSave( pBuffer, pPart );

		return (strSize + 2);
	}

};
