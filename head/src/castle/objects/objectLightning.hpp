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
	bool		mMachineState;
	bool		mSwitchState;
	byte		mMachine;

public:
	cObjectLightning( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLightning;
		
		mParts[0].mDragDirection = eDirectionDown;
		mParts[0].mDrags = true;
		mParts[0].mLength = 0;
		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;

		mParts[1].mCursorWidth = 3;
		mParts[1].mCursorHeight = 3;
		mPartCount = 2;

		mMachine = 0;
		mMachineState = false;
		mSwitchState = false;
	}

	size_t		objectLoadMachine( byte **pBuffer, size_t pPart ) {
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
		if( *(*pBuffer)++ == 0x80 )
			mSwitchState = true;
		else
			mSwitchState = false;

		cObject::objectLoad( pBuffer, 1 );
		*(*pBuffer)++;

		mMachine = *(*pBuffer)++;
		mMachine = *(*pBuffer)++;
		mMachine = *(*pBuffer)++;
		mMachine = *(*pBuffer)++;
	}

	size_t		objectSaveMachine( byte **pBuffer, size_t pPart ) {
		if( mMachineState == true )
			*(*pBuffer)++ = 0x40;
		else
			*(*pBuffer)++ = 0x00;

		size_t size = cObject::objectSave( pBuffer,0 );

		*(*pBuffer)++ = mParts[0].mLength;

		(*pBuffer) += 4;

		return size + 6;
	}

	size_t		objectSaveSwitch( byte **pBuffer , size_t pPart ) {	
		if( mSwitchState == true )
			*(*pBuffer)++ = 0x80;
		else
			*(*pBuffer)++ = 0xC0;

		size_t strSize = cObject::objectSave( pBuffer, 1 );
		*(*pBuffer)++ = 0x00;

		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;
		*(*pBuffer)++ = mMachine;

		return (strSize + 6);
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	

		return 0;
	}

};