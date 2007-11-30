// File:	ManoAssembler.cpp
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
//		At any point in the assembly process, the CManoAssembler object 
//		may throw a CErrorException which will contain an error message 
//		and severity.  If the severity is a warning only, the assembly 
//		can continue.  If the severity is an error, continue the 
//		assembly, but don't execute the resulting instructions because 
//		they will be incomplete.  If the severity is fatal, 
//		immediately abort the assembly.
//
// Revision History:
//		0.0:	Initial Revision
//

#include "ManoAssembler.hpp"

#include <cstdlib>
#include <cstring>
#include <iomanip>

using namespace std;

//
// Name:	(constructor)
//
CManoAssembler::CManoAssembler(format setformat) : m_format(setformat) {
	m_file_name = 0;
	m_line_number = 0;
	m_symbol_table = 0;

	if (m_format == coe)
	{
		for (int i = 0; i < 4096; i++)
			m_data[i] = 0;
		m_last_valid_address = 0;
	}

	// Reset the assembler
	Reset();
	
} // (constructor)

//
// Name:	(destructor)
//
CManoAssembler::~CManoAssembler() {
	
	if( m_symbol_table ) {
		delete m_symbol_table;
	}
	
} // (destructor)

//
// Name:	Reset
//
void CManoAssembler::Reset() {
	
	// If the symbol table does not yet exist, create it:
	if( !m_symbol_table ) {
		m_symbol_table = new CSymbolTable();
	}
	else
	{
		// Initialize the location counter and symbol table:
		m_symbol_table->Reset();
	}

	// Reset the location counter and flags:
	ResetLocationCounter();
	
} // Reset

//
// Name:	AssembleFromFile
//
void CManoAssembler::AssembleFromFile(
	const char *filename, 
	CStringList &output_list,
	ostream &status_stream,
	ostream &error_stream ) 
{

	char line[90];
	unsigned int i;
	int errors = 0, warnings = 0;

	// Read the input file into a string list:
	CStringList string_list;
	try {
		string_list.ReadFromFile( filename );
	}
	catch( CErrorException error ) {
		error.Display( error_stream );
		throw CErrorException( filename, 0, "A0001", 
			"Could not open input file", CErrorException::FATAL );
	}

	SetFilename( filename );

	// Pass 1: Send all lines of input to the assembler for symbol
	// parsing

	status_stream << "Assembling Pass 1..." << endl;

	const size_t num_lines = string_list.size();

	for (unsigned line_number = 0;
		(line_number < num_lines) && !EndEncountered();
		line_number++) {

		// Copy the line from the string list:
		line[81] = 0;
		strncpy( line, string_list[line_number].c_str(), 81 );

		if( strlen( line ) > 80 ) {
			throw CErrorException( filename, line_number, "A0004", 
				"Line longer than 80 characters encountered", 
				CErrorException::FATAL );
		}

		// Convert the line to uppercase:
		_strupr( line );

		// Try assembling this line:
		try {
			SetLineNumber( line_number );
			ParseSymbolic( line );
		}
		catch( CErrorException e ) {
			e.Display( error_stream );
			switch( e.GetSeverity() ) {
			case CErrorException::FATAL:
				throw;
				break;
			case CErrorException::ERROR:
				errors++;
				break;
			case CErrorException::WARNING:
				warnings++;
				break;
			}
		}
	}

	// If there are errors, abort the assembly process:
	if( errors > 0 ) {
		status_stream << "Assembly aborted - " << errors 
			<< " error(s), " << warnings << " warning(s)" << endl;
		return;
	}

	// Pass 2: Send all lines of input to the assembler again, for
	// assembly

	status_stream << "Assembling Pass 2..." << endl;

	// Ready the assembler:
	ResetLocationCounter();

	for (unsigned line_number = 0;
		(line_number < num_lines) && !EndEncountered();
		line_number++) {
		const char *result;

		// Copy the line from the string list:
		line[81] = 0;
		strncpy( line, string_list[line_number].c_str(), 81 );

		if( strlen( line ) > 80 ) {
			throw CErrorException( filename, line_number, "A0004", 
				"Line longer than 80 characters encountered", 
				CErrorException::FATAL );
		}

		// Convert the line to uppercase:
		for( i=0; i < strlen(line); i++ ) {
			line[i] = (char)toupper( line[i] );
		}

		try {
			SetLineNumber( line_number );
			result = Assemble( line );

			// If the line wasn't blank, output this instruction
			if( result && m_format != coe ) {
				output_list.push_back( result );
			}
		}
		catch( CErrorException e ) {
			e.Display( error_stream );
			switch( e.GetSeverity() ) {
			case CErrorException::FATAL:
				throw;
				break;
			case CErrorException::ERROR:
				errors++;
				break;
			case CErrorException::WARNING:
				warnings++;
				break;
			}
		}
	}

	// If there are errors, abort the assembly process:
	if( errors > 0 ) {
		status_stream << "Assembly aborted - " << errors 
			<< " error(s), " << warnings << " warning(s)" << endl;

		return;
	}

	// Display Symbol table:
	status_stream << endl
		 << "Symbol Table" << endl
		 << "------------" << endl;

	DumpSymbolTable( status_stream );

	status_stream << endl;

	// Display success message:

	status_stream << "Assembly successful - " << errors << " error(s), "
		 << warnings << " warning(s)" << endl;

	if (m_format == coe)
	{
		ostrstream ts;
		ts << "memory_initialization_radix=16;\n"
			"memory_initialization_vector=\n";
		for (int y = 0; y < 4096/8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				ts << setw(4) << setfill('0') << hex << m_data[x+8*y];
				ts << setw(1);
				if (x + 8*y == m_last_valid_address)
				{
					ts << ";\n";
					goto done;
				}
				else
					ts << ',';
			}
			ts << '\n';
		}
		done:
		ts << '\0';
		output_list.push_back(ts.str());
	}
} // AssembleFromFile

