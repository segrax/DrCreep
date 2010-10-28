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

struct sObjectPart {
	byte		mX, mY;
	bool		mSet;
	bool		mDrags;					// object will be dragged when being placed
	eDirection	mDragDirection;
	size_t		mLength;
	bool		mPlaced;
	byte		mCursorWidth, mCursorHeight;

	sObjectPart() { mX = mY = 0; 
				  mSet = false;
				  mDrags = false;
				  mPlaced = false;
				  mLength = 0;
				  mDragDirection = eDirectionNone;
				  mCursorWidth = mCursorHeight = 1;
				}
};

class cObject {
protected:
	eRoomObjects	 mObjectID;
	sObjectPart		 mParts[16];
	
	cRoom			*mRoom;
	
	virtual void partSetPosition( byte pPosX, byte pPosY, byte pPart ) {
		mParts[pPart].mX = pPosX;
		mParts[pPart].mY = pPosY;

		mParts[pPart].mSet = true;
	}

	bool			 mPartsAdd;
	bool			 mPlaced;
	byte			 mPart,		mPartCount;

public:

	inline bool  isPlaced()			{ return mPlaced; }

	void		 mPartAdd()			{ ++mPart; }
	sObjectPart	*partGet()			{ return &mParts[ mPart ]; }
	sObjectPart	*partGet( size_t pPart )			{ return &mParts[ pPart ]; }
	void partSetPosition( byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, mPart );
	}

	virtual void partPlace() {
		mParts[mPart].mPlaced = true;

		++mPart;

		if(mPart >= mPartCount)
			mPlaced = true;
	}

	virtual void partDel() {
		return;
	}

	cObject( cRoom *pRoom, byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, 0 );
		partSetPosition( pPosX, pPosY, 1 );
		partSetPosition( pPosX, pPosY, 2 );

		mPlaced = false;
		mPart = 0;
		mPartCount = 1;
		mPartsAdd = false;

		mObjectID = eObjectsFinished;

		mRoom = pRoom;
	}
	
	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual void	partAdd() {
		mPlaced = false;
		++mPartCount;
		return;
	}

	virtual size_t	objectLoad( byte **pBuffer ) = 0;	// Load object from room stream
	virtual size_t	objectSave( byte **pBuffer, size_t pPart ) {	// Save object to room stream
		*(*pBuffer)++ = mParts[pPart].mX;
		*(*pBuffer)++ = mParts[pPart].mY;

		return 2;
	}
};
