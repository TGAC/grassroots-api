#ifndef BLAST_TOOL_HPP
#define BLAST_TOOL_HPP

#ifdef __cplusplus
extern "C" 
{
#endif

/**
 * Convert the iRods arguments into an argc, argv set 
 */
int ConvertArguments (char **argv_ss, msParamArray_t *params_p);

#ifdef __cplusplus
}
#endif


#endif		/* #ifndef BLAST_TOOL_HPP */

