class cRoom;

class cObjectDoorBell : public cObject {
public:
	byte		mDoorID;

public:
	cObjectDoorBell( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectDoorBell;
		mDoorID = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		size_t strSize = cObject::objectSave( pBuffer );

		*(*pBuffer)++ = mDoorID;
		return (strSize + 1);
	}

};
