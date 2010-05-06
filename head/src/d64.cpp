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
 *  Commodore 64 Disk Image Handling
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "d64.h"

// Function to rip a string from a byte array
string stringRip(byte *pBuffer, byte pTerminator, size_t pLengthMax) {
	string tmpString;

	for(size_t i = 0; *pBuffer != pTerminator && i <= pLengthMax; i++) {
		tmpString += (char) *pBuffer++;
	}

	return tmpString;
}

// D64 Constructor
cD64::cD64( string pD64, bool pCreate, bool pDataSave ) {
	mBufferSize = 0;
	mCreated = false;

	mFilename = pD64;
	mDataSave = pDataSave;

	mTrackCount = 35;

	mBuffer = local_FileRead( pD64, mBufferSize, pDataSave );

	if( !mBuffer ) {
		
		mBuffer = new byte[ 0x2AB00 ];
		mBufferSize = 0x2AB00;
		memset( mBuffer, 0, 0x2AB00 );

		if( local_FileCreate( pD64, pDataSave ) == false ) 
			return;

		bamCreate();

		diskWrite();
		mCreated = true;

	} else
		bamLoadFromBuffer();

	
	directoryLoad();
}

// D64 Destructor
cD64::~cD64( ) {
	vector< sD64File * >::iterator		fileIT;

	// Cleanup files
	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT )
		delete *fileIT;

	delete mBuffer;
}

// Create the BAM Track
void cD64::bamCreate() {
	const char *diskname = "CREEP SAVE";
	
	byte *buffer = sectorPtr( 18, 0 );

	// Track / Sector of First Directory Sector
	buffer[0x00] = 0x12;		// Track 18
	buffer[0x01] = 0x01;		// Sector 1

	// Dos Version Type
	buffer[0x02] = 0x41;
	
	// Unused Areas
	buffer[0xA4] = 0xA0;
	buffer[0xA5] = 0x2A;
	buffer[0xA6] = 0x2A;
	buffer[0xA0] = buffer[0xA1] = 0xA0;
	buffer[0xA7] = buffer[0xA8] = buffer[0xA9] = buffer[0xAA] = 0xA0;
	
	// Disk Name
	memset( &buffer[0x90], 0xA0, 0x10 );
	memcpy( &buffer[0x90], diskname, strlen( diskname ) );

	// Disk ID
	buffer[0xA2] = 0x00;	buffer[0xA3] = 0x00;

	// Buffer points to start of BAM Data
	buffer += 0x04;
	
	// Clear the BAM to empty disk state
	bamClear();

	// Mark both Track 18 sectors 0/1 as in use
	bamSectorMark( 18, 0, false );
	bamSectorMark( 18, 1, false );

	// Save the new bam to the buffer
	bamSaveToBuffer();
}

// Set BAM to empty disk state
void cD64::bamClear() {

	// Clear BAM memory 
	for( size_t T = 1; T <= mTrackCount; ++T ) {
		mBamFree[T-1] = trackRange(T);

		for( size_t S = 0; S <= 0x17; ++S ) {

			if(S < trackRange(T))
				mBamTracks[T-1][S] = true;
			else
				mBamTracks[T-1][S] = false;
		}
	}
}

// Load the internal 'mBamTracks' flags from the buffer
void cD64::bamLoadFromBuffer() {
	byte *buffer = sectorPtr( 18, 0 );

	bamClear();

	// Load the BAM
	buffer += 0x04;

	for( size_t T = 1; T <= mTrackCount; ++T ) {
		// Free Sector Count
		mBamFree[T-1] = *buffer++;
			
		// Load each sector
		for( size_t S = 0; S <= 0x17; ) {

			byte data = *buffer++;

			// Load the value for each sector in this byte
			for( byte count = 0; count < 8; ++count, ++S ) {
				if( data & 0x01 )
					mBamTracks[T-1][S] = true;
				else
					mBamTracks[T-1][S] = false;

				data >>= 1;
			}
		}
	}

}

// Save the BAM from the internal 'mBamTracks' to the buffer
void cD64::bamSaveToBuffer() {
	byte *buffer = sectorPtr( 18, 0 );

	buffer += 4;

	for( size_t track = 1; track <= mTrackCount; ++track ) {
		size_t sectors = trackRange( track );

		buffer[0] = mBamFree[track-1];
		++buffer;

		for( size_t sector = 0; sector < sectors; ) {
			byte final = 0;

			for( size_t count = 0; count < 8; ++count, ++sector ) {
				final >>= 1;

				if( mBamTracks[track-1][sector] == true )
					final |= 0x80;
			}

			*buffer++ = final;	
		}
	}
}

