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
 *  Debug
 *  ------------------------------------------
 */

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
