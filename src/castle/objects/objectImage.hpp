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

class cObjectImage : public cObject {
public:
	byte		 mWidth, mHeight;
	byte		*mBitmap;
	word		 mDataSize;

public:
	cObjectImage( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID	= eObjectImage;
		mWidth		= mHeight		= 0;

		mBitmap = 0;
		mDataSize = 0;

		mParts[0].mCursorWidth = 3;
		mParts[0].mCursorHeight = 3;
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {

		mWidth = *(*pBuffer)++;
		mHeight = *(*pBuffer)++;
		*(*pBuffer)++;

		mPart = 0;

		word word_30 = 0;
		
		size_t X = mWidth;
		size_t height = (mHeight - 1) >> 3;
		++height;

		//1B18
		for(;;) {
			if( X == 0 )
				break;

			word_30 += height;
			--X;
		}
		// 1B2D
		word_30 <<= 1;
		X = mHeight;

		for(;;) {
			if(X == 0 )
				break;

			word_30 += mWidth;
			--X;
		}
		// 1B4D
		mDataSize = word_30;
		delete mBitmap;
		
		mBitmap = new byte[ mDataSize ];

		for( X = 0; X < mDataSize; ++X )
			mBitmap[X] = *(*pBuffer)++;


		while( *(*pBuffer) != 0x00 ) {
			cObject::objectLoad( pBuffer,  mPart++ );
			++mPartCount;
		}
		
		--mPartCount;

		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		if(!mBitmap)
			return 0;

		*(*pBuffer)++ = mWidth;
		*(*pBuffer)++ = mHeight;
		*(*pBuffer)++ = 0;

		size_t strSize = 3;

		for( int x = 0; x < mDataSize; ++x ) {
			*(*pBuffer)++ = mBitmap[x];
			 ++strSize;
		}

		for( int x = 0; x < mPartCount; ++x )
			strSize += cObject::objectSave( pBuffer, x );

		return strSize;
	}

};
