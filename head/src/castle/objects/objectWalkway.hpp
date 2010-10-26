class cRoom;

class cObjectWalkway : public cObject {
public:

public:
	cObjectWalkway( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectWalkway;

		mParts[0].mDrags = true;
		mParts[0].mDragDirection = eDirectionRight;

		mParts[0].mLength = 1;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = (byte) mParts[0].mLength;
			
		size_t strSize = cObject::objectSave( pBuffer );

		return (strSize + 1);
	}

};