//
// Name:	ParseSymbolic
//
void CManoAssembler::ParseSymbolic( const char *line ) {
	char label[4];
	char instruction[4];
	char argument[11];
	char indirect[2];
	
	////////////////////////////////////////////////////////////////////////
	// PARSE THE LINE INTO ITS COMPONENTS
	////////////////////////////////////////////////////////////////////////
	Parse( line, label, instruction, argument, indirect );

	////////////////////////////////////////////////////////////////////////
	// PROCESS THE LABEL
	////////////////////////////////////////////////////////////////////////

	// Process the label only if it exists, that is:
	if( strcmp( label, "" ) != 0 ) {
		// Check to see if this symbol already exists:
		if( m_symbol_table->GetAddress( label ) != -1 ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A2000", "Duplicate symbol encountered", 
				CErrorException::ERROR );
		}

		// Add the label and its associated address to the symbol 
		// table:
		m_symbol_table->AddSymbol( label, m_location_counter );
	}

	////////////////////////////////////////////////////////////////////////
	// UPDATE THE LOCATION COUNTER
	////////////////////////////////////////////////////////////////////////
	UpdateLocationCounter( instruction, argument );

	// If we encountered the end, set the flag as such.
	if( strcmp( instruction, "END" ) == 0 ) {
		m_end_encountered = 1;
	}

	// If we encountered an org, set the flag as such.
	if( strcmp( instruction, "ORG" ) == 0 ) {
		m_org_encountered = 1;
	}

} // ParseSymbolic

//
// Name:	ResetLocationCounter
//
void CManoAssembler::ResetLocationCounter() {

	m_location_counter = 0;
	m_end_encountered = 0;
	m_org_encountered = 0;

} // ResetLocationCounter


