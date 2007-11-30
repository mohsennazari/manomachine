// File:	ErrorException.cpp
// Author:	Mark Roth
// Description:
//		A generic class to deal with error exceptions.
// Usage:	
//		Create an instance of this object and specify a filename,
//		line number, error code, description of the error and
//		severity of the error, and throw it.
//		Processors of the error should continue processing for 
//		severities of WARNING and ERROR, but immediately stop if
//		encountering a severity of FATAL.
//		Use Display to get a readable form of this message
// Revision History:
//		0.0:	Initial Revision
//

#include "ErrorException.hpp"

#include <cstring>

using namespace std;

//
// Name:	(constructor)
//
CErrorException::CErrorException( const char *filename, int linenumber, 
		const char *code, const char *message, ESeverity severity )
{
	SetFilename( filename );
	SetLineNumber( linenumber );
	SetCode( code );
	SetMessage( message );
	SetSeverity( severity );
} // (constructor)

//
// Name:	(destructor)
//
CErrorException::~CErrorException() {
} // (destructor)

// 
// Name:	GetFilename
//
const char *CErrorException::GetFilename() const {

	return m_filename;

} // GetFilename

// 
// Name:	GetLineNumber
//
int CErrorException::GetLineNumber() const {

	return m_linenumber;

} // GetLineNumber

// 
// Name:	GetCode
//
const char *CErrorException::GetCode() const {
	return m_code;
} // GetCode

// 
// Name:	GetMessage
//
const char *CErrorException::GetMessage() const {
	return m_message;
} // GetMessage

// 
// Name:	GetSeverity 
//
CErrorException::ESeverity CErrorException::GetSeverity() const {
	return m_severity;
} // GetSeverity

//
// Name:	Display
//
void CErrorException::Display( ostream &out ) {
	
	// Display the filename and line number:
	if( m_linenumber != 0 ) {
		out << GetFilename() << "(" << GetLineNumber() << ") : ";
	}

	// Display the severity:
	switch( GetSeverity() ) {
	case WARNING:
		out << "warning ";
		break;
	case ERROR:
		out << "error ";
		break;
	case FATAL:
		out << "fatal error ";
		break;
	};

	// Display the code:
	out << GetCode() << ": ";

	// Display the message:
	out << GetMessage() << endl;

} // Display

// 
// Name:	SetFilename
//
void CErrorException::SetFilename( const char *filename ) {

	m_filename = filename;

} // SetFilename

// 
// Name:	SetLineNumber
//
void CErrorException::SetLineNumber( int linenumber ) {

	// The output index is one-based; the internal index is zero-based
	m_linenumber = linenumber+1;
	
} // SetLineNumber

// 
// Name:	SetCode
//
void CErrorException::SetCode( const char *code ) {

	m_code = code;

} // SetCode

// 
// Name:	SetMessage
//
void CErrorException::SetMessage( const char *message ) {

	m_message = message;

} // SetMessage

// 
// Name:	SetSeverity 
//
void CErrorException::SetSeverity( ESeverity severity ) {

	m_severity = severity;

} // SetSeverity
