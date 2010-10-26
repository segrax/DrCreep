class cRoom;

class cObject {
protected:
	eRoomObjects	 mObjectID;
public:

	bool			 mDrags;					// object will be dragged when being placed
	eDirection		 mDragDirection;
	size_t			 mLength;
public:
	byte			 mPositionX, mPositionY;
	cRoom			*mRoom;

	cObject( cRoom *pRoom, byte pPosX, byte pPosY ) {
		mPositionX = pPosX;
		mPositionY = pPosY;

		mLength = 0;

		mDrags = false;
		mDragDirection = eDirectionNone;
		mObjectID = eObjectsFinished;

		mRoom = pRoom;
	}
	
	eRoomObjects	objectTypeGet() { return mObjectID; }
	virtual size_t	objectLoad( byte **pBuffer ) = 0;	// Load object from room stream
	virtual size_t	objectSave( byte **pBuffer ) {	// Save object to room stream
		*(*pBuffer)++ = mPositionX;
		*(*pBuffer)++ = mPositionY;

		return 2;
	}
};
