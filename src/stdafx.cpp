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
 *  Standard Functions
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "creep.h"
#include "castle/objects/object.hpp"

const char   *VERSION = "v1.3";

const char	 *gDataPath = "data/";
const char	 *gSavePath = "data/save/";

#ifdef _MACOSX
int SDL_main( int argc, char *argv[]) {
#else
int	main( int argc, char *argv[] ) {
#endif
    
#ifdef WIN32
	SetConsoleTitle( L"The Castles of Dr.Creep" );
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
#endif

	cCreep* gCreep = new cCreep();

	cSteam* Steam = new cSteam();
	gCreep->run( argc, argv );

	delete Steam;
	delete gCreep;

	return 0;
}

string local_PathGenerate( string pFile, string pPath, bool pDataSave ) {
	stringstream	 filePathFinal;

#ifdef FREEBSD
                filePathFinal << "/usr/local/share/drcreep/";
#endif

	// Build the file path
	if(!pDataSave)
		filePathFinal << gDataPath;
	else
		filePathFinal << gSavePath;

	if( pPath.size() )
		filePathFinal << pPath << "/";

	filePathFinal << pFile;

	return filePathFinal.str();
}

bool local_FileCreate( string pFile, string pPath, bool pDataSave ) {
	ofstream		*fileStream;

	string finalPath = local_PathGenerate( pFile, pPath, pDataSave );

	fileStream = new ofstream ( finalPath.c_str(), ios::binary );
	if( fileStream->is_open() == false) {
		delete fileStream;
		return false;
	}

	delete fileStream;
	return true;
}

bool local_FileSave( string pFile, string pPath, bool pDataSave, byte *pBuffer, size_t pBufferSize ) {
	ofstream		*fileStream;

	string finalPath = local_PathGenerate( pFile, pPath, pDataSave );
	fileStream = new ofstream( finalPath.c_str(), ios::binary | ios::out );
	if(fileStream->is_open() == false) {
		delete fileStream;
		return false;
	}

	fileStream->write( (char*) pBuffer, pBufferSize );
	fileStream->close();
	delete fileStream;

	return true;
}

byte *local_FileRead( string pFile, string pPath, size_t	&pFileSize, bool pDataSave ) {
	ifstream		*fileStream;
	byte			*fileBuffer = 0;
	string finalPath = local_PathGenerate( pFile, pPath, pDataSave );

	// Attempt to open the file
	fileStream = new ifstream ( finalPath.c_str(), ios::binary );
	if(fileStream->is_open() != false) {
	
		// Get file size
		fileStream->seekg(0, ios::end );
		pFileSize = (size_t) fileStream->tellg();
		fileStream->seekg( ios::beg );

		// Allocate buffer, and read the file into it
		fileBuffer = new byte[ pFileSize ];
		fileStream->read( (char*)fileBuffer, pFileSize );

		if (!(*fileStream)) {
			delete fileBuffer;
			fileBuffer = 0;
		}
	}
	
	// Close the stream
	fileStream->close();
	delete fileStream;

	// All done ;)
	return fileBuffer;
}

// WiN32 Functions
#ifdef WIN32
#include <direct.h>
bool CtrlHandler( dword fdwCtrlType ) {
	
	switch( fdwCtrlType ) {

		case CTRL_CLOSE_EVENT:
		case CTRL_C_EVENT:
			exit(0);

	default:
		return FALSE;
	}
}

vector<string> directoryList(string pPath, string pExtension, bool pDataSave) {
    WIN32_FIND_DATA fdata;
    HANDLE dhandle;
	vector<string> results;

	char path[2000];
	_getcwd(path, 2000);

	// Build the file path
	stringstream finalPath;
	finalPath << path << "/";
	if(!pDataSave)
		finalPath << gDataPath;
	else
		finalPath << gSavePath;

	if(pPath.size())
		finalPath << pPath;

	finalPath << "/*" << pExtension;

	size_t size = MultiByteToWideChar( 0,0, finalPath.str().c_str(), (int) finalPath.str().length(), 0, 0);
	WCHAR    *pathFin = new WCHAR[ size + 1];
	memset( pathFin, 0, size + 1);

	size = MultiByteToWideChar( 0,0, finalPath.str().c_str(), (int) size, pathFin, (int) size);
	pathFin[size] = 0;

	if((dhandle = FindFirstFile(pathFin, &fdata)) == INVALID_HANDLE_VALUE) {
		delete pathFin;
		return results;
	}
	
	delete pathFin;
	char *file = new char[ wcslen(fdata.cFileName) + 1];
	memset(file, 0, wcslen(fdata.cFileName) + 1 );

	wcstombs( file, fdata.cFileName, wcslen(fdata.cFileName) );
    results.push_back(string(file));
	delete file;

    while(1) {
            if(FindNextFile(dhandle, &fdata)) {
				char *file = new char[ wcslen(fdata.cFileName) + 1];
				memset(file, 0, wcslen(fdata.cFileName) + 1 );
				wcstombs( file, fdata.cFileName, wcslen(fdata.cFileName) );
				results.push_back(string(file));
				delete file;
				
            } else {
                    if(GetLastError() == ERROR_NO_MORE_FILES) {
                            break;
                    } else {
                            FindClose(dhandle);
                            return results;
                    }
            }
    }

    FindClose(dhandle);

	return results;
}

// End Win32 Functions

#else
	
#ifndef FREEBSD
#ifndef _MACOSX
#ifndef LINUX
#include <direct.h>
#endif
#endif
#endif
#include <dirent.h>
#include <stdlib.h>

bool CtrlHandler( dword fdwCtrlType ) {
	
	return true;
}

string findType;

int file_select(const struct dirent *entry) {
	string name = entry->d_name;

	transform( name.begin(), name.end(), name.begin(), ::toupper );
   	
	if( name.find( findType ) == string::npos )
		return false;
	
	return true;
}

vector<string> directoryList(string pPath, string pExtension, bool pDataSave) {
	struct dirent		**directFiles;
	vector<string>		  results;

	char path[2000];
	#ifndef FREEBSD
            getcwd(path, 2000);
    #else
	strcpy(&path[0],"/usr/local/share/drcreep");
	#endif

	// Build the file path
	stringstream finalPath;

	finalPath << path << "/";

	if(!pDataSave)
		finalPath << gDataPath;
	else
		finalPath << gSavePath;

	if(pPath.size())
		finalPath << pPath;

	findType = pExtension;
		
    transform( findType.begin(), findType.end(), findType.begin(), ::toupper);

	int count = scandir(finalPath.str().c_str(), (dirent***) &directFiles, file_select, alphasort);
	
	for( int i = 0; i < count; ++i ) {

		results.push_back( string( directFiles[i]->d_name ) );
	}
	
	return results;
}

#endif
