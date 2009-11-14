/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbDebug.h
 *
 *  @brief    This file contains debug log levels, log display defines and API
 *            timing calculation macros.
 *
 * <B> History: </B>
 *
 *    -# May-11-2009    AVME Syste Test   Inital Version
 *
 */

#ifndef __OMTB_DEBUG_H__
#define __OMTB_DEBUG_H__


/******************************************************************************\
*     Includes
\******************************************************************************/

#include "omtbMain.h"


/******************************************************************************/
 
#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
*     Defines
\******************************************************************************/
#ifdef WTSD
#define APP_PRINT_LEVEL       (PRINT_NO_OUTPUT|PRINT_SIMPLE_MESSAGE|DEFAULT_MESSAGES |PRINT_FUNCTION_NAME|PRINT_DEBUG_MESSAGE)
#else
#define APP_PRINT_LEVEL       (PRINT_NO_OUTPUT)
#endif
#define PRINT_NO_OUTPUT       (0x00)          /* Remove All Print Messages */
#define PRINT_SIMPLE_MESSAGE  (0x02)          /* Print Simple Message */
#define DEFAULT_MESSAGES      (0x04)          /* Print Default Message */
#define PRINT_FUNCTION_NAME   (0x08)          /* Print Function Name*/
#define PRINT_DEBUG_MESSAGE   (0x10)          /* Print Debug Mesaage */
#define PRINT_MUST            (0x80)          /* Print Compulsary Mesaage */
#define PRINT_ERR_MSG         (0x80)          /* Print Error Mesaage */

#define PRINT_LEVEL           (APP_PRINT_LEVEL | PRINT_MUST | PRINT_ERR_MSG )


/******************************************************************************\
*     Includes
\******************************************************************************/

/* Flag to change OMTB debug log level */
extern unsigned int omtbDbgLogLevel;


/******************************************************************************\
*     Data type definitions
\******************************************************************************/


/******************************************************************************\
*     Macros
\******************************************************************************/

#if PRINT_LEVEL > 0

  #define PRINT(n, args...) if ((PRINT_LEVEL | omtbDbgLogLevel) & (n))  \
                            {                                           \
                              fprintf(stderr, "OMTB-");                 \
                              fprintf(stdout, args);                    \
                            }

#else 

  #define PRINT(n, args...) if ((PRINT_LEVEL | omtbDbgLogLevel) & (n))  \
                            {                                           \
                              fprintf(stderr, "OMTB-");                 \
                              fprintf(stdout, args);                    \
                            }

#endif  // #if PRINT_LEVEL > 0

/* Macro to declare API timing calculation variables */
#if OMTB_OMX_API_TIMING_ENABLE == 1

  #define OMTB_OMX_API_TIMING_VAR_DECL                                  \
    struct timeval enteringAPI, leavingAPI;                             \
    OMX_U64 enteringTime = 0, leavingTime = 0, timingDifference = 0;

#else

  #define OMTB_OMX_API_TIMING_VAR_DECL 

#endif  // #if OMTB_OMX_API_TIMING_ENABLE == 1

/* Macro to get API Entering Time */

#if OMTB_OMX_API_TIMING_ENABLE == 1

  #define OMTB_OMX_API_ENTRY_TIME_CALC                                  \
  {                                                                     \
    gettimeofday(&enteringAPI, 0);                                      \
  }

#else

  #define OMTB_OMX_API_ENTRY_TIME_CALC 

#endif  // #if OMTB_OMX_API_TIMING_ENABLE == 1


/* Macro to get API Leaving Time and display the time taken by API */
#if OMTB_OMX_API_TIMING_ENABLE == 1

  #define OMTB_OMX_API_EXIT_TIME_CALC(__OMX_API_NAME__)                       \
  {                                                                           \
    gettimeofday(&leavingAPI,0);                                              \
    enteringTime = ((enteringAPI.tv_sec * 1000 * 1000) + enteringAPI.tv_usec);\
    leavingTime  = ((leavingAPI.tv_sec * 1000 * 1000) + leavingAPI.tv_usec);  \
    timingDifference = (leavingTime - enteringTime);                          \
    PRINT(PRINT_MUST,"Time Taken by %s: %u usecs\n",                          \
                      __OMX_API_NAME__, timingDifference);                    \
  }

#else

  #define OMTB_OMX_API_EXIT_TIME_CALC(__OMX_API_NAME__)  

#endif  // #if OMTB_OMX_API_TIMING_ENABLE == 1


/******************************************************************************\
*     Globals
\******************************************************************************/


/******************************************************************************\
*     External Object Definitions 
\******************************************************************************/


/******************************************************************************\
*     Public Function Prototypes
\******************************************************************************/


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/
