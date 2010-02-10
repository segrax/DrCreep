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

cD64::cD64( string pD64 ) {
	mBufferSize = 0;

	mBuffer = local_FileRead( pD64, mBufferSize );

	mTrackCount = 35;

	directoryLoad();
}

cD64::~cD64( ) {

	vector< sFile * >::iterator		fileIT;

	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT )
		delete *fileIT;

	delete mBuffer;

}

size_t cD64::trackRange(size_t pTrack) {
	return 21 - (pTrack > 17) * 2 - (pTrack > 24) - (pTrack > 30);
}

string stringRip(byte *pBuffer, byte pTerminator, size_t pLengthMax) {
	string tmpString;

	for(size_t i = 0; *pBuffer != pTerminator && i <= pLengthMax; i++) {
		tmpString += (char) *pBuffer++;
	}

	return tmpString;
}

void cD64::directoryLoad() {
	size_t   Track = 0x12, Sector = 1;
	byte	*sectorBuffer;

	while( (Track > 0 && Track < mTrackCount) && (Sector <= trackRange( Track )) ) {
		sectorBuffer = sectorRead( Track, Sector );
		byte *buffer = sectorBuffer;

		for( byte i = 0; i <= 7; ++i ) {
			
			sFile *file = new sFile();
			
			file->mName = stringRip( buffer + 0x05, 0xA0, 16 );
			if( file->mName.size() == 0 ) {
				delete file;
				continue;
			}
			
			file->mTrack = *(buffer + 0x03);
			file->mSector = *(buffer + 0x04);
			
			file->mFileSize = readWord( buffer + 0x1E );
			if(file->mFileSize > 0)
				fileLoad( file );
			
			buffer += 0x20;
			mFiles.push_back( file );
		}

		Track = sectorBuffer[0];
		Sector = sectorBuffer[1];
	}
}

bool cD64::fileLoad( sFile *pFile ) {
	size_t copySize = 254;
	size_t Track = pFile->mTrack, Sector = pFile->mSector;

	pFile->mBuffer = new byte[ pFile->mFileSize * 254 ];
	pFile->mBufferSize = pFile->mFileSize * 254;

	byte *destBuffer = pFile->mBuffer;

	while( Track > 0 && (Sector <= trackRange( Track )) ) {
		
		byte *buffer = sectorRead( Track, Sector );
		if(!buffer)
			return false;
		
		if( buffer[0] == 0 ) {
			copySize = (buffer[1] - 1);
			pFile->mFileSize -= (254 - copySize);
		}

		// Copy sector data, excluding the T/S Chain data
		memcpy( destBuffer, buffer + 2, copySize );
		
		destBuffer += copySize;

		Track = buffer[0];
		Sector = buffer[1];
	}

	
	return true;
}

byte *cD64::sectorRead( size_t pTrack, size_t pSector ) {
	size_t T,S,R = 0;
	long Offset = 0;

	if(pTrack <= 0 || pTrack > mTrackCount)
		return 0;

	if(pSector < 0 || pSector > trackRange(pTrack))
		return 0;

	for(T = 1; T <= pTrack; T++) {
		R = trackRange(T);

		for(S = 0; S < R; S++) {
			if(pTrack == T && pSector == S)
				return mBuffer + Offset;

			Offset += 256;
		}
	}

	return 0;
}

sFile *cD64::fileGet( string pFilename ) {
	vector< sFile* >::iterator fileIT;

	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {
		if( (*fileIT)->mName == pFilename )
			return *fileIT;
	}

	return 0;
}

vector< sFile* > cD64::directoryGet( string pFind ) {
	vector< sFile* > result;
	vector< sFile* >::iterator	fileIT;
	
	size_t	pos = pFind.find("*");
	if(pos != string::npos)
		pFind = pFind.substr(0, pos);

	for( fileIT = mFiles.begin(); fileIT != mFiles.end(); ++fileIT ) {
		sFile *file = *fileIT;

		if( file->mName.substr(0, pFind.length()) ==  pFind )
			result.push_back( file );
	}

	return result;
}