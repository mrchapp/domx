/******************************************************************************\

 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbMain.c
 *
 *  @brief    Initial Parsing Code Implementation, template configuration
 *            and script handling code. 
 *
 * <B> History: </B>
 *
 *    # June-1-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#ifdef WTSD
#include <string.h>
#ifdef DUSE_LIBEDIT
       #include <curses.h>
       #include <term.h>
#endif
#endif

/* OMX standard header files */
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#ifdef WTSD
#include "omx_ti_video.h"
#include "omx_ti_index.h"
#else
#include "wtsd17_omx_ti_video.h"
#include "wtsd17_omx_ti_index.h"
#endif


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbDebug.h"
//#include "platform/osal/timm_osal_interfaces.h"
//sujatha
#include "timm_osal_interfaces.h"
#include "readLine.h"
#include "mxDbg.h"
#include "omtbMain.h"
#include "omtbCommon.h"
#include "omtbCmd.h"

#ifdef WTSD
#include <memmgr/memmgr.h>
#include <mmplatform.h>
#else
/* Codec engine header files */
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/trace/gt.h>
#include <ti/sdo/ce/utils/trace/TraceUtil.h>
#endif

/******************************************************************************\
 *      Local Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Extern Object Definitions 
\******************************************************************************/

/* display driver cfg command list */
#ifdef OMTB_FF_DRIVER_DISPLAY
extern MX_DBG_CMD_ENTRY_T*  cmdListDispConfig[];
#endif

#ifdef OMTB_FF_VD_H264
/* h264 vdec cfg command list */
extern MX_DBG_CMD_ENTRY_T*  cmdListH264VdecConfig[];
#endif
#ifdef OMTB_FF_AD_G711
/* g711 adec cfg command list */
extern MX_DBG_CMD_ENTRY_T*  cmdListG711AdecConfig[];
#endif
#ifdef OMTB_FF_VE_H264
/* h264 venc cfg command list */
extern MX_DBG_CMD_ENTRY_T*  cmdListH264VencConfig[];
#endif
#ifdef OMTB_FF_VPS_FCC
/* vpss vfcc cfg command list */
extern MX_DBG_CMD_ENTRY_T*  cmdListVPSSVfccConfig[];
#endif
#ifdef OMTB_FF_VPS_FDC
/* vpss vfdc cfg command list */
extern MX_DBG_CMD_ENTRY_T*  cmdListVPSSVfdcConfig[];
#endif

#ifdef OMTB_FF_DRIVER_DISPLAY
/* display driver cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblDispConfig;
#endif

#ifdef OMTB_FF_VD_H264
/* h264 vdec cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblH264VdecConfig;
#endif
#ifdef OMTB_FF_AD_G711
/* g711 adec cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblG711AdecConfig;
#endif
#ifdef OMTB_FF_VE_H264
/* h264 venc cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblH264VencConfig;
#endif
#ifdef OMTB_FF_VPS_FCC
/* vpss vfcc cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblVPSSVfccConfig;
#endif
#ifdef OMTB_FF_VPS_FDC
/* vpss vfdc cfg command table */
extern MX_DBG_CMD_TABLE_T   cmdTblVPSSVfdcConfig;
#endif
/* command table used for setp/getp/store cfg commands */
extern MX_DBG_CMD_TABLE_T   setpCmdTbl;

/* h264 vdec cfg command entry */
#ifdef OMTB_FF_VD_H264
extern MX_DBG_CMD_ENTRY_T   cmdEntryH264VdecConfig;
#endif
#ifdef OMTB_FF_AD_G711
/* g711 adec cfg command entry */
extern MX_DBG_CMD_ENTRY_T   cmdEntryG711AdecConfig;
#endif
#ifdef OMTB_FF_VE_H264
/* h264 venc cfg command entry */
extern MX_DBG_CMD_ENTRY_T   cmdEntryH264VencConfig;
#endif
#ifdef OMTB_FF_VPS_FCC
/* vpss vfcc cfg command entry */
extern MX_DBG_CMD_ENTRY_T   cmdEntryVPSSVfccConfig;
#endif
#ifdef OMTB_FF_VPS_FDC
/* vpss vfdc cfg command entry */
extern MX_DBG_CMD_ENTRY_T   cmdEntryVPSSVfdcConfig;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
/* display driver cfg command entry */
extern MX_DBG_CMD_ENTRY_T   cmdEntryDisplayConfig;
#endif

/******************************************************************************\
 *      Globals 
\******************************************************************************/

/* Flag to toggle arm CPU load dispplay */
unsigned int armCpuLoadDispFlag = OMX_FALSE;

/* Flag to toggle component log dispplay */
int globDebugDisable = OMX_TRUE;

/* Flag to change OMTB debug log level */
unsigned int omtbDbgLogLevel = OMX_FALSE;

/* Flag to control ARMM CPU load display task exit */
unsigned int armCpuLoadDispTaskExitFlag = OMX_FALSE;

/* ARM CPU Load display thread ID */
void *cpuLoadDispThreadId = NULL;

/* OMTB configuration templates */
OMTB_TEMPLATE_INFO  *omtbCfgTemplate[OMTB_MAX_TEMPLATE];


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
 *      OMTB_ReadScript Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    This function reads one line from the file and store it in buff
 *            component 
 *
 *  @param in:
 *            fp: Script file pointer
 *
 *            line: character Pointer for storing the read line
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            nBytes : No of bytes read
 *
 */

static int OMTB_ReadScript(FILE *fp, char *line);


/******************************************************************************\
 *      OMTB_SetTemplateDefVal Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    This function copies the default values in specified template.
 *
 *  @param in:
 *            template: Template no which is to be filled with default values
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

static MX_Result OMTB_SetTemplateDefVal(unsigned int template);


/******************************************************************************\
 *      OMTB_CpuLoadDispTask Function Definition
\******************************************************************************/
/*
 *
 *  @brief    
 *            This thread prints the CPU load once in a second.
 *
 *  @param in:
 *            threadArg: Thread arguments
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            None
 */

static void *OMTB_CpuLoadDispTask(void *threadArg);


/******************************************************************************\
 *      OMTB_GetCpuStatsDuration Function Prototype
\******************************************************************************/
/*
 *
 * \brief 
 *    Displays the % of CPU usage once in a second for the duration of secs+1.
 * 
 *    This API reads the /proc/stat CPU usage and idle time between a second and
 *    calculates the the % of CPU usage.
 *    The first row of this file has the numbers identify the amount of time 
 *    the CPU has spent performing different kinds of work.
 *  
 *    The first four numbers of this row (except the string "CPU") is idetified 
 *    as follows.
 *      1. user: normal processes executing in user mode 
 *      2. nice: niced processes executing in user mode 
 *      3. system: processes executing in kernel mode 
 *      4. idle: twiddling thumbs 
 *
 * \param secs
 *        None - For OMTB_QNX and OMTB_LINUX
 *        secs - Number of seconds this function to display the CPU load.
 * \return 
 *        None
 */

