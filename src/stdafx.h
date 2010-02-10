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
 *  Standard Header
 *  ------------------------------------------
 */

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include<algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/timeb.h>

using namespace std;

#include <sdl.h>
#include "types.h"
#include "graphics/scale/scalebit.h"


extern const char *SVNREV;
extern const char *SVNDATE;

#define ENDIAN_SMALL

// Endian functions
#ifdef ENDIAN_BIG

	// Read a word from the buffer
	inline word	readWord( const void *buffer ) {
		const byte *wordByte = (const byte *) buffer;
		return (wordByte[0] << 8) + wordByte[1];
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

	// Swap the bytes around in a word
	inline word swapWord(  word buffer ) {
		return buffer;
	}
#endif

vector<string>	 directoryList(string pPath);
byte			*local_FileRead( string pFile, size_t	&pFileSize );

