
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

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = mState;

		size_t strSize = cObject::objectSave( pBuffer, 0 );
		strSize += cObject::objectSave( pBuffer, 1 );

		return (strSize + 1);
	}

};