#if defined(OMTB_QNX) || defined(OMTB_LINUX)
  
  static void OMTB_GetCpuStatsDuration(void);
  
#else
  
  static void OMTB_GetCpuStatsDuration(int secs);

#endif /* defined(OMTB_QNX) || defined(OMTB_LINUX) */


/******************************************************************************\
 *      OMTB_ReadScript Function Definition
\******************************************************************************/
/*
 *
 *  @brief    This function reads one line from the file and store it in buff
 *            component 
 *
 *  @param in:
 *            fp: Script file pointer
 *
 *            line: character Pointer for storing the read line
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            nBytes : No of bytes read
 *
 */

static int OMTB_ReadScript(FILE *fp, char *line)
{
  int nBytes = 0;
  char *buff = NULL;

  /* Read one line from the specified file */
  buff = fgets(line, OMTB_MAX_LEN, fp);
  
  if (NULL == buff)
  {
    nBytes = OMX_FALSE;
  }
  else
  {
    /* Decrease string length by one to exclude new line character and replace 
       it by NULL */
    nBytes = strlen(buff) - 1;
    buff[nBytes] = 0;
  }
  return nBytes;
}


/******************************************************************************\
 *      main Function Definition
\******************************************************************************/
/*
 *
 *  @brief    This function is the entry point of program 
 *
 *  @param in:
 *            argc: No of arguments  
 *            argv: Array of pointers containing the strings enetered 
 *                  through command line
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            OMTB_SUCESS
 *            OMTB_FAIL
 *
 */

int main (int argc, char ** argv)
{
  unsigned int useScript = FALSE;
  int i = 0, nBytes = 0;
  FILE *fp = NULL; 
  unsigned int template = 0;
  char inputCmdLine[OMTB_MAX_LEN] = {'\0'};
  char *buff = NULL;
  char *script = NULL;
  MX_Result result = ERR_NOERR;
  int threadRes = TIMM_OSAL_ERR_NONE;

#ifdef WTSD
#ifdef TILER
  OMX_U32 setup =2; /*Only domx setup will be done by platform init since basic ipc setup has been 
                      done by syslink daemon*/
#else
                    /*Will do basic ipc setup, call proc load and aetup domx as well*/
    OMX_U32 setup = 1;
#endif 
    mmplatform_init(setup);    
    PRINT(PRINT_DEBUG_MESSAGE, "Called mmplatform_deinit()\n");
    
#else
  /* codec engine initialization */
  CERuntime_init();
#endif  

  /* Reset all available template pointers */
  for (i = 0; i < OMTB_MAX_TEMPLATE; i++)
    omtbCfgTemplate[i] = NULL;

  /* Allocate the default configuration template */
  result = OMTB_AddCfg(template);
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error adding the template memory\n",\
                                    __func__);
    return (OMTB_FAIL);
  }

  /* Default template is 0 */
  omtbCfgTemplate[template]->currTemplate = template;
  
  /* Set the template to default values */
  result = OMTB_SetTemplateDefVal(template);
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error configuring the template params\n",\
                                    __func__);
    return (OMTB_FAIL);
  }

  /* Reset CPU load task exit flag */
  armCpuLoadDispTaskExitFlag = OMX_FALSE;
  
  /* Create the thread to display ARM CPU load per second */
  threadRes = TIMM_OSAL_CreateTask(
                        (void *)&cpuLoadDispThreadId,
                        (void *)OMTB_CpuLoadDispTask,
                        0,
                        NULL,
                        OMTB_ARM_LOAD_TSK_STACK_SIZE,
                        OMTB_ARM_LOAD_GROUP_PRIORITY,
                        (signed char*)"ARM_CPU_LOAD_CALC_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG, "Unable to start CPU load display task\n");  
  }
  
#ifdef CEDEBUG
  GT_setprintf( (GT_PrintFxn)printf );
  GT_set( "*+01234567,GT_prefix=1235,GT_time=0" );
#endif
  
  /* DVS script input */
  if (OMTB_SCRIPT_CMD_LINE_ARG_CNT <= argc)
  {
    if (0 == strcmp((char *)argv[OMTB_SCRIPT_OPTION_IDX], "-s"))
    {
      script = (char *)argv[OMTB_SCRIPT_NAME_IDX];
      useScript = OMX_TRUE;
    }
    
    if (useScript)
    {
      /* open the script file */
      if(NULL == (fp = fopen(script, "rb")))
      {
        /* Set the ARM CPU load task exit flag */
        armCpuLoadDispTaskExitFlag = OMX_TRUE;
        
        PRINT(PRINT_ERR_MSG, "Error in opening script file %s\n",\
                              script);

        /* Remove the default template from the memory */
        result = OMTB_RemoveCfg(template);
        if(ERR_NOERR != result)
          return (OMTB_FAIL);
        
        return (OMTB_FAIL);
      }
    
      while(OMTB_LOOP)
      {
        /* read the command from the script */
        nBytes = OMTB_ReadScript(fp, inputCmdLine);

        if(OMX_FALSE == nBytes)
        {
          /* Set the ARM CPU load task exit flag */
          armCpuLoadDispTaskExitFlag = OMX_TRUE;
          
          /* Exit the loop when there is no data available from the file */
          break;
        }      
        else
        {
          /* process the command read */
          PRINT(PRINT_MUST,"\r     \nOMTB> %s\n",inputCmdLine);
          procCmdLine(inputCmdLine);
          continue;
          
        }   // if(FALSE == nBytes) ... else ...
        
      }   // while(OMTB_LOOP)

    }   // if (useScript)
    
  }   // if (argc >= 3)
  else 
  {
    /* Command line input - With/without libedit library */
    
#ifndef USE_LIBEDIT  /* If libedit library is not used */

    while(OMTB_LOOP)
    {
      /* OMTB command prompt */
      PRINT(PRINT_MUST, "\nOMTB > ");
      fflush(stdout);

      /* read the input from the command line */
      gets(inputCmdLine);

      /* exit the application */
      if(0 == strcmp(inputCmdLine, OMTB_APP_EXIT_CMD_STR))
      {
        /* Set the ARM CPU load task exit flag */
        armCpuLoadDispTaskExitFlag = OMX_TRUE;
        
        PRINT(PRINT_MUST,"AVME application finished\n");
        break;
      }
      /* switch to Khronos OMX Conformance Test Suite */
      else if(0 == strcmp(inputCmdLine, OMTB_KHRONOS_TS_CMD_STR))
      {
        PRINT(PRINT_MUST, "OMX Conformance Test Bench Start\n");
//        omx_conformance_main(1, NULL);
        PRINT(PRINT_MUST, "OMX Conformance Test Bench Exit\n");
        continue;
      }
      else
      {
        /* parse and process the command */
        procCmdLine(inputCmdLine);
      }
      
    }   // while(OMTB_LOOP)
    
#else             /* If libedit library is used */

    while(OMTB_LOOP)
    {
      printf("\n");

      /* read input from the command line */
      buff = readline((const char *)OMTB_PROMPT); 
      
      if(NULL == buff)
      {
        /* Set the ARM CPU load task exit flag */
        armCpuLoadDispTaskExitFlag = OMX_TRUE;
        
        return (OMTB_FAIL);
      }

      /* copy the command into local string variable */
      strcpy(inputCmdLine, buff);

      /* add the command to history */
      add_history(inputCmdLine);
      
      /* exit the application */
      if(0 == strcmp(inputCmdLine, OMTB_APP_EXIT_CMD_STR))
      {
        /* Set the ARM CPU load task exit flag */
        armCpuLoadDispTaskExitFlag = OMX_TRUE;
        
        PRINT(PRINT_MUST, "OMTB Exit\n");
        break;
      }
      /* switch to Khronos OMX Conformance Test Suite */
      else if(0 == strcmp(inputCmdLine, OMTB_KHRONOS_TS_CMD_STR))
      {
        PRINT(PRINT_MUST, "OMX Conformance Test Bench Start\n");
//        omx_conformance_main(1, NULL);
        PRINT(PRINT_MUST, "OMX Conformance Test Bench Exit\n");
        continue;
      }
      else
      {
        /* parse and process the command */
        procCmdLine(inputCmdLine);
      }
      
    }   // while(OMTB_LOOP)
    
#endif
  }   /* if (argc >= 3) ... else ... */
  
  // Release the template memory if allocated
  for (i = 0; i < OMTB_MAX_TEMPLATE; i++)
  {
    if(NULL != omtbCfgTemplate[i])
    {
      result = OMTB_RemoveCfg(i);
      if(ERR_NOERR != result)
        return (OMTB_FAIL);
    }
      
  }
