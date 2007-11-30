// File:	SymbolTable.cpp
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

#include "SymbolTable.hpp"

#include <cstring>

using namespace std;

//
// Name:	(constructor)
//
CSymbolTable::CSymbolTable() {
} // (constructor)

//
// Name:	(destructor)
//
CSymbolTable::~CSymbolTable() {
} // (destructor)

//
// Name:	Reset
//
void CSymbolTable::Reset() {
} // Reset

//
// Name:	AddSymbol
//
void CSymbolTable::AddSymbol( const char *symbol, int address ) {
	
	// Increment the number of symbols by one, and add the symbol, if 
	// there's room:

	m_symbols[string(symbol)] = address;

} // AddSymbol

//
// Name:		GetAddress
//
int CSymbolTable::GetAddress( const char *symbol ) const {
	const map<string, int>::const_iterator i = m_symbols.find(symbol);
	// If not found, return failure code
	if (i == m_symbols.end())
		return -1;
	// Otherwise, return the address of the symbol!
	return (*i).second;

} // GetAddress


//
// Name:	Dump
//
void CSymbolTable::Dump( ostream &out ) {

	map<string, int>::const_iterator index;

	// Dump each symbol, one at a time
	for (index = m_symbols.begin(); index != m_symbols.end(); index++) {
		out << (*index).first << "\t" 
			<< hex << "0x" << (*index).second << endl;
	}

} // DumpSymbolTable

