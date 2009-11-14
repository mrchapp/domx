/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbDisplay.c
 *
 *  @brief    This file handles the display driver interfaces.
 *
 * <B> History: </B>
 *
 *    # June-11-2009    AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbMain.h"
#include "omtbDebug.h"
#include "omtbDisplay.h"
#ifdef WTSD
#include "timm_osal_interfaces.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
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
 *      OMTB_DispCleanup Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function unmaps the driver buffers mapped to user space
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   None
 *
 */

static void OMTB_DispCleanup(OmtbDispInfo *disp);


/******************************************************************************\
 *      OMTB_WriteSysFs Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function writes to SysFs.
 *
 *  @param    fileName  : Pointer to file name
 *
 *            val       : Pointer to string 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 */

static OmtbRet OMTB_WriteSysFs(char *fileName, char *val); 


/******************************************************************************
 *      Function Definitions
 ******************************************************************************/


/******************************************************************************\
 *  OMTB_DispInit Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function Initializes and configures the display as per the 
 *            display device selected 
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 */

OmtbRet OMTB_DispInit(OmtbDispInfo *disp)
{
  OmtbRet retval = OMTB_FAIL;
  struct v4l2_format fmt;
  int receivedBufsCount = 0;
  int i = 0, j = 0;
  char str[OMTB_MAX_LEN] = {"\0"};

  if(NULL == disp)
   {
     PRINT(PRINT_SIMPLE_MESSAGE,"Disp Pointer had Null Value\n");
     return retval;
   }
   
  switch(disp->dispOutput)
  {
    case OMTB_DISP_SVIDEO:
      PRINT(PRINT_SIMPLE_MESSAGE,"Setting SVIDEO Display Out \n");
      strcpy(str, "channel1");
      if(OMTB_WriteSysFs(SYSFS_OUTPUT_VIDEO1,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set video1 pipeline to channel0\n");
        exit(0);
      }
      /* Set the output of channel1 to SVIDEO */
      strcpy(str, "SVIDEO");
      if(OMTB_WriteSysFs(SYSFS_OUTPUT_CH1_OUTPUT,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set output to SVIDEO\n");
        exit(0);
      }
      break;
    case OMTB_DISP_LCD:
      PRINT(PRINT_SIMPLE_MESSAGE,"Setting LCD Display Out \n");
      strcpy(str, "channel0");
      if(OMTB_WriteSysFs(SYSFS_OUTPUT_VIDEO1,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set video1 pipeline to channel0\n");
        exit(0);
      }
      /* Set the output of channel0 to LCD */
      strcpy(str, "LCD");

      if(OMTB_WriteSysFs(SYSFS_OUTPUT_CH0_OUTPUT,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set output to LCD\n");
        exit(0);
      }
      break;
    case OMTB_DISP_DVI:
      PRINT(PRINT_SIMPLE_MESSAGE,"Setting DVI Display Out \n");

      strcpy(str, "channel0");
      if(OMTB_WriteSysFs(SYSFS_OUTPUT_VIDEO1,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set video1 pipeline to channel0\n");
        exit(0);
      }
      /* Set the output of channel0 to DVI */
      strcpy(str, "DVI");
      if(OMTB_WriteSysFs(SYSFS_OUTPUT_CH0_OUTPUT,str))
      {
        PRINT(PRINT_ERR_MSG, "Cannot set output to DVI\n");
        exit(0);
      }
      break;
    default:
      PRINT(PRINT_SIMPLE_MESSAGE,"Invalid Display Selected\n");
      return (OMTB_FAIL);
  }

  if (0 == disp->device[0])
    strcpy(disp->device, VID_DEVICE1);

  if (-1  == (disp->display_fd= open(disp->device, O_RDWR)))
  {
    PRINT(PRINT_ERR_MSG, "Unable to initialize video device:%s\n",\
                              disp->device);
    return (retval);
  }
  PRINT(PRINT_SIMPLE_MESSAGE,"Initialized video device:%s\n ",disp->device);
  /* Set Format */

  fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  fmt.fmt.pix.width = disp->width;
  fmt.fmt.pix.height = disp->height;
  fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
  if (ioctl(disp->display_fd, VIDIOC_S_FMT, &fmt) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to get video format\n");
    OMTB_DispClose(disp);
    return retval;
  }
  fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;    
  if (ioctl(disp->display_fd, VIDIOC_G_FMT, &fmt) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to get video format \n");
    OMTB_DispClose(disp);
    return retval;
  }
  if(!disp->userbuffer)
  {
    // Allocate the buffers 
    disp->req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    disp->req.count = disp->maxBuffers;
    disp->req.memory = V4L2_MEMORY_MMAP;
    PRINT(PRINT_SIMPLE_MESSAGE,"Max buffer %d \n",disp->req.count);
    if (ioctl(disp->display_fd, VIDIOC_REQBUFS, &disp->req) < 0)
    {
      PRINT(PRINT_ERR_MSG, "Unable to allocate memory\n");
      OMTB_DispClose(disp);
      return retval;
    }
    if( disp->req.count < 2) /* Driver should allocate minimum two buffers */
    {
      PRINT(PRINT_ERR_MSG, "Driver allocated only %d buffers, \
                                 it should allocate at lease 2 buffer\n",\
                                 disp->req.count);
      OMTB_DispClose(disp);
      return retval;
    }
  
    PRINT(PRINT_SIMPLE_MESSAGE,"buffer count %d \n",disp->req.count);
    receivedBufsCount = disp->req.count;
    PRINT(PRINT_SIMPLE_MESSAGE,"Received buf count = %d\n", receivedBufsCount);

    /* Get the buffer information and mmap them */
    for (i=0; i < disp->req.count; i++)
    {
      disp->buf.index = i;
      disp->buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
      disp->buf.memory = V4L2_MEMORY_MMAP;
      if (ioctl(disp->display_fd, VIDIOC_QUERYBUF, &disp->buf) < 0)
      {
        PRINT(PRINT_ERR_MSG, "Unable to query buffer\n");
        OMTB_DispClose(disp);
        return retval;
      }

      PRINT(PRINT_SIMPLE_MESSAGE,"Size of the buffer = %d\n", disp->buf.length);
      disp->buffInfo[i].length = disp->buf.length;
      disp->buffInfo[i].index = i;
      disp->buffInfo[i].start = mmap(NULL, disp->buf.length, 
                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                 disp->display_fd, disp->buf.m.offset);
      /* Set display to gray */
      memset(disp->buffInfo[i].start, 0x80, disp->buf.length);

      if((unsigned int) disp->buffInfo[i].start == MAP_SHARED)
      {
        PRINT(PRINT_ERR_MSG, "Unable to mmap = %d buffer\n", i);
        for (j = 0; j < i; j++)
        {
          munmap(disp->buffInfo[j].start, disp->buffInfo[j].length);
        }
        OMTB_DispClose(disp);
        return retval;
      }
    }

    for(i=0; i<disp->req.count; i++)
    {
      disp->buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
      disp->buf.memory = V4L2_MEMORY_MMAP;
      disp->buf.index = i;
      if (ioctl(disp->display_fd, VIDIOC_QBUF, &disp->buf) < 0)
      {
        PRINT(PRINT_ERR_MSG, "Unable to queue the %d th buffer\n", i);
        for (j = 0; j < disp->req.count; j++)
        {
          munmap(disp->buffInfo[j].start, disp->buffInfo[j].length);
        }
        OMTB_DispCleanup(disp);
        return retval;
      }
    }
  }
  else
  {
    disp->req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    disp->req.count = disp->numbuffers;
    disp->req.memory = V4L2_MEMORY_USERPTR;
    PRINT(PRINT_SIMPLE_MESSAGE,"Max buffer %d \n",disp->req.count);
    if (ioctl(disp->display_fd, VIDIOC_REQBUFS, &disp->req) < 0)
    {
      PRINT(PRINT_ERR_MSG, "Unable to allocate memory\n");
      OMTB_DispClose(disp);
      return retval;
    }
    if( disp->req.count < 2) // Driver should allocate minimum two buffers
    {
      PRINT(PRINT_ERR_MSG, "Driver allocated only %d buffers, it should \
allocate at lease 2 buffer \n",disp->req.count);
      OMTB_DispClose(disp);
      return retval;
    }
    receivedBufsCount = disp->req.count;
    for(i=0; i<disp->req.count; i++)
    {

      disp->buffInfo[i].length = disp->userbufferlen[i];
      disp->buffInfo[i].index = i;
      disp->buffInfo[i].start = (char *)disp->userbufferptr[i];
      /* Set display to gray */
      memset(disp->buffInfo[i].start, 0x80, disp->buffInfo[i].length);

      disp->buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
      disp->buf.memory = V4L2_MEMORY_USERPTR ;
      disp->buf.index = i;
      disp->buf.m.userptr = (unsigned long )disp->userbufferptr[i];
      disp->buf.length = disp->userbufferlen[i];
      if (ioctl(disp->display_fd, VIDIOC_QBUF, &disp->buf) < 0)
      {
        PRINT(PRINT_ERR_MSG, "Unable to queue the %d th buffer\n", i);
        for (j = 0; j < disp->req.count; j++)
        {
          munmap(disp->buffInfo[j].start, disp->buffInfo[j].length);
        }
        OMTB_DispCleanup(disp);
        return retval;

      }
    }
  }
  
  /* Start Disply */
  disp->a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  if (ioctl(disp->display_fd, VIDIOC_STREAMON, &disp->a) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to STREAMON video\n");
    OMTB_DispCleanup(disp);
  }
  else
  {
    retval = OMTB_SUCCESS;
  }

  return retval;
}


/******************************************************************************\
 *      OMTB_GetDispBuff Function Definition
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

void *OMTB_GetDispBuff(OmtbDispInfo *disp)
{

  PRINT(PRINT_FUNCTION_NAME,"In __FUNC__\n");

  disp->buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  if(disp->userbuffer)
  {
    disp->buf.memory = V4L2_MEMORY_USERPTR;
  }
  else
  {
    disp->buf.memory = V4L2_MEMORY_MMAP;
  }

  /* Get the empty buffer */
  if (ioctl(disp->display_fd, VIDIOC_DQBUF, &disp->buf) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to DQBUF %d %s \n",errno,strerror(errno));
    OMTB_DispCleanup(disp);
  }
  else
  {
    return (disp->buffInfo[disp->buf.index].start);
  }

  PRINT(PRINT_FUNCTION_NAME,"Leaving __FUNC__\n");

  return NULL;
}


/******************************************************************************\
 *      OMTB_DisplayBuf Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function Displays the buffer passed by application 
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *            buff  : Buffer to be displayed
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_DisplayBuf(OmtbDispInfo *disp, void *buff)
{
  int i = 0, index = -1;
  OmtbRet retval = OMTB_FAIL;
  
  PRINT(PRINT_FUNCTION_NAME,"Entering __FUNC__\n");

  if(disp == NULL || buff == NULL)
  {
    PRINT(PRINT_ERR_MSG, "Pointer is NULL");
    return (retval);
  }
  
  memset(&disp->buf, 0, sizeof(disp->buf));
  for(i=0; i<disp->req.count; i++)
  {
    if(buff == disp->buffInfo[i].start)
    {
      index = disp->buffInfo[i].index;
      break;
    }
  }
  disp->buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  disp->buf.index = index;
  if(disp->userbuffer)
  {
    disp->buf.memory = V4L2_MEMORY_USERPTR ;
    disp->buf.m.userptr =(unsigned long ) buff;
    disp->buf.length = disp->userbufferlen[index];

  }
  else
  {
    disp->buf.memory = V4L2_MEMORY_MMAP;
  }
  /* Enqueue filled buffer */
  if (ioctl(disp->display_fd, VIDIOC_QBUF, &disp->buf) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to QBUF.\n");
    OMTB_DispCleanup(disp);
  }
  else
  {
    retval = OMTB_SUCCESS;
  }

  PRINT(PRINT_FUNCTION_NAME,"Leaving __FUNC__\n");

  return retval;
}


/******************************************************************************\
 *      OMTB_CloseDes Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function closes the file descriptor of display device 
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   None
 *
 */

static void OMTB_CloseDes(OmtbDispInfo *disp)
{
  if (disp->display_fd != OMTB_FAIL)
    close(disp->display_fd);
    
  disp->display_fd = OMTB_FAIL;
}


/******************************************************************************\
 *      OMTB_DispCleanup Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function unmaps the driver buffers mapped to user space
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   None
 *
 */

static void OMTB_DispCleanup(OmtbDispInfo *disp)
{
  int j = 0;
  if(!disp->userbuffer)
  {
    for ( j = 0; j < disp->req.count; j++)
      munmap(disp->buffInfo[j].start, disp->buffInfo[j].length);
  }
  OMTB_CloseDes(disp);
}


/******************************************************************************\
 *      OMTB_DispClose Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function dequeue all buffer present in video driver
 *
 *  @param    disp  : Pointer to OmtbDispInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL           
 *
 */

OmtbRet OMTB_DispClose(OmtbDispInfo *disp)
{
  OmtbRet retval = OMTB_FAIL;

  if(disp == NULL)
  {
    PRINT(PRINT_ERR_MSG, "Pointer is NULL");
    return (retval);
  }

  if (ioctl(disp->display_fd, VIDIOC_STREAMOFF, &disp->a) < 0)
  {
    PRINT(PRINT_ERR_MSG, "Unable to stream off video\n");
    return (OMTB_FAIL);
  }

  OMTB_DispCleanup(disp);

  return (OMTB_SUCCESS);
}


/******************************************************************************\
 *      OMTB_WriteSysFs Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function writes to SysFs.
 *
 *  @param    fileName  : Pointer to file name
 *
 *            val       : Pointer to string 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 */

static OmtbRet OMTB_WriteSysFs(char *fileName, char *val) 
{
    FILE *fp = NULL;
    char *valString = NULL;

    valString = (char*)TIMM_OSAL_Malloc( strlen(val) + 1,
                                         TIMM_OSAL_TRUE, 
                                         0, 
                                         TIMMOSAL_MEM_SEGMENT_EXT);
    if(NULL == valString) 
    {
      PRINT(PRINT_ERR_MSG, "Failed to allocate memory\
                            for temporary string\n");
      return OMTB_FAIL;
    }

    fp = fopen(fileName, "w");

    if(NULL == fp)
    {
      PRINT(PRINT_ERR_MSG, "Failed to open %s for writing\n", fileName);
      TIMM_OSAL_Free(valString);
      return OMTB_FAIL;
    }

    if(1 != fwrite(val, strlen(val) + 1, 1, fp)) 
    {
      PRINT(PRINT_ERR_MSG, "Failed to write sysfs variable %s to %s\n",\
                                  fileName, val);
      fclose(fp);
      TIMM_OSAL_Free(valString);
      return OMTB_FAIL;
    }

    fclose(fp);
    fp = fopen(fileName, "r");
    fread(valString, 1, strlen(val) + 1, fp);
    if(0 != ferror(fp)) 
    {
      PRINT(PRINT_ERR_MSG, "Failed to read sysfs value from %s\n",\
                                  fileName);
      fclose(fp);
      TIMM_OSAL_Free(valString);
      return OMTB_FAIL;
    }
    fclose(fp); 
      
    valString[strlen(val)] = 0;

    if(0 != strcmp(valString, val)) 
    {
      PRINT(PRINT_ERR_MSG, "Failed to verify sysfs variable %s to %s\
                                  (is %s)\n", fileName, val, valString);
      TIMM_OSAL_Free(valString);
      return OMTB_FAIL;
    }

    TIMM_OSAL_Free(valString);
    return OMTB_SUCCESS;
}


/*----------------------------- end of file ----------------------------------*/
