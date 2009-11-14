/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/

/** @file       omtbCommon.h
 *
 *  @brief      This file contains common structures required across the
 *              components.
 *
 * <B> History: </B>
 *
 *    -# May-11-2009  AVME System Test     Inital Version
 *
 */

#ifndef __OMTB_COMMON_H__
#define __OMTB_COMMON_H__

/******************************************************************************\
*     Includes
\******************************************************************************/

#include "omtbMain.h"

#ifdef OMTB_FF_VD_H264
#include "omtbH264Vdec.h"
#endif
#ifdef OMTB_FF_AD_G711
#include "omtbG711Adec.h"
#endif
#ifdef OMTB_FF_VE_H264
#include "omtbH264Venc.h"
#endif
#ifdef OMTB_FF_VPS_FCC
#include "omtbHDVPSSVfcc.h"
#endif
#ifdef OMTB_FF_VPS_FDC
#include "omtbHDVPSSVfdc.h"
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
#include "omtbDisplay.h"
#endif
#ifdef OMTB_FF_DRIVER_AUDIO
#include "omtbAudio.h"
#endif

/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
*     Data type definitions
\******************************************************************************/

/*!
 *******************************************************************************
 *  \struct OMTB_TEMPLATE_INFO
 *  \brief  Common Global structure used all component configuration
 *******************************************************************************
*/
typedef struct OMTB_TEMPLATE_INFO_T 
{
  #ifdef OMTB_FF_VD_H264
  OMTB_H264_VDEC_CONFIG_PARAMS h264VdecTemplate;/* H264 vdec app cfg struct */
  #endif

  #ifdef OMTB_FF_AD_G711
  OMTB_G711_ADEC_CONFIG_PARAMS g711AdecTemplate;/* G711 adec app cfg struct */
  #endif

  #ifdef OMTB_FF_VE_H264
  OMTB_H264_VENC_CONFIG_PARAMS h264VencTemplate;/* H264 venc app cfg struct */
  #endif

  #ifdef OMTB_FF_VPS_FCC
  OMTB_VPSS_VFCC_CONFIG_PARAMS vpssVfccTemplate;/* VPSS VFCC app cfg struct */
  #endif

  #ifdef OMTB_FF_VPS_FDC
  OMTB_VPSS_VFDC_CONFIG_PARAMS vpssVfdcTemplate;/* VPSS VFDC app cfg struct */
  #endif

  OmtbDispInfo v4l2DispTemplate;                /* V4L2 driver app cfg struct */
  OmtbAudInfo  alsaPlayTemplate;                /* ALSA driver app cfg struct */
  unsigned int currTemplate;                    /* Current template number */
}OMTB_TEMPLATE_INFO;

/*!
 *******************************************************************************
 *  \struct OMTB_DATA_THREAD_ARG
 *  \brief  Streaming thread data structure
 *******************************************************************************
 */
typedef struct _OmtbDataThreadArg
{
  #ifdef OMTB_FF_VD_H264
  OMTB_H264_VDEC_PRIVATE* h264VdecClient;     /* H264 vdec app pvt data */
  #endif

  #ifdef OMTB_FF_AD_G711
  OMTB_G711_ADEC_PRIVATE* g711AdecClient;     /* G711 adec app pvt data */
  #endif
  
  #ifdef OMTB_FF_VE_H264
  OMTB_H264_VENC_PRIVATE* h264VencClient;     /* H264 vdec app pvt data */
  #endif
  
  #ifdef OMTB_FF_VPS_FCC
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient;     /* VPSS VFCC app pvt data */
  #endif
  
  #ifdef OMTB_FF_VPS_FDC
  OMTB_VPSS_VFDC_PRIVATE* vpssVfdcClient;     /* VPSS VFDC app pvt data */
  #endif
  #ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient;                    /* V4L2 driver app pvt data */
  #endif
  #ifdef OMTB_FF_DRIVER_AUDIO
  OmtbAudInfo  *audPlayDrvClient;             /* ALSA driver app pvt data */
  #endif
}OMTB_DATA_THREAD_ARG;

/*!
 *******************************************************************************
 *  \struct OMTB_COMP_HANDLE
 *  \brief  Structure for openmax component handle to class name mapping,
 *  rewuired to support multiple instance
 *******************************************************************************
 */
typedef struct _OMTB_COMP_HANDLE_T 
{
  OMX_HANDLETYPE handle[OMTB_MAX_INS];            /* OMX component handle */
  
  #ifdef OMTB_FF_VD_H264
  struct OMTB_H264_VDEC_PRIVATE *h264VdecClient[OMTB_MAX_INS];
                                                  /* H264 vdec app pvt data */
  #endif

  #ifdef OMTB_FF_AD_G711
  struct OMTB_G711_ADEC_PRIVATE *g711AdecClient[OMTB_MAX_INS];
                                                  /* G711 adec app pvt data */
  #endif
  
  #ifdef OMTB_FF_VE_H264
  struct OMTB_H264_VENC_PRIVATE *h264VencClient[OMTB_MAX_INS];
                                                  /* H264 venc app pvt data */
  #endif
  
  #ifdef OMTB_FF_VPS_FCC
  struct OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient[OMTB_MAX_INS];
                                                  /* VPSS vfcc app pvt data */
  #endif
  
  #ifdef OMTB_FF_VPS_FDC
  struct OMTB_VPSS_VFDC_PRIVATE *vpssVfdcClient[OMTB_MAX_INS];
                                                  /* VPSS vfdc app pvt data */
  #endif
  
  #ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient[OMTB_MAX_INS];          /* V4L2 driver app pvt data */
  #endif
  
  #ifdef OMTB_FF_DRIVER_AUDIO
  OmtbAudInfo  *audPlayDrvClient[OMTB_MAX_INS];   /* ALSA driver app pvt data */
  #endif
  
  void *execThreadId[OMTB_MAX_INS];               /* Execution Thread ID  */
  unsigned int componentInstanceNo;               /* Component instance no */
}OMTB_COMP_HANDLE;

