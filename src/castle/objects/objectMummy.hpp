class cRoom;

class cObjectMummy : public cObject {
public:

public:
	cObjectMummy( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectMummy;
		mPartCount = 2;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = 0x01;

		size_t strSize = cObject::objectSave( pBuffer, 0 );
		strSize += cObject::objectSave( pBuffer, 1 );

		*(*pBuffer)++ = 0x00;
		*(*pBuffer)++ = 0x00;

		return (strSize + 3);
	}

};
