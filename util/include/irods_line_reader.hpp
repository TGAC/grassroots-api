/*
** Copyright 2014-2015 The Genome Analysis Centre
** 
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
** 
**     http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
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
