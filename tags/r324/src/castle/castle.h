/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2012 Robert Crossfield
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
 *  Castle
 *  ------------------------------------------
 */

class cCastleInfo;
class cRoom;
class cBuilder;

class cCastle {
private:
	byte						*mBuffer;
	cCastleInfo					*mCastleInfo;
	cCreep						*mCreep;

	string						 mName;
	
	byte						 mStart_Door_Player1, mStart_Door_Player2;
	byte						 mStart_Room_Player1, mStart_Room_Player2;
	byte						 mLives_Player1, mLives_Player2;

	byte						*mFinalScreen;
	word						 mFinalPtr;
	cRoom						*mFinalRoom;

	map< int, cRoom *>			 mRooms;


public:
						 cCastle( cCreep *pCreep, cCastleInfo *pCastleInfo );
						~cCastle();

	void				 castleLoad( cBuilder *pBuilder );
	void				 castleSave( byte *pTarget );
	map< int, cRoom *>	*roomsGet()			{ return &mRooms; }
	
	void				 roomCleanup();
	cRoom				*roomCreate( cBuilder *pBuilder, int pNumber );

	byte				*bufferGet( size_t &pBufferSize ) { return mBuffer; }
	
	inline cCreep		*creepGet() { return mCreep; }
	inline string		 nameGet() { return mName; }		// Name of the castle
	inline cCastleInfo	*infoGet() { return mCastleInfo; }
	inline byte			*roomPtrGet( size_t pAddress ) {	// Get pointer to room
		return (mBuffer + (pAddress - 0x7800));
	}

	inline byte			 mStart_Room_Player1Get() { return mStart_Room_Player1; }
	inline byte			 mStart_Room_Player2Get() { return mStart_Room_Player2; }

	inline void			 mStart_Room_Player1Set( byte pVal ) { mStart_Room_Player1 = pVal; }
	inline void			 mStart_Room_Player2Set( byte pVal ) { mStart_Room_Player2 = pVal; }
	
	inline void			 mStart_Door_Player1Set( byte pVal ) { mStart_Door_Player1 = pVal; }
	inline void			 mStart_Door_Player2Set( byte pVal ) { mStart_Door_Player2 = pVal; }

	inline cRoom		*mFinalRoomGet()					 { return mFinalRoom; }
};
