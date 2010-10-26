class cRoom;

class cObjectLock : public cObject {
public:
	byte	mLockColor, mDoorNumber;

public:
	cObjectLock( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLock;
		mDrags = false;
		mLength = 0;
		mParts = true;
		mLockColor = 1;
		mDoorNumber = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mLockColor;
		*(*pBuffer)++ = mLockColor + 0x57;
		*(*pBuffer)++ = mDoorNumber;

		size_t strSize = cObject::objectSave( pBuffer );

		return (strSize + 2);
	}

};
