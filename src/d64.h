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

struct sD64File {
	string	  mName;					// Name of the file
	size_t	  mTrack, mSector;			// Starting T/S of file
	size_t	  mFileSize;				// Number of blocks used by file
	eD64FileType mFileType;				// Type of file

	byte	*mBuffer;					// Copy of file
	size_t	 mBufferSize;				// Size of 'mBuffer'

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
	byte				*mBuffer;										// Disk image buffer
	size_t				 mBufferSize, mTrackCount;
	vector< sD64File* >	 mFiles;										// Files in disk
	
	void				 directoryLoad();								// Load the disk directory
	bool				 fileLoad( sD64File *pFile );						// Load a file 

	byte				*sectorPtr( size_t pTrack, size_t pSector );	// Read a sector

	inline size_t		 trackRange(size_t pTrack) {					// Number of sectors in 'pTrack'
		return 21 - (pTrack > 17) * 2 - (pTrack > 24) - (pTrack > 30);
	}

public:
						 cD64( string pD64 );
						~cD64( );

	vector< sD64File* >	 directoryGet( string pFind );		// Get a file list, with all files starting with 'pFind'
	vector< sD64File* >	*directoryGet();					// Get the file list
	sD64File				*fileGet( string pFilename );		// Get a file
};
