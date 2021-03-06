/*
 *  The Castles of Dr. Creep 
 *  ------------------------
 *
 *  Copyright (C) 2009-2016 Robert Crossfield
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
 *  Standard Header
 *  ------------------------------------------
 */

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/timeb.h>

using namespace std;

#include <SDL.h>

#include "types.h"
#include "Singleton.hpp"
#include "Dimension.hpp"
#include "Position.hpp"
#include "Event.hpp"
#include "Menu.hpp"
#include "graphics/screenSurface.h"
#include "graphics/window.hpp"
#include "Steam.hpp"
class cCreep;

extern const char *VERSION;

#ifndef _WIN32
	#include <unistd.h>
    #define	 Sleep( a ) usleep( a * 1000 );
#endif

#ifdef _WII
	#define ENDIAN_BIG
#else
	#define ENDIAN_SMALL
#endif

// Endian functions
#ifdef ENDIAN_BIG
	// Read a word from the buffer
	inline word readWord( const void *buffer ) {
		const word *wordBytes = (const word *) buffer;
		return *wordBytes;
	}

	// Read a word from the buffer
	inline word	readLEWord( const void *buffer ) {
		const byte *wordByte = (const byte *) buffer;

		return (wordByte[1] << 8) | wordByte[0];
	}

	inline void writeLEWord( const void *buffer, word pValue ) {
		byte *wordBytes = (byte *) buffer;
		
		wordBytes[0] = (pValue & 0x00FF);
		wordBytes[1] = (pValue & 0xFF00) >> 8;
	}

	// Swap the bytes around in a word
	inline word	swapWord(  word buffer ) {
		const byte *wordByte = (const byte *) &buffer;
		return (wordByte[0] << 8) + wordByte[1];
	}
#else
	// Read a word from the buffer
	inline word readWord( const void *buffer ) {
		const word *wordBytes = (const word *) buffer;
		return *wordBytes;
	}

	// Read a word from the buffer
	inline word readLEWord( const void *buffer ) {
		const word *wordBytes = (const word *) buffer;
		return *wordBytes;
	}

	inline void writeLEWord( const void *buffer, word pValue ) {
		word *wordBytes = (word *) buffer;
		*wordBytes = pValue;
	}

	// Swap the bytes around in a word
	inline word swapWord(  word buffer ) {
		return buffer;
	}
#endif

bool CtrlHandler( dword fdwCtrlType );
vector<string>	 directoryList(string pPath, string pExtension, bool pDataSave);
byte			*local_FileRead( string pFile, string pPath, size_t	&pFileSize, bool pDataSave );
bool			 local_FileCreate( string pFile, string pPath, bool pDataSave );
bool			 local_FileSave( string pFile, string pPath, bool pDataSave, byte *pBuffer, size_t pBufferSize );

#define g_Window cWindow::GetSingleton()
#define g_Creep cCreep::GetSingleton()
#define g_Steam cSteam::GetSingleton()
