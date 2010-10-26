class cRoom;

class cObjectDoor : public cObject {
public:
	byte		mDirection;
	byte		mLinkRoom, mLinkDoor;
	byte		mMapX, mMapY;
	byte		mCastleExitDoor;

public:
	cObjectDoor( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectDoor;

		mDirection = eDirectionTop;
		mLinkRoom = 0;
		mLinkDoor = 0;
		mMapX = mMapY = 0;
		mCastleExitDoor = 0;

	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		size_t strSize = cObject::objectSave( pBuffer );

		*(*pBuffer)++ = (byte) mDirection;
		*(*pBuffer)++ = mLinkRoom;
		*(*pBuffer)++ = mLinkDoor;
		*(*pBuffer)++ = mMapX;
		*(*pBuffer)++ = mMapY;
		*(*pBuffer)++ = mCastleExitDoor;

		return (strSize + 6);
	}

};
