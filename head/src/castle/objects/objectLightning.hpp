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

class cObjectLightning : public cObject {
public:
	bool		mMachineMode;
	bool		mMachineState;
	bool		mSwitchState;
	byte		mMachine[4];

public:
	cObjectLightning( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLightning;
		
		mLinkObject = eObjectLightning;
		mLinked = true;

		mMachineMode = false;
		
		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;

		mMachine[0] = 0xFF;
		mMachine[1] = 0xFF;
		mMachine[2] = 0xFF;
		mMachine[3] = 0xFF;
		
		mMachineState = false;
		mSwitchState = false;
	}

	size_t		objectLoadMachine( byte **pBuffer, size_t pPart ) {
		mMachineMode = true;

		if( *(*pBuffer)++ == 0x40 )
			mMachineState = true;
		else
			mMachineState = false;

		cObject::objectLoad( pBuffer, 0 );

		mParts[0].mLength = *(*pBuffer)++;

		(*pBuffer) += 4;

		return 0;
	}

	void		objectLoadSwitch( byte **pBuffer, size_t pPart ) {
		mMachineMode = false;

		if( *(*pBuffer)++ == 0x80 )
			mSwitchState = true;
		else
			mSwitchState = false;

		cObject::objectLoad( pBuffer, 0 );
		*(*pBuffer)++;

		mMachine[0] = *(*pBuffer)++;
		mMachine[1] = *(*pBuffer)++;
		mMachine[2] = *(*pBuffer)++;
		mMachine[3] = *(*pBuffer)++;
	}

	size_t		objectSaveMachine( byte **pBuffer, size_t pPart ) {
		if( mMachineState == true )
			*(*pBuffer)++ = 0x40;
		else
			*(*pBuffer)++ = 0x00;

		size_t size = cObject::objectSave( pBuffer,0 );

		*(*pBuffer)++ = mParts[0].mLength;

		*(*pBuffer)++ = 0;		*(*pBuffer)++ = 0;
		*(*pBuffer)++ = 0;		*(*pBuffer)++ = 0;

		return size + 6;
	}

	size_t		objectSaveSwitch( byte **pBuffer , size_t pPart ) {	
		if( mSwitchState == true )
			*(*pBuffer)++ = 0x80;
		else
			*(*pBuffer)++ = 0xC0;

		size_t strSize = cObject::objectSave( pBuffer, 0 );
		*(*pBuffer)++ = 0x00;

		*(*pBuffer)++ = mMachine[0];
		*(*pBuffer)++ = mMachine[1];
		*(*pBuffer)++ = mMachine[2];
		*(*pBuffer)++ = mMachine[3];

		return (strSize + 6);
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		size_t size = 0;

		if( mMachineMode )
			size = objectSaveMachine( pBuffer, 0 );
		else
			size = objectSaveSwitch( pBuffer, 0 );
		
		return size;
	}

	void		mLinkedSet( byte pNumber ) {
		size_t x = 0;

		while( mMachine[x] != 0xFF && x < 4)
			++x;

		if( x > 3 )
			x = 0;

		mMachine[x] = (pNumber * 0x08);
		
	}

	void		stateChange() {
		if( mMachineMode ) {

			if(mMachineState) {
				mMachineMode = false; 
				mMachineState = false;

				mParts[0].mDrags = false;
				mParts[0].mLength = 1;
			} else
				mMachineState = true;

		} else {

			if(mSwitchState) {
				mParts[0].mDragDirection = eDirectionDown;
				mParts[0].mDrags = true;
				mParts[0].mLength = 1;

				mMachineMode = true;
				mSwitchState = false;
			} else
				mSwitchState = true;
		}
	}

};
