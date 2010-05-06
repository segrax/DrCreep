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

struct sD64Chain {
	size_t mTrack, mSector;

	sD64Chain( size_t pTrack, size_t pSector ) {
		mTrack = pTrack;
		mSector = pSector;
	}
};


struct sD64File {
	string	  mName;					// Name of the file
	size_t	  mTrack, mSector;			// Starting T/S of file
	size_t	  mFileSize;				// Number of blocks used by file
	eD64FileType mFileType;				// Type of file

	byte	*mBuffer;					// Copy of file
	size_t	 mBufferSize;				// Size of 'mBuffer'

	vector< sD64Chain >		mTSChain;	// Track/Sectors used by file

	sD64File() {
		mBuffer = 0;
		mBufferSize = 0;
		mFileSize = 0;
	}

	~sD64File() {
		delete mBuffer;
	}
};

class cD64 {
private:
	bool				 mBamTracks[35][24];							// Track/Sector Availability Map
	byte				 mBamFree[35];									// Number of free sectors per track

	byte				*mBuffer;										// Disk image buffer
	size_t				 mBufferSize, mTrackCount;
	
	bool				 mCreated;										// Was a file created

	vector< sD64File* >	 mFiles;										// Files in disk
	string				 mFilename;										// Name of current D64
	bool				 mDataSave;										// Save to Saves folder?

	void				 bamClear();									// Clear the internal BAM
	void				 bamCreate();									// Create Track 18/0
	void				 bamLoadFromBuffer();							// Load Free Tracks/Sectors
	void				 bamSaveToBuffer();								// Save the mBamTracks data to the buffer

	void				 bamSectorMark( size_t pTrack, size_t pSector, bool pValue = true );

	bool				 bamTrackSectorFree( size_t &pTrack, size_t &pSector );
	bool				 bamSectorFree( size_t &pTrack, size_t &pSector, size_t pDirectoryTrack = 0x12 );	// Find a free track/sector 

	void				 chainLoad( sD64File *pFile );					// Gather list of all tracks/sectors used by file

	void				 directoryLoad();								// Load the disk directory
	bool				 directoryAdd( sD64File *pFile );				// Add file to directory
	bool				 directoryEntrySet( byte pEntryPos, byte *pBuffer );
	bool				 directoryEntrySet( byte pEntryPos, sD64File *pFile, byte *pBuffer );
	
	bool				 fileLoad( sD64File *pFile );					// Load a file 

	byte				*sectorPtr( size_t pTrack, size_t pSector );	// Read a sector

	inline size_t		 trackRange(size_t pTrack) {					// Number of sectors in 'pTrack'
		return 21 - (pTrack > 17) * 2 - (pTrack > 24) - (pTrack > 30);
	}

	inline bool			 bamTrackSectorUse( size_t pTrack, size_t pSector ) {
		if( pTrack == 0 || pTrack > mTrackCount || pSector > trackRange(pTrack) )
			return false;

		return mBamTracks[ pTrack - 1 ][ pSector ];
	}

public:
						 cD64( string pD64, bool pCreate = false, bool pDataSave = false);
						~cD64( );

	vector< sD64File* >	 directoryGet( string pFind );		// Get a file list, with all files starting with 'pFind'
	vector< sD64File* >	*directoryGet();					// Get the file list
	
	void				 diskWrite();						// Write the buffer to the D64

	sD64File			*fileGet( string pFilename );		// Get a file
	bool				 fileSave( string pFilename, byte *pData, size_t pBytes, word pLoadAddress );// Save a file to the disk

	inline size_t		 sectorsFree() {					// Number of free sectors on disk
		size_t result = 0;

		for(size_t i = 0; i < 35; ++i )
			result += mBamFree[i];
		return result;
	}

	inline bool			 createdGet() {						// Was file created
		return mCreated;
	}
};
