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
 *  Castle Room
 *  ------------------------------------------
 */

class cCastle;
class cCastle_Image;
class cCastle_Object;
class cCastle_Object_Door;

class cCastle_Room {
private:
	byte			 byte_5CE6[ 0x20 ];
	byte			 byte_5D06[ 0x20 ];

	byte			 mObjectMap[0x800];	// FIXME: Check real required size
	cCastle			*mCastle;

	byte			*mRoomDirPtr;						// pointer in the castle room directory
	byte			*mRoomPtr;							// pointer to start of room data

	byte			*mDataDoorPtr, *mDataLightingPtr;

	vector< cCastle_Object* >	 mObjects;				// objects in room

	byte						 mColor;
	byte						 mMapTopLeftX, mMapTopLeftY;
	byte						 mMapWidthHeight;
	byte						 mMapWidth, mMapHeight;

	void						 obj_Door_Load( byte *&pObjectBuffer );
	void						 obj_Walkway_Load( byte *&pObjectBuffer );
	void						 obj_SlidingPole_Load( byte *&pObjectBuffer );
	void						 obj_Ladder_Load( byte *&pObjectBuffer );
	void						 obj_Door_Button_Load( byte *&pObjectBuffer );
	void						 obj_Lightning_Load( byte *&pObjectBuffer );

	void						 obj_Load();									// Load all objects in room

public:

							 cCastle_Room( cCastle *pCastle, byte *pRoomDirPtr );
	
	inline byte				 colorGet()	 { return mColor; }
	inline cCastle			*castleGet() { return mCastle; }

	cCastle_Object_Door		*objectDoorGet( word pCount );
	void					 draw();

	byte					*objectMapGet( byte byte_5FD5, byte byte_5FD6 );
	void					 objectMapPtrPrepare();
	
	void					 objectAdd( cCastle_Object *pObject );
	bool					 objectRemove( cCastle_Object *pObject, bool pDelete );

	vector< cCastle_Object* > *objectsGet() { return &mObjects; }

	void			  		 screenDraw(  word pDecodeMode, word pGfxID, byte pGfxPosX, byte pGfxPosY, byte pTxtX_0 = 0, byte pTxtY_0 = 0, byte pTxtCurrentID = 0);

};
