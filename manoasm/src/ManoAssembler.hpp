// File:	ManoAssembler.hpp
// Author:	Mark Roth
// Description:
//		Mano 2-pass assembler class.  Keeps track of a symbol table,
//		and translates each line into a Mano Simulator instruction.
// Usage:	
//		1. create one instance of this class
//		2. send code one line at a time through the ParseSymbolic 
//		   method
//		3. Call ResetLocationCounter method
//		4. for the second pass, send each line of code into the Assemble
//		   method, and retrieve it's return value, which is a single
//		   simulator instruction to store a value at a specified
//		   address.
//		5. if assembling more than one file, call the Reset method.
//
// Alternative Usage:
//
//		1. create one instance of this class
//		2. Call the AssembleFromFile function, and pass it a file, or
//		   "con" to assemble from DOS input, or "stdin" to assemble
//		   from UNIX standard input.  Pass in a CStringList object to
//		   hold the output of the compilation.
//
// Notes:
//
//
//		At any point in the assembly process, the CManoAssembler 
//		object may throw a CErrorException which will contain an error 
//		message and severity.  If the severity is a warning only, the 
//		assembly can continue.  If the severity is an error, continue 
//		the assembly, but don't execute the resulting instructions 
//		because they will be incomplete.  If the severity is fatal, 
//		immediately abort the assembly.
//
// Revision History:
//		0.0:	Initial Revision
//

#pragma once

#include "StringList.hpp"
#include "SymbolTable.hpp"
#include "ErrorException.hpp"
#include "Instruction.hpp"

#include <iostream>
#include <strstream>

// Number of valid instructions and pseudo-instructions:
#define NUM_VALID_INSTRUCTIONS	29

class CManoAssembler {
public:
	enum format
	{
		normal,
		verilog,
		coe
	} m_format;

public:	// Construction / Destruction

	//
	// Name:		(constructor)
	//
	// Description:	Constructs a CManoAssembler object
	// Modifies:	Resets m_location_counter to 0
	//				Resets m_symbol_table
	// 
	CManoAssembler(format setformat);

	//
	// Name:		(destructor)
	//
	// Description:	Destroys a CManoAssembler object.
	// 
	~CManoAssembler();

public:	// Initialization

	//
	// Name:		Reset
	//
	// Description:	Resets the location counter and the symbol table.
	// Modifies:	m_location_counter, m_symbol_table
	//
	void Reset();

public: // Automated Assembly from file:

	//
	// Name:		AssembleFromFile
	//
	// Description:	Assembles from a file to a CStringList.  The CStringList
	//		will not be cleared, but rather appended to.
	// Arguments:	The file to read
	//		a CStringList to store the assembled information to
	//		the stream to send status messages to (defaults to cout)
	//		the stream to send error messages to (defaults to cerr).
	// Exceptions:	A CErrorException is thrown if a fatal error occurs.
	//
	void AssembleFromFile( const char *filename, CStringList &output_list,
		std::ostream &status_stream = std::cout, std::ostream &error_stream = std::cerr );

public:	// Parsing / Assembly

	//
	// Name:		ParseSymbolic
	//
	// Description:	Parses the symbolic information from a line of
	//		Mano assembly code, and adds any new symbols to the
	//		symbol table.  Also checks for 
	// Arguments:	The line of code to parse for symbolic information
	// Exceptions:	Throws a CErrorException if the line contains a
	//		syntax error, or a symbol that is already defined.
	//
	void ParseSymbolic( const char *line );

	//
	// Name:		ResetLocationCounter
	//
	// Description:	Resets the location counter and the end flag after 
	//		pass 1, and prepares the assembler for pass 2
	// Modifies:	m_location_counter, m_end_encountered
	//
	void ResetLocationCounter();

	//
	// Name:		Assemble
	//
	// Description:	Assembles a single line of code into a 16-bit
	//		value, and forms a Mano Simulator instruction to store
	//		that value to the appropriate location in memory.
	// Arguments:	The line of code to assemble
	// Exceptions:	Throws a CErrorException if the line contains a
	//		symbol that is not defined.
	//
	char *Assemble( const char *line );

	//
	// Name:	SetLineNumber
	//
	// Description:	Updates the assembler's internal line number counter
	//		so that it can provide more detailed line number
	//		information in error messages.
	// Arguments:	The current line number
	// Modifies:	m_linenumber;
	//
	void SetLineNumber( int number );

	//
	// Name:		SetFileName
	//
	// Description:	Sets the name of the file we are working with
	//		for more detailed error messages
	// Arguments:	The file name for the file that is being assembled
	// Modifies:	m_file_name;
	//
	void SetFilename( const char *filename );

	//
	// Name:		EndEncountered
	//
	// Returns:		1 if the keyword "END" was encountered.  
	//			Else, 0.
	//
	bool EndEncountered() const;

	//
	// Name:		OrgEncountered
	//
	// Returns:		1 if the keyword "ORG" was encountered.  
	//			Else, 0.
	//
	bool OrgEncountered() const;

public:		// Disassembly

	//
	// Name:		Disassemble
	//
	// Description:	Disassembles a single word of memory.
	// Arguments:	The word of memory to disassemble, and a buffer
	//		to store the disassembled word to.  The buffer
	//		must be at least 80 characters wide.
	//
	void Disassemble( unsigned short int word, char *buffer );

public:		// Output

	//
	// Name:		DumpSymbolTable
	//
	// Description:	Dumps the symbol table to the given stream
	// Arguments:	The output stream to display the symbol table to
	//
	void DumpSymbolTable( std::ostream &out );

protected: // Utility functions