#ifdef WTSD
  PRINT(PRINT_DEBUG_MESSAGE, "Calling mmplatform_deinit()\n");
  mmplatform_deinit();
  PRINT(PRINT_DEBUG_MESSAGE, "mmplatform_deinit END ()\n");
#endif

  return (OMTB_SUCCESS);
}


/******************************************************************************\
 *      OMTB_RunScript Function Definition
\******************************************************************************/
/*
 *
 *  @brief   
 *            This function runs the DVS script containing set of commands.
 *
 *  @param in:
 *            scriptFileName: Pointer to script file name
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

MX_Result OMTB_RunScript( char *scriptFileName)
{
  FILE *scriptFp = NULL;
  char inputCmdLine[OMTB_MAX_LEN] = {'\0'};
  unsigned int nBytes = 0;

  /* open the script file */
  scriptFp = fopen(scriptFileName, "rb");
  
  if(NULL == scriptFp)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error in opening script file %s\n",\
                                __func__, scriptFileName);
    return ERR_INVNAME;
  }
  
  while(OMTB_LOOP)
  {
    /* read the commands from the script */
    nBytes = OMTB_ReadScript(scriptFp, &(inputCmdLine[nBytes]));
    if(OMX_FALSE == nBytes)
    {
      /* Exit the loop when there is no data available from the file */
      break;
    }      
    else
    {
      PRINT(PRINT_MUST, "\r     \nOMTB> %s\n", inputCmdLine);
      procCmdLine(inputCmdLine);
      continue;
    }
    
  } // while(OMTB_LOOP)

  /* Close the script file */
  fclose(scriptFp);

  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdParse Function Definition
\******************************************************************************/
/*
 *
 *  @brief    This function copies the content from command line into a local
 *            buffer and send it for further parsing 
 *
 *  @param in:
 *            argc: No of arguments  
 *            argv: Array of pointers containing the strings enetered 
 *                  through command line
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            None
 */

void OMTB_CmdParse (int argc, char ** argv)
{
  int i = 0;
  char  *pStr = NULL;
  
  if(1u > argc)
  {
    return;
  }  

  /* Reconstruct the command line from the (argc, argv[]) format */
  for( i = 0; i < (argc - 1u); i++ )
  {
    pStr = argv[i];
    pStr[strlen((char *)pStr)] = ' ';
  }  // for( i = 0; i < (argc - 1u); i++ )

  pStr = argv[i];
  pStr[strlen((char *)pStr)] = '\0';
  pStr = argv[1];
  
  if (0 == memcmp(argv[0], "omx", strlen("omx")))
  {
    /* Invoke the omx command processing function */
    OMTB_CmdParseOmx(argc, (char **)argv);
  }
  else if(0 == memcmp(argv[0], "help", strlen("help")))
  {
    /* Invoke the omx command processing function */
    OMTB_CmdParseOmx(argc, (char **)argv);
  }
  else
  {
    PRINT(PRINT_MUST, "Unknown command : %s\n", argv[0]);
    PRINT(PRINT_MUST, "Enter \'help\' or \'omx\' for more info...\n");
  }
  return;
}


/******************************************************************************\
 *      OMTB_SetTemplateDefVal Function Definition
\******************************************************************************/
/*
 *
 *  @brief    This function copies the default values in specified template.
 *
 *  @param in:
 *            template: Template no which is to be filled with default values
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

static MX_Result OMTB_SetTemplateDefVal(unsigned int template)
{
#ifdef OMTB_FF_VD_H264
  OMTB_H264_VDEC_CONFIG_PARAMS *h264VidDecCfg = NULL;
#endif
#ifdef OMTB_FF_AD_G711
  OMTB_G711_ADEC_CONFIG_PARAMS *g711AudDecCfg = NULL;
#endif
#ifdef OMTB_FF_VE_H264
  OMTB_H264_VENC_CONFIG_PARAMS *h264VidEncCfg = NULL;
#endif
#ifdef OMTB_FF_VPS_FCC
  OMTB_VPSS_VFCC_CONFIG_PARAMS *vpssHdVfccCfg = NULL;
#endif
#ifdef OMTB_FF_VPS_FDC
  OMTB_VPSS_VFDC_CONFIG_PARAMS *vpssHdVfdcCfg = NULL;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssCfg = NULL;
#endif
#ifdef OMTB_FF_DRIVER_AUDIO
  OmtbAudInfo  *alsaCfg = NULL;
#endif
  MX_Result result = ERR_NOERR;
  
  OMTB_TEMPLATE_INFO *pTemplate = NULL;
    
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }
  
  if(NULL == omtbCfgTemplate[template])
  {
    /* Allocate the template memory if it is not already allocated */
    result = OMTB_AddCfg(template);
    if(ERR_NOERR != result)
    {
      PRINT(PRINT_ERR_MSG, "%s: Memory for Template %d not allocated\n",\
                                  __func__, template);
      return result;
    }
  }

  /* update the local copy of appropriate pointers */
  pTemplate = omtbCfgTemplate[template];