//
// Name:	Assemble
//
char *CManoAssembler::Assemble( const char *line ) {
	char label[4];
	char instruction[4];
	char argument[11];
	char indirect[2];

	char buffer[81];
	char hex_data[16];

	SInstruction info;

	// Reset the output and create a stream:
	strcpy( m_instruction, "" );
	
	////////////////////////////////////////////////////////////////////////
	// PARSE THE LINE INTO ITS COMPONENTS
	////////////////////////////////////////////////////////////////////////
	Parse( line, label, instruction, argument, indirect );

	// If we have no instruction, get the next line.
	if( strcmp( instruction, "" ) == 0 ) {
		return 0;
	}

	// Do some research on our instruction:
	GetInstructionInfo( instruction, &info );

	////////////////////////////////////////////////////////////////////////
	// RESOLVE REFERENCES
	////////////////////////////////////////////////////////////////////////
	
	// If this operation is marked for resolving references, do so:
	if( info.resolve_references ) {
		if( strcmp( argument, "" ) != 0 ) {
			// Replace symbolic variable it with it's numeric 
			// equivalent:
			int address;

			address = m_symbol_table->GetAddress( argument );

			// If the address is not found, yell at the user 
			// a little:
			if( address == -1 )
			{
				if ( !IsHexNumeric(argument) )
				{
					throw CErrorException( m_file_name, 
						m_line_number, "A2001", 
						"Undeclared symbol encountered", 
						CErrorException::ERROR );
				}
			}
			else
			{
				// Replace the argument string with the numeric value
				IntegerToHex( address, argument );
			}
		}
	}

	////////////////////////////////////////////////////////////////////////
	// CONSTRUCT INSTRUCTION HEX DATA
	////////////////////////////////////////////////////////////////////////
	unsigned short int data;

	// Start with 0:
	data = 0;

	// Start with the opcode and shift to appropriate position:
	data = info.opcode;
	data = data << 12;

	// Add in our indirect bit:
	if( strcmp( indirect, "I" ) == 0 ) {
		data += 0x8000;
	}

	// Add in operand:
	data += info.operand;

	// Add in user operand, if it exists:
	if( strcmp( argument, "" ) != 0 ) {
		unsigned short user_operand;

		// If it's DEC, read it in decimal, else in hexadecimal
		if( strcmp( instruction, "DEC" ) == 0 ) {
			user_operand = (unsigned short)atol( argument );
		}
		else {
			user_operand = HexToInteger( argument );
		}

		data += user_operand;
	}

	// Convert to hex string:
	IntegerToHex( data, hex_data );

	////////////////////////////////////////////////////////////////////////
	// CONSTRUCT OUTPUT
	////////////////////////////////////////////////////////////////////////
	if( strcmp( instruction, "" ) != 0 ) {
		IntegerToHex( m_location_counter, buffer );
		switch (m_format)
		{
		case normal:
			// Command m = "Modify memory"
			strcat( m_instruction, "m\t" );

			// output address to modify:
			strcat( m_instruction, buffer );
			strcat( m_instruction, "\t" );

			// output what to store at that address:
			strcat( m_instruction, hex_data );

			// output a comment indicating the line of code
			strcat( m_instruction, "\t/ " );
			break;
		case verilog:
			// Output to a format suitable for Verilog $readmemh
			strcat( m_instruction, "@");
			strcat( m_instruction, buffer);
			strcat( m_instruction, "\t");
			strcat( m_instruction, hex_data);
			strcat( m_instruction, "\t// ");
			break;
		case coe:
			m_data[m_location_counter] = data;
			if (m_last_valid_address < m_location_counter)
				m_last_valid_address = m_location_counter;
			break;
		}
		strcat( m_instruction, instruction );
		strcat( m_instruction, " " );
		strcat( m_instruction, argument );
		strcat( m_instruction, " " );
		strcat( m_instruction, indirect );
	}
	
	////////////////////////////////////////////////////////////////////////
	// UPDATE THE LOCATION COUNTER
	////////////////////////////////////////////////////////////////////////
	try {
		UpdateLocationCounter( instruction, argument );
	}
	catch( CErrorException error ) {
		// Ignore any errors from updating the location counter.  
		// The user has already seen them in pass 1, and they are 
		// all warnings anyway.
	}

	// If we just tried to assemble an ORG or an END, don't output anything
	if( strcmp( instruction, "ORG" ) == 0) {
		m_org_encountered = 1;
		return 0;
	}

	if( strcmp( instruction, "END" ) == 0 ) {
		m_end_encountered = 1;
		return 0;
	}

	return m_instruction;
} // Assemble

