
class cRoom;

class cObjectFrankenstein : public cObject {
public:
	byte	mState;

public:
	cObjectFrankenstein( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectFrankenstein;
		mState = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mState;

		size_t strSize = cObject::objectSave( pBuffer );
		
		*(*pBuffer)++ = 0; *(*pBuffer)++ = 0;
		*(*pBuffer)++ = 0; *(*pBuffer)++ = 0;

		return (strSize + 5);
	}

};
