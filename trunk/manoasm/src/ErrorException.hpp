// File:	ErrorException.hpp
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

#pragma once

#include <ostream>

class CErrorException {

public: // Enumerated types

	enum ESeverity { WARNING, ERROR, FATAL };

public:	// Construction / Destruction

	//
	// Name:	(constructor)
	//
	// Description:	Constructs a CErrorException object
	// Arguments:	The filename to be associated with this error, or ""
	//		if none, the line number to be associated, or 0 if 
	//		none, the error code for this error, the message
	//		for this error, and the severity for this error.  
	//		The filename can be a max of 80 characters.  The code 
	//		can be a max of 10 characters, and the message can 
	//		be a max of 80 characters.
	// Modifies:	m_filename, m_linenumber, m_code, m_severity, and 
	//		m_message
	// 
	CErrorException( const char *filename, int linenumber, 
		const char *code, const char *message, ESeverity severity );

	//
	// Name:	(destructor)
	//
	// Description:	Destroys a CErrorException object.
	// 
	~CErrorException();

public:	// Accessors

	// 
	// Name:	GetFilename
	//
	// Description:	Returns the filename associated with this error.
	//
	const char *GetFilename() const;

	// 
	// Name:	GetLineNumber
	//
	// Description:	Returns the line number associated with this error.
	//
	int GetLineNumber() const;

	// 
	// Name:	GetCode
	//
	// Description:	Returns the code associated with this error.
	//
	const char *GetCode() const;

	// 
	// Name:	GetMessage
	//
	// Description:	Returns the message associated with this error.
	//
	const char *GetMessage() const;

	// 
	// Name:	GetSeverity 
	//
	// Description:	Returns the severity associated with this error.
	//
	ESeverity GetSeverity() const;

public:	// Modifiers

	// 
	// Name:	SetFilename
	//
	// Description:	Sets the filename associated with this error.
	//
	void SetFilename( const char *filename );

	// 
	// Name:	SetLineNumber
	//
	// Description:	Sets the line number associated with this error.
	//
	void SetLineNumber( int linenumber );

	// 
	// Name:	SetCode
	//
	// Description:	Sets the code associated with this error.
	//
	void SetCode( const char *code );

	// 
	// Name:	SetMessage
	//
	// Description:	Sets the message associated with this error.
	//
	void SetMessage( const char *message );

	// 
	// Name:	SetSeverity 
	//
	// Description:	Sets the severity associated with this error.
	//
	void SetSeverity( ESeverity severity );

public:	// Error output

	//
	// Name:	Display
	//
	// Description:	Display a readable form of this error to
	//		the specified output stream.
	//		If the line number is 0, no filename info is
	//		provided.
	// Arguments:	The output stream to display the message to
	//
	void Display( std::ostream &out );

protected: // Attributes

	const char *m_filename;	// The filename associated with this exception

	int m_linenumber;	// The line number for this exception

	const char *m_code;	// The code associated with this exception
	
	const char *m_message;	// The message associated with this exception

	ESeverity m_severity;	// The severity associated with this exception

};
