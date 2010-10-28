class cObjectText;

struct sString {
	byte mPosX, mPosY;
	byte mColor;
	string mString;

	sString( byte pPosX, byte pPosY, byte pColor, string pString ) {
		mPosX = pPosX; mPosY = pPosY; mColor = pColor; mString = pString;
	}
};

class cRoom {
public:
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

					 cRoom( byte pNumber ) {
						 mNumber = pNumber;
						 mRoomDirPtr = 0;
					 }

	vector< cObject* > objectFind( eRoomObjects pType );

	size_t			 roomSaveObjects( byte **pBuffer );
	size_t			 roomSave( byte **pBuffer );

private:
	size_t			 saveCount( byte **pBuffer, eRoomObjects pObjectType );
	size_t			 saveObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker = 0x00 );
};

class cBuilder : public cCreep {

private:
	byte					 mCursorX,		mCursorY;
	eRoomObjects			 mSelectedObject;

	cRoom					*mCurrentRoom;
	cObject					*mCurrentObject;

	bool					 mDragMode;

	byte					 mStart_Door_Player1, mStart_Door_Player2;
	byte					 mStart_Room_Player1, mStart_Room_Player2;
	byte					 mLives_Player1, mLives_Player2;

	map< size_t, cRoom *>	 mRooms;
	vector< sString >		 mStrings;

private:

	void					 castleCreate();
	void					 castlePrepare();
	void					 castleSave( );

	void					 cursorObjectUpdate();


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

	void					 parseInput();
	void					 selectedObjectChange( bool pChangeUp );

	void					 objectStringsClear();
	void					 objectStringPrint( sString pString );
	void					 objectStringsPrint();

public:
							 cBuilder();
							~cBuilder();

	void					 mainLoop();

	cObject					*objectCreate( eRoomObjects pObject, byte pPosX, byte pPosY );
	cRoom					*roomCreate( size_t pNumber );

	void					 objectStringAdd( string pMessage, byte pPosX, byte pPosY, byte pColor );
	void					 messagePrint( string pMessage );
};
