class cRoom;

class cObjectWalkway : public cObject {
public:

public:
	cObjectWalkway( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectWalkway;
		mDrags = true;
		mDragDirection = eDirectionRight;
		mLength = 1;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer ) {	
		*(*pBuffer)++ = (byte) mLength;
			
		size_t strSize = cObject::objectSave( pBuffer );

		return (strSize + 1);
	}

};
