class cRoom;

class cObjectMummy : public cObject {
public:

public:
	cObjectMummy( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectMummy;
		mDrags = false;
		mLength = 0;
		mParts = true;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = 0x01;

		size_t strSize = cObject::objectSave( pBuffer );
		strSize += cObject::objectSave2( pBuffer );

		*(*pBuffer)++ = 0x00;
		*(*pBuffer)++ = 0x00;

		return (strSize + 3);
	}

};
