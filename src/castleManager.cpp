#include "stdafx.h"
#include "d64.h"
#include "castleManager.h"

cCastleInfoD64::cCastleInfoD64( cD64 *pD64, sFile *pFile ) : cCastleInfo( pFile->mName ) {
	mD64 = pD64;
	mFile = pFile;

	mBufferSize	= mFile->mBufferSize;
}

byte *cCastleInfoD64::bufferGet() {
	return mFile->mBuffer;
}

cCastleInfoLocal::cCastleInfoLocal( sFileLocal *pFileLocal ) : cCastleInfo( pFileLocal->mFilename ) {
	mLocal = pFileLocal;
	mBufferSize = mLocal->mBufferSize;

}

byte *cCastleInfoLocal::bufferGet() {
	return mLocal->mBuffer;
}

cCastleManager::cCastleManager() {
	castlesFind();
}

cCastleManager::~cCastleManager() {
	castlesCleanup();
	diskCleanup();
	localCleanup();
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
	
	castlesCleanup();
	diskCleanup();
	localCleanup();

	disksFind( "*.d64" );

	diskLoadCastles();
	localLoadCastles();

}

cCastleInfo *cCastleManager::castleGet( string pName ) {
	vector< cCastleInfo* >::iterator castleIT;

	for( castleIT = mCastles.begin(); castleIT != mCastles.end(); ++castleIT )
		if( (*castleIT)->nameGet() == pName )
			return (*castleIT);

	return 0;
}

void cCastleManager::castleListDisplay() {
	vector< cCastleInfo* >::iterator castleIT;
	size_t number = 1;

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
	vector< sFile*>::iterator	fileIT;

	// Loop thro each file starting with Z of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sFile*>	files = (*diskIT)->directoryGet( "Z*" );

		for( fileIT = files.begin(); fileIT != files.end(); ++fileIT ) {
			
			cCastleInfoD64 *castle = new cCastleInfoD64( (*diskIT), (*fileIT) );

			// Dont add castles with the same name as already existing castles
			if( castleGet( castle->nameGet() ) ) {
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
	vector< sFile*>::iterator	fileIT;

	// Loop thro each file starting with Z of each disk
	for( diskIT = mDisks.begin(); diskIT != mDisks.end(); ++diskIT ) {
		vector<sFile*>	files = (*diskIT)->directoryGet( pFilename );

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
		sFileLocal *file = fileFind( (*fileIT) );

		if(!file) {
			string final = "castles\\";
			final.append( (*fileIT) );

			size_t size = 0;
			byte *buffer = local_FileRead( final, size );
			if(buffer)
				mFiles.push_back( file = new sFileLocal( (*fileIT), buffer, size ));
			else
				continue;

		}
		cCastleInfoLocal *local = new cCastleInfoLocal( file );
		
		// Dont add castles with the same name as already existing castles
		if( castleGet( local->nameGet() ) ) {
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

byte *cCastleManager::fileLoad( string pFilename, size_t &pBufferSize ) {
	byte *buffer = 0;
	
	size_t size;
	
	// Attempt to load from a D64
	buffer = diskLoadFile( pFilename, pBufferSize );

	// Load from local if that fails
	if(!pBufferSize) {
		
		sFileLocal *file = fileFind( pFilename );
		if(!file) {
			buffer = local_FileRead( pFilename, size );
			mFiles.push_back( file = new sFileLocal( pFilename, buffer, size ));
		}

		buffer = file->mBuffer;
		pBufferSize = file->mBufferSize;
	}

	return buffer;
}

cCastle *cCastleManager::castleGet( size_t pNumber ) {

	if( pNumber >= mCastles.size() )
		return 0;

	cCastle *castle = new cCastle( mCastles[ pNumber ] );

	return castle;
}

cCastle::cCastle( cCastleInfo *pCastleInfo ) {
	mName = pCastleInfo->nameGet();
	mBuffer = pCastleInfo->bufferGet();
	mCastleInfo = pCastleInfo;

}

cCastle::~cCastle() {
	
}

byte *cCastle::bufferGet(size_t &pBufferSize) {
	pBufferSize = mCastleInfo->bufferSizeGet();
	return mBuffer;
}
