/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2016 Robert Crossfield
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

class cObjectConveyor : public cObject {
public:
	byte	mState;

public:
	cObjectConveyor( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectConveyor;
		mPartCount = 2;
		mState = 0;

		mParts[0].mCursorWidth = 8;
		mParts[0].mCursorHeight = 1;

		mParts[1].mCursorWidth = 3;
		mParts[1].mCursorHeight = 1;
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {
		mState = *(*pBuffer)++;

		cObject::objectLoad( pBuffer, 0 );
		cObject::objectLoad( pBuffer, 1 );

		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = mState;

		size_t strSize = cObject::objectSave( pBuffer, 0 );
		strSize += cObject::objectSave( pBuffer, 1 );

		return (strSize + 1);
	}

	void		stateChange() {
		++mState;
		if(mState > 3 )
			mState = 0;
	}

};
