class cRoom;

class cObjectLadder : public cObject {
public:

public:
	cObjectLadder( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectLadder;
		mDrags = true;
		mDragDirection = eDirectionDown;
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
