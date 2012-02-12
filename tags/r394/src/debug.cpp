#include "stdafx.h"
#include "debug.h"

cDebug::cDebug() {

}

cDebug::~cDebug() {

}

void cDebug::error( string pMessage ) {
	string str;

	cout << pMessage << endl;
	cout << "Press Enter to continue" << endl;

	getline( cin, str );
}
