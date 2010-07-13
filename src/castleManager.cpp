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

#ifdef _WII
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
#endif

cCastleInfoD64::cCastleInfoD64( cCastleManager *pCastleManager, cD64 *pD64, sD64File *pFile ) : cCastleInfo( pCastleManager, pFile->mName ) {
	mD64 = pD64;
	mFile = pFile;

	mBufferSize	= mFile->mBufferSize;
}

byte *cCastleInfoD64::bufferGet() {
	return mFile->mBuffer;
}

cCastleInfoLocal::cCastleInfoLocal( cCastleManager *pCastleManager, sFileLocal *pFileLocal ) : cCastleInfo( pCastleManager, pFileLocal->mFilename ) {
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
	diskPosCleanup();
	localCleanup();

	delete mCastle;
}

void cCastleManager::castlesCleanup() {
	vector< cCastleInfo* >::iterator castleIT;

	for( castleIT = mCastles.begin(); castleIT != mCastles.end(); ++castleIT )
		delete *castleIT;
}

void cCastleManager::localCleanup() {
	vector< sFileLocal* >::iterator		fileIT;

	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT )
		delete (*fileIT);
}

void cCastleManager::castlesFind() {
	
	// Cleanup current open files
	castlesCleanup();
	diskCleanup();
	localCleanup();

	// Find any d64 images in data
	disksFind( ".d64" );

	// Load castles from Disk images, and from data\castles folder
	diskLoadCastles();
	localLoadCastles();

	// Ensure atleast one disk was found
	if( mDisks.size() == 0 ) {
		cout << "No Commodore 64 Disk Images found in data directory\nPress Enter to exit";
		string a;
		getline( cin, a );

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

	mDisks.clear();
}

void cCastleManager::diskPosCleanup() {
	vector< cD64* >::iterator diskIT;

	for( diskIT = mDisksPositions.begin(); diskIT != mDisksPositions.end(); ++diskIT )
		delete *diskIT;

	mDisksPositions.clear();
}

void cCastleManager::diskLoadCastles() {
	vector< cD64* >::iterator	diskIT;
	vector< sD64File*>::iterator	fileIT;

	// Loop thro each file starting with Z of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sD64File*>	files = (*diskIT)->directoryGet( "Z*" );

		for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
			
			cCastleInfoD64 *castle = new cCastleInfoD64( this, (*diskIT), (*fileIT) );

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
	vector<string> disks = directoryList( "", pExtension, false );
	vector<string>::iterator diskIT;

	for( diskIT = disks.begin(); diskIT != disks.end(); ++diskIT ) {
		mDisks.push_back( new cD64( *diskIT ) );
	}
}

void cCastleManager::diskPosFind( string pExtension ) {
	vector<string> disks = directoryList( "", pExtension, true );
	vector<string>::iterator diskIT;
	
	diskPosCleanup();

	for( diskIT = disks.begin(); diskIT != disks.end(); ++diskIT ) {
		
		mDisksPositions.push_back( new cD64( *diskIT, false, true, false ) );
	}
}

void cCastleManager::localLoadCastles() {
	vector<string>			 files = directoryList( "castles", "Z", false);
	vector<string>::iterator fileIT;

	for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
		sFileLocal *file = fileFind( (*fileIT) );

		if(!file) {
			string final = "castles\\";
			final.append( (*fileIT) );

			size_t size = 0;
			byte *buffer = local_FileRead( final, size, false );
			if(buffer)
				mFiles.push_back( file = new sFileLocal( (*fileIT), buffer, size ));
			else
				continue;

		}
		cCastleInfoLocal *local = new cCastleInfoLocal( this, file );
		
		// Dont add castles with the same name as already existing castles
		if( castleInfoGet( local->nameGet() ) ) {
			delete local;
			continue;
		}

		mCastles.push_back( local );
	}
	
}

sFileLocal *cCastleManager::fileFind( string pFilename ) {
	vector< sFileLocal* >::iterator fileIT;
	
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {
		if( (*fileIT)->mFilename == pFilename )
			return (*fileIT);
	}

	return 0;
}

