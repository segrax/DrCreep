/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2016 Robert Crossfield
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
 *  Castle Builder
 *  ------------------------------------------
 */

class cObjectText;
class cObject;
class cRoom;

struct sString {
	byte mPosX, mPosY;
	byte mColor;
	string mString;

	sString( byte pPosX, byte pPosY, byte pColor, string pString ) {
		mPosX = pPosX; mPosY = pPosY; mColor = pColor; mString = pString;
	}
};

class cBuilder : public cCreep {

private:
	cCreep					*mCreepParent;
	byte					 mCursorX,				mCursorY;
	eRoomObjects			 mSelectedObject,		mSearchObject;
	size_t					 mRoomSelectedObject;

	cRoom					*mCurrentRoom,	 *mOriginalRoom;
	cObject					*mCurrentObject, *mOriginalObject;

	bool					 mDragMode, mLinkMode;

	vector< sString >		 mStrings;
	bool					 mTest;

private:

	void					 castlePrepare( );

	void					 cursorObjectUpdate();
	void					 cursorUpdate();
	size_t					 findItemIndex( cObject *pObject );

	void					 castleSaveToDisk();

	void					 parseInput();
	void					 selectedObjectChange( bool pChangeUp );
	void					 selectedObjectDelete();
	void					 selectedObjectLink();
	void					 selectPlacedObject( bool pChangeUp );

	void					 save( bool pRemoveCursor );

	void					 roomChange( int pRoomNumber );

	void					 objectStringsClear();
	void					 objectStringPrint( sString pString );
	void					 objectStringsPrint();

	void					 loadCount( byte **pBuffer, eRoomObjects pObjectType );

	void					 mapRoomsDraw( size_t pArrowRoom );
	void					 mapBuilder();
	void					 playerDraw();

public:
							 cBuilder( cCreep *pParent );
							~cBuilder();

	void					 mainLoop();

	void					 objectStringAdd( string pMessage, byte pPosX, byte pPosY, byte pColor );
	void					 messagePrint( string pMessage );

	inline bool				 mTestGet() { return mTest; }

	inline void				 mSoundSet( cSound *pSound ) { mSound = pSound; }
};