	//
	// Name:		UpdateLocationCounter
	//
	// Description:	Updates the internal location counter based on the type
	//		of instruction just assembled.
	// Arguments:	The name of the instruction assembled, and the operand
	//		to that instruction
	// Modifies:	m_location_counter
	//
	void UpdateLocationCounter( const char *instruction, const char *operand );

	//
	// Name:		Parse
	//
	// Description:	Parses a line into it's label, instruction, 
	//		argument, and indirect flag.
	// Arguments:	The line to parse, pointers to the strings
	//		to receive the label and instruction (both no
	//		more than 3 characters), a pointer to store
	//		the argument (up to 10 characters), and a pointer
	//		to receive the indirect flat (up to 1 character)
	// Exceptions:	Throws a CErrorException if there are parsing errors
	//
	void Parse( const char *line, char *label, char *instruction, 
		char *argument, char *indirect );

	//
	// Name:		SearchAndParseLabel
	//
	// Description:	Searches a stream for a label, and if found, fills
	//		the label pointer with the label.  Also performs
	//		syntax checking on whatever appears before the comma
	// Arguments:	A pointer to the string to store the label into
	//		A reference to the stream to read for label data
	// Exceptions:	Throws a CErrorException if there are parsing errors
	//
	void SearchAndParseLabel( char *label, std::istrstream &input );

	//
	// Name:	ParseInstruction
	//
	// Description:	Searches a stream for an instruction, and fills
	//		the instruction pointer with the instruction.  Also
	//		performs syntax checking on the instruction.
	// Arguments:	A pointer to the string to store the instruction into
	//		A reference to the stream to read for instruction data
	// Exceptions:	Throws a CErrorException if there are parsing errors
	//
	void ParseInstruction( char *instruction, std::istrstream &input );

	//
	// Name:	ParseOperand
	//
	// Description:	Searches a stream for an operand, and fills
	//		the operand pointer with the operand.  
	// Arguments:	A pointer to the string to store the operand into
	//		A reference to the stream to read for operand data
	// Exceptions:	Throws a CErrorException if there are parsing errors
	//
	void ParseOperand( char *operand, std::istrstream &input );

	//
	// Name:		ParseIndirect
	//
	// Description:	Searches a stream for an indirect bit, and fills
	//		the indirect pointer with the operand.
	// Arguments:	A pointer to the string to store the indirect bit into
	//		A reference to the stream to read for indirect bit data
	// Exceptions:	Throws a CErrorException if there are parsing errors
	//
	void ParseIndirect( char *indirect, std::istrstream &input );

	//
	// Name:	IsValidIdentifier
	//
	// Description:	Determines if a given string is a valid identifier.
	//		An identifier is valid if all of these hold true:
	//			1. It is 3 characters or less
	//			2. The first character is alphabetic [A-Z]
	//			3. All remaining characters are 
	//			   alphanumeric [A-Z0-9]
	// Arguments:	The identifier to examine
	// Returns:	1 if the identifier is valid, or 0 otherwise.
	//
	static bool IsValidIdentifier( char *identifier );

	//
	// Name:	IsNumeric
	//
	// Description:	Determines if a given string is numeric
	//		A string is numeric if all of these hold true:
	//		    1. The first character is -, or [0-9]
	//		    2. The remaining characters are [0-9]
	// Arguments:	The string to examine
	// Returns:	1 if the string is numeric, or 0 otherwise.
	//
	static bool IsNumeric( const char *str );

	//
	// Name:	IsHexNumeric
	//
	// Description:	Determines if a given string is hexnumeric
	//		A string is hexnumeric if all of these hold true:
	//		    1. The first character is -, or [0-9][A-F]
	//		    2. The remaining characters are [0-9][A-F]
	// Arguments:	The string to examine
	// Returns:	1 if the string is numeric, or 0 otherwise.
	//
	static bool IsHexNumeric( const char *str );

	//
	// Name:	GetInstructionInfo
	//
	// Description:	Obtains information about a given instruction
	// Arguments:	The name of the instruction, and a pointer to where to
	//		store the instruction information
	// Returns:	1 if the instruction exists, or else 0.
	//
	bool GetInstructionInfo( const char *name, SInstruction *info ) const;

	//
	// Name		HexToInteger
	//
	// Description:	Converts a hexadecimal string to an integer
	// Arguments:	The string to convert
	// Returns:	An unsigned long value, that is the integer that
	//		hex string represents.
	//
	static unsigned short HexToInteger( const char *str );

	//
	// Name		IntegerToHex
	//
	// Description:	Converts a integer to a hexadecimal string
	// Arguments:	The integer to convert, and a string to store the
	//		result in.  The result will be 4 characters long
	//
	static void IntegerToHex( unsigned long integer, char *hex_data );

protected: // Attributes

	int				m_location_counter;	// Location counter (LC)

	bool			m_org_encountered;	// ORG encountered flag

	bool			m_end_encountered;	// END encountered flag

	CSymbolTable	*m_symbol_table; 	// Symbol Table

	char			m_instruction[80];	// Most recently assembled 
										// result

	int				m_line_number;		// The current line number

	const char		*m_file_name;		// The file we are currently 
										// assembling

	unsigned short m_data[4096];
	unsigned m_last_valid_address;

	// List of available instructions and info:
	static SInstruction m_instruction_list[NUM_VALID_INSTRUCTIONS]; 
};
