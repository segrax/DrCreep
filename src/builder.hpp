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

	byte					 mStart_Door_Player1, mStart_Door_Player2;
	byte					 mStart_Room_Player1, mStart_Room_Player2;
	byte					 mLives_Player1, mLives_Player2;
	byte					*mFinalScreen;
	word					 mFinalPtr;
	cRoom					*mFinalRoom;

	map< int, cRoom *>		 mRooms;
	vector< sString >		 mStrings;
	bool					 mTest;

private:

	void					 castleCreate();
	void					 castlePrepare( );
	void					 castleLoad( );
	void					 castleSave( bool pRemoveCursor );

	void					 cursorObjectUpdate();
	void					 cursorUpdate();
	size_t					 findItemIndex( cObject *pObject );

	cObject					*obj_Door_Create( byte pPosX, byte pPosY );
	cObject					*obj_Walkway_Create( byte pPosX, byte pPosY );
	cObject					*obj_SlidingPole_Create( byte pPosX, byte pPosY );
	cObject					*obj_Ladder_Create( byte pPosX, byte pPosY );
	cObject					*obj_Door_Button_Create( byte pPosX, byte pPosY );
	cObject					*obj_Lightning_Create( byte pPosX, byte pPosY );
	cObject					*obj_Forcefield_Create( byte pPosX, byte pPosY );
	cObject					*obj_Mummy_Create( byte pPosX, byte pPosY );
	cObject					*obj_Key_Create( byte pPosX, byte pPosY );
	cObject					*obj_Door_Lock_Create( byte pPosX, byte pPosY );
	cObject					*obj_RayGun_Create( byte pPosX, byte pPosY );
	cObject					*obj_Teleport_Create( byte pPosX, byte pPosY );
	cObject					*obj_TrapDoor_Create( byte pPosX, byte pPosY );
	cObject					*obj_Conveyor_Create( byte pPosX, byte pPosY );
	cObject					*obj_Frankie_Create( byte pPosX, byte pPosY );
	cObject					*obj_string_Create( byte pPosX, byte pPosY );
	cObject					*obj_Image_Create( byte pPosX, byte pPosY );
	cObject					*obj_Multi_Create( byte pPosX, byte pPosY );

	void					 castleSaveToDisk();

	void					 parseInput();
	void					 selectedObjectChange( bool pChangeUp );
	void					 selectedObjectDelete();
	void					 selectedObjectLink();
	void					 selectPlacedObject( bool pChangeUp );

	void					 roomCleanup();
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

	cObject					*objectCreate( cRoom *pRoom, eRoomObjects pObject, byte pPosX, byte pPosY );
	cRoom					*roomCreate( int pNumber );

	void					 objectStringAdd( string pMessage, byte pPosX, byte pPosY, byte pColor );
	void					 messagePrint( string pMessage );

	inline bool				 mTestGet() { return mTest; }
};
