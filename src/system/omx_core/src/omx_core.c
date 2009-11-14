/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  omx_core.c
*   This file contains methods that provides the functionality for the OpenMAX Core
*   responsible for environmental setup, components tunneling and communication.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_core\src
*
*  @rev 1.0
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*-------program files ----------------------------------------*/
#include <omx_core.h>
#include <omx_component.h>
#include "../inc/omx_componenttable.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

/* Note: This OpenMAX core is OMX 1.1.0.0 compliant(In BCD format) */
static const OMX_VERSIONTYPE OMXCoreVersion = {0x01,0x01, 0x00, 0x00};

/*--------function prototypes ---------------------------------*/

/* ==========================================================================*/
/**
 * @fn OMX_Init()  This method is used to initialize the OMX core.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_Init (void)
{
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   //tError = TIMM_OSAL_Init();

   return tError;
}

/* ==========================================================================*/
/**
 * @fn OMX_Deinit() This method is used to deinitalize the core.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_Deinit (void)
{
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   //tError = TIMM_OSAL_Deinit();

   return tError;
}

/* ==========================================================================*/
/**
 * @fn OMX_GetHandle() This method will locate the component specified by the
 *                     component name given, loads that component into memory
 *                     and validate it.If the component is valid, this method will
 *                     invoke the component's method to fill the comp handle and
 *                     setup the callbacks. Each time this method returns
 *                     successfully,a new component instance is created.
 *
 *  @ param [out]pHandle        : pointer to the OMX_HANDLETYPE to be
 *                                filled in by this method.
 *
 *  @ param [in] cComponentName : pointer to Null terminated string with
 *                                component name.
 *
 *  @ param [in] pAppData       : pointer to an IL client-defined value that
 *                                will be returned during callbacks, so that
 *                                client can identify the source of the callback.
 *
 *  @ param [in] pCallBacks     : pointer to an OMX_CALLBACKTYPE structure
 *                                containing the callbacks that the comp
 *                                will use for this IL client.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetHandle (OMX_HANDLETYPE* pHandle,
                             OMX_STRING cComponentName,
                             OMX_PTR pAppData,
                             OMX_CALLBACKTYPE* pCallBacks)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* componentType = NULL;
    int i = 0;
    int bFound = 0;

    /* Check for valid component handle, Application has to allocate and
     *  fill the size of the structure and version fields;
     *  Also validate other input parameters. */
    if ((NULL == pHandle) ||
        (NULL == cComponentName) ||
        (NULL == pCallBacks)) {
        retVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    /* Verify that the name is not too long and could cause a crash. Notice
     * that the comparison is a greater than or equals. This is to make
     * sure that there is room for the terminating NULL at the end of the name */
    if(strlen(cComponentName) >= OMX_MAX_STRINGNAME_SIZE)
    {
        retVal = OMX_ErrorInvalidComponentName;
        goto EXIT;
    }

    /* Search through the component list for the matching component name
     * and execute the Init function of the component     */
    i = 0;
    while (NULL != OMXComponentList[i].cComponentName)
    {
       if (strcmp(cComponentName,
                  OMXComponentList[i].cComponentName) == 0)
       {
           /* Component found */
           bFound = 1;
           break;
       }
       i++;
    }

    if (bFound) {
       /* Allocate memory for the component handle which is of the type
        * OMX_COMPONENTTYPE and fill in the size and version
        * field of the structure */
       *pHandle = NULL;
       *pHandle = (OMX_HANDLETYPE)TIMM_OSAL_Malloc(
                               sizeof(OMX_COMPONENTTYPE),
                               TIMM_OSAL_TRUE, 0,
                               TIMMOSAL_MEM_SEGMENT_EXT);
       if(NULL == *pHandle) {
           retVal = OMX_ErrorInsufficientResources;
           goto EXIT;
       }
       TIMM_OSAL_Memset(*pHandle, 0x0,sizeof(OMX_COMPONENTTYPE));

       componentType = (OMX_COMPONENTTYPE*) *pHandle;
       componentType->nSize = sizeof(OMX_COMPONENTTYPE);

       componentType->nVersion.nVersion = OMXCoreVersion.nVersion;

       /* Call the Init function of the component using Init function
        * pointer updated in the component list table  */
       retVal = OMXComponentList[i].pComponentInit(*pHandle);
       if (OMX_ErrorNone != retVal) {
            goto EXIT;
       }

       /* Setting the callback function pointers */
       retVal = (componentType->SetCallbacks)(*pHandle,
                                        pCallBacks, pAppData);
       if (OMX_ErrorNone != retVal) {
           /* Since the SetCallbacks() failed, need to restore to the original
            * state, hence call ComponentDeInit() here         */
           componentType->ComponentDeInit(*pHandle);
           goto EXIT;
       }
    }
    else {
        /* Component was not found in the component table */
        retVal = OMX_ErrorComponentNotFound;
    }

EXIT:
   /* Perform cleanup on error */
   if ((OMX_ErrorNone  != retVal) && (NULL != *pHandle)) {
       TIMM_OSAL_Free(*pHandle);
       *pHandle = NULL;
   }
   return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_FreeHandle()
 *     This method will free a handle allocated by the OMX_GetHandle method.
 *
 *  @ param [in]  :  hComponent   :
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_FreeHandle (OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE* pHandle = (OMX_COMPONENTTYPE *)hComponent;

    /* Check for valid input handle */
    if (NULL == hComponent) {
       retVal = OMX_ErrorBadParameter;
       goto EXIT;
    }

    /* Call the ComponentDeInit function */
    retVal = pHandle->ComponentDeInit(hComponent);
    if (OMX_ErrorNone != retVal) {
        goto EXIT;
    }
    /* Free the memory for the component handle */
    TIMM_OSAL_Free(hComponent);

EXIT:
    return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_SetupTunnel()
 *     This method setup tunneled communciation between an
 *     output port and input port.This method calls the component's
 *     ComponentTunnelRequest method to setup the tunnel. This method
 *     first issues a call to ComponentTunnelRequest on the comp with
 *     output port, if the call is sucessfull a second call to CompTunnelRequst
 *     on the comp with input port is made. shoudl either the call to
 *     CompTunnelRequest fail, the method will set up both the
 *     o/p and i/p for non-tunneled communication.
 *
 *  @ param [in] hOutput    : Handle of the comp containing the o/p port used in tunnel
 *
 *  @ param [in] nPortOutput: indicates the o/p port of comp specified by hOutput.
 *
 *  @ param [in] hInput     : Handle of the comp containing the I/p port used in tunnel
 *
 *  @ param [in] nPortInput : indicates the I/p port of comp specified by hInput.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_SetupTunnel(OMX_IN  OMX_HANDLETYPE hOutput,
                              OMX_IN  OMX_U32 nPortOutput,
                              OMX_IN  OMX_HANDLETYPE hInput,
                              OMX_IN  OMX_U32 nPortInput)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pInComp, *pOutComp;
    OMX_TUNNELSETUPTYPE tunnelSetup;

    /* Check for input and output handles */
    if(hOutput == NULL && hInput == NULL) {
        retVal = OMX_ErrorBadParameter;
        goto EXIT;
    }

    pInComp  = (OMX_COMPONENTTYPE *)hInput;
    pOutComp = (OMX_COMPONENTTYPE *)hOutput;

    /* setting default values before  tunneling */
    tunnelSetup.nTunnelFlags = 0;
    tunnelSetup.eSupplier    = OMX_BufferSupplyUnspecified;

    if(hOutput) {

        /* requesting tunnel on the component with the output port */
        retVal = pOutComp->ComponentTunnelRequest(hOutput,
                                 nPortOutput, hInput,
                                 nPortInput, &tunnelSetup);
    }

    if(retVal == OMX_ErrorNone && hInput) {
        /* reques tunnel on the component with the Input port */
        retVal = pInComp->ComponentTunnelRequest(hInput,
                                   nPortInput, hOutput,
                                   nPortOutput, &tunnelSetup);
        if(retVal != OMX_ErrorNone ) {
            /* cancel tunnel request on output port if input port fails */
            pOutComp->ComponentTunnelRequest(hOutput,
                                 nPortOutput, NULL, 0, NULL);

        }
    }

EXIT:
   return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_ComponentNameEnum()
 *     This method enumerates through all the names of recognized
 *     components to detect all the components in the system-runtime.
 *
 *  @ param [out] cComponentName : pointer to an Null terminated string
 *                                 with the componentname.
 *
 *  @ param [in]  nNameLength    : number of characters in the
 *                                 cComponentName string
 *
 *  @ param [in]  nIndex         : number containing the enumeration
 *                                 index for the comp.
 *
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_ComponentNameEnum(OMX_OUT OMX_STRING cComponentName,
                                    OMX_IN  OMX_U32 nNameLength,
                                    OMX_IN  OMX_U32 nIndex)
{
    OMX_ERRORTYPE retVal = OMX_ErrorNone;
    OMX_U32 nCompIdx = 0;
    OMX_BOOL bFound = OMX_FALSE;

    if(nNameLength == 0) {
        retVal = OMX_ErrorInvalidComponentName;
        goto EXIT;
    }

    /* Enumerate through all the names of the component to detect
     * all the components in the system run-time .*/
    while(NULL != OMXComponentList[nCompIdx].cComponentName)
    {
        if(nCompIdx == nIndex) {
            strncpy(cComponentName,
                    OMXComponentList[nCompIdx].cComponentName,
                    nNameLength);
            bFound = OMX_TRUE;
            break;
        }
        nCompIdx++;
    }

    /* Component has not found and return OMX_ErrorNoMore */
    if(!bFound) {
        cComponentName = NULL;
        retVal = OMX_ErrorNoMore;
    }

EXIT:
   return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_GetRolesOfComponent()
 *     This method enables the IL client to query all the roles
 *     fulfilled by a component.
 *
 *  @ param [in]    compName :  Name of the comp being queried
 *
 *  @ param [inout] pNumRoles:  This is used as both input and ouput,on input
 *                              it bounds size of the input strucutre, on ouput
 *                              it specifies how many roles were retrieved.
 *
 *  @ param [out]   roles    :  list of the names of all standard components
 *                              implemented on the specified physical comp name.
 *                              If this pointer is NULL this function populates
 *                              the pNumRoles filed with the number of roles
 *                              the component supports and ignore the roles field.
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetRolesOfComponent (
                        OMX_IN   OMX_STRING compName,
                        OMX_INOUT OMX_U32 *pNumRoles,
                        OMX_OUT   OMX_U8 **roles)
{
   OMX_BOOL bFound = OMX_FALSE;
   OMX_U32 nCompIndx, nRoleIndx;
   OMX_U32 nMaxRoles;
   OMX_ERRORTYPE retVal = OMX_ErrorNone;

   /* Check for an Valid component name */
   if(compName == NULL) {
      retVal = OMX_ErrorBadParameter;
      goto EXIT;
   }

   /* Verify that the compName is not more than
          * OMX_MAX_STRINGNAME_SIZE */
   if(strlen(compName) >= OMX_MAX_STRINGNAME_SIZE) {
      retVal = OMX_ErrorInvalidComponentName;
      goto EXIT;
   }

   nMaxRoles = *pNumRoles;

   nCompIndx = 0;

   /* Search through the component list for the matching component name
    * and query the number of roles and list of roles supported  */
   while(NULL != OMXComponentList[nCompIndx].cComponentName) {
      if(strcmp(compName,
                OMXComponentList[nCompIndx].cComponentName) == 0)
      {

        /* Component Found over here, so identify the num of roles
         * that a component supports */
        bFound = OMX_TRUE;
        *pNumRoles = OMXComponentList[nCompIndx].nNumRoles;

        /* if "roles" ptr is NULL, just populate the pNumRoles field with the
         * number of roles the component supports and ignore the roles field ,
         * to allow the client to properly size the roles array on a subsequent call*/
        if(NULL == roles) goto EXIT;

        /* copy the roles that a component supports */
        for(nRoleIndx = 0;
            nRoleIndx < OMXComponentList[nCompIndx].nNumRoles; nRoleIndx++) {
            if(nRoleIndx < nMaxRoles) {
              strcpy((roles[nRoleIndx]),
                     (OMXComponentList[nCompIndx].roleArray[nRoleIndx]));
            }
        }
        break;
      }
      nCompIndx++;
   }

   if(bFound == OMX_FALSE) {
      /* Component was not found in the component table  */
      *pNumRoles = 0;
      retVal = OMX_ErrorComponentNotFound;
    }

EXIT:
  return retVal;
}

/* ==========================================================================*/
/*
 * @fn OMX_GetRolesOfComponent()
 *     This method enables the IL client to query
 *     the names  of all installed components that supports a given role.
 *
 *  @ param [in] role         : Name of the Specified role.
 *
 *  @ param [inout] pNumComps : This is use both as input and output, on input
 *                              it bounds the size of input struct. on ouptut
 *                              it specifies how many names were retrieved.
 *
 *  @ param [inout] compNames : This is a list of the names of all physical comps
 *                              implement the specified standard component name.
 *                              if this pointer is NULL thisfunction populates
 *                              the pNumComps field with the number of components
 *                              that support the given role and ignores the compNames
 *                              field.This allows the client to properly size the
 *                              compNames field on a subsequent call.
 *
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetComponentsOfRole (
                    OMX_IN   OMX_STRING role,
                    OMX_INOUT   OMX_U32 *pNumComps,
                    OMX_INOUT   OMX_U8  **compNames)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_U32 nNumComps = 0;
  OMX_U32 maxNumComp;
  OMX_U32 nCompIndx, nRoleIndx;

  /* Check for the valid role name */
  if(NULL == role) {
      retVal = OMX_ErrorBadParameter;
     goto EXIT;
  }

  maxNumComp = *pNumComps;
  nCompIndx = 0;

  /* Search through the component list and query the names of
   * components that supports the given role */
   while(NULL != OMXComponentList[nCompIndx].cComponentName) {
       for(nRoleIndx = 0;
           nRoleIndx < OMXComponentList[nCompIndx].nNumRoles;
           nRoleIndx++) {
         if(strcmp(OMXComponentList[nCompIndx].roleArray[nRoleIndx], role) == 0) {

            /* given role found over here, now identify the number of
             * components that support a given role */

            /* If "compNames" ptr is NULL,just populate the number of comp's
             * that supports the given role and ignore the compNames field, this
             * allows the client to properly size the compNames field on subsequent call*/
            if(compNames != NULL) {
                if(nNumComps < maxNumComp) {
                  strcpy(compNames[nNumComps], OMXComponentList[nCompIndx].cComponentName);
                }
            }
            nNumComps++;
         }
       }
      nCompIndx++;
    }

   /*Assigning the num of comp's supporting the given role */
   *pNumComps = nNumComps;

EXIT:
    return retVal;
}


