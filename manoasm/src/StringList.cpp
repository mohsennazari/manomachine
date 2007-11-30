// File:	StringList.cpp
// Author:	Mark Roth
// Description:
//		A generic class to hold a sequential list of strings
// Usage:	
//		To add a string to the end of the list, call AddString
//		To retrieve a string at a given line, call GetString
// Revision History:
//		0.0:	Initial Revision
//

#include "StringList.hpp"
#include "ErrorException.hpp"

#include <cstring>
#include <fstream>

using namespace std;


//
// Name:		ReadFromFile
//
void CStringList::ReadFromFile( const char *filename ) {
	char buffer[257];
	ifstream input_stream( filename, ios::in /* | ios::nocreate */ );
	bool isopen;

	// Check to see if the file is open:
	isopen = input_stream.is_open();

	// Check to make sure the file is actually open:
	if( !isopen ) {
		throw CErrorException( filename, 0, "SL1001",
			"Could not open input file", CErrorException::ERROR );
	}

	// Start reading the file, until an eof is reached:
	while( !input_stream.eof() ) {
		// Get a line:
		input_stream.getline( buffer, 257 );

		// If the line is > 255 characters, throw an exception:
		if( strlen( buffer ) > 255 ) {
			throw CErrorException( filename, size(), "SL1003",
				"Line longer than 80 characters encountered", 
				CErrorException::ERROR );
		}

		// Add the line to the list:
		push_back( buffer );
	}

	// Close the file:
	input_stream.close();

} // ReadFromFile


//
// Name:		Dump
//
void CStringList::Dump( ostream &out ) {
	for (const_iterator i = begin();
		i != end();
		i++)
	{
		out << *i << endl;
	}
} // DumpStringList