/* Different OMX Classes */
typedef enum _OMX_CLASS
{

  OMTB_H264_VDEC_CLASS,      /* H264 decoder identifier */
  OMTB_G711_ADEC_CLASS,      /* G711 decoder identifier */
  OMTB_H264_VENC_CLASS,      /* H264 encoder identifier */
  OMTB_VPSS_VFCC_CLASS,      /* VPSS VFCC identifier */
  OMTB_VPSS_VFDC_CLASS,      /* VPSS VFDC identifier */
  OMTB_V4L2_DISPLAY_CLASS,   /* V4L2 Display Driver identifier */
  OMTB_G711_VENC_CLASS,      /* G711 encoder identifier */
  OMTB_MAX_CLASS = OMTB_V4L2_DISPLAY_CLASS,

}OMX_CLASS;


/******************************************************************************\
*     Macros
\******************************************************************************/

#define OMTB_CMEM_ALLOCATION      (101)   /* CMEM allocation flag for OSAL */

/******************************************************************************\
*     Globals
\******************************************************************************/



/******************************************************************************\
*     External Object Definitions
\******************************************************************************/


/******************************************************************************\
*     Public Function Prototypes
\******************************************************************************/

/******************************************************************************\
 *      OMTB_SetHeader Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function sets the OMX Version header information
 *
 *  @param    header: Pointer to OMX structure
 *            Size  : Size of OMX Structure
 *
 *  @return   None
 *
 */
void OMTB_SetHeader(OMX_PTR header, unsigned int size);


/******************************************************************************\
 *      H264VDEC_GetHandle Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function invokes the getHandle API
 *
 *  @param in:
 *            classPtr         : Pointer to OMTB_COMP_HANDLE struct
 *
 *            unsigned int : Instance no of the component
 *
 *  @return   OMX_ERRORTYPE : OMX standard error code
 *
 */
#ifdef OMTB_FF_VD_H264
OMX_ERRORTYPE H264VDEC_GetHandle(OMTB_COMP_HANDLE * , unsigned int );
#endif

/******************************************************************************\
 *      G711ADEC_GetHandle Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Get the handle for G711 Audio Decode component
 *
 *  @param in:
 *            classPtr: Pointer to OMTB_COMP_HANDLE structure
 *
 *            unsigned int : Instance no of the component
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE G711ADEC_GetHandle(OMTB_COMP_HANDLE * , unsigned int );


/******************************************************************************\
 *      H264VENC_GetHandle Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function invokes the getHandle API
 *
 *  @param in:
 *            classPtr         : Pointer to OMTB_COMP_HANDLE struct
 *
 *            unsigned int : Instance no of the component
 *
 *  @return   OMX_ERRORTYPE : OMX standard error code
 *
 */
#ifdef OMTB_FF_VE_H264
OMX_ERRORTYPE H264VENC_GetHandle(OMTB_COMP_HANDLE * , unsigned int );
#endif

/******************************************************************************\
 *      VPSSVFCC_GetHandle Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function invokes the getHandle API
 *
 *  @param in:
 *            classPtr         : Pointer to OMTB_COMP_HANDLE struct
 *
 *            unsigned int : Instance no of the component
 *
 *  @return   OMX_ERRORTYPE : OMX standard error code
 *
 */

#ifdef OMTB_FF_VPS_FCC
OMX_ERRORTYPE VPSSVFCC_GetHandle(OMTB_COMP_HANDLE * , unsigned int );
#endif

/******************************************************************************\
 *      VPSSVFDC_GetHandle Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function invokes the getHandle API
 *
 *  @param in:
 *            classPtr         : Pointer to OMTB_COMP_HANDLE struct
 *
 *            unsigned int : Instance no of the component
 *
 *  @return   OMX_ERRORTYPE : OMX standard error code
 *
 */

#ifdef OMTB_FF_VPS_FDC
OMX_ERRORTYPE VPSSVFDC_GetHandle(OMTB_COMP_HANDLE * , unsigned int );
#endif

/******************************************************************************\
 *      OMTB_LoadCfg Function Definition
\******************************************************************************/
/*
 *
 *  @brief   This function loads the configuration into specified template 
 *           from file.
 *
 *  @param in:
 *            pProfile: Pointer to OMTB_TEMPLATE_INFO structure
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

MX_Result OMTB_LoadCfg( OMTB_TEMPLATE_INFO *pProfile, 
                               char *cfgFileName,
                               unsigned int template);


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
 *            pProfile: Pointer to OMTB_TEMPLATE_INFO structure
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

MX_Result OMTB_StoreCfg(OMTB_TEMPLATE_INFO *pProfile, 
                                char *cfgFileName,
                                unsigned int template);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif

/*--------------------------------- end of file ------------------------------*/