//
// Name:	UpdateLocationCounter
//
void CManoAssembler::UpdateLocationCounter( const char *instruction, 
	const char *operand ) 
{

	// If we did process an instruction, we must modify the location counter
	if( strcmp( instruction, "" ) != 0 ) {
		// if it was an ORG instruction, use the operand as a hex value:
		if( strcmp( instruction, "ORG" ) == 0 ) {
			m_location_counter = HexToInteger(operand);
		}
		// if it was any other instruction, increase by 1
		else {
			m_location_counter++;

			// If we haven't encountered an ORG yet, warn the user:
			if( !m_org_encountered ) {
				m_org_encountered = 1;
				throw CErrorException( m_file_name, 
					m_line_number, "A3000", 
					"ORG not encountered.  Assuming "
					"000 as origin.", 
					CErrorException::WARNING );
			}
		}
	}

	// Check to make sure the location counter is within memory range:
	if( m_location_counter > 0x1000 ) {
		throw CErrorException( m_file_name, m_line_number, "A0005", 
			"Attempt to assemble past address FFF", 
			CErrorException::FATAL );
	}	
	
} // UpdateLocationCounter

//
// Name:	Parse
//
void CManoAssembler::Parse( const char *line, char *label, 
			    char *instruction, char *argument, 
			    char *indirect )
{
	char local[81];
	SInstruction instruction_info;

	// Make a local copy of the line that we can manipulate.
	strncpy( local, line, 80 );

	// Find the comment, if it exists, and strip it off.
	char *p;
	p = strchr( local, '/' );
	if( p != NULL ) {
		*p = 0;
	}
	

	// Null out all fields:
	strcpy( label, "" );
	strcpy( instruction, "" );
	strcpy( argument, "" );
	strcpy( indirect, "" );
	
	// Construct an input stream, and eat any white space:
	istrstream input( local, 78 );
	input >> ws;

	// If we hit the terminating character, this line is blank.  Skip it.
	if( !input.peek() ) {
		return;
	}

	////////////////////////////////////////////////////////////////////////
	// SEARCH FOR AND PARSE LABEL
	////////////////////////////////////////////////////////////////////////
	if( strchr( local, ',' ) != 0 ) {
		SearchAndParseLabel( label, input );
	}
	input >> ws;

	// If we hit the terminating character, there was a label with no line:
	if( !input.peek() ) {
		throw CErrorException( m_file_name, m_line_number, "A1005", 
			"Instruction Expected", CErrorException::ERROR );
	}

	////////////////////////////////////////////////////////////////////////
	// PARSE INSTRUCTION
	////////////////////////////////////////////////////////////////////////
	ParseInstruction( instruction, input );
	input >> ws;

	// Get additional information about this instruction:
	GetInstructionInfo( instruction, &instruction_info );

	// If we have an ORG or END instruction, make sure there is no label:
	if( (strcmp( instruction, "ORG" ) == 0) || 
		(strcmp( instruction, "END" ) == 0) ) 
	{
		if( strcmp( label, "" ) != 0 ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1014", 
				"Label invalid for this instruction", 
				CErrorException::ERROR );
		}
	}

	// If we hit the terminating character, there are no operands:
	if( !input.peek() ) {

		// If we don't have an operand, and we should, speak up!
		if( instruction_info.operand == 0x000 ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1012", 
				"Operand expected", CErrorException::ERROR );
		}

		return;
	}

	///////////////////////////////////////////////////////////////////////
	// PARSE OPERAND
	///////////////////////////////////////////////////////////////////////
	ParseOperand( argument, input );
	input >> ws;

	// If we have an operand and we shouldn't, speak up!:
	if( strcmp( argument, "" ) != 0 ) {
		if( instruction_info.operand != 0x000 ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1008", 
				"Operand invalid for this instruction", 
				CErrorException::ERROR );
		}
	}

	// If the instruction is ORG, the operand must be numeric and positive:
	if( strcmp( instruction, "ORG" ) == 0 ) {
		if( (!IsHexNumeric( argument )) || (atoi( argument ) < 0) ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1009", 
				"Operand must be a positive numeric for ORG", 
				CErrorException::ERROR );
		}

		// Check to make sure this value is at most 12-bits wide:
		long value = HexToInteger( argument );
		if( value > 0xFFF ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1013", 
				"Addresses must be between 000 and FFF", 
				CErrorException::ERROR );
		}

	}

	// If the instruction is DEC, make sure the operand is numeric
	if( strcmp( instruction, "DEC" ) == 0 ) {
		if( !IsNumeric( argument ) ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1010", 
				"Operand must be a numeric for DEC", 
				CErrorException::ERROR );
		}
	}

