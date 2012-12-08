/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2013 Robert Crossfield
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

class cCastleManager;
class cCastle;
class cD64;
struct sD64File;

struct sFileLocal {
	string	 mFilename;
	byte	*mBuffer;
	size_t	 mBufferSize;

	sFileLocal( string pFilename, byte *pBuffer, size_t pBufferSize ) {
		mFilename = pFilename;
		mBuffer = pBuffer;
		mBufferSize = pBufferSize;
	}

	~sFileLocal() {
		delete mBuffer;
	}
};

class cCastleInfo {
protected:
	cCastleManager	*mCastleManager;
	string			 mName;
	size_t			 mBufferSize,		mCastleNumber;
	
public:

	cCastleInfo( cCastleManager *pCastleManager, string pFilename ) {
		mCastleNumber = 0;
		mBufferSize = 0;
		mCastleManager = pCastleManager;

		mName = pFilename.substr(1);
		std::transform(mName.begin(), mName.end(), mName.begin(), ::tolower);
		mName[0] = toupper( mName[0] );
	}

	string			  nameGet() { return mName; }
	size_t			  bufferSizeGet() { return mBufferSize; }
	virtual byte	 *bufferGet() = 0;
	
	inline cCastleManager *managerGet() { return mCastleManager; }
	inline void				castleNumberSet( size_t pNumber ) { mCastleNumber = pNumber; }
	inline size_t			castleNumberGet() { return mCastleNumber; }
};

class cCastleInfoD64 : public cCastleInfo {
private:
	cD64			*mD64;
	sD64File		*mFile;
	
public:
					 cCastleInfoD64( cCastleManager *pCastleManager, cD64 *pD64, sD64File *pFile );

	byte			*bufferGet();
};

class cCastleInfoLocal : public cCastleInfo {
private:
	sFileLocal		*mLocal;
	
public:
					 cCastleInfoLocal( cCastleManager *pCastleManager, sFileLocal *pFileLocal );

	byte			*bufferGet();
};

class cCastleManager {
private:
	cCastle					*mCastle;				// Current Castle
	vector< cCastleInfo* >	 mCastles;				// All castles found
	vector< cD64* >			 mDisks;				// Open disk images
	vector< cD64* >			 mDisksPositions;		// Save Game Disks
	vector< cD64* >			 mDisksCastles;			// Castle Disks
	vector< sFileLocal* >	 mFiles;				// Open Local Files

	void					 castlesCleanup();		// Cleanup mCastles vector
	void					 castlesFind();			// Find all available castles


	void					 diskCleanup();			// Cleanup mDisks vector
	void					 diskPosCleanup();
	void					 diskCastlesCleanup();
	void					 diskLoadCastles();		// Load all castles off disks
	void					 diskLoadCastle();		// Load all castles off castle folder disks

	void					 disksFind( string pExtension );		// Find all D64 Images
	void					 diskCastleFind( string pExtension );
	void					 diskPosFind( string pExtension );

	void					 localCleanup();
	void					 localLoadCastles();	// Load all castles in data folder

public:
							 cCastleManager();
							~cCastleManager();

	cCastle					*castleLoad( size_t pNumber );
	cCastleInfo				*castleInfoGet( string pName );
	cCastleInfo				*castleInfoGet( size_t pNumber );

	void					 castleListDisplay();	// Display list of castles

	byte					*diskLoadFile( string pFilename, size_t &pBufferSize );
	
	sFileLocal				*fileFind( string pFilename );
	byte					*fileLoad( string pFilename, size_t &pBufferSize );

	vector<string>			 filesFind( string pFilemask );

    byte                    *introLoad( size_t &pBufferSize );

	bool					 castleSave( string pFilename, size_t pSaveSize, byte *pData );

	bool					 positionLoad( string pFilename, byte *pTarget );
	bool					 positionSave( string pFilename, size_t pSaveSize, byte *pData );
	cD64					*positionDiskCreate();
	cD64					*castleDiskCreate();

	bool					 scoresLoad( string pCastleName, byte *pData );
	bool					 scoresSave( string pCastleName, size_t pSaveSize, byte *pData );

	vector<string>			 musicGet();

};
