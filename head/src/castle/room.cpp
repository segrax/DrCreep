/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2010 Robert Crossfield
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  ------------------------------------------
 *  Castle Room
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "objects/object.hpp"
#include "room.hpp"
#include "creep.h"
#include "builder.hpp"

size_t cRoom::roomSaveObjects( byte **pBuffer ) {
	size_t size = 0;

	size = saveCount( pBuffer, eObjectDoor );
	size += saveObject( pBuffer, eObjectWalkway, 0x00 );
	size += saveObject( pBuffer, eObjectSlidingPole, 0x00 );
	size += saveObject( pBuffer, eObjectLadder, 0x00 );
	size += saveCount( pBuffer, eObjectDoorBell );
	size += saveObjectLightning( pBuffer, eObjectLightning, 0x20 );
	size += saveObject( pBuffer, eObjectForcefield, 0x00 );
	size += saveObject( pBuffer, eObjectMummy, 0x00 );
	size += saveObject( pBuffer, eObjectKey, 0x00 );
	size += saveObject( pBuffer, eObjectLock, 0x00 );
	size += saveObject( pBuffer, eObjectRayGun, 0x80 );
	size += saveObject( pBuffer, eObjectTeleport, 0x00 );
	size += saveObject( pBuffer, eObjectTrapDoor, 0x80 );
	size += saveObject( pBuffer, eObjectConveyor, 0x80 );
	size += saveObject( pBuffer, eObjectFrankenstein, 0x80 );
	size += saveObject( pBuffer, eObjectText, 0x00 );

	size += saveObjects( pBuffer, eObjectImage, 0x00 );

	return size;
}

void cRoom::roomLoadObjects( byte **pBuffer ) {
	word func = 0;

	while( (func = readLEWord( *pBuffer )) != 0 ) {
		*pBuffer += 2;

		switch( func ) {
			case eObjectDoor:
			case eObjectDoorBell:
				loadCount( pBuffer, (eRoomObjects) func );
				break;

			case eObjectWalkway:
			case eObjectSlidingPole:
			case eObjectLadder:
			case eObjectForcefield:
			case eObjectMummy:
			case eObjectKey:
			case eObjectLock:
			case eObjectTeleport:
			case eObjectText:
			case eObjectImage:
				loadObject( pBuffer, (eRoomObjects) func, 0x00 );
				break;

			case eObjectLightning:
				loadObjectLightning( pBuffer, (eRoomObjects) func, 0x20 );
				break;

			case eObjectRayGun:
			case eObjectTrapDoor:
			case eObjectConveyor:
			case eObjectFrankenstein:
				loadObject( pBuffer, (eRoomObjects) func, 0x80 );
				break;

			default:
				cout << "0x";
				cout << hex << (int) func << endl;
				break;
		}
	}

	return;
}

size_t cRoom::roomSave( byte **pBuffer ) {
	byte sizeFinal;
	
	mRoomDirPtr = *pBuffer;

	*(*pBuffer)++ = mColor;
	*(*pBuffer)++ = mMapX;
	*(*pBuffer)++ = mMapY;

	sizeFinal = mMapWidth;
	sizeFinal |= (mMapHeight << 3);

	*(*pBuffer)++ = sizeFinal;

	return 8;
}

void cRoom::roomLoad( byte **pBuffer ) {
	mColor = *(*pBuffer)++;
	mMapX  = *(*pBuffer)++;
	mMapY  = *(*pBuffer)++;
	size_t sizeFinal = *(*pBuffer)++;

	mMapWidth = (sizeFinal & 7);
	mMapHeight = (sizeFinal >> 3) & 7;
}

vector< cObject* > cRoom::objectFind( eRoomObjects pType ) {
	vector< cObject* >			objects;
	vector< cObject* >::iterator	objectIT;

	for( objectIT = mObjects.begin(); objectIT != mObjects.end(); ++objectIT ) {

		if( (*objectIT)->objectTypeGet() == pType )
			objects.push_back( *objectIT );
	}	

	return objects;
}

void cRoom::loadCount( byte **pBuffer, eRoomObjects pObjectType ) {

	// Number of objects
	byte count = *(*pBuffer)++;

	// Read each object
	for( int x = 0; x < count; ++x ) {
		cObject *obj = mBuilder->objectCreate( this, pObjectType, 0, 0 );
		obj->objectLoad(pBuffer, 0);
	}

	return;
}

void cRoom::loadObjectLightning( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker) {

	while(*(*pBuffer) != pEndMarker ) { 
		cObjectLightning *obj = (cObjectLightning*) mBuilder->objectCreate( this, pObjectType, 0, 0 );
		
		if( (*(*pBuffer) & 0x80) ) 
			obj->objectLoadSwitch(pBuffer, 0 );
		else
			obj->objectLoadMachine(pBuffer, 0);
	};


	(*pBuffer)++;

	return;
}

size_t cRoom::saveObjectLightning( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;
	size_t size = 0;

	if(objects.size() == 0)
		return 0;

	writeLEWord( *pBuffer, (word) pObjectType );
	*pBuffer += 2;

	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) {
		cObjectLightning *obj = (cObjectLightning*) *objectIT;

		size += obj->objectSave(pBuffer, 0);
	}

	*(*pBuffer)++ = pEndMarker;

	return size + 3;
}

void cRoom::loadObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker ) {

	// Load each object until end marker
	while(*(*pBuffer) != pEndMarker ) { 
		cObject *obj = mBuilder->objectCreate( this, pObjectType, 0, 0 );
		obj->objectLoad(pBuffer, 0);
	}

	(*pBuffer)++;

	return;
}

size_t cRoom::saveCount( byte **pBuffer, eRoomObjects pObjectType ) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;
	size_t	size = 3;

	// No objects to save?
	if( objects.size() == 0 )
		return 0;

	// Write ID
	writeLEWord( *pBuffer, (word) pObjectType );
	*pBuffer += 2;

	// Number of objects
	*(*pBuffer)++ = objects.size();

	// Write each door
	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) 
		size += (*objectIT)->objectSave( pBuffer, 0 );

	return size;
}

size_t cRoom::saveObject( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker ) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;

	size_t	size = 3;

	// No poles to save?
	if( objects.size() == 0 )
		return 0;

	// Write ObjectID
	writeLEWord( *pBuffer, (word) pObjectType );
	*pBuffer += 2;

	// Write each
	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) 
		size += (*objectIT)->objectSave( pBuffer, 0 );

	if(size == 3 ) {
		*pBuffer -= 2;
		writeLEWord( *pBuffer, 0 );
		return 0;
	}

	*(*pBuffer)++ = pEndMarker;

	return size;
}

size_t cRoom::saveObjects( byte **pBuffer, eRoomObjects pObjectType, byte pEndMarker ) {
	vector< cObject* >	objects = objectFind( pObjectType );
	vector< cObject* >::iterator	objectIT;

	size_t	size = 0;

	// No objects to save?
	if( objects.size() == 0 )
		return 0;

	// Write each
	for( objectIT = objects.begin(); objectIT != objects.end(); ++objectIT ) {
		// Write ObjectID
		writeLEWord( *pBuffer, (word) pObjectType );
		*pBuffer += 2;

		size += 3;
		size += (*objectIT)->objectSave( pBuffer, 0 );

		*(*pBuffer)++ = pEndMarker;
	}

	return size;
}