vector<string>	cCastleManager::filesFind( string pFilemask ) {
	vector< cD64* >::iterator		diskIT;
	vector< sD64File*>::iterator	fileIT;
	vector< string >				musicFiles;

	// Loop thro each file starting with MUSIC of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sD64File*>	files = (*diskIT)->directoryGet( pFilemask );

		for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
			
			// If we dont have this music file, add it to the list of available music
			if( find( musicFiles.begin(), musicFiles.end(), (*fileIT)->mName ) == musicFiles.end() )
				musicFiles.push_back( (*fileIT)->mName );

		}
	}

	return musicFiles;
}

cD64 *cCastleManager::positionDiskCreate() {
	cD64		 *newDisk	= 0;
	stringstream  filename;

	// FIXME: Temp for now, but who needs 100 save disks?
	for(size_t count = 0; count < 100; ++count) {
		filename.str("");
		filename << "SAVES_";
		filename << count;
		filename << ".D64";

		newDisk = new cD64( filename.str().c_str(), true, true, false );
		
		if( newDisk->createdGet() == true )
			break;

		delete newDisk;
		newDisk = 0;
	}

	return newDisk;
}

bool cCastleManager::positionLoad( string pFilename, byte *pTarget ) {
	diskPosFind(".d64");
	// Search all open disks  for filename

	vector< cD64* >::iterator		 diskIT;
	sD64File *file = 0;

	for( diskIT = mDisksPositions.begin(); diskIT != mDisksPositions.end(); ++diskIT ) {
		if( (file = (*diskIT)->fileGet( pFilename )) ) {
			
			// Copy the file from the d64 buffer, to the target memory buffer
			memcpy( pTarget, file->mBuffer + 2, file->mBufferSize - 2);
			return true;
		}
	}

	return false;
}

bool cCastleManager::positionSave( string pFilename, size_t pSaveSize, byte *pData ) {
	vector< cD64* >::iterator		 diskIT;
	cD64							*disk = 0;

	diskPosFind(".d64");
	
	// Calculate number of sectors required for file we're saving
	size_t size = pSaveSize + 2;
	size_t sectors = (size / 254);
	if(size % 254)
		++sectors;

	// Search all available disks for enough available sectors 
	for( diskIT = mDisksPositions.begin(); diskIT != mDisksPositions.end(); ++diskIT ) {
		
		if( (*diskIT)->sectorsFree() >= sectors)
			break;
	}

	// No Disk found? create a new save disk
	if( diskIT == mDisksPositions.end() ) {
		disk = positionDiskCreate();
		mDisksPositions.push_back( disk );
	} else
		disk = *diskIT;

	// Save the file to the disk
	return disk->fileSave( pFilename, pData, pSaveSize, 0x7800 );
}

bool cCastleManager::scoresLoad( string pCastleName, byte *pData ) {
	stringstream filename;

	filename << "Y";
	filename << pCastleName;

	return positionLoad( filename.str().c_str(), pData );
}

bool cCastleManager::scoresSave( string pCastleName, size_t pSaveSize, byte *pData ) {
	stringstream filename;

	filename << "Y";
	filename << pCastleName;

	return positionSave( filename.str().c_str(), pSaveSize, pData );

}

vector<string> cCastleManager::musicGet() {
	vector<string>		musicFiles = filesFind("MUSIC*");

	return musicFiles;
}

byte *cCastleManager::fileLoad( string pFilename, size_t &pBufferSize ) {
	byte *buffer = 0;
	
	size_t size;
	
	// Attempt to load from a D64
	buffer = diskLoadFile( pFilename, pBufferSize );

	// Load from local if that fails
	if(!pBufferSize) {
		
		sFileLocal *file = fileFind( pFilename );
		if(!file) {
			buffer = local_FileRead( pFilename, size, false );
			mFiles.push_back( file = new sFileLocal( pFilename, buffer, size ));
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

	mCastle->infoGet()->castleNumberSet( pNumber );
	return mCastle;
}
