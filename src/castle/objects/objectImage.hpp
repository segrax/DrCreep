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

class cObjectImage : public cObject {
public:
	byte		 mWidth, mHeight;
	byte		*mBitmap, *mVid, *mCol;

public:
	cObjectImage( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID	= eObjectImage;
		mWidth		= mHeight		= 0;

		mBitmap = mVid = mCol = 0;

		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {

		mWidth = *(*pBuffer)++;
		mHeight = *(*pBuffer)++;
		*(*pBuffer)++;

		mPart = 0;
		
		int count =  mWidth *mHeight;

		delete mBitmap;
		delete mVid;
		delete mCol;

		mBitmap = new byte[ count ];
		mVid = new byte[ count / 8];
		mCol = new byte[ count / 8];

		// 
		for( int x = 0; x < count; ++x ) 
			*mBitmap++ = *(*pBuffer)++;

		for( int x = 0; x < (count/8); ++x ) 
			*mVid++ = *(*pBuffer)++;

		for( int x = 0; x < (count/8); ++x ) 
			*mCol++ = *(*pBuffer)++;
	

		while( *(*pBuffer) != 0x00 ) {
			cObject::objectLoad( pBuffer,  mPart++ );
			mPartCount++;
		}
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		if(!mBitmap)
			return 0;

		*(*pBuffer)++ = mWidth;
		*(*pBuffer)++ = mHeight;
		*(*pBuffer)++;

		int count =  mWidth *mHeight;
		size_t strSize = 3;

		byte *bitmap = mBitmap;
		byte *vid = mVid;
		byte *col = mCol;

		for( int x = 0; x < count; ++x ) {
			*(*pBuffer)++ = *bitmap++;
			 ++strSize;
		}

		for( int x = 0; x < (count/8); ++x ) {
			 *(*pBuffer)++ = *vid++;
			 ++strSize;
		}

		for( int x = 0; x < (count/8); ++x ) {
			 *(*pBuffer)++ = *col++;
			 ++strSize;
		}

		for( int x = 0; x < mPartCount; ++x )
			strSize += cObject::objectSave( pBuffer, x );

		return strSize;
	}

};
