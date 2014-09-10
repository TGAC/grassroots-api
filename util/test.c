#include <stdio.h>

#include <syslog.h>

#include "io_utils.h"


int main (int argc, char *argv [])
{
	char *buffer_s = GetIntAsString (10);
	
	if (buffer_s)
		{
			printf ("num is %s\n", buffer_s);
			WriteToLog (NULL, LOG_INFO, "Hello world %s\n", buffer_s);
		}
	else
		{
			printf ("num is NULL\n");			
		}
		
		
	return 0;
}
