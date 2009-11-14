/******************************************************************************\
* FILE PURPOSE: Provide Command Parsing in the Argc, Argv methodology
*
********************************************************************************
* FILE NAME: cmdLine.c
*
* DESCRIPTION: 
*
*  $Header: cmd_line.c 1.2 1994/12/29 14:12:28 suneetad Development $
*
* NOTE: 
*
* REVISION HISTORY:
*
* DATE          PROGRAMMER      REVISION DESCRIPTION
* =========     ============    ================================================
* 03-Jan-93         KRH           Genesis
*                               
* (C) Copyright 1994, Telogy NETWORKS, Inc.
*******************************************************************************/


/******************************************************************************\
* DATA DEFINITION: Include Files
*
*******************************************************************************/

/******************************************************************************\
 *     Standard Includes
\******************************************************************************/

#include <string.h>


/******************************************************************************\
 *     Customized Includes
\******************************************************************************/

#include "rtxData.h"
#include "omtbMain.h"


/*******************************************************************************
* DATA DEFINES:
*
*******************************************************************************/


#define CMD_MAX_ARGS  (80)    /* maximum arg count */
#define COMMENT_CHAR  (';')   /* character indicating that */
                              /* the line is just a comment */


/* Small Utility routines to decode command line */
/*******************************************************************************
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
int charIsUpper( char testChar )
{
  if ((testChar >= 'A') && (testChar <= 'Z')) 
{
  return( MX_TRUE );
}
  return( MX_FALSE );
}

/*******************************************************************************
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
char charToLower( char modChar )
{
  return( (char)(modChar | 0x20) );
}

/*******************************************************************************
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
int charIsSpace( char testChar )
{
  if ( testChar == ' ' )
  {
    return MX_TRUE;
  }
  else
  {
    return MX_FALSE;
  }
}

/*******************************************************************************
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
int charIsLineEnd( char testChar )
{
  if((testChar == '\0' )|| (testChar == COMMENT_CHAR)|| (testChar == '\r' )|| 
    (testChar == '\n' )
)
{
  return MX_TRUE;
}

  return MX_FALSE;
}

/*******************************************************************************
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
int charIsWordEnd( char testChar )
{
  return ((charIsSpace(testChar)) || (charIsLineEnd(testChar)));
}



/*******************************************************************************
* [GLOBAL] FUNCTION PURPOSE:   Parse a command line string and call resulting
*                 Command Function.  
*
********************************************************************************
* DESCRIPTION:  This function takes a pointer to a typed in string and
*              calls the resulting function in the command table with
*               (fn*)( argc, argv ) in the unix calling convention.
*
* UNIT TEST COMPLETED:  
*******************************************************************************/

void procCmdLine(char *cmdStrPtr )      /* Pointer to user typed in string */
{
  int     cmdArgc = 0;        /* command argc */
  char    *cmdArgv[ CMD_MAX_ARGS + 1 ];   /* command argv */
  char  **argvPtr = cmdArgv;    /* argument vector pointer */

  for ( ; ; cmdStrPtr++ )
  {
    /* Skip leading white space. Stop if comment or end of line. */
    while ( charIsSpace( *cmdStrPtr ) )
    {
      cmdStrPtr++;
    }

    if  (charIsLineEnd(*cmdStrPtr))
    {
      /* Successfully parsed entire line. */
      *argvPtr = MX_NULL;
      break;
    }

      /* Start a new arg and Save pointer in argv */
    if ( ++cmdArgc >= CMD_MAX_ARGS )
    {
      /* Error - Maximum command argument limit reached */
      *argvPtr = MX_NULL;
      break;
    }

    *argvPtr++ = cmdStrPtr;

      /* while character is not NULL and not a Space */
    while (!charIsWordEnd(*cmdStrPtr))
    {
      /* if at the command, convert to lower case */
      if ( cmdArgc == 1 ) 
      {
        if (charIsUpper( *cmdStrPtr ) ) 
        {
          *cmdStrPtr = charToLower( *cmdStrPtr );
        }
      }
      /* move through command */
      cmdStrPtr++;
    }

    /* if at the end of the line or at a comment */
    if (charIsLineEnd(*cmdStrPtr))
    {
      /* Successfully parsed entire line. */
      *cmdStrPtr = '\0';            /* null-terminate argument */
      *argvPtr = MX_NULL;
      break;
    }

    *cmdStrPtr = '\0';                /* null-terminate argument */

  }

  /* argv[0] contains the command, can compare with table to call */
  /* appropriate function */

  OMTB_CmdParse( cmdArgc, cmdArgv );

}

/*----------------------------- end of file ----------------------------------*/