#ifdef OMTB_FF_VD_H264
  h264VidDecCfg = &pTemplate->h264VdecTemplate;
   
  /* H264 VDEC configuration */
  #ifdef WTSD
   strcpy(h264VidDecCfg->inputFile, "H264BP_176x144_120_frm_ice.264");
  strcpy(h264VidDecCfg->outputFile,"H264BP_176x144_120_frm_ice.yuv");
  strcpy(h264VidDecCfg->compName,   OMTB_H264_VDEC_COMPONENT_NAME);
  strcpy(h264VidDecCfg->frameSizeFile,"H264BP_176x144_120_frm_ice.txt");
  #else
  strcpy(h264VidDecCfg->inputFile, "h264vdec_sample.264");
  strcpy(h264VidDecCfg->outputFile,"h264vdec_test.yuv");
  strcpy(h264VidDecCfg->compName,   OMTB_H264_VDEC_COMPONENT_NAME);
  strcpy(h264VidDecCfg->frameSizeFile,"frame_data.txt");
  #endif
  h264VidDecCfg->frameWidth       = OMTB_VIDDEC_DEF_WIDTH;
  h264VidDecCfg->frameHeight      = OMTB_VIDDEC_DEF_HEIGHT;
  h264VidDecCfg->colorFormat      = OMX_COLOR_FormatYUV420Planar;
  h264VidDecCfg->dataOutPutMode   = OMTB_FILE;
  h264VidDecCfg->numInBuf         = OMTB_H264_VDEC_NUM_IN_BUFS;
  h264VidDecCfg->numOutBuf        = OMTB_H264_VDEC_NUM_OUT_BUFS;
  h264VidDecCfg->flagUseInBuffer  = OMX_TRUE;
  h264VidDecCfg->flagUseOutBuffer = OMX_TRUE;
#endif
  
#ifdef OMTB_FF_AD_G711
  /* G711 ADEC configuration */
  g711AudDecCfg = &pTemplate->g711AdecTemplate;
  
  strcpy(g711AudDecCfg->inputFile, "g711adec_sample.g711");
  strcpy(g711AudDecCfg->outputFile,"g711adec_test.pcm");
  strcpy(g711AudDecCfg->compName,   OMTB_G711_ADEC_COMPONENT_NAME);
  g711AudDecCfg->dataOutPutMode   = OMTB_FILE;
  g711AudDecCfg->numInBuf         = OMTB_G711_ADEC_NUM_IN_BUFS;
  g711AudDecCfg->numOutBuf        = OMTB_G711_ADEC_NUM_OUT_BUFS;
  g711AudDecCfg->flagUseInBuffer  = OMX_TRUE;
  g711AudDecCfg->flagUseOutBuffer = OMX_TRUE;

#endif
#ifdef OMTB_FF_VE_H264
  /* H264 VENC configuration */
  h264VidEncCfg = &pTemplate->h264VencTemplate;
  
  strcpy(h264VidEncCfg->inputFile, "h264venc_sample.yuv");
  strcpy(h264VidEncCfg->outputFile,"h264venc_test.264");
  strcpy(h264VidEncCfg->compName,   OMTB_H264_VENC_COMPONENT_NAME);
  h264VidEncCfg->frameWidth       = OMTB_VIDENC_DEF_WIDTH;
  h264VidEncCfg->frameHeight      = OMTB_VIDENC_DEF_HEIGHT;
  h264VidEncCfg->colorFormat      = OMX_COLOR_FormatYUV420SemiPlanar;

  h264VidEncCfg->profileLevel.eProfile  = OMX_VIDEO_AVCProfileHigh;
  h264VidEncCfg->profileLevel.eLevel    = OMX_VIDEO_AVCLevel41;

  h264VidEncCfg->inputImgFmt.eContentType         = OMX_Video_Progressive;
  h264VidEncCfg->inputImgFmt.eInterlaceCodingType = OMX_Video_Interlace_PICAFF;

  h264VidEncCfg->avcParams.eLoopFilterMode     = OMX_VIDEO_AVCLoopFilterDisable;
  h264VidEncCfg->avcParams.bEntropyCodingCABAC = OMX_FALSE;
  h264VidEncCfg->avcParams.bEnableFMO     = OMX_FALSE;
  h264VidEncCfg->avcParams.bconstIpred    = OMX_FALSE;
  h264VidEncCfg->avcParams.bUseHadamard   = OMX_FALSE;
  h264VidEncCfg->avcParams.bEnableASO     = OMX_FALSE;
  h264VidEncCfg->avcParams.bEnableRS      = OMX_FALSE;
  h264VidEncCfg->avcParams.bEnableUEP     = OMX_FALSE;
  h264VidEncCfg->avcParams.bFrameMBsOnly  = OMX_FALSE;
  h264VidEncCfg->avcParams.bMBAFF         = OMX_FALSE;
  h264VidEncCfg->avcParams.bWeightedPPrediction   = OMX_FALSE;
  h264VidEncCfg->avcParams.bDirect8x8Inference    = OMX_FALSE;
  h264VidEncCfg->avcParams.bDirectSpatialTemporal = OMX_FALSE;

  h264VidEncCfg->avcParams.nSliceHeaderSpacing    = 
                 OMTB_H264_VENC_DEF_SLICE_HDR_SPACING;
  h264VidEncCfg->avcParams.nPFrames               = 
                 OMTB_H264_VENC_DEF_P_FRM_INTERVAL;
  h264VidEncCfg->avcParams.nBFrames               = 
                 OMTB_H264_VENC_DEF_B_FRM_INTERVAL;
  h264VidEncCfg->avcParams.nRefFrames             = 
                  OMTB_H264_VENC_DEF_NUM_REF_FRM;
  h264VidEncCfg->avcParams.nRefIdx10ActiveMinus1  = 
                 OMTB_H264_VENC_DEF_REF_IDX_10;
  h264VidEncCfg->avcParams.nRefIdx11ActiveMinus1  = 
                 OMTB_H264_VENC_DEF_REF_IDX_11;
  h264VidEncCfg->avcParams.nAllowedPictureTypes   = 
                 OMTB_H264_VENC_DEF_ALLOWED_PIC_TYPE;
  h264VidEncCfg->avcParams.nCabacInitIdc          = 
                 OMTB_H264_VENC_DEF_CABAC_INIT_IDX;
  h264VidEncCfg->avcParams.nWeightedBipredicitonMode = 
                 OMTB_H264_VENC_DEF_WEIGHTED_BIPRED_MODE;

  h264VidEncCfg->avcFMO.nNumSliceGroups     = 0;
  h264VidEncCfg->avcFMO.nSliceGroupMapType  = 0;
  h264VidEncCfg->avcFMO.eSliceMode          = OMX_VIDEO_SLICEMODE_AVCDefault;
  
  h264VidEncCfg->rateControlAlg     = OMX_Video_ControlRateDisable;
  h264VidEncCfg->transformBlockSize = OMX_Video_Transform_Block_Size_8x8;
  h264VidEncCfg->bitStreamFormat    = OMX_Video_BitStreamFormatByte;
  
  h264VidEncCfg->encoderPreset.eEncodingModePreset = OMX_Video_Enc_User_Defined;
  h264VidEncCfg->encoderPreset.eRateControlPreset  = OMX_Video_RC_User_Defined;
  
  h264VidEncCfg->frameRate.xEncodeFramerate = OMTB_H264_VENC_DEF_FRM_RATE;
  h264VidEncCfg->bitRate.nEncodeBitrate     = OMTB_H264_VENC_DEF_BIT_RATE;

  h264VidEncCfg->meSearchrange.eMVAccuracy = OMX_Video_MotionVectorQuarterPel;
  h264VidEncCfg->meSearchrange.nHorSearchRangeP = OMTB_H264_VENC_DEF_MESR_HORP;
  h264VidEncCfg->meSearchrange.nVerSearchRangeP = OMTB_H264_VENC_DEF_MESR_VERP;
  h264VidEncCfg->meSearchrange.nHorSearchRangeB = OMTB_H264_VENC_DEF_MESR_HORB;
  h264VidEncCfg->meSearchrange.nVerSearchRangeB = OMTB_H264_VENC_DEF_MESR_VERB;

  h264VidEncCfg->forceFrame.IntraRefreshVOP = OMX_FALSE;
  
  h264VidEncCfg->qpSettings.nQpI        = OMTB_H264_VENC_DEF_QP_I;
  h264VidEncCfg->qpSettings.nQpP        = OMTB_H264_VENC_DEF_QP_P;
  h264VidEncCfg->qpSettings.nQpOffsetB  = OMTB_H264_VENC_DEF_QP_B;
    
  h264VidEncCfg->qpSettings.nQpMinI = OMTB_H264_VENC_MIN_QP_IPB;
  h264VidEncCfg->qpSettings.nQpMinP = OMTB_H264_VENC_MIN_QP_IPB;
  h264VidEncCfg->qpSettings.nQpMinB = OMTB_H264_VENC_MIN_QP_IPB;
  
  h264VidEncCfg->qpSettings.nQpMaxI = OMTB_H264_VENC_MAX_QP_IPB;
  h264VidEncCfg->qpSettings.nQpMaxP = OMTB_H264_VENC_MAX_QP_IPB;
  h264VidEncCfg->qpSettings.nQpMaxB = OMTB_H264_VENC_MAX_QP_IPB;

  h264VidEncCfg->avcIntraPeriod.nIDRPeriod  = 
                                OMTB_H264_VENC_DEF_I_FRM_INTERVAL;
  
  h264VidEncCfg->sliceSettings.eSliceMode   = OMX_VIDEO_SLICEMODE_AVCDefault;
  h264VidEncCfg->sliceSettings.nSlicesize   = 0;
  
  h264VidEncCfg->pixelInfo.nWidth   = 0;
  h264VidEncCfg->pixelInfo.nHeight  = 0;

  h264VidEncCfg->dataOutPutMode     = OMTB_FILE;
  h264VidEncCfg->numInBuf           = OMTB_H264_VENC_NUM_IN_BUFS;
  h264VidEncCfg->numOutBuf          = OMTB_H264_VENC_NUM_OUT_BUFS;
  h264VidEncCfg->flagUseInBuffer    = OMX_TRUE;
  h264VidEncCfg->flagUseOutBuffer   = OMX_TRUE;

