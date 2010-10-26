class cRoom;

class cObjectSlidingPole : public cObject {
public:

public:
	cObjectSlidingPole( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectSlidingPole;
		mParts[0].mDrags = true;
		mParts[0].mDragDirection = eDirectionDown;
		mParts[0].mLength = 1;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		*(*pBuffer)++ = (byte) mParts[0].mLength;
			
		size_t strSize = cObject::objectSave( pBuffer, pPart );

		return (strSize + 1);
	}

};