/*	Labels in HEX directives are just fine, thanks.

// If the instruction is HEX, make sure the operand is numeric
	if( strcmp( instruction, "HEX" ) == 0 ) {
		if( !IsHexNumeric( argument ) ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1011", 
				"Operand must be a hex numeric for HEX", 
				CErrorException::ERROR );
		}
	}
*/
	// If we hit the terminating character, there is no indirect bit:
	if( !input.peek() ) {
		return;
	}

	///////////////////////////////////////////////////////////////////////
	// PARSE INDIRECT BIT
	///////////////////////////////////////////////////////////////////////
	ParseIndirect( indirect, input );
	input >> ws;

	// If we encountered the indirect bit when we were not supposed 
	// to, say so
	if( strcmp( indirect, "I" ) == 0 ) {
		if( !instruction_info.i_bit_valid ) {
			throw CErrorException( m_file_name, m_line_number, 
				"A1007", 
				"Indirect bit invalid for this instruction", 
				CErrorException::ERROR );
		}
	}

	// If we don't hit the terminating character, there's too much data 
	// on the line:
	if( input.peek() ) {
		throw CErrorException( m_file_name, m_line_number, "A1006", 
			"Invalid text after indirect bit", 
			CErrorException::ERROR );
	}

} // Parse

//
// Name:	SetLineNumber
//
void CManoAssembler::SetLineNumber( int number ) {
	
	m_line_number = number;
	
} // SetLineNumber

//
// Name:	SetFileName
//
void CManoAssembler::SetFilename( const char *filename ) {
	
	m_file_name = filename;
	
} // SetLineNumber

//
// Name:	EndEncountered
//
bool CManoAssembler::EndEncountered() const {

	return m_end_encountered;

} // EndEncountered

//
// Name:	OrgEncountered
//
bool CManoAssembler::OrgEncountered() const {

	return m_org_encountered;

} // OrgEncountered

