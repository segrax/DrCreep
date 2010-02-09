#include <direct.h>
#include "stdafx.h"
#include "creep.h"
#include "..\rev.h"

const char	 *gDataPath = "data\\";
cCreep		 *gCreep;

int	main( int argc, char *argv[] ) {
	
#ifdef WIN32
	SetConsoleTitle( L"The Castles of Dr.Creep" );
#endif

	gCreep = new cCreep();
	
	gCreep->run( argc, argv );

	delete gCreep;

	return 0;
}

byte *fileRead( string pFile, size_t	&pFileSize ) {
	stringstream	 filePathFinal;
	ifstream		*fileStream;
	byte			*fileBuffer = 0;

	// Build the file path
	filePathFinal << gDataPath;
	filePathFinal << pFile;

	// Attempt to open the file
	fileStream = new ifstream ( filePathFinal.str().c_str(), ios::binary );
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
		}
	}

	// Close the stream
	fileStream->close();
	delete fileStream;

	// All done ;)
	return fileBuffer;
}

vector<string> directoryList(string pPath) {
    WIN32_FIND_DATA fdata;
    HANDLE dhandle;
	vector<string> results;

	char path[2000];
	_getcwd(path, 2000);

	// Build the file path
	stringstream finalPath;
	finalPath << path << "\\";
	finalPath << gDataPath;
	finalPath << pPath;

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