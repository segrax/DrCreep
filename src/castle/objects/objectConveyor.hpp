
class cRoom;

class cObjectConveyor : public cObject {
public:
	byte	mState;

public:
	cObjectConveyor( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectConveyor;
		mDrags = false;
		mLength = 1;
		mParts = true;
		mState = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mState;

		size_t strSize = cObject::objectSave( pBuffer );
		strSize += cObject::objectSave2( pBuffer );

		return (strSize + 1);
	}

};
