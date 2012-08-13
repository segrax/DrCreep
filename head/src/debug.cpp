#include "stdafx.h"
#include "creep.h"
#include "debug.h"

cDebug::cDebug() {

}

cDebug::~cDebug() {

}

void cDebug::consoleOpen() {
#ifdef WIN32
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_SHOW );
#endif
}

void cDebug::consoleHide() {
#ifdef WIN32
	HWND hWnd = GetConsoleWindow();
	ShowWindow( hWnd, SW_HIDE );
#endif
}

void cDebug::error( string pMessage ) {
	string str;

    consoleOpen();
	cout << pMessage << endl;
	cout << "Press Enter to continue" << endl;

	getline( cin, str );
    exit(1);
}