//
// Name:	Disassemble
//
void CManoAssembler::Disassemble( unsigned short int word, char *buffer ) {

	// Reset the output buffer:
	strncpy( buffer, "", 80 );

	SInstruction current_instruction;
	ostrstream buffer_stream( buffer, 79 );

	// Break up the word into indirect bit, opcode, and address
	const bool indirect = (word & 0x8000) == 0x8000;
	const unsigned short opcode = (word & 0xF000) >> 12;
	const unsigned short address = (word & 0x0FFF);

	// Copy the instruction(s) to the buffer, from the opcode:
	
	// Search through the available list of instructions for matching 
	// opcodes
	bool found = false;
	for( int i=0; i < NUM_VALID_INSTRUCTIONS; i++ ) {
		unsigned short int compare_opcode;
		current_instruction = m_instruction_list[i];

		// If the indirect bit is valid, we will compare only the 
		// low-order three bits of the opcode.  Otherwise, compare 
		// the entire opcode:
		if( current_instruction.i_bit_valid ) {
			compare_opcode = opcode & 7;
		}
		else {
			compare_opcode = opcode;
		}

		// If the opcodes match:
		if( current_instruction.opcode == compare_opcode ) {
			// If resolve_references is set, that means the 
			// address actually stands for an address.  If not, 
			// it is part of the instruction(s).
			if( current_instruction.resolve_references ) {
				// There can be no more instructions, so 
				// drop out:
				buffer_stream << current_instruction.name 
					<< " ";
				found = true;
				break;
			} 
			else {
				// We must match operands as well:
				if( address & current_instruction.operand ) {
					// Add this to the instruction list, 
					// but keep processing since there 
					// may be other instructions 
					// that match.
					buffer_stream
						<< current_instruction.name 
						<< " ";
					found = true;
				}
			}
		}
	}

	// If no instructions matched:
	if( !found ) {
		buffer_stream << "RESERVED (" << hex << setw(4)
			<< setfill( '0' ) << word << ")";
		current_instruction.resolve_references = 0;
		current_instruction.i_bit_valid = 0;
	}

	// Append the address, if applicable:
	if( current_instruction.resolve_references ) {
		buffer_stream << hex << setw(3) << setfill( '0' ) 
			<< address << " ";
	}

	// If the indirect bit is set, append "I" to the instruction:
	if( current_instruction.i_bit_valid && indirect ) {
		buffer_stream << "I";
	}

} // Disassemble

//
// Name:	DumpSymbolTable
//
void CManoAssembler::DumpSymbolTable( ostream &out ) {

	m_symbol_table->Dump( out );

} // DumpSymbolTable

//
// Name:	SearchAndParseLabel
//
void CManoAssembler::SearchAndParseLabel( char *label, istrstream &input ) {
	char word[81];
	char buffer[81];

	// Use getline with a ',' delimiter to get all words before comma
	input.getline( word, 78, ',' );
	
	// Eat any white space
	input >> ws;
	
	// Create a temporary input stream so we can strip spaces:
	istrstream lstream( word, 80 );
	lstream >> buffer;
	
	// Copy buffer into label, up to 3 characters:
	strncpy( label, buffer, 4 );
	
	// Do some syntax checking on the label:
	if( !IsValidIdentifier( label ) ) {
		throw CErrorException( m_file_name, m_line_number, "A1001",
			"Label is an invalid identifier", 
			CErrorException::ERROR );
	}
	
	// Ensure that there were no other words before the comma:
	lstream >> ws;

	lstream.getline( buffer, 80, 0 );
	if( *buffer ) {
		throw CErrorException( m_file_name, m_line_number, "A1002", 
			"Unexpected extra argument before comma", 
			CErrorException::ERROR );
	}

} // SearchAndParseLabel

//
// Name:	ParseInstruction
//
void CManoAssembler::ParseInstruction( char *instruction, istrstream &input ) {
	char buffer[81];

	// The stream will now contain the instruction.
	input >> buffer;

	// Do some syntax checking on the instruction:
	if( strlen( buffer ) > 3 ) {
		throw CErrorException( m_file_name, m_line_number, "A1003",
			"Invalid instruction encountered", 
			CErrorException::ERROR );
	}

	// Copy buffer to the instruction:
	strncpy( instruction, buffer, 4 );

	// Verify instruction is valid:
	SInstruction info;
	if( !GetInstructionInfo( instruction, &info ) ) {
		throw CErrorException( m_file_name, m_line_number, "A1003",
			"Invalid instruction encountered", 
			CErrorException::ERROR );
	}

} // ParseInstruction