// Mark a sector as being Used/Free in the internal 'mBamTracks'
void cD64::bamSectorMark( size_t pTrack, size_t pSector, bool pValue ) {
	
	// Check if the sector was marked in use already
	if( mBamTracks[pTrack-1][pSector] == true ) {

		// Marked Free
		if( pValue == false )
			--mBamFree[pTrack-1];

	} else {
		// Marked in use
		if( pValue == true )
			++mBamFree[pTrack-1];
	}

	mBamTracks[pTrack-1][pSector] = pValue;
}

// Check a track for free sectors
bool cD64::bamTrackSectorFree( size_t &pTrack, size_t &pSector ) {
	size_t track = pTrack;
	bool wrapped = false;

	for( size_t sector = pSector; sector < trackRange( track ); ) {
			
		// Is this sector free
		if( mBamTracks[track-1][sector] == true ) {
			pTrack = track;
			pSector = sector;
			return true;
		}
	
		++sector;

		// Check its in range for the track
		if( sector >= trackRange( track ) ) {
			if( wrapped == false ) {
				wrapped = true;
				sector = 0;
				pSector = 0;
				continue;
			}
			
			pSector = sector;
			return false;

		}	// If >= trackrange

	}	// For Sector
}

// Find a free sector
// 
// Starting from the directory track, attempt to find a free sector, moving the head in one direction
//
bool cD64::bamSectorFree( size_t &pTrack, size_t &pSector, size_t pDirectoryTrack ) {
	bool first = false, moveDown = true;
	size_t	moveSize = 0;

	// No track specified, start near the directorytrack
	if(pTrack == 0) {
		pTrack = (pDirectoryTrack - 1);
		first = true;
	}

	// Get the distance between the directory track and the specified track
	if( pTrack < pDirectoryTrack ) {
		moveDown = true;
		moveSize = pDirectoryTrack - pTrack;
	} else {
		moveDown = false;
		moveSize = pTrack - pDirectoryTrack;
	}
	
	// Increase sector by the drive increment
	if(!first)
		pSector += 10;

	// Calculated sector has passed max for the track?
	if( pSector >= trackRange( pTrack ) ) {
		pSector = 0;
	}

	for( size_t track = pTrack; track > 0 && track <= mTrackCount; ) {
		bool wrapped = false;
		
		// Test this track, starting at 'pSector'
		if( bamTrackSectorFree( track, pSector ) == true ) {
			pTrack = track;
			return true;
		}

		// If we're trying to locate the first sector for a new 'file save'
		// Move around based on the directory track
		if(first) {
			if(moveDown) {
				moveDown = false;
				moveSize += 1;
				track += moveSize;
			} else {
				moveDown = true;
				moveSize += 1;
				track -= moveSize;
			}
			
			if( track == 0 || track >= mTrackCount )
				return false;

			pSector = 0;
			continue;
		}

		// Move the track down or up depending on current direction
		if(moveDown) {
			track -= 1;
		} else {
			track += 1;
		}

		// Passed first track on disk?
		if( track == 0 ) {
			track += (pDirectoryTrack + 1);
			moveDown = false;
		}
		
		// At last track on disk?
		if( track >= mTrackCount ) {
			track = (pDirectoryTrack - 1);
			moveDown = true;
		}

		pSector = 0;
	}	// For Track

	return false;
}

// Follow a file based on its first track/sector
// And create a list of track/sectors used by it
void cD64::chainLoad( sD64File *pFile ) {
	byte  track = pFile->mTrack, sector = pFile->mSector;
	byte *buffer = 0;
	
	pFile->mTSChain.clear();

	for(; track > 0 && track <= mTrackCount && sector < trackRange( track ) ;) {
		pFile->mTSChain.push_back( sD64Chain( track, sector ) );
		
		buffer = sectorPtr( track, sector );

		// Final sector of file
		if( buffer[0] == 0 )
			break;

		track = buffer[0];
		sector = buffer[1];
	}
}

