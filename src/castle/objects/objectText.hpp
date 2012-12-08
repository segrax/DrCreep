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

class cObjectText : public cObject {
public:
	byte	mTextColor,	mStyle;
	string	mString;

public:
	cObjectText( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectText;
		mTextColor = 2;
		mStyle = 0x22;		// single Height
		mString = "TEXT";
	}
	
	void partPlace() {
		textSet();
		cObject::partPlace();
	}

	size_t		objectLoad( byte **pBuffer, size_t pPart ) {
		cObject::objectLoad( pBuffer, 0 );

		mTextColor = *(*pBuffer)++;
		mStyle = *(*pBuffer)++;

		mString.clear();

		while( !(*(*pBuffer) & 0x80) ) {
			mString.push_back( (*(*pBuffer) & 0x7F) );
			(*pBuffer)++;
		}

		mString.push_back( (*(*pBuffer) & 0x7F) );
		(*pBuffer)++;

		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		
		*(*pBuffer)++ = mTextColor;
		*(*pBuffer)++ = mStyle;
		
		for( string::iterator stringIT = mString.begin(); stringIT != mString.end(); ++stringIT, ++strSize )
			*(*pBuffer)++ = toupper( (*stringIT));

		// Set bit7 of last character
		(*pBuffer)--;
		*(*pBuffer) |= 0x80;
		(*pBuffer)++;
		
		++strSize;

		return (strSize + 2);
	}

	void		textSet();

	void		stateChange();
	void		directionChange() {
		if(mStyle == 0x22 )
			mStyle = 0x21;
		else
			mStyle = 0x22;
	}

};
