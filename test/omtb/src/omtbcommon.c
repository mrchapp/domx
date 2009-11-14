/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbCommon.c
 *
 *  @brief    This file fills OMX version header information for each structure
 *            passed to OMX component.
 *
 * <B> History: </B>
 *
 *    # June-1-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>

/* OMX standard header files */
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbMain.h"
#include "omtbDebug.h"

/******************************************************************************\
 *      Local Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Extern Object Definitions 
\******************************************************************************/


/******************************************************************************\
 *      Globals 
\******************************************************************************/


/******************************************************************************\
 *      Public Function Prototype
\******************************************************************************/


/******************************************************************************\
 *      Static Globals
\******************************************************************************/


/******************************************************************************\
 *      Private Function Declarations 
\******************************************************************************/


/******************************************************************************\
 *      OMTB_SetHeader Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function Sets the OMX version no for the application
 *            allocated omx structures  
 *
 *  @param    header: Void Pointer 
 *            size  : Size of structure
 *
 *  @return   None
 */

void OMTB_SetHeader(OMX_PTR header, unsigned int size)
{
  OMX_VERSIONTYPE* ver = (OMX_VERSIONTYPE*)(header + sizeof(unsigned int));

  /* return if structure pointer is NULL */
  if(NULL == header)
  {
    PRINT(PRINT_ERR_MSG, "%s: OMX Structure pointer is NULL !!!\n", __func__);
    return;
  }
  
  *((unsigned int*)header) = size;
  ver->s.nVersionMajor = OMTB_VERSION_MAJOR;
  ver->s.nVersionMinor = OMTB_VERSION_MINOR;
  ver->s.nRevision = OMTB_VERSION_REVISION;
  ver->s.nStep = OMTB_VERSION_STEP;
}


/*----------------------------- end of file ---------------------------------*/
