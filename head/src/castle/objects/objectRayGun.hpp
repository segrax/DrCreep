

class cRoom;

class cObjectRayGun : public cObject {
public:
	byte	mDirection, mRayPosY;

public:
	cObjectRayGun( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectRayGun;
		mDrags = true;
		mDragDirection = eDirectionDown;
		mLength = 1;
		mParts = true;
		mDirection = 0;
		mRayPosY = 10;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mDirection;
		size_t strSize = cObject::objectSave( pBuffer );
		
		*(*pBuffer)++ = mLength;
		*(*pBuffer)++ = mRayPosY;

		strSize += cObject::objectSave2( pBuffer );

		return (strSize + 3);
	}

};
