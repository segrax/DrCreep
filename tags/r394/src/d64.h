/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2012 Robert Crossfield
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
 *  D64 Handler
 *  ------------------------------------------
 */

enum eD64FileType {
	eD64FileType_DEL = 0,
	eD64FileType_SEQ = 1,
	eD64FileType_PRG = 2,
	eD64FileType_USR = 3,
	eD64FileType_REL = 4,
	eD64FileType_UNK
};

struct sD64File;

struct sD64Chain {
	size_t		 mTrack, mSector;
	sD64File		*mFile;

	sD64Chain( size_t pTrack, size_t pSector, sD64File *pFile ) {
		mTrack = pTrack;
		mSector = pSector;
		mFile = pFile;
	}
};


struct sD64File {
	bool	  mChainBroken;				// File Chain Broken
	string	  mName;					// Name of the file
	byte	  mTrack, mSector;			// Starting T/S of file
	word	  mFileSize;				// Number of blocks used by file
	eD64FileType mFileType;				// Type of file

	byte	*mBuffer;					// Copy of file
	size_t	 mBufferSize;				// Size of 'mBuffer'

	vector< sD64Chain >		mTSChain;	// Track/Sectors used by file

	sD64File() {						// Constructor
		mChainBroken = false;

		mTrack = 0;
		mSector = 0;

		mBuffer = 0;
		mBufferSize = 0;
		mFileSize = 0;
	}

	~sD64File() {						// Destructor
		delete mBuffer;
	}
};

class cD64 {
private:
	bool						 mBamTracks[36][24];							// Track/Sector Availability Map (first allocation is ignored
	byte						 mBamFree[36];									// Number of free sectors per track

	sD64File					*mBamRealTracks[36][24];						// Information about disk loaded based on files loaded

	byte						*mBuffer;										// Disk image buffer
	size_t						 mBufferSize, mTrackCount;
	
	bool						 mCreated;										// Was a file created
	bool						 mRead;											// Disk is read only
	bool						 mReady;
	bool						 mLastOperationFailed;

	vector< sD64Chain >			 mCrossLinked;									// Files which are cross linked

	vector< sD64File* >			 mFiles;										// Files in disk
	string						 mFilename;										// Name of current D64
	bool						 mDataSave;										// Save to Saves folder?
	string						 mPath;


	void						 bamClear();									// Clear the internal BAM
	void						 bamCreate();									// Create Track 18/0
	void						 bamLoadFromBuffer();							// Load Free Tracks/Sectors
	void						 bamSaveToBuffer();								// Save the mBamTracks data to the buffer

	void						 bamSectorMark( size_t pTrack, size_t pSector, bool pValue = true );				// Mark a sector used/free

	bool						 bamTrackSectorFree( size_t &pTrack, size_t &pSector );								// Check for free sectors in a track
	bool						 bamSectorFree( size_t &pTrack, size_t &pSector, size_t pDirectoryTrack = 0x12 );	// Check for free sectors on the disk

	bool						 bamTest( );															// Test the BAM against the real one

	void						 directoryLoad();														// Load the disk directory
	sD64File					*directoryEntryLoad( byte *pBuffer );									// Load an entry
	bool						 directoryAdd( sD64File *pFile );										// Add file to directory
	bool						 directoryEntrySet( byte pEntryPos, sD64File *pFile, byte *pBuffer );	// Set the directory entry in the buffer
	
	void						 filesCleanup();
	bool						 fileLoad( sD64File *pFile );							// Load a file from the disk

	byte						*sectorPtr( size_t pTrack, size_t pSector );			// Obtain pointer to 'pTrack'/'pSector' in the disk buffer

	inline size_t				 trackRange(size_t pTrack) {							// Number of sectors in 'pTrack'
		return 21 - (pTrack > 17) * 2 - (pTrack > 24) - (pTrack > 30);
	}

	inline bool					 bamTrackSectorUse( size_t pTrack, size_t pSector ) {	// Is 'pTrack' / 'pSector' free?
		if( pTrack == 0 || pTrack > mTrackCount || pSector > trackRange(pTrack) )
			return false;

		return mBamTracks[ pTrack ][ pSector ];
	}

public:
								 cD64( string pD64, string pPath, bool pCreate = false, bool pDataSave = false, bool pReadOnly = true );
								~cD64( );

	vector< sD64File* >			 directoryGet( string pFind );		// Get a file list, with all files starting with 'pFind'
	vector< sD64File* >			*directoryGet();					// Get the file list
	
	bool						 diskTest();						// Check a disk for errors
	bool						 diskWrite();						// Write the buffer to the D64

	sD64File					*fileGet( string pFilename );		// Get a file
	bool						 fileSave( string pFilename, byte *pData, size_t pBytes, word pLoadAddress );// Save a file to the disk

	inline size_t				 sectorsFree() {					// Number of free sectors on disk
		size_t result = 0;

		for(size_t i = 1; i <= mTrackCount; ++i )
			result += mBamFree[i];
		return result;
	}

	inline bool					 createdGet() {						// Was file created
		return mCreated;
	}
};
