// File:	StringList.hpp
// Author:	Mark Roth
// Description:
//		A generic class to hold a sequential list of strings
// Usage:	
//		To add a string to the end of the list, call AddString
//		To retrieve a string at a given line, call GetString
// Revision History:
//		0.0:	Initial Revision
//

#pragma once

#include <vector>
#include <ostream>
#include <string>

class CStringList : public std::vector<std::string> {
public:
	//
	// Name:		ReadFromFile
	//
	// Description:	Reads in lines from an ASCII file
	// Arguments:	The ASCII file to read from
	// Exceptions:	Throws a CErrorException if the file could not
	//		be opened (SL1001), if there was an I/O error reading 
	//		the file (SL1002), or if there are lines in the file 
	//		that are greater than 255 characters (SL1003).
	//
	void ReadFromFile( const char *filename );

public:		// Output

	//
	// Name:	Dump
	//
	// Description:	Dumps the string list to the given stream
	// Arguments:	The output stream to display the string list to
	//
	void Dump( std::ostream &out );

};
