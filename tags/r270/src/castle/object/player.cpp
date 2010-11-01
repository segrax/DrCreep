#include "stdafx.h"
#include "castle/object/object.h"
#include "castle/room.h"
#include "player.h"

cCastle_Object_Player::cCastle_Object_Player( cCastle_Room *pRoom, cCastle_Object_Door *pDoor ) : cCastle_Object( pRoom ) {
	
}

void cCastle_Object_Player::graphicUpdate() {

}

void cCastle_Object_Player::roomChange( cCastle_Room *pRoom ) {
	
	mRoom->objectRemove( this, false );
	mRoom = pRoom;
	mRoom->objectAdd( this );
}
