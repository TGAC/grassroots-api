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
#include "irods_line_reader.hpp"


IRodsLineReader *IRodsLineReader :: Create (rcComm_t *connection_p)
{
	IRodsLineReader *reader_p = NULL;
	
	if (!connection_p)
		{
			connection_p = GetConnection ();
		}
		
		
	if (connection_p)
		{
			reader_p = new IRodsLineReader (connection_p);
		}
		
	return reader_p;
}


bool IRodsLineReader :: OpenDataObject (const char * const name_s)
{
	bool success_flag = false;
	dataObjInp_t input_data;
	
	/* Clear the input data structure */
	memset (&input_data, 0, sizeof (dataObjInp)); 

	if (rstrcpy (input_data.objPath, name_s, MAX_NAME_LEN))
		{
			input_data.openFlags = O_RDONLY;
			
			status = rcDataObjOpen (conn, &dataObjInp); 
		}
	else
		{
			
		}
	
	return success_flag;
}


/// Indicates (negatively) whether there is any more input.
bool IRodsLineReader :: AtEOF (void) const
{
	
}

/// Return the next character to be read without consuming it.
char IRodsLineReader :: PeekChar (void) const
{
	
}

/// Make a line available.  MUST be called even for the first line;
/// MAY trigger EOF conditions even when also retrieving data.
ILineReader& IRodsLineReader :: operator++ (void)
{
	
}

/// Unget current line, which must be valid.
/// After calling this method:
///   AtEOF() should return false,
///   PeekChar() should return first char of the line
///   call to operator*() is illegal
///   operator++() will make the line current
void IRodsLineReader :: UngetLine (void)
{
	
}

/// Return the current line, minus its terminator.
CTempString IRodsLineReader :: operator* (void) const
{
	
}

/// Return the current (absolute) position.
CT_POS_TYPE IRodsLineReader :: GetPosition (void) const
{
	
}

/// Return the current line number (counting from 1, not 0).
unsigned int IRodsLineReader :: GetLineNumber (void) const
{
	
}
