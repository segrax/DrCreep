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
 *  Standard Functions
 *  ------------------------------------------
 */

#include "stdafx.h"
#include "creep.h"
#include "../rev.h"

const char	 *gDataPath = "data/";
const char	 *gSavePath = "data/save/";
const char	 *wiiBasePath = "sd:/apps/drcreep/";

cCreep		 *gCreep;

#ifdef _WII
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <dirent.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

void wiiButtonWait() {
	while(1)
	{
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		if ( pressed )
			break;

		// Wait for the next frame
		VIDEO_WaitVSync();
	}
}
void wiiStart() {
		// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	SDL_ShowCursor(SDL_DISABLE);

	if(!fatInitDefault()) {
		cout << "fatInitDefault failed\n";
		return;
	}


}
#endif


int	main( int argc, char *argv[] ) {
	
#ifdef WIN32
	SetConsoleTitle( L"The Castles of Dr.Creep" );
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
#endif

#ifdef _WII
	wiiStart();
#endif

	gCreep = new cCreep();
	gCreep->run( argc, argv );

	delete gCreep;

	return 0;
}

string local_PathGenerate( string pFile, bool pDataSave ) {
	stringstream	 filePathFinal;

#ifdef _WII
		filePathFinal << wiiBasePath;
#endif

	// Build the file path
	if(!pDataSave)
		filePathFinal << gDataPath;
	else
		filePathFinal << gSavePath;

	filePathFinal << pFile;
	return filePathFinal.str();
}

bool local_FileCreate( string pFile, bool pDataSave ) {
	ofstream		*fileStream;

	string finalPath = local_PathGenerate( pFile, pDataSave );

	fileStream = new ofstream ( finalPath.c_str(), ios::binary );
	if( fileStream->is_open() == false) {
		delete fileStream;
		return false;
	}

	return true;
}

bool local_FileSave( string pFile, bool pDataSave, byte *pBuffer, size_t pBufferSize ) {
	ofstream		*fileStream;

	string finalPath = local_PathGenerate( pFile, pDataSave );
	fileStream = new ofstream( finalPath.c_str(), ios::binary | ios::out );
	if(fileStream->is_open() == false) {
		delete fileStream;
		return false;
	}

	fileStream->write( (char*) pBuffer, pBufferSize );
	fileStream->close();

	return true;
}

byte *local_FileRead( string pFile, size_t	&pFileSize, bool pDataSave ) {
	ifstream		*fileStream;
	byte			*fileBuffer = 0;
	string finalPath = local_PathGenerate( pFile, pDataSave );

	// Attempt to open the file
	fileStream = new ifstream ( finalPath.c_str(), ios::binary );
	if(fileStream->is_open() != false) {
	
		// Get file size
		fileStream->seekg(0, ios::end );
		pFileSize = fileStream->tellg();
		fileStream->seekg( ios::beg );

		// Allocate buffer, and read the file into it
		fileBuffer = new byte[ pFileSize ];
		if(fileStream->read( (char*) fileBuffer, pFileSize ) == false) {
			delete fileBuffer;
			fileBuffer = 0;
	#ifdef _WII
		cout << "File Read Failed" << endl;
		wiiButtonWait();
	#endif
		}
	} else {
	#ifdef _WII
		cout << "File Open Failed" << endl;
		wiiButtonWait();
	#endif
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
	finalPath << path << "\\";
	if(!pDataSave)
		finalPath << gDataPath;
	else
		finalPath << gSavePath;

	if(pPath.size())
		finalPath << pPath;

	finalPath << "\\*" << pExtension;

	int size = MultiByteToWideChar( 0,0, finalPath.str().c_str(), finalPath.str().length(), 0, 0);
	WCHAR    *pathFin = new WCHAR[ size + 1];
	memset( pathFin, 0, size + 1);

	size = MultiByteToWideChar( 0,0, finalPath.str().c_str(), size, pathFin, size);
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

#ifdef _WII

vector<string> directoryList(string pPath, string pExtension, bool pDataSave) {
	vector<string> ret;
	transform( pExtension.begin(), pExtension.end(), pExtension.begin(), ::toupper);
	
	string path = local_PathGenerate( pPath, pDataSave );

	DIR* pdir = opendir(path.c_str());
	if (pdir != NULL) {
		
		while(true)  {
			struct dirent* pent = readdir(pdir);
			if(pent == NULL) 
				break;
		
			if(strcmp(".", pent->d_name) != 0 && strcmp("..", pent->d_name) != 0) {

				string name = string(pent->d_name);
				transform( name.begin(), name.end(), name.begin(), ::toupper);

				if( name.find( pExtension ) != string::npos ) {
					ret.push_back( name );
				}
			}
		}

		closedir(pdir);
	}
	return ret;
}

int ftime(timeb *nul) {
	static long long i = 0;
	nul->time = i;
	nul->millitm = i;
	return i++;
}

#else
#include <direct.h>
#include <dirent.h>

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
	getcwd(path, 2000);

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


	int count = scandir(finalPath.str().c_str(), &directFiles, file_select, 0);
	
	for( int i = 0; i < count; ++i ) {

		results.push_back( string( directFiles[i]->d_name ) );
	}
	
	return results;
}

#endif
#endif
