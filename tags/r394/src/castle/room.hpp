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
 *  Castle Room
 *  ------------------------------------------
 */

class cBuilder;
class cObject;
class cCastle;

class cRoom {
public:
	cCastle			*mCastle;
	cBuilder		*mBuilder;
	byte			 mNumber;
	byte			 mColor;
	byte		 	 mMapX, mMapY;
	byte			 mMapWidth, mMapHeight;
	byte			*mRoomDirPtr;

	vector< cObject* >	mObjects;
	
	void			 objectAdd( cObject *pObject ) { 
		if(pObject) 
			if( find( mObjects.begin(), mObjects.end(), pObject ) == mObjects.end() )
				mObjects.push_back( pObject ); 
	}

	void			 objectDelete( cObject *pObject ) {
		vector<cObject* >::iterator objIT = find( mObjects.begin(), mObjects.end(), pObject );
		if( objIT == mObjects.end() )
			return;

		mObjects.erase( objIT );
	}

	cObject			*objectGet( size_t pNumber ) {
		if( pNumber >= mObjects.size() )
			return 0;

		return mObjects[pNumber];
	}

					 cRoom( cBuilder *pBuilder, cCastle *pCastle, byte pNumber ) {
						 mCastle = pCastle;
						 mNumber = pNumber;
						 mRoomDirPtr = 0;
						 mBuilder = pBuilder;

						 mColor = 3;
						 mMapHeight = 1;
						 mMapWidth = 1;
						 mMapX = 0x10;
						 mMapY = 0;
					 }


	cObject				*objectCreate( cRoom *pRoom, eRoomObjects pObject, byte pPosX, byte pPosY );
	vector< cObject* >	 objectFind( eRoomObjects pType );

	void			 roomLoad( byte **pBuffer );
	void			 roomLoadObjects( byte **pBuffer );
	size_t			 roomSaveObjects( byte **pBuffer );
	size_t			 roomSave( byte **pBuffer );
	
private:
	size_t			 saveCount( byte **pBuffer, eRoomObjects pObjectType );
	size_t			 saveObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker = 0x00 );
	size_t			 saveObjects( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker = 0x00 );
	size_t			 saveObjectLightning( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker);

	void			 loadCount( byte **pBuffer, eRoomObjects pObjectType );
	void			 loadObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker);
	void			 loadObjectLightning( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker);

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
};
