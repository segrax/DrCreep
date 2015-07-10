/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2015 Robert Crossfield
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

class cObjectKey : public cObject {
public:
	byte	mKeyColor;

public:
	cObjectKey( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectKey;
		mKeyColor = 1;

		mParts[0].mCursorWidth = 2;
		mParts[0].mCursorHeight = 3;
	}

	virtual void		stateChange()	 { 
		++mKeyColor;
		if(mKeyColor > 7)
			mKeyColor = 1;
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {
		mKeyColor = *(*pBuffer)++;
		*(*pBuffer)++;
		cObject::objectLoad( pBuffer, pPart );

		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = mKeyColor;
		*(*pBuffer)++ = mKeyColor + 0x50;

		size_t strSize = cObject::objectSave( pBuffer, pPart );

		return (strSize + 2);
	}

};
