

class cRoom;

class cObjectRayGun : public cObject {
private:
	byte	mDirection;

protected:
	
	void partSetPosition( byte pPosX, byte pPosY, byte pPart ) {

		cObject::partSetPosition( pPosX, pPosY, pPart );
		// Set raygun position
		if( pPart == 0 )
			mParts[2].mY = pPosY;

	}
public:
	cObjectRayGun( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectRayGun;

		mParts[0].mDrags = true;
		mParts[0].mDragDirection = eDirectionDown;
		mParts[0].mLength = 1;

		mPartCount = 3;
		mDirection = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mDirection;
		size_t strSize = cObject::objectSave( pBuffer );
		
		*(*pBuffer)++ = mParts[0].mLength;
		*(*pBuffer)++ = mParts[2].mY;

		strSize += cObject::objectSave2( pBuffer );

		return (strSize + 3);
	}

};
