// File:	SymbolTable.hpp
// Author:	Mark Roth
// Description:
//		A generic symbol table that associates n-letter symbols
//		with integer addresses.
// Usage:	
//		To add a symbol, call the AddSymbol method
//		To retrieve the value of a symbol, call the GetAddress accessor
// Revision History:
//		0.0:	Initial Revision
//

#pragma once

#include <ostream>
#include <string>
#include <map>

class CSymbolTable {

public:	// Construction / Destruction

	//
	// Name:	(constructor)
	//
	// Description:	Constructs a CSymbolTable object
	// Modifies:	m_num_symbols
	// 
	CSymbolTable();

	//
	// Name:	(destructor)
	//
	// Description:	Destroys a CSymbolTable object.
	// 
	~CSymbolTable();

public:	// Initialization

	//
	// Name:		Reset
	//
	// Description:	Resets the symbol table
	// Modifies:	m_num_symbols
	//
	void Reset();

public:	// Symbol creation

	//
	// Name:	AddSymbol
	//
	// Description:	Adds a symbol to the symbol table
	// Arguments:	The MAX_SYM_LENGTH-character symbol name, and the 
	//		integer	to associate it with.  If the symbol is more 
	//		than MAX_SYM_LENGTH characters, only the first three 
	//		are used.  Case is ignored.  If the maximum number of 
	//		symbols are exceeded, the application aborts 
	//		immediately.
	// Modifies:	m_num_symbols, and the symbol table
	//
	void AddSymbol( const char *symbol, int address );

public: // Symbol retrieval

	//
	// Name:	GetAddress
	//
	// Description:	Retrieves the address of a given symbol
	// Arguments:	The MAX_SYM_LENGTH-character symbol to research.  
	//		If the symbol is larger than MAX_SYM_LENGTH characters,
	//		only the first MAX_SYM_LENGTH characters are used.  
	//		Case is ignored.
	// Returns:	The integer address of the given symbol, or -1 if the
	//		symbol is not found in the table.
	//
	int GetAddress( const char *symbol ) const;

public:		// Output

	//
	// Name:	Dump
	//
	// Description:	Dumps the symbol table to the given stream
	// Arguments:	The output stream to display the symbol table to
	//
	void Dump( std::ostream &out );

protected: // Attributes

	// The name of each symbol
	std::map<std::string, int>	m_symbols;
};