#endif
#ifdef OMTB_FF_VPS_FCC
  /* VPSS VFCC configuration */
  vpssHdVfccCfg = &pTemplate->vpssVfccTemplate;
  
  strcpy(vpssHdVfccCfg->inputFile, "vpssvfcc_sample.yuv");
  strcpy(vpssHdVfccCfg->outputFile,"vpssvfcc_test.yuv");
  strcpy(vpssHdVfccCfg->compName,   OMTB_VPSS_VFCC_COMPONENT_NAME);
  vpssHdVfccCfg->frameWidth       = OMTB_VPSS_VFCC_DEF_WIDTH;
  vpssHdVfccCfg->frameHeight      = OMTB_VPSS_VFCC_DEF_HEIGHT;
  vpssHdVfccCfg->colorFormat      = OMX_COLOR_FormatYUV422Planar;
  vpssHdVfccCfg->dataOutPutMode   = OMTB_DRIVER;
  vpssHdVfccCfg->numInBuf         = OMTB_VPSS_VFCC_NUM_IN_BUFS;
  vpssHdVfccCfg->numOutBuf        = OMTB_VPSS_VFCC_NUM_OUT_BUFS;
  vpssHdVfccCfg->flagUseInBuffer  = OMX_FALSE;
  vpssHdVfccCfg->flagUseOutBuffer = OMX_FALSE;

#endif
#ifdef OMTB_FF_VPS_FDC
  /* VPSS VFDC configuration */
  vpssHdVfdcCfg = &pTemplate->vpssVfdcTemplate;
  
  strcpy(vpssHdVfdcCfg->inputFile, "vpssvfdc_sample.yuv");
  strcpy(vpssHdVfdcCfg->outputFile,"vpssvfdc_test.yuv");
  strcpy(vpssHdVfdcCfg->compName,   OMTB_VPSS_VFDC_COMPONENT_NAME);
  vpssHdVfdcCfg->frameWidth       = OMTB_VPSS_VFDC_DEF_WIDTH;
  vpssHdVfdcCfg->frameHeight      = OMTB_VPSS_VFDC_DEF_HEIGHT;
  vpssHdVfdcCfg->colorFormat      = OMX_COLOR_FormatYUV422Planar;
  vpssHdVfdcCfg->dataOutPutMode   = OMTB_DRIVER;
  vpssHdVfdcCfg->numInBuf         = OMTB_VPSS_VFDC_NUM_IN_BUFS;
  vpssHdVfdcCfg->numOutBuf        = OMTB_VPSS_VFDC_NUM_OUT_BUFS;
  vpssHdVfdcCfg->flagUseInBuffer  = OMX_FALSE;
  vpssHdVfdcCfg->flagUseOutBuffer = OMX_FALSE;
  
  
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
  /* define is not used as it will be removed once DSS component integrated */
  dssCfg        = &pTemplate->v4l2DispTemplate;
 /* V4L2 display driver configuration */
  dssCfg->dispOutput    = OMTB_DISP_LCD;

  dssCfg->maxBuffers    = 2;
  dssCfg->width         = h264VidDecCfg->frameWidth;
  dssCfg->height        = h264VidDecCfg->frameHeight;
  dssCfg->userbuffer    = 0;
  dssCfg->a             = 0;
  
