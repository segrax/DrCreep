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

struct sFile {
	byte	*mBuffer;
	size_t	 mBufferSize, mTrack, mSector;
	size_t	 mFileSize;
	string	 mName;
	
	sFile() {
		mBuffer = 0;
		mBufferSize = 0;
		mFileSize = 0;
	}

	~sFile() {
		delete mBuffer;
	}
};

class cD64 {
private:
	byte				*mBuffer;
	size_t				 mBufferSize, mTrackCount;
	vector< sFile* >	 mFiles;
	
	void				 directoryLoad();
	bool				 fileLoad( sFile *pFile );

	size_t				 trackRange(size_t pTrack);
	byte				*sectorRead( size_t pTrack, size_t pSector );

public:
						 cD64( string pD64 );
						~cD64( );

	vector< sFile* >	 directoryGet( string pFind );
	vector< sFile* >	*directoryGet();
	sFile				*fileGet( string pFilename );
};