void cD64::directoryLoad() {
	// Directory starts at Track 18, sector 1
	size_t   currentTrack = 0x12, currentSector = 1;
	byte	*sectorBuffer;

	// Loop until the current Track/Sector is invalid
	while( (currentTrack > 0 && currentTrack <= mTrackCount) && (currentSector <= trackRange( currentTrack )) ) {
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

bool cD64::directoryEntrySet( byte pEntryPos, byte *pBuffer ) {
	if( pEntryPos > 0)
		pBuffer[ 0x00 ] = pBuffer[ 0x01 ] = 0;
	
	// File Type
	pBuffer[ 0x02 ] = 0;

	// Filename
	memset( &pBuffer[ 0x05 ], 0xA0, 0x0F );
	return true;
}

bool cD64::directoryEntrySet( byte pEntryPos, sD64File *pFile, byte *pBuffer ) {
	if( pFile->mName.size() > 0x0F )
		return false;

	if( pEntryPos > 0)
		pBuffer[ 0x00 ] = pBuffer[ 0x01 ] = 0;
	
	// File Type
	pBuffer[ 0x02 ] = pFile->mFileType;
	
	// Starting Track/Sector
	pBuffer[ 0x03 ] = pFile->mTrack;
	pBuffer[ 0x04 ] = pFile->mSector;

	// Filename
	memset( &pBuffer[ 0x05 ], 0xA0, 0x10 );
	memcpy( &pBuffer[ 0x05 ], pFile->mName.c_str(), pFile->mName.size() );
	
	// Number of sectors used by file
	word totalBlocks = (pFile->mBufferSize / 254);
	if( pFile->mBufferSize % 254 )
		++totalBlocks;

	writeWord( &pBuffer[ 0x1E ], totalBlocks );
	return true;
}

bool cD64::directoryAdd( sD64File *pFile ) {
	
	// Directory starts at Track 18, sector 1
	size_t   currentTrack = 0x12, currentSector = 1;
	byte	*sectorBuffer;

	// Loop until the current Track/Sector is invalid
	while( (currentTrack > 0 && currentTrack <= mTrackCount) && (currentSector <= trackRange( currentTrack )) ) {
		sectorBuffer = sectorPtr( currentTrack, currentSector );
		byte *buffer = sectorBuffer;
		
		if(!buffer)
			break;

		// Is the next T/S set?
		if( sectorBuffer[0] == 0 ) {

			// TODO: Handle this better
			if( currentSector + 3 > trackRange( currentTrack ) )
				return false;

			sectorBuffer[0] = 0x12;
			sectorBuffer[1] = currentSector + 3;

			bamSectorMark( 0x12, sectorBuffer[1], false );
		}

		for( byte i = 0; i <= 7; ++i, buffer += 0x20 ) {
			 byte fileType = (*(buffer + 0x02) & 0x0F);

			 // Deleted Entries
			 if( fileType == 0x00 || fileType == 0x80 ) {
				directoryEntrySet( i, pFile, buffer );
				return true;
			 }
		}
		// This entry table is full
		
		// Get the next Track/Sector in the chain
		currentTrack = sectorBuffer[0];
		currentSector = sectorBuffer[1];
	};

	return false;
}

bool cD64::fileLoad( sD64File *pFile ) {
	size_t bytesCopied = 0;
	size_t copySize = 254;
	size_t currentTrack = pFile->mTrack, currentSector = pFile->mSector;
	bool   noCopy = false;

	// Cleanup old buffer
	delete pFile->mBuffer;
	
	// Prepare the buffer, (Each block is 254 bytes, the remaining two is used for the T/S chain)
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

bool cD64::fileSave( string pFilename, byte *pData, size_t pBytes, word pLoadAddress ) {
	pBytes += 2;	// Load Address Bytes
	
	size_t bytesRemain = pBytes;
	
	sD64File	File;
	transform( pFilename.begin(), pFilename.end(), pFilename.begin(), toupper );

	File.mName = pFilename;
	File.mFileType = (eD64FileType) 0x82;		// PRG
	File.mBufferSize = pBytes;
	
	byte	*buffer = 0, *bufferSrc = pData;
	size_t	 track = 0, sector = 0, copySize = 0;

	bool sectorFirst = true;

	while( bytesRemain ) {
		
		// Get available T/S
		if( bamSectorFree( track, sector ) == false )
			return false;
		
		// Set the next T/S in the previous sector
		if(buffer) {
			buffer[0] = track;
			buffer[1] = sector;

		} else {
			File.mTrack = track;
			File.mSector = sector;
		}

		// Grab buffer to next sector
		buffer = sectorPtr( track, sector );
		
		if( sectorFirst ) {
			writeWord( &buffer[0x02], pLoadAddress );

			// Copy filedata
			if(bytesRemain < 0xFC )
				copySize = bytesRemain;
			else		
				copySize = 0xFC;

			sectorFirst = false;

			memcpy( buffer + 4, bufferSrc, copySize );
		} else {

			// Copy filedata
			if(bytesRemain < 0xFE )
				copySize = bytesRemain;
			else		
				copySize = 0xFE;

			memcpy( buffer + 2, bufferSrc, copySize );
		}

		bufferSrc += copySize;

		// Mark the sector in use
		bamSectorMark( track, sector, false );
		
		if(bytesRemain > 0)
			bytesRemain -= copySize;
	};
	
	buffer[0] = 0;
	buffer[1] = copySize - 1;

	directoryAdd( &File );

	diskWrite();
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

void cD64::diskWrite() {

	bamSaveToBuffer();
	local_FileSave( mFilename, mDataSave, mBuffer, mBufferSize );
}

sD64File *cD64::fileGet( string pFilename ) {
	vector< sD64File* >::iterator fileIT;
	
	// Upper case names only
	transform( pFilename.begin(), pFilename.end(), pFilename.begin(), toupper );

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
