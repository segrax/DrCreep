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

class cObjectForcefield : public cObject {
public:

public:
	cObjectForcefield( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectForcefield;
		mPartCount = 2;
		
		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;

		mParts[1].mCursorWidth = 2;
		mParts[1].mCursorHeight = 4;

	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {
		cObject::objectLoad( pBuffer, 0 );
		cObject::objectLoad( pBuffer, 1 );
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		strSize += cObject::objectSave( pBuffer, 1 );

		return (strSize);
	}

};
