
enum eRoomObjects {
	eObjectDoor			= 0x0803,
	eObjectWalkway		= 0x0806,
	eObjectSlidingPole	= 0x0809,
	eObjectLadder		= 0x080C,
	eObjectDoorBell		= 0x080F,
	eObjectLightning	= 0x0812,
	eObjectForcefield	= 0x0815,
	eObjectMummy		= 0x0818,
	eObjectKey			= 0x081B,
	eObjectLock			= 0x081E,
	eObjectRayGun		= 0x0824,
	eObjectTeleport		= 0x0827,
	eObjectTrapDoor		= 0x082A,
	eObjectConveyor		= 0x082D,
	eObjectFrankenstein = 0x0830,
	eObjectStringPrint	= 0x0833,
	eObjectImageDraw	= 0x0821,

	eObjectsFinished	= 0x0000,
};

struct sRoomObject {
	eRoomObjects	mObjectID;
	byte			mPosX, mPosY;
};

struct sRoom {
	byte			mNumber;
	byte			mColor;
	byte			mMapX, mMapY;
	byte			mMapWidth, mMapHeight;

	vector< sRoomObject* >	mObjects;
};

class cBuilder : public cCreep {

private:
	word			 mCursorX,		mCursorY;
	sRoom			*mCurrentRoom;
	sRoomObject		*mCurrentObject;

	map< size_t, sRoom *>	mRooms;

public:
					 cBuilder();
					~cBuilder();

	void			 mainLoop();

	sRoom			*roomCreate( size_t pNumber );
};
