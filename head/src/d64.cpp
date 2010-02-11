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

#include "stdafx.h"
#include "d64.h"

string stringRip(byte *pBuffer, byte pTerminator, size_t pLengthMax) {
	string tmpString;

	for(size_t i = 0; *pBuffer != pTerminator && i <= pLengthMax; i++) {
		tmpString += (char) *pBuffer++;
	}

	return tmpString;
}

cD64::cD64( string pD64 ) {
	mBufferSize = 0;

	mBuffer = local_FileRead( pD64, mBufferSize );

	mTrackCount = 35;

	directoryLoad();
}

cD64::~cD64( ) {
	vector< sD64File * >::iterator		fileIT;

	// Cleanup files
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT )
		delete *fileIT;

	delete mBuffer;
}

void cD64::directoryLoad() {
	// Directory starts at Track 18, sector 1
	size_t   currentTrack = 0x12, currentSector = 1;
	byte	*sectorBuffer;

	// Loop until the current Track/Sector is invalid
	while( (currentTrack > 0 && currentTrack < mTrackCount) && (currentSector <= trackRange( currentTrack )) ) {
		sectorBuffer = sectorPtr( currentTrack, currentSector );
		byte *buffer = sectorBuffer;
		
		if(!buffer)
			break;

		for( byte i = 0; i <= 7; ++i, buffer += 0x20 ) {
			sD64File *file = new sD64File();
			
			// Get the filetype
			file->mFileType = (eD64FileType) (*(buffer + 0x02) & 0x0F);

			// Get the filename
			file->mName = stringRip( buffer + 0x05, 0xA0, 16 );
			if( file->mName.size() == 0 ) {
				delete file;
				continue;
			}
			
			// Get the starting Track/Sector
			file->mTrack = *(buffer + 0x03);
			file->mSector = *(buffer + 0x04);
			
			// Total number of blocks
			file->mFileSize = readWord( buffer + 0x1E );

			// Load the file into a new buffer
			if(file->mFileSize > 0)
				fileLoad( file );
				
			mFiles.push_back( file );	
		}

		// Get the next Track/Sector in the chain
		currentTrack = sectorBuffer[0];
		currentSector = sectorBuffer[1];
	}
}

bool cD64::fileLoad( sD64File *pFile ) {
	size_t bytesCopied = 0;
	size_t copySize = 254;
	size_t currentTrack = pFile->mTrack, currentSector = pFile->mSector;
	bool   noCopy = false;

	// 
	delete pFile->mBuffer;
	
	// Prepare the buffer, (Each block is 254 bytes)
	pFile->mBuffer = new byte[ pFile->mFileSize * 254 ];
	pFile->mBufferSize = pFile->mFileSize * 254;

	// Temp buffer ptr
	byte *destBuffer = pFile->mBuffer;

	// Loop until invalid track/sector
	while( currentTrack > 0 && (currentSector <= trackRange( currentTrack )) ) {
		
		// Get ptr to current sector
		byte *buffer = sectorPtr( currentTrack, currentSector );

		if(!buffer)
			return false;
		
		// T/S is broken, or the directory entry is wrong about size
		if( bytesCopied >= pFile->mBufferSize )
			noCopy = true;

		// Last Sector of file? 
		if( buffer[0] == 0 ) {
			// Bytes used is stored in the T/S chain sector value
			copySize = (buffer[1] - 1);
		}

		// Copy sector data, excluding the T/S Chain data
		if(!noCopy)
			memcpy( destBuffer, buffer + 2, copySize );
		
		// Move the dest buffer forward
		destBuffer += copySize;
		bytesCopied += copySize;

		// Next Track/Sector for this file
		currentTrack = buffer[0];
		currentSector = buffer[1];
	}

	// If it failed because of a broken T/S or wrong directory entry
	// Do it again with the proper size
	if(noCopy) {
		pFile->mFileSize = (bytesCopied / 254) + 1;
		
		return fileLoad( pFile );
	}

	return true;
}

byte *cD64::sectorPtr( size_t pTrack, size_t pSector ) {
	size_t	currentTrack, currentRange;
	dword	offset = 0;

	// Invalid track?
	if(pTrack <= 0 || pTrack > mTrackCount)
		return 0;

	// Invalid sector?
	if(pSector < 0 || pSector > trackRange(pTrack))
		return 0;

	// Loop through each track, and add up
	for(currentTrack = 1; currentTrack <= pTrack; currentTrack++) {
		currentRange = trackRange( currentTrack );

		// If we're not looking for this track
		if(currentTrack != pTrack) {

			//increase the offset to the end of the track
			offset += (256 * currentRange);
			continue;
		} else {
			//increase offset to the sector we're after
			offset += (256 * pSector);

			if( offset >= mBufferSize )
				return 0;

			// Return pointer
			return (mBuffer + offset);
		}
	}

	return 0;
}

sD64File *cD64::fileGet( string pFilename ) {
	vector< sD64File* >::iterator fileIT;
	
	// Loop thro all files on disk for specific filename
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {

		if( (*fileIT)->mName == pFilename )
			return *fileIT;
	}

	return 0;
}

vector< sD64File* > cD64::directoryGet( string pFind ) {
	vector< sD64File* > result;
	vector< sD64File* >::iterator	fileIT;
	
	// Strip any astrix
	size_t	pos = pFind.find("*");
	if(pos != string::npos)
		pFind = pFind.substr(0, pos);

	// Loop all files found in directory
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {
		sD64File *file = *fileIT;

		// Skip deleted files
		if(file->mFileType == eD64FileType_DEL )
			continue;
		
		// Check if the current file matches the 'pFind' string
		if( file->mName.substr(0, pFind.length()) ==  pFind )
			result.push_back( file );
	}

	return result;
}
