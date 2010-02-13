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
 *  Castle Manager
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "d64.h"
#include "castle/castle.h"
#include "castleManager.h"

cCastleInfoD64::cCastleInfoD64( cD64 *pD64, sD64File *pFile ) : cCastleInfo( pFile->mName ) {
	mD64 = pD64;
	mFile = pFile;

	mBufferSize	= mFile->mBufferSize;
}

byte *cCastleInfoD64::bufferGet() {
	return mFile->mBuffer;
}

cCastleInfoLocal::cCastleInfoLocal( sD64FileLocal *pFileLocal ) : cCastleInfo( pFileLocal->mFilename ) {
	mLocal = pFileLocal;
	mBufferSize = mLocal->mBufferSize;

}

byte *cCastleInfoLocal::bufferGet() {
	return mLocal->mBuffer;
}

cCastleManager::cCastleManager() {
	mCastle = 0;

	castlesFind();
}

cCastleManager::~cCastleManager() {
	castlesCleanup();
	diskCleanup();
	localCleanup();

	delete mCastle;
}

void cCastleManager::castlesCleanup() {
	vector< cCastleInfo* >::iterator castleIT;

	for( castleIT = mCastles.begin(); castleIT != mCastles.end(); ++castleIT )
		delete *castleIT;
}

void cCastleManager::localCleanup() {
	vector< sD64FileLocal* >::iterator		fileIT;

	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT )
		delete (*fileIT);
}

void cCastleManager::castlesFind() {
	
	// Cleanup current open files
	castlesCleanup();
	diskCleanup();
	localCleanup();

	// Find any d64 images in data
	disksFind( "*.d64" );

	// Load castles from Disk images, and from data\castles folder
	diskLoadCastles();
	localLoadCastles();

	// Ensure atleast one disk was found
	if( mDisks.size() == 0 ) {
		cout << "No Disk Image found in data directory\n";
		exit(1);
	}

}

cCastleInfo *cCastleManager::castleInfoGet( string pName ) {
	vector< cCastleInfo* >::iterator castleIT;

	// Find a castle
	for( castleIT = mCastles.begin(); castleIT != mCastles.end(); ++castleIT )
		if( (*castleIT)->nameGet() == pName )
			return (*castleIT);

	return 0;
}

cCastleInfo *cCastleManager::castleInfoGet( size_t pNumber ) {

	if( pNumber >= mCastles.size() ) {
		return 0;
	}

	return mCastles[pNumber];
}

void cCastleManager::castleListDisplay() {
	vector< cCastleInfo* >::iterator castleIT;
	size_t number = 1;
	
	// Display a list of castles
	for(castleIT = mCastles.begin(); castleIT != mCastles.end(); ++castleIT ) {
	
		cout << number++ << ". ";
		cout << (*castleIT)->nameGet() << "\n";
	}
}

void cCastleManager::diskCleanup() {
	vector< cD64* >::iterator diskIT;

	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT )
		delete *diskIT;
}

void cCastleManager::diskLoadCastles() {
	vector< cD64* >::iterator	diskIT;
	vector< sD64File*>::iterator	fileIT;

	// Loop thro each file starting with Z of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sD64File*>	files = (*diskIT)->directoryGet( "Z*" );

		for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
			
			cCastleInfoD64 *castle = new cCastleInfoD64( (*diskIT), (*fileIT) );

			// Dont add castles with the same name as already existing castles
			if( castleInfoGet( castle->nameGet() ) ) {
				delete castle;
				continue;
			}
			
			mCastles.push_back( castle );	
		}
	}

	return;
}

byte *cCastleManager::diskLoadFile( string pFilename, size_t &pBufferSize ) {
	vector< cD64* >::iterator	diskIT;
	vector< sD64File*>::iterator	fileIT;

	// Loop thro each file starting with Z of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sD64File*>	files = (*diskIT)->directoryGet( pFilename );

		if(files.size() == 0)
			continue;

		pBufferSize = files[0]->mBufferSize;

		return files[0]->mBuffer;
	}

	return 0;
}

void cCastleManager::disksFind( string pExtension ) {
	vector<string> disks = directoryList( pExtension );
	vector<string>::iterator diskIT;

	for( diskIT = disks.begin(); diskIT != disks.end(); ++diskIT ) {
		
		mDisks.push_back( new cD64( *diskIT ) );
	}
}

void cCastleManager::localLoadCastles() {
	vector<string>			 files = directoryList( "castles\\Z*" );
	vector<string>::iterator fileIT;

	for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
		sD64FileLocal *file = fileFind( (*fileIT) );

		if(!file) {
			string final = "castles\\";
			final.append( (*fileIT) );

			size_t size = 0;
			byte *buffer = local_FileRead( final, size );
			if(buffer)
				mFiles.push_back( file = new sD64FileLocal( (*fileIT), buffer, size ));
			else
				continue;

		}
		cCastleInfoLocal *local = new cCastleInfoLocal( file );
		
		// Dont add castles with the same name as already existing castles
		if( castleInfoGet( local->nameGet() ) ) {
			delete local;
			continue;
		}

		mCastles.push_back( local );
	}
	
}

sD64FileLocal *cCastleManager::fileFind( string pFilename ) {
	vector< sD64FileLocal* >::iterator fileIT;
	
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {
		if( (*fileIT)->mFilename == pFilename )
			return (*fileIT);
	}

	return 0;
}

byte *cCastleManager::fileLoad( string pFilename, size_t &pBufferSize ) {
	byte *buffer = 0;
	
	size_t size;
	
	// Attempt to load from a D64
	buffer = diskLoadFile( pFilename, pBufferSize );

	// Load from local if that fails
	if(!pBufferSize) {
		
		sD64FileLocal *file = fileFind( pFilename );
		if(!file) {
			buffer = local_FileRead( pFilename, size );
			mFiles.push_back( file = new sD64FileLocal( pFilename, buffer, size ));
		}

		buffer = file->mBuffer;
		pBufferSize = file->mBufferSize;
	}

	return buffer;
}

cCastle *cCastleManager::castleLoad( size_t pNumber ) {

	if( pNumber >= mCastles.size() )
		return 0;

	delete mCastle;
	mCastle = new cCastle( gCreep, mCastles[ pNumber ] );

	return mCastle;
}