#endif
#ifdef OMTB_FF_DRIVER_AUDIO  /* audio alsa driver configuration */
   alsaCfg       = &pTemplate->alsaPlayTemplate;
   alsaCfg->sampleRate   = DEF_SAMPLE_RATE;
  alsaCfg->mode         = SND_PCM_STREAM_PLAYBACK;
  alsaCfg->numChannels  = DEF_NUM_CHANNELS;
  alsaCfg->format       = SND_PCM_FORMAT_S16_LE;
  alsaCfg->blockingMode = 0;
#endif    
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_StoreCfg Function Definition
\******************************************************************************/
/*
 *
 *  @brief   
 *            This function copies the default values of specified template to
 *            file.
 *
 *  @param in:
 *            pTemplate: Pointer to OMTB_TEMPLATE_INFO structure
 *
 *            cfgFileName: Pointer to configuration file name
 *
 *            template: Template no for which cfg is to be stored
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/file name.
 */

MX_Result OMTB_StoreCfg( OMTB_TEMPLATE_INFO *pTemplate, 
                     char *cfgFileName,
                     unsigned int template)
{
#ifdef OMTB_FF_VD_H264
  OMTB_H264_VDEC_CONFIG_PARAMS *h264VidDecCfg = NULL;
#endif
#ifdef OMTB_FF_AD_G711
  OMTB_G711_ADEC_CONFIG_PARAMS *g711AudDecCfg = NULL;
#endif
#ifdef OMTB_FF_VE_H264
  OMTB_H264_VENC_CONFIG_PARAMS *h264VidEncCfg = NULL;
#endif
#ifdef OMTB_FF_VPS_FCC
  OMTB_VPSS_VFCC_CONFIG_PARAMS *vpssHdVfccCfg = NULL;
#endif
#ifdef OMTB_FF_VPS_FDC
  OMTB_VPSS_VFDC_CONFIG_PARAMS *vpssHdVfdcCfg = NULL;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssCfg = NULL;
#endif
#ifdef OMTB_FF_DRIVER_AUDIO
  OmtbAudInfo  *alsaCfg = NULL;
#endif

  
  FILE *templateCfgFp = NULL;
  int i = 0, j = 0;
  void *paramLocation = NULL;
  MX_DBG_ENUM_ENTRY_T *enumTbl = NULL;
  MX_Result result = ERR_NOERR;
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }
  
  /* Allocate the template memory if it is not already allocated */
  result = OMTB_AddCfg(template);
  if(ERR_NOERR != result)
    return result;

  /* update the local copy of appropriate pointers */
  pTemplate = omtbCfgTemplate[template];
  
  /* Open the cfg file to write */
  templateCfgFp = fopen (cfgFileName, "wb");
  
  if(NULL == templateCfgFp)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error opening configuration file: %s\n",\
                                    __func__, cfgFileName);
    return ERR_INVNAME;
  }

  /* update the local copy of appropriate pointers */
#ifdef OMTB_FF_VD_H264
  h264VidDecCfg = &pTemplate->h264VdecTemplate;
#endif
#ifdef OMTB_FF_AD_G711
  g711AudDecCfg = &pTemplate->g711AdecTemplate;
#endif
#ifdef OMTB_FF_VE_H264
  h264VidEncCfg = &pTemplate->h264VencTemplate;
#endif
#ifdef OMTB_FF_VPS_FCC
  vpssHdVfccCfg = &pTemplate->vpssVfccTemplate;
#endif
#ifdef OMTB_FF_VPS_FDC
  vpssHdVfdcCfg = &pTemplate->vpssVfdcTemplate;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
  dssCfg        = &pTemplate->v4l2DispTemplate;
#endif
  
  /* Run the loop for available entries in SETP command table */
  for(i = 0; i < setpCmdTbl.max_entries - 1; i++)
  {
    int numEntries = 0, paramType;
            
    switch(i)
    {
#ifdef OMTB_FF_VD_H264
      case OMTB_H264_VDEC_CLASS:
        numEntries = cmdTblH264VdecConfig.max_entries;
        break;
#endif
#ifdef OMTB_FF_AD_G711        
      case OMTB_G711_ADEC_CLASS:
        numEntries = cmdTblG711AdecConfig.max_entries;
        break;
#endif
#ifdef OMTB_FF_VE_H264
      case OMTB_H264_VENC_CLASS:
        numEntries = cmdTblH264VencConfig.max_entries;
        break;
#endif
#ifdef OMTB_FF_VPS_FCC
      case OMTB_VPSS_VFCC_CLASS:
        numEntries = cmdTblVPSSVfccConfig.max_entries;
        break;
#endif
#ifdef OMTB_FF_VPS_FDC
      case OMTB_VPSS_VFDC_CLASS:
        numEntries = cmdTblVPSSVfdcConfig.max_entries;
        break;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY       
      case OMTB_V4L2_DISPLAY_CLASS:
        numEntries = cmdTblDispConfig.max_entries;
        break;
#endif      
      default:
        PRINT(PRINT_DEBUG_MESSAGE, "%s: Invalid class entry\n",\
                                    __func__);
        break;
    }
    
    /* Run the loop for available entries in algorithm configuration table */  
    for(j = 0; j < numEntries - 1; j++)
    {
      switch(i)
      {
#ifdef OMTB_FF_VD_H264
        case OMTB_H264_VDEC_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryH264VdecConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListH264VdecConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListH264VdecConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListH264VdecConfig[j]->format;
          }
          else
            paramType = cmdListH264VdecConfig[j]->type;
            
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)h264VidDecCfg + 
                                    cmdListH264VdecConfig[j]->location);
          break;
#endif
#ifdef OMTB_FF_AD_G711          
        case OMTB_G711_ADEC_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryG711AdecConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListG711AdecConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListG711AdecConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListG711AdecConfig[j]->format;
          }
          else
            paramType = cmdListG711AdecConfig[j]->type;
          
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)g711AudDecCfg + 
                                    cmdListG711AdecConfig[j]->location);
          break;
#endif
#ifdef OMTB_FF_VE_H264
        case OMTB_H264_VENC_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryH264VencConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListH264VencConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListH264VencConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListH264VencConfig[j]->format;
          }
          else
            paramType = cmdListH264VencConfig[j]->type;
            
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)h264VidEncCfg + 
                                    cmdListH264VencConfig[j]->location);
          break;
#endif
#ifdef OMTB_FF_VPS_FCC
        case OMTB_VPSS_VFCC_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryVPSSVfccConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListVPSSVfccConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListVPSSVfccConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListVPSSVfccConfig[j]->format;
          }
          else
            paramType = cmdListVPSSVfccConfig[j]->type;
            
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)vpssHdVfccCfg + 
                                    cmdListVPSSVfccConfig[j]->location);
          break;
