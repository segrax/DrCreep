class cRoom;

class cObjectKey : public cObject {
public:
	byte	mKeyColor;

public:
	cObjectKey( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectKey;
		mKeyColor = 1;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = mKeyColor;
		*(*pBuffer)++ = mKeyColor + 0x50;

		size_t strSize = cObject::objectSave( pBuffer );

		return (strSize + 2);
	}

};
