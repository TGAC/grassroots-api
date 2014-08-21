#ifndef IRODS_LINE_READER_HPP
#define IRODS_LINE_READER_HPP

#include "rodsDef.h"

class IRodsLineReader : public ILineReader
{
public:
	/// Indicates (negatively) whether there is any more input.
	virtual bool AtEOF (void) const;

	/// Return the next character to be read without consuming it.
	virtual char PeekChar (void) const;

	/// Make a line available.  MUST be called even for the first line;
	/// MAY trigger EOF conditions even when also retrieving data.
	virtual ILineReader& operator++ (void);

	/// Unget current line, which must be valid.
	/// After calling this method:
	///   AtEOF() should return false,
	///   PeekChar() should return first char of the line
	///   call to operator*() is illegal
	///   operator++() will make the line current
	virtual void UngetLine (void);

	/// Return the current line, minus its terminator.
	virtual CTempString operator* (void) const;

	/// Return the current (absolute) position.
	virtual CT_POS_TYPE GetPosition (void) const;

	/// Return the current line number (counting from 1, not 0).
	virtual unsigned int GetLineNumber (void) const;

protected:
	unsigned int irlr_line_number;
	rcComm_t *irlr_connection_p;
	
	/**
	 * This is the buffer that is used when copying the data
	 * from the iRods data object.
	 */
	BytesBuf_t irlr_buffer;
};


#endif		/* #ifndef IRODS_LINE_READER_HPP */