#endif
#ifdef OMTB_FF_VPS_FDC
        case OMTB_VPSS_VFDC_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryVPSSVfdcConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListVPSSVfdcConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListVPSSVfdcConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListVPSSVfdcConfig[j]->format;
          }
          else
            paramType = cmdListVPSSVfdcConfig[j]->type;
            
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)vpssHdVfdcCfg + 
                                    cmdListVPSSVfdcConfig[j]->location);
          break;
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY          
        case OMTB_V4L2_DISPLAY_CLASS:
          
          fprintf(templateCfgFp, "%s ", cmdEntryDisplayConfig.name);
          fprintf(templateCfgFp, "%s ", cmdListDispConfig[j]->name);
          
          /* Store the parameter type */
          if(NULL != cmdListDispConfig[j]->format)
          {
            /* Used later for getting equivalent string for enum values */
            paramType = MX_DBG_CMD_OPT_ENUM;
            enumTbl = cmdListDispConfig[j]->format;
          }
          else
            paramType = cmdListDispConfig[j]->type;
          
          /* Pointer to the configuration parameter */
          paramLocation = (void *)((void *)dssCfg + 
                                    cmdListDispConfig[j]->location);
          break;
#endif        
        default:
          
          paramType = OMTB_INVALID_PARAM;
          PRINT(PRINT_DEBUG_MESSAGE, "%s: Invalid class entry\n",\
                                    __func__);
          break;
          
      } // switch(i)
      
      /* Dispaly the value according to the parameter type */
      switch(paramType)
      {

        case MX_DBG_CMD_TYPE_VAR_SINT8:
          {
            SINT8*      pSint8 = (SINT8*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pSint8);
            break;
          }
          
        case MX_DBG_CMD_TYPE_VAR_SINT16:
          {
            SINT16* pSint16 = (SINT16*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pSint16);
            break;
          }
          
        case MX_DBG_CMD_TYPE_VAR_SINT32:
          {
            SINT32* pSint32 = (SINT32*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pSint32);
            break;
          }
          
        case MX_DBG_CMD_TYPE_VAR_UINT8:
          {
            UINT8* pUint8 = (UINT8*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pUint8);
            break;
          }
          
        case MX_DBG_CMD_TYPE_VAR_UINT16:
          {
            UINT16* pUint16 = (UINT16*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pUint16);
            break;
          }
          
        case MX_DBG_CMD_TYPE_VAR_UINT32:
          {
            UINT32* pUint32 = (UINT32*)paramLocation;
            fprintf(templateCfgFp, "%d\n", *pUint32);
            break;
          }
        
        case MX_DBG_CMD_TYPE_VAR_STR_BUF:
          {
            char *pStr = (char*)paramLocation;
            fprintf(templateCfgFp, "%s\n", pStr);
            break;
          }
        
        case MX_DBG_CMD_OPT_ENUM:
          {
            int enumCount = 0;
            UINT32* pUint32 = (UINT32*)paramLocation;
            
            /* Get the corresponding string for enum values from enum table */
            while(strcmp(enumTbl[enumCount].name , ""))
            {
              if( enumTbl[enumCount].value == *pUint32)
                break;
                
              enumCount++;
            }
            fprintf(templateCfgFp, "%s\n", enumTbl[enumCount].name);
            break;  
          }  
          
        case OMTB_INVALID_PARAM:
        default:
          fprintf(templateCfgFp, "\n");
          PRINT(PRINT_DEBUG_MESSAGE, "%s: Invalid parameter\n",\
                                    __func__);                                    
          break;
          
      } // switch(paramType)
      
    } // for(j = 0; j < numEntries - 1; j++)
    
  } // for(i = 0; i < setpCmdTbl.max_entries - 1; i++)
  
  /* Close the cfg file */
  fclose(templateCfgFp);
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_LoadCfg Function Definition
\******************************************************************************/
/*
 *
 *  @brief    This function loads the configuration into specified template 
 *            from file.
 *
 *  @param in:
 *            pTemplate: Pointer to OMTB_TEMPLATE_INFO structure
 *
 *            cfgFileName: Pointer to configuration file name
 *
 *            template: Template no to which cfg is to be loaded
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/file name.
 */

MX_Result OMTB_LoadCfg( OMTB_TEMPLATE_INFO *pTemplate, 
                    char *cfgFileName,
                    unsigned int template)
{
  FILE *templateCfgFp = NULL;
  char inputCmdLine[OMTB_MAX_LEN] = {'\0'};
  unsigned int nBytes = 0;
  MX_Result result = ERR_NOERR;
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }
  
  /* Allocate the template memory if it is not already allocated */
  result = OMTB_AddCfg(template);
  
  if(ERR_NOERR != result)
    return result;

  /* Open the cfg file */
  templateCfgFp = fopen(cfgFileName, "rb");
  
  if(NULL == templateCfgFp)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error in opening cfg file: %s\n",\
                                __func__, cfgFileName);
    return ERR_INVNAME;
  }
  
  while(OMTB_LOOP)
  {
    /* required to load specific template */
    nBytes = sprintf(inputCmdLine, "omx setp %d ", template);

    if(OMX_FALSE >= nBytes)
    {
      PRINT(PRINT_ERR_MSG, "%s: Error in appending string to form command\n",\
                                __func__);

      /* Close the cfg file */
      fclose(templateCfgFp);
      
      return ERR_INVNAME;
    }
    
    /* read the commands from the file */
    nBytes = OMTB_ReadScript(templateCfgFp, &(inputCmdLine[nBytes]));
    if(OMX_FALSE == nBytes)
    {
      break;
    }      
    else
    {
      /* Dispaly the command on the command prompt */
      PRINT(PRINT_MUST, "\r     \nOMTB> %s\n", inputCmdLine);

      /* parse and process the command */
      procCmdLine(inputCmdLine);
      continue;
    }
    
  } // while(OMTB_LOOP)
  
  /* Close the cfg file */
  fclose(templateCfgFp);
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_AddCfg Function Definition
\******************************************************************************/
/*
 *
 *  @brief    
 *            This function allocates the memory for the specified template.
 *
 *  @param in:
 *            template: Template no which is to be allocated.
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

MX_Result OMTB_AddCfg(unsigned int template)
{
  MX_Result result = ERR_NOERR;
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }
  
  /* Allocate the template memory if it is not already allocated */
  if(NULL == omtbCfgTemplate[template])
  {
    omtbCfgTemplate[template]= (OMTB_TEMPLATE_INFO*)
                  TIMM_OSAL_Malloc( sizeof(OMTB_TEMPLATE_INFO),
                                    TIMM_OSAL_TRUE,
                                    0,
                                    TIMMOSAL_MEM_SEGMENT_EXT);

    if(NULL == omtbCfgTemplate[template])
    {
      PRINT(PRINT_ERR_MSG, "%s: Error allocating config params memory\n",\
                                    __func__);
      return ERR_INVNAME;
    }
    PRINT(PRINT_DEBUG_MESSAGE, "%s: Template %d memory allocated\n", 
                                __func__, template);
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE, "%s: Template %d memory already allocated\n", 
                                __func__, template);
    return ERR_NOERR;
  }
  
  /* Set the template to default values */
  result = OMTB_SetTemplateDefVal(template);
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error configuring the template params\n",\
                                    __func__);
    return (ERR_INVNAME);
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_RemoveCfg Function Definition
\******************************************************************************/
/*
 *
 *  @brief    
 *            This function releases the memory of the specified template.
 *
 *  @param in:
 *            template: Template no which is to be removed from memory
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/file name.
 */

