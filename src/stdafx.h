#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
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

// Endian functions
// Read a word from the buffer
inline word	readWord( const void *buffer ) {
	const byte *wordByte = (const byte *) buffer;
	return (wordByte[0] << 8) + wordByte[1];
}

// Endian swap a word
inline word	swapWord(  word buffer ) {
	const byte *wordByte = (const byte *) &buffer;
	return (wordByte[0] << 8) + wordByte[1];
}

vector<string>	 directoryList(string pPath);
byte			*fileRead( string pFile, size_t	&pFileSize );

