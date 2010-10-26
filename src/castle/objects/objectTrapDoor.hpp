
class cRoom;

class cObjectTrapDoor : public cObject {
public:
	byte	mState;

public:
	cObjectTrapDoor( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectTrapDoor;
		mPartCount = 2;
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