MX_Result OMTB_RemoveCfg(unsigned int template)
{
  if(OMTB_MAX_TEMPLATE <= template)
  {
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }
  
  /* Release the memory allocated for the specified template */
  if(NULL != omtbCfgTemplate[template])
  {
    TIMM_OSAL_Free(omtbCfgTemplate[template]);
    omtbCfgTemplate[template] = NULL;
    PRINT(PRINT_DEBUG_MESSAGE, "%s: Template %d memory released\n", __func__,
                                template);
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CpuLoadDispTask Function Definition
\******************************************************************************/
/*
 *
 *  @brief    
 *            This thread prints the CPU load once in a second.
 *
 *  @param in:
 *            threadArg: Thread arguments
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            None
 */

static void *OMTB_CpuLoadDispTask(void *threadArg)
{

  PRINT(PRINT_DEBUG_MESSAGE,
                 "Entering %s thread\n", __func__);

#ifndef WTSD_TEMP
  while (OMTB_LOOP) 
  {
    if (armCpuLoadDispFlag) 
    {
      #if defined(OMTB_QNX) || defined(OMTB_LINUX)
      
        OMTB_GetCpuStatsDuration();
        
      #else
      
        OMTB_GetCpuStatsDuration(0);
        
      #endif /*defined(OMTB_QNX) || defined(OMTB_LINUX)*/
    }
    else 
    {
 
       TIMM_OSAL_SleepTask (1);
       
    }
    
  } // while(OMTB_LOOP)

  PRINT(PRINT_DEBUG_MESSAGE,
                 "Leaving %s thread\n", __func__);

  /* exit the thread */
  TIMM_OSAL_ExitTask(cpuLoadDispThreadId, NULL);
#endif
}


/******************************************************************************\
 *      OMTB_GetCpuStatsDuration Function Definition
\******************************************************************************/
/*
 *
 * \brief 
 *    Displays the % of CPU usage once in a second for the duration of secs+1.
 * 
 *    This API reads the /proc/stat CPU usage and idle time between a second and
 *    calculates the the % of CPU usage.
 *    The first row of this file has the numbers identify the amount of time 
 *    the CPU has spent performing different kinds of work.
 *  
 *    The first four numbers of this row (except the string "CPU") is idetified 
 *    as follows.
 *      1. user: normal processes executing in user mode 
 *      2. nice: niced processes executing in user mode 
 *      3. system: processes executing in kernel mode 
 *      4. idle: twiddling thumbs 
 *
 * \param secs
 *        None - For OMTB_QNX and OMTB_LINUX
 *        secs - Number of seconds this function to display the CPU load.
 * \return 
 *        None
 */
 
#ifdef OMTB_QNX

static void OMTB_GetCpuStatsDuration(void)
{
  char buf[10] = {"\0"};
  char commandStr[OMTB_MAX_LEN] = {"\0"};
  static FILE *ptr = NULL;
  
  PRINT(PRINT_DEBUG_MESSAGE,
                "Entering %s function\n", __func__);

  sprintf(commandStr, "hogs -n -s 1| awk '{if($1 == %u)print$4}'"
          ">temph.c & ", getpid());

  system (commandStr);

  TIMM_OSAL_SleepTask(3u);
  system("slay hogs");

  if (NULL != (ptr = fopen("temph.c", "r")))
  {

    while(NULL != fgets(buf, sizeof(buf), ptr))
    {

      PRINT(PRINT_MUST, "ARM CPU usage: %s\n",buf);
      break;
    }
    fclose(ptr);
  }

  PRINT(PRINT_DEBUG_MESSAGE,
                "Leaving %s function\n", __func__);
  return;
}

#elif defined(OMTB_LINUX)

static void OMTB_GetCpuStatsDuration(void)
{
  
  PRINT(PRINT_MUST,
              "\nUse 'top' from another shell for CPU stats\n");

  armCpuLoadDispFlag = OMX_FALSE;

  return;
}
 
#else

static void OMTB_GetCpuStatsDuration(int secs)
{
  static FILE *fp = NULL;
  unsigned int u1,n1,s1,i1,w1,x1,y1, tUsage, tIdle;
  unsigned int u2,n2,s2,i2,w2,x2,y2;
  int num;
  /* enough for a /proc/stat CPU line (not the intr line) */
  char buf[50];

  do 
  {
    if (!fp) 
    {
  
      if (!(fp = fopen("/proc/stat", "r"))) 
      {
        PRINT(PRINT_MUST, "Failed /proc/stat open: %s", strerror(errno));  
        /* note: we allocate one more CPU_t than Cpu_tot so that the last slot*/
        /* can hold tics representing the /proc/stat cpu summary (the first   */
        /* line read) -- that slot supports our View_CPUSUM toggle            */
           
        TIMM_OSAL_SleepTask(secs);

        return;
      }
    }
  
    rewind(fp);
    fflush(fp);
    
    /* first value the last slot with the cpu summary line */
    if (!fgets(buf, sizeof(buf), fp))
    {
      fprintf(stderr, "failed /proc/stat read");
    }
    num = sscanf(buf, "cpu %u %u %u %u %u %u %u",
           &u1,
           &n1,
           &s1,
           &i1,
           &w1,
           &x1,
           &y1
           );
    if (num < 4)
    {
      fprintf(stderr, "failed /proc/stat read");
    }
  
    TIMM_OSAL_SleepTask(1);
      
    rewind(fp);
    fflush(fp);
    
    /* first value the last slot with the cpu summary line */
    if (!fgets(buf, sizeof(buf), fp))
    {
      fprintf(stderr, "failed /proc/stat read");
    }
    num = sscanf(buf, "cpu %u %u %u %u %u %u %u",
           &u2,
           &n2,
           &s2,
           &i2,
           &w2,
           &x2,
           &y2
           );
    if (4 > num)
    {
      fprintf(stderr, "failed /proc/stat read");
    }
  
    tUsage = (u2 - u1) + (n2 - n1) + (s2 - s1);
  
    tIdle = (i2 - i1);
  
    PRINT(PRINT_MUST,
               "ARM Cpu Usage %.2f\n", (double)(tUsage * 100) / (tUsage+tIdle));
  
    
  } while(secs--);

  fclose(fp);
}

#endif /*OMTB_QNX OMTB_LINUX*/

/*----------------------------- end of file ---------------------------------*/
