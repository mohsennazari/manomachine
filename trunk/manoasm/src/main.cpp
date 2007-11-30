// File:	main.cpp
// Author:	Mark Roth
// Description:	Main module for Mano Assembler
// Date: 6/22/97
// Revision History:
//		0.0:	Initial Revision
//		0.1:	(7/27/1997) Changed to use a string list instead of 
//				reading	the file in twice.
//

#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "ManoAssembler.hpp"
#include "StringList.hpp"

using namespace std;

int main( int argc, const char *argv[] ) {

	char banner[] = "Mano Assembler (c) 1997 Rochester Institute " \
			"of Technology.\nProgrammed by Mark Roth\n" \
			"Based on textbook by Mano, M. M.,\nComputer " \
			"System Architecture, 3rd ed. Englewood Cliffs, " \
			"NJ: Prentice Hall, 1993\n";

	char syntax[] = "Syntax: manoasm <infile> <outfile> [-v|-c|-n]\n";

	const char *infile, *outfile;
	
	// Output banner:
	cout << banner << endl;
	
	// Check command-line arguments:
	if( argc < 3 || argc > 4 ) {
		cout << syntax << endl;
		return 1;
	}

	// Retrieve infile and outfile parameters:
	infile = argv[1];
	outfile = argv[2];

	// Open the files, and assemble away!
	CManoAssembler::format format;
	if (argc == 4 && argv[3][0] == '-')
	{
		switch (argv[3][1])
		{
		case 'v':
			format = CManoAssembler::verilog;
			break;
		case 'c':
			format = CManoAssembler::coe;
			break;
		case 'n':
		default:
			format = CManoAssembler::normal;
			break;
		}
	}
	else
		format = CManoAssembler::normal;
	CManoAssembler assembler(format);
	CStringList output_list;

	try {
		assembler.AssembleFromFile( infile, output_list );
	}
	catch( CErrorException e ) {
		e.Display( cerr );
	}

	// Write the output list to the output file:
	ofstream out_stream( outfile );

	if( !out_stream.is_open() ) {
		CErrorException e( "", 0, "A0002", 
			"Could not open output file for writing", 
			CErrorException::FATAL );

		e.Display( cerr );
		return 1;
	}


	output_list.Dump( out_stream );

	return 0;
}