//
// Name:	ParseOperand
//
void CManoAssembler::ParseOperand( char *operand, istrstream &input ) {
	char buffer[81];

	// The stream will now contain the operand.
	input >> buffer;
	
	// Copy buffer to the operand:
	strncpy( operand, buffer, 11 );

	// We cannot perform any checking on the operand besides determining
	// if it is a label or a number, and that will be done in pass 2.
	// anyway.

} // ParseOperand

//
// Name:	ParseIndirect
//
void CManoAssembler::ParseIndirect( char *indirect, istrstream &input ) {
	char buffer[81];

	// The stream will now contain the indirect bit.
	input >> buffer;
	
	// Copy buffer to the indirect bit pointer:
	strncpy( indirect, buffer, 2 );

	// Make sure the indirect string is "I" and nothing else:
	if( strcmp( indirect, "I" ) != 0 ) {
		throw CErrorException( m_file_name, m_line_number, "A1004",
			"Unexpected text in Indirect Column", 
			CErrorException::ERROR );
	}

} // ParseIndirect

//
// Name:	IsValidIdentifier
//
bool CManoAssembler::IsValidIdentifier( char *identifier ) {

	// If the identifier is more than 3 characters, it is invalid:
	if( strlen( identifier ) > 3 ) {
		return false;
	}

	// If the first character is not alphabetic, it is invalid:
	if( (identifier[0] < 'A') || (identifier[0] > 'Z') ) {
		return false;
	}

	// If the remaining characters are not alphanumeric, it is invalid:

	for( unsigned int index = 1; index < strlen( identifier ); index++ ) {
		if( !(((identifier[index] >= 'A') 
			&& (identifier[index] <= 'Z')) 
			|| ((identifier[index] >= '0') 
			&& (identifier[index] <= '9'))) ) 
		{
			return false;
		}
	}

	return true;

} // IsValidIdentifier

//
// Name:	IsNumeric
//
bool CManoAssembler::IsNumeric( const char *str ) {

	// If the first character is not numeric or -, it is invalid:
	if( (str[0] != '-') && 
		((str[0] < '0') || (str[0] > '9')) ) 
	{
		return false;
	}

	// If the remaining characters are not numeric, it is invalid:

	for( unsigned int index = 1; index < strlen( str ); index++ )
	{
		if( !( (str[index] >= '0') && (str[index] <= '9') ) )
			return false;
	}

	return true;

} // IsNumeric

//
// Name:	IsHexNumeric
//
bool CManoAssembler::IsHexNumeric( const char *str ) {

	// If the first character is not numeric or -, it is invalid:
	if( (str[0] != '-') && 
		((str[0] < '0') || (str[0] > '9')) &&
		((str[0] < 'A') || (str[0] > 'F')) ) 
	{
		return false;
	}

	// If the remaining characters are not numeric, it is invalid:

	for( unsigned int index = 1; index < strlen( str ); index++ ) {
		if( ((str[index] < '0') || (str[index] > '9')) &&
			((str[index] < 'A') || (str[index] > 'F')) )
			return false;
	}

	return true;

} // IsHexNumeric

//
// Name:	GetInstructionInfo
//
bool CManoAssembler::GetInstructionInfo( const char *name, SInstruction *info ) const {

	bool valid = false;

	// Get an index into the array of valid instructions:
	unsigned int index;
	for( index = 0; index < NUM_VALID_INSTRUCTIONS; index++ ) {
		if( strcmp( m_instruction_list[index].name, name ) == 0 ) {
			valid = true; 
			break;
		}
	}

	// If it was not found, return 0 to indicate an invalid instruction:
	if( !valid ) {
		return false;
	}

	// Assign all members of instruction to the correct instruction:
	*info = m_instruction_list[index];

	// This was an invalid instruction:
	return true;

} // GetInstructionInfo


