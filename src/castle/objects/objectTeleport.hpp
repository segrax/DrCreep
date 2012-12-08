/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2013 Robert Crossfield
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

class cObjectTeleport : public cObject {
public:
	byte	mTarget;

public:
	cObjectTeleport( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectTeleport;
		mPartCount = 2;
		mTarget = 1;

		mParts[0].mCursorWidth = 4;
		mParts[0].mCursorHeight = 4;

		mParts[1].mCursorWidth = 2;
		mParts[1].mCursorHeight = 2;
	}

	void	partAdd() {
		++mPartCount;
		mPlaced = false;
		return;
	}

	void partPlace() {
		cObject::partPlace();
		
		if(mPart >= mPartCount)
			partAdd();

		if( mPartCount > 0x13 ) {
			--mPartCount;
			mPlaced = true;
		}
		
	}

	void partDel() {
		--mPart;
		--mPartCount;
		mPlaced = true;
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {
		cObject::objectLoad( pBuffer, 0 );

		mTarget = *(*pBuffer)++;

		mPart = 1;
		while( *(*pBuffer) != 0x00 ) {
			cObject::objectLoad( pBuffer, mPart );
			++mPart;
			if(mPart == mPartCount)
				++mPartCount;
		}

		--mPartCount;
		(*pBuffer)++;
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		
		*(*pBuffer)++ = mTarget;

		for(word i = 1; i < mPartCount; ++i ) {
			strSize += cObject::objectSave( pBuffer, i );
		}
	
		return (strSize + 1);
	}

};
