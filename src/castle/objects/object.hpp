/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2014 Robert Crossfield
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

enum eRoomObjects {
	eObjectDoor			= 0x0803,
	eObjectWalkway		= 0x0806,
	eObjectSlidingPole	= 0x0809,
	eObjectLadder		= 0x080C,
	eObjectDoorBell		= 0x080F,
	eObjectLightning	= 0x0812,
	eObjectForcefield	= 0x0815,
	eObjectMummy		= 0x0818,
	eObjectKey			= 0x081B,
	eObjectLock			= 0x081E,
	eObjectRayGun		= 0x0824,
	eObjectTeleport		= 0x0827,
	eObjectTrapDoor		= 0x082A,
	eObjectConveyor		= 0x082D,
	eObjectFrankenstein = 0x0830,
	eObjectText			= 0x0833,
	eObjectImage		= 0x0836,
	eObjectMultiDraw	= 0x0821,

	eObjectNone	= 0x0000,
};

enum eDirection {
	eDirectionUp	 = 0,
	eDirectionRight	 = 1,
	eDirectionDown	 = 2,
	eDirectionLeft	 = 3,
	eDirectionNone	 = 4
};

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
	eRoomObjects	 mLinkObject;
	bool			 mSelected, mLinked;

public:

	virtual void		stateChange()	{ }
	inline cRoom		*mRoomGet()	{ return mRoom; }

	virtual void		directionChange()	{ }

	inline bool			mLinkedGet()	 { return mLinked; }

	inline eRoomObjects mLinkObjectGet() { return mLinkObject; }
	
	virtual void mLinkedSet( byte pNumber )	{ return; }
	virtual void mLinked2Set( byte pNumber ) { return; }

	inline byte	 mPartCountGet()	{ return mPartCount; }
	inline byte	 mPartGet()			{ return mPart; }

	inline bool  isPlaced()			{ return mPlaced; }

	void		 mPartAdd()			{ ++mPart; }

	void		 partSet( size_t pVal ) { mPart = pVal; }

	sObjectPart	*partGet()			{ return &mParts[ mPart ]; }
	sObjectPart	*partGet( size_t pPart )			{ return &mParts[ pPart ]; }
	void partSetPosition( byte pPosX, byte pPosY ) {
		partSetPosition( pPosX, pPosY, mPart );
	}

	virtual void partPlace() {
		mSelected = false;
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

		mLinked = true;
		mLinkObject = eObjectNone;
		mObjectID = eObjectNone;

		mRoom = pRoom;
		mSelected = false;
	}

	inline bool		isSelected() { return mSelected; }
	inline void		isSelected(bool pValue) { mSelected = pValue; }

	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual void	partAdd() {
		mPlaced = false;
		++mPartCount;
		return;
	}
	
	inline cRoom *roomGet()			{ return mRoom; }

	virtual size_t	objectLoad( byte **pBuffer, size_t pPart ) {	// Load object from room stream
		if(pPart > 15)
			return 0;

		mParts[pPart].mX = *(*pBuffer)++;
		mParts[pPart].mY = *(*pBuffer)++;

		mParts[pPart].mPlaced = true;
		mPlaced = true;
		return 2;
	}

	virtual size_t	objectSave( byte **pBuffer, size_t pPart ) {	// Save object to room stream
		if(pPart > 15)
			return 0;
				
		*(*pBuffer)++ = mParts[pPart].mX;
		*(*pBuffer)++ = mParts[pPart].mY;

		return 2;
	}
};

#include "castle/objects/objectDoor.hpp"
#include "castle/objects/objectWalkway.hpp"
#include "castle/objects/objectSlidingPole.hpp"
#include "castle/objects/objectLadder.hpp"
#include "castle/objects/objectDoorBell.hpp"
#include "castle/objects/objectLightning.hpp"
#include "castle/objects/objectForcefield.hpp"
#include "castle/objects/objectMummy.hpp"
#include "castle/objects/objectKey.hpp"
#include "castle/objects/objectLock.hpp"
#include "castle/objects/objectRayGun.hpp"
#include "castle/objects/objectTeleport.hpp"
#include "castle/objects/objectTrapDoor.hpp"
#include "castle/objects/objectConveyor.hpp"
#include "castle/objects/objectFrankenstein.hpp"
#include "castle/objects/objectText.hpp"
#include "castle/objects/objectImage.hpp"
