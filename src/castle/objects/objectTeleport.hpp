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

class cObjectTeleport : public cObject {
public:
	byte	mTarget;

public:
	cObjectTeleport( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectTeleport;
		mPartCount = 2;
		mTarget = 1;
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

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		
		*(*pBuffer)++ = mTarget;

		for(word i = 1; i < mPartCount; ++i ) {
			strSize += cObject::objectSave( pBuffer, i );
		}
	
		//*(*pBuffer)++ = 0x00;

		return (strSize + 1);
	}

};
