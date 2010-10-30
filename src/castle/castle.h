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
 *  Castle
 *  ------------------------------------------
 */

class cCastleInfo;
class cCastle_Room;
class cCastle_Object_Player;
class cCastle_Object_Door;

class cCastle {
private:
	byte						*mBuffer;
	cCastleInfo					*mCastleInfo;
	cCreep						*mCreep;

	string						 mName;
	

public:
						 cCastle( cCreep *pCreep, cCastleInfo *pCastleInfo );
						~cCastle();

	byte				*bufferGet( size_t &pBufferSize ) { return mBuffer; }
	
	inline cCreep		*creepGet() { return mCreep; }
	inline string		 nameGet() { return mName; }		// Name of the castle
	inline cCastleInfo	*infoGet() { return mCastleInfo; }
	inline byte			*roomPtrGet( size_t pAddress ) {	// Get pointer to room
		return (mBuffer + (pAddress - 0x7800));
	}


};
