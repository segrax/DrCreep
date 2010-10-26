
class cRoom;

class cObjectTeleport : public cObject {
public:
	byte	mTarget;

public:
	cObjectTeleport( cRoom *pRoom, byte pPosX, byte pPosY ) : cObject( pRoom, pPosX, pPosY ) {
		mObjectID = eObjectTeleport;
		mPartCount = 2;
		mTarget = 1;
	}

	void	partAdd() {
		++mPartCount;
		mPlaced = false;
		return;
	}

	void partPlace() {
		cObject::partPlace();
		
		if(mPart >= mPartCount)
			partAdd();

		if( mPartCount > 0x13 ) {
			--mPartCount;
			mPlaced = true;
		}
		
	}

	void partDel() {
		--mPart;
		--mPartCount;
		mPlaced = true;
	}

	size_t		objectLoad( byte **pBuffer ) {
		
		return 0;
	}

	size_t		objectSave( byte **pBuffer , size_t pPart ) {	
		
		size_t strSize = cObject::objectSave( pBuffer, 0 );
		
		*(*pBuffer)++ = mTarget;

		for(word i = 1; i < mPartCount; ++i ) {
			strSize += cObject::objectSave( pBuffer, i );
		}
	
		//*(*pBuffer)++ = 0x00;

		return (strSize + 1);
	}

};