//
// Name:	m_instruction_list
//
SInstruction CManoAssembler::m_instruction_list[NUM_VALID_INSTRUCTIONS] = {
	// { name, resolve_references, i_bit_valid, opcode, operand }
	
	// Memory-reference instructions (note these opcodes must be increased
	// by 0x8 if the i bit is set)
	{ "AND", 1, 1, 0x0, 0x000 },
	{ "ADD", 1, 1, 0x1, 0x000 }, 
	{ "LDA", 1, 1, 0x2, 0x000 }, 
	{ "STA", 1, 1, 0x3, 0x000 }, 
	{ "BUN", 1, 1, 0x4, 0x000 }, 
	{ "BSA", 1, 1, 0x5, 0x000 }, 
	{ "ISZ", 1, 1, 0x6, 0x000 }, 

	// Register-reference instructions:
	{ "CLA", 0, 0, 0x7, 0x800 }, 
	{ "CLE", 0, 0, 0x7, 0x400 }, 
	{ "CMA", 0, 0, 0x7, 0x200 }, 
	{ "CME", 0, 0, 0x7, 0x100 }, 
	{ "CIR", 0, 0, 0x7, 0x080 }, 
	{ "CIL", 0, 0, 0x7, 0x040 }, 
	{ "INC", 0, 0, 0x7, 0x020 }, 
	{ "SPA", 0, 0, 0x7, 0x010 }, 
	{ "SNA", 0, 0, 0x7, 0x008 }, 
	{ "SZA", 0, 0, 0x7, 0x004 }, 
	{ "SZE", 0, 0, 0x7, 0x002 }, 
	{ "HLT", 0, 0, 0x7, 0x001 }, 

	// Input-output instructions:
	{ "INP", 0, 0, 0xF, 0x800 }, 
	{ "OUT", 0, 0, 0xF, 0x400 }, 
	{ "SKI", 0, 0, 0xF, 0x200 }, 
	{ "SKO", 0, 0, 0xF, 0x100 }, 
	{ "ION", 0, 0, 0xF, 0x080 }, 
	{ "IOF", 0, 0, 0xF, 0x040 }, 

	// Pseudo-instructions:
	{ "ORG", 0, 0, 0x0, 0x000 }, 
	{ "END", 0, 0, 0x0, 0xFFF },	// Operand included so user can't 
									// specify one
	{ "HEX", 1, 0, 0x0, 0x000 }, 
	{ "DEC", 0, 0, 0x0, 0x000 }

};	// m_instruction_list


//
// Name:	HexToIngeger
//
unsigned short CManoAssembler::HexToInteger( const char *str ) {
	char buffer[81];
	unsigned short result;
	int negative_flag = 0;
	const char *p;

	strcpy( buffer, "" );
	p = str;

	// If there is a negative sign, set a flag, and add it in before the 0x:
	if( str[0] == '-' ) {
		negative_flag = 1;
		p++;
	}

	// Make a copy of the string, in uppercase, with 0X preceeding it:
	if( negative_flag ) {
		strcat( buffer, "-" );
	}
	strcat( buffer, "0x" );
	strcat( buffer, p );

	_strupr( buffer );

	// Call strtoul to convert this to a decimal value:
	char *stopscan;
	result = (unsigned short)strtoul( buffer, &stopscan, 16 );

	return result;

}; // HexToInteger

//
// Name:	IntegerToHex
//
void CManoAssembler::IntegerToHex( unsigned long integer, char *hex_data ) {
	char buffer[12];
	char buffer2[12];

	_ultoa( integer, buffer, 16 );

	// If the hex data is longer than 4 characters, take the last 4:
	if( strlen( buffer ) > 4 ) {
		strcpy( buffer2, buffer + strlen(buffer) - 4 );
		strcpy( buffer, buffer2 );
	}

	// If the hex data is shorter than 4 characters, lead with 0's:
	if( strlen( buffer ) < 4 ) {
		strcpy( buffer2, buffer );
		strcpy( buffer, "0000" );
		strcpy( buffer + (4-strlen(buffer2)), buffer2 );
	}

	// Copy the final result:
	strcpy( hex_data, buffer );

} // IntegerToHex
