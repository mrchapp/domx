/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbDisplay.h
 *
 *  @brief    This file contains display driver related macros and structures
 *
 * <B> History: </B>
 *
 *  -# June-13-2009   AVME Syste Test    Inital Version
 *
 */

#ifndef _OMTB_DISPLAY_H_
#define _OMTB_DISPLAY_H_


/******************************************************************************\
*     Includes
\******************************************************************************/

#include <sys/time.h>
#include <linux/videodev.h>


/******************************************************************************/
 
#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
*     Defines
\******************************************************************************/

#define DEF_BPP                     (16)            // Default Bits Per Pixel
#define MAX_COUNT                   (16)            // Max num of Buffers
#define VGA_XRES                    (640)           // VGA X resolution
#define VGA_YRES                    (480)           // VGA Y resolution
#define D1_PAL_XRES                 (720)           // D1 X resolution
#define D1_PAL_YRES                 (576)           // D1 Y resolution
#define DVI_XRES                    (1280)          // VGA X resolution
#define DVI_YRES                    (720)           // VGA Y resolution
#define DEF_XRES                    (720)           // Default X resolution
#define DEF_YRES                    (576)           // Default Y resolution
#define OUTPUT_ID                   (1)
#define SCALING_ON                  (1)
#define DEF_VIDFRAME_SIZE           (VID_FRAME_SIZE(DEF_XRES, DEF_YRES,DEF_BPP))
#define MAX_BUFFER                  
#define MAX_PITCH                   (1920)
#define MAX_HEIGHT                  (1080)

#define VID_DEVICE1                 ("/dev/video1")
#define VID_DEVICE2                 ("/dev/video2")

#define SYSFS_OUTPUT_VIDEO1         \
              ("/sys/class/display_control/omap_disp_control/video1")
#define SYSFS_OUTPUT_VIDEO2         \
              ("/sys/class/display_control/omap_disp_control/video2")
#define SYSFS_OUTPUT_CH0_OUTPUT     \
              ("/sys/class/display_control/omap_disp_control/ch0_output")
#define SYSFS_OUTPUT_CH1_OUTPUT     \
              ("/sys/class/display_control/omap_disp_control/ch1_output")
#define SYSFS_OUTPUT_CH0_MODE       \
              ("/sys/class/display_control/omap_disp_control/ch0_mode")
#define SYSFS_OUTPUT_CH1_MODE       \
              ("/sys/class/display_control/omap_disp_control/ch1_mode")
#define SYSFS_LCD_ALPHABLEND        \
              ("/sys/class/display_control/omap_disp_control/lcd_alphablend")
#define SYSFS_TV_ALPHABLEND         \
              ("/sys/class/display_control/omap_disp_control/tv_alphablend")
#define SYSFS_GFX_GLOBALALPHA       \
              ("/sys/class/display_control/omap_disp_control/gfx_global_alpha")
#define SYSFS_VID2_GLOBALALPHA      \
              ("/sys/class/display_control/omap_disp_control/vid2_global_alpha")
#define SYSFS_LCD_BACKLIGHT         \
              ("/sys/class/display_control/omap_disp_control/lcdbacklight")


/******************************************************************************\
*     Data type definitions
\******************************************************************************/

typedef enum
{
  OMTB_DISP_LCD,
  OMTB_DISP_SVIDEO,
  OMTB_DISP_DVI
} OmtbDispOut;


/* Enum consisting of different resolution */

typedef enum
{
  OMTB_QCIF_PAL,   //176x144
  OMTB_QCIF_NTSC,  //176x140
  OMTB_CIF_PAL,    //352x288
  OMTB_CIF_NTSC,   //352x240
  OMTB_PAL,        //720x576
  OMTB_NTSC,       //720x480
} OmtbDispFormat;


/* structure used to store information of the buffers */

struct buf_info
{
  int index;
  unsigned int length;
  char *start;
};


/* structure holding the display control information */

typedef struct OmtbDispInfo_T
{

  char device[MAX_COUNT];               /* Name of the VPBE device */
  OmtbDispOut dispOutput;
  int display_fd;                       /* FD for the VPBDE device */
  unsigned int xres;                    /* FrameBuffer X resolution */
  unsigned int yres;                    /* FrameBuffer Y resolution */
  unsigned int bpp;                     /* Bits per pixel */
  unsigned int fbsize;                  /* Size in bytes of FrameBuffer/Video
                                           frame */
  unsigned int width;                   /* Width of image to be displayed */
  unsigned int height;                  /* Height of image to be displayed */
  unsigned int maxBuffers;
  unsigned int numbuffers;
  int scaling;
  int rotate;
  char chMode[MAX_COUNT];
  struct v4l2_requestbuffers req;
  int output_idx;
  OmtbDispFormat std;
  struct v4l2_buffer buf;
  struct v4l2_output output;
  struct buf_info buffInfo[MAX_COUNT];
  int a;
  unsigned int queriedDisplayWidth;
  unsigned int queriedDisplayHeight;
  unsigned int queriedDisplayBytesPerLine;
  unsigned int userbuffer;
  char *userbufferptr[MAX_COUNT];
  unsigned int userbufferlen[MAX_COUNT];
  
} OmtbDispInfo;


/******************************************************************************\
*     Macros
\******************************************************************************/


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
 *      OMTB_DispInit Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function initializes the display
 *
 *  @param    disp  : Pointer to OMX OmtbDispInfo
 *
 *  @return  OMTB_FAIL
 *           OMTB_SUCCESS
 *
 */

OmtbRet OMTB_DispInit(OmtbDispInfo *disp);


/******************************************************************************\
 *      OMTB_DisplayBuf Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function Displays the buffer passed by application 
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *            buff  : Buffer to be displayed
 *
 *  @return  OMTB_SUCCESS
 *           OMTB_FAIL
 *
 */

OmtbRet OMTB_DisplayBuf(OmtbDispInfo *disp, void *buff);


/******************************************************************************\
 *      OMTB_DispClose Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function dequeue all buffer present in video driver
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return  OMTB_SUCCESS
 *           OMTB_FAIL           
 *
 */

OmtbRet OMTB_DispClose(OmtbDispInfo *disp);


/******************************************************************************\
 *      OMTB_GetDispBuff Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function gets the display buffer from the driver queue
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   Pointer to the buffer
 *
 */

void *OMTB_GetDispBuff(OmtbDispInfo *disp);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/
