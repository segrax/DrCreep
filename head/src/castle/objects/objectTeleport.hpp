
class cRoom;

class cObjectTeleport : public cObject {
public:
	byte	mTarget;

public:
	cObjectTeleport( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectTeleport;
		mPartCount = 2;
		mTarget = 0;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		
		size_t strSize = cObject::objectSave( pBuffer );
		
		*(*pBuffer)++ = mTarget;

		strSize += cObject::objectSave2( pBuffer );
		*(*pBuffer)++ = 0;

		return (strSize + 2);
	}

};
