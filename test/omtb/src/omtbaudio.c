/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbAudio.c
 *
 *  @brief    OMTB System Test facilitator functions for Audio devices.
 *
 * <B> History: </B>
 *
 *    # July-7-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>

/* OMX standard header files */
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbDebug.h"
#ifdef WTSD
#include "timm_osal_interfaces.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
#endif

#include "omtbCommon.h"



/******************************************************************************
 *      Function Definitions
 ******************************************************************************/


/******************************************************************************\
 *      OMTB_MonoToStereo Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function performs mono to stereo conversion.
 *
 *  @param    stereoSamples : Pointer to stereo samples buffer 
 *
 *            monoSamples   : Pointer to mono samples buffer
 *
 *            numSamples    : No of samples in mono buffer
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

void OMTB_MonoToStereo(short *stereoSamples, short *monoSamples, int numSamples)
{
  int i = 0;
  
  for (i = 0; i < numSamples; i++)
  {
    *stereoSamples++ = *monoSamples;
    *stereoSamples++ = 0;
    monoSamples++;
  }
}


/******************************************************************************\
 *      OMTB_AudioSetSampleRate Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function sets the audio playback sample rate based on the 
 *            information received from application.
 *
 *  @param    aud   : Pointer to OmtbAudInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_AudioSetSampleRate(OmtbAudInfo *aud)
{
  return OMTB_AudioControl(aud); 
}


/******************************************************************************\
 *      OMTB_AudioControl Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function configures the audio playback hardware based on the 
 *            configuration received from application.
 *
 *  @param    aud   : Pointer to OmtbAudInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_AudioControl(OmtbAudInfo *aud)
{
  int err = OMTB_SUCCESS;
  unsigned int period = 0;
  unsigned int bufferTime = 0,periodTime = 0;  
  int var = 0, tmp = 0; 
  int frameBytes = 0; 
  snd_pcm_uframes_t buffSize = 0;
  snd_pcm_uframes_t periodFrames = 0, bufferFrames = 0;
  
  aud->access = SND_PCM_ACCESS_RW_INTERLEAVED; 

  // Fill the params with the defaults 
  if ((err = snd_pcm_hw_params_any (aud->handle, aud->hwParams)) < 0) 
  {
    PRINT(PRINT_ERR_MSG,\
          "cannot initialize hardware parameter structure (%s)\n",\
           snd_strerror (err));
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_malloc, successful\n");
  }

  // Set the access type to interleaved 
  if ((err = snd_pcm_hw_params_set_access (aud->handle, aud->hwParams,
                                           aud->access)) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "cannot set access type (%s)\n",snd_strerror (err));
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_set_access, successful\n");
  }

  // Set the format to 16 bit little endian 
  if ((err = snd_pcm_hw_params_set_format (aud->handle, aud->hwParams,
                                           aud->format )) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "cannot set sample format (%s)\n",snd_strerror (err));
      return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_set_format, successful\n");
  }
  
  aud->sampleSize = snd_pcm_format_width(aud->format);
  // Set the rate 
  if ((err = snd_pcm_hw_params_set_rate_near(aud->handle, aud->hwParams,
                                             &(aud->sampleRate), 0)) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "cannot set sample rate (%s)\n",snd_strerror (err));
     return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_set_rate_near, successful\n");
  }

  // Set the number of channels 
  if ((err = snd_pcm_hw_params_set_channels (aud->handle, aud->hwParams,
                                             aud->numChannels)) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "cannot set channel count (%s)\n",snd_strerror (err));
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_set_channels, successful\n");
  }
   
  if (bufferTime == 0 && bufferFrames == 0) 
  {
    err = snd_pcm_hw_params_get_buffer_time_max(aud->hwParams,&bufferTime, 0);
    assert(err >= 0);
    
    if (bufferTime > 500000)
      bufferTime = 500000;
  }
  
  if (periodTime == 0 && periodFrames == 0) 
  {
    if (bufferTime > 0)
      periodTime = bufferTime / 4;
    else
      periodFrames = bufferFrames / 4;
  }
  
  if (periodTime > 0)
    err = snd_pcm_hw_params_set_period_time_near(aud->handle, aud->hwParams,
                                                 &periodTime, 0);
  else
    err = snd_pcm_hw_params_set_period_size_near(aud->handle, aud->hwParams,
                                                 &periodFrames, 0);
  
  assert(err >= 0);
  if (bufferTime > 0) 
  {
    err = snd_pcm_hw_params_set_buffer_time_near(aud->handle, aud->hwParams,
                                                 &bufferTime, 0);
  } 
  else 
  {
    err = snd_pcm_hw_params_set_buffer_size_near(aud->handle, aud->hwParams,
                                                 &bufferFrames);
  }

  // Prepare the PCM interface with the parameters chosen 
  if ((err = snd_pcm_hw_params (aud->handle, aud->hwParams)) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "cannot set hw parameters (%s)\n",snd_strerror (err));
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params, successful\n");
  }

  if(NULL != aud->buff1)
  {
    TIMM_OSAL_CMEM_Free(aud->buff1,aud->buffSize);
    aud->buff1 = NULL; 
  }
  
  if(NULL != aud->buff2)
  {
    TIMM_OSAL_CMEM_Free(aud->buff2,aud->buffSize);
    aud->buff2 = NULL;
  }
    
  if ( (err = snd_pcm_hw_params_get_buffer_size ( aud->hwParams,&buffSize ))
        < 0)
  {
     PRINT(PRINT_ERR_MSG,\
           "get buffer size failed (%s)\n",  snd_strerror (err));
    return OMTB_FAIL;
  }

  if ( (err = snd_pcm_hw_params_get_periods ( aud->hwParams,&period,0 ))  < 0)
  {
     PRINT(PRINT_ERR_MSG,\
             "get period failed (%s)\n",  snd_strerror (err));
    return OMTB_FAIL;
  }
  
  frameBytes = (snd_pcm_format_width(aud->format) / 8) * aud->numChannels;
  aud->buffSize =  2*( frameBytes * (buffSize/period));

  if(aud->buffSize < aud->frameSize)
  {
    tmp = ( frameBytes * (buffSize/period));
    var = aud->frameSize/tmp;
    var +=( (aud->frameSize%tmp) != 0); 
    aud->buffSize = var*tmp; 
  }

//  aud->buff1 = (char*)Memory_contigAlloc(aud->buffSize,Memory_DEFAULTALIGNMENT);
  aud->buff1 = (char*)TIMM_OSAL_Malloc( 
                            aud->buffSize,
                            OMTB_CMEM_ALLOCATION,
                            0,
                            TIMMOSAL_MEM_SEGMENT_EXT);
//  aud->buff2 = (char*)Memory_contigAlloc(aud->buffSize,Memory_DEFAULTALIGNMENT);
  aud->buff2 = (char*)TIMM_OSAL_Malloc( 
                            aud->buffSize,
                            OMTB_CMEM_ALLOCATION,
                            0,
                            TIMMOSAL_MEM_SEGMENT_EXT);
  
  if(!aud->buff1 || !aud->buff2)
  {
     PRINT(PRINT_ERR_MSG,\
           "can not allocate memory for buff1 or buff2 \n");
    return OMTB_FAIL;
  }

  aud->buffPtr = &aud->buff1[0];
  aud->usedBuffer = 0;
  aud->bytesLeft = aud->buffSize;
  
  return OMTB_SUCCESS;
}


/******************************************************************************\
 *      OMTB_AudioSetSwParam Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function configures the audio playback software based on the 
 *            configuration received from application.
 *
 *  @param    aud   : Pointer to OmtbAudInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_AudioSetSwParam(OmtbAudInfo *aud)
{
  int err = OMTB_SUCCESS;
  snd_pcm_t *handle = aud->handle;
  snd_pcm_sw_params_t *swParams = aud->swParams;
  snd_pcm_uframes_t xferAlign = 0;
  snd_pcm_uframes_t startThreshold = 0, stopThreshold = 0;
  unsigned sleepMin = 0;
  snd_pcm_uframes_t chunkSize = 0, bufferSize = 0;
  size_t n = 0;
  int availMin = -1;
  int startDelay = 0;
  int stopDelay = 0;
  int rate = 0;

  snd_pcm_hw_params_get_period_size(aud->hwParams, &chunkSize, 0);
  snd_pcm_hw_params_get_buffer_size(aud->hwParams, &bufferSize);

  snd_pcm_sw_params_current(handle, swParams);
  err = snd_pcm_sw_params_get_xfer_align(swParams, &xferAlign);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "Unable to obtain xfer align: (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }

  if (sleepMin)
    xferAlign = 1;
  
  err = snd_pcm_sw_params_set_sleep_min(handle, swParams,sleepMin);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "Unable to set sleep min: (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }

  rate = aud->sampleRate; 
  
  if (availMin < 0)
    n = chunkSize;
  else
    n = (double) rate * availMin / 1000000;

  err = snd_pcm_sw_params_set_avail_min(handle, swParams, n);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "Unable to set avail min : (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }
  
  /* round up to closest transfer boundary */ 
  n = (bufferSize / xferAlign) * xferAlign;
  
  if (startDelay <= 0) 
  {
    startThreshold = n + (double) rate * startDelay / 1000000;
  }
  else
  {
    startThreshold = (double) rate * startDelay / 1000000;
  }
  
  if (startThreshold < 1)
    startThreshold = 1;
  
  if (startThreshold > n)
    startThreshold = n;
  
  err = snd_pcm_sw_params_set_start_threshold(handle,
                                              swParams,
                                              startThreshold);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
             "Unable to set start threshold : (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }
  
  if (stopDelay <= 0)
    stopThreshold = bufferSize + (double) rate * stopDelay / 1000000;
  else
    stopThreshold = (double) rate * stopDelay / 1000000;
  
  err = snd_pcm_sw_params_set_stop_threshold( handle,
                                              swParams,
                                              stopThreshold);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
           "Unable to set stop threshold : (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }

  err = snd_pcm_sw_params_set_xfer_align(handle, swParams, xferAlign);
  if (err < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
             "Unable to set xfer align : (%s) \n",snd_strerror (err));
    return OMTB_FAIL;
  }

  if (snd_pcm_sw_params(handle, swParams) < 0) 
  {
     PRINT(PRINT_ERR_MSG,\
             "Unable to set swParams \n");
    return OMTB_FAIL;
  }

  PRINT(PRINT_DEBUG_MESSAGE,"OMTB_AudioSetSwParam is done \n"); 
  return OMTB_SUCCESS;
}


/******************************************************************************\
 *  OMTB_AudInit Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function Initializes and configures the audio play as per the 
 *            audio device selected 
 *
 *  @param    disp  : Pointer to OmtbAudInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 */

OmtbRet OMTB_AudInit(OmtbAudInfo *aud)
{
  int err = OMTB_SUCCESS;
  
  aud->buff1 = NULL;
  aud->buff2 = NULL;

  PRINT(PRINT_FUNCTION_NAME, "Entering %s Function\n", __func__);

  
  if (0 == aud->device[0])
    strcpy(aud->device, DEF_SND_DEVICE);

  // Open the PCM interface 
  if ((err = snd_pcm_open( &(aud->handle), aud->device, aud->mode ,
                           aud->blockingMode)) < 0) 
  {
     PRINT(PRINT_ERR_MSG, "cannot open audio device (%s)\n",\
            snd_strerror (err));
    return OMTB_FAIL;
  } 
  else 
  {
     PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_open, successful\n");
  }
  
  // Allocate a PCM HW Param structure 
  if ((err = snd_pcm_hw_params_malloc ( &(aud->hwParams) )) < 0) 
  {
     PRINT(PRINT_ERR_MSG, "cannot allocate hardware parameter structure\
                                 (%s)\n",  snd_strerror (err));
    return OMTB_FAIL;
  } 
  else 
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_hw_params_malloc, successful\n");
  }
  if( OMTB_FAIL  == OMTB_AudioControl(aud) ) 
  {
     PRINT(PRINT_ERR_MSG, "Audio control failed   \n");
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"Audio control, successful\n");
  }
  if((err = snd_pcm_sw_params_malloc ( &(aud->swParams) )) < 0)
  {
     PRINT(PRINT_ERR_MSG, "cannot allocate software parameter structure\
                                 (%s)\n",   snd_strerror (err));
    return OMTB_FAIL;
  }
  else
  {
    PRINT(PRINT_DEBUG_MESSAGE,"snd_pcm_sw_params_malloc, successful\n");
  }
  if(SND_PCM_STREAM_CAPTURE != aud->mode)
    OMTB_AudioSetSwParam(aud);

  PRINT(PRINT_FUNCTION_NAME, "Leaving %s Function\n", __func__);
  
  return OMTB_SUCCESS;
}


/******************************************************************************\
 *      OMTB_AudioOut Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function plays the buffer passed by application 
 *
 *  @param    aud   : Pointer to OmtbAudInfo structure 
 *            buff  : Buffer to be played
 *            size  : Size of the buffer
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_AudioOut(OmtbAudInfo *aud, char *buff, int size)
{
  int err = OMTB_SUCCESS, left = 0;
  int frameByte = (snd_pcm_format_width(aud->format) / 8) * aud->numChannels; 
  int i = 0;

  PRINT(PRINT_FUNCTION_NAME, "Entering %s Function\n", __func__);
    
#if 1 
// start : logic to avoide under run
  if(size <= aud->bytesLeft)
  {
    memcpy(aud->buffPtr,buff,size);
    aud->buffPtr+=size;
    aud->bytesLeft -=size;
    size = 0;
  }
  else 
  {
    left = aud->bytesLeft;
    if(0!=left)
    {
      memcpy(aud->buffPtr,buff,left);
    }
    if(0 == aud->usedBuffer)
    {
      aud->buffPtr = aud->buff2;
      memcpy(aud->buffPtr,&buff[left],size-left);
      aud->buffPtr+=(size-left);
      aud->bytesLeft =  aud->buffSize- (size-left);
      aud->usedBuffer = 1;
      buff = aud->buff1;
    }
    else
    {
      aud->buffPtr = aud->buff1;
      memcpy(aud->buffPtr,&buff[left],size-left);
      aud->buffPtr+=(size-left);
      aud->bytesLeft = aud->buffSize - (size-left);
      aud->usedBuffer = 0;
      buff = aud->buff2;
    }

    size = aud->buffSize /frameByte;

  }
// end : logic to avoid underrun
#else
  size = size/frameByte;
#endif

  while(size)
  {
    err = snd_pcm_writei(aud->handle, buff, size );
    if( (err <0 ) && (err != -EAGAIN) )
    {
      if (err == -EPIPE) 
      {
        //TBD next release
        //PRINT(PRINT_ERR_MSG, "audio underrun\n");
        snd_pcm_prepare(aud->handle);
      }
      else
      {
         PRINT(PRINT_ERR_MSG,\
               "write to audio interface failed (%s) %d %d %d %d %d\n",
               snd_strerror (err),err,EAGAIN,EPIPE,EBUSY,EFAULT);
         return OMTB_FAIL;
      }
    }
    else
    {
      buff += err * frameByte ;
      size -=err;
    }
    if(size < 0)
    {
       PRINT(PRINT_ERR_MSG, "size can not be -ve  \n");
      return OMTB_FAIL;
    }
    i++;
  }

  PRINT(PRINT_FUNCTION_NAME, "Leaving %s Function\n", __func__);
  
  return OMTB_SUCCESS;
}


/******************************************************************************\
 *      OMTB_AudioIn Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function captures the audio & copies data to the buffer 
 *            passed by application.
 *
 *  @param    aud   : Pointer to OmtbAudInfo structure 
 *            buff  : Captured audio data buffer
 *            size  : Size of the buffer
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL
 *
 */

OmtbRet OMTB_AudioIn(OmtbAudInfo *aud, char *buff, int size)
{
  int err = OMTB_SUCCESS;
  int frameBytes = (snd_pcm_format_width(aud->format) / 8) * aud->numChannels;
  char *buffPtr = buff;
  
  size = size/frameBytes; // number of frames
  
  if(SND_PCM_NONBLOCK != aud->blockingMode)
  {
    do
    {
      err = snd_pcm_readi( aud->handle, buffPtr, size);
      if(0 < err)
      {
        buffPtr += err * frameBytes ;
        size -= err ;
      }
      else if ( -EPIPE == err)
      {
        PRINT(PRINT_ERR_MSG, "audio over run\n");
        snd_pcm_prepare(aud->handle);
      }
    }while( ((1 <= err)|| (-EPIPE == err) ) && (0 <  size ) );
  }
  else
  {
    while( (0 < size) && (0 <= err) )
    {
      do
      {
        err = snd_pcm_readi( aud->handle, buff, size)  ;
      }while( -EAGAIN == err);
      if(0 < err)
      {
        buff += err * frameBytes ;
        size -= err ;
      }
    }
  }

  if(0 > err)
  {
     PRINT(PRINT_ERR_MSG,\
            "read from audio interface failed (%s) %d \n",
             snd_strerror (err),err);
    return OMTB_FAIL;
  }
  
  return OMTB_SUCCESS;
}


/******************************************************************************\
 *      OMTB_AudioClose Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function dequeue all buffer present in video driver
 *
 *  @param    disp  : Pointer to OmtbAudInfo structure 
 *
 *  @return   OMTB_SUCCESS
 *            OMTB_FAIL           
 *
 */

OmtbRet OMTB_AudioClose(OmtbAudInfo *aud)
{
  // Free the param structure
  snd_pcm_drain(aud->handle);
  snd_pcm_hw_params_free (aud->hwParams);
  snd_pcm_sw_params_free (aud->swParams);
  if(aud->buff1)
  {
    TIMM_OSAL_CMEM_Free(aud->buff1,aud->buffSize);
  }
  if(aud->buff2)
  {
    TIMM_OSAL_CMEM_Free(aud->buff2,aud->buffSize);
  }
  // Close the PCM interface 
  snd_pcm_close (aud->handle);

  return OMTB_SUCCESS;
}


/*----------------------------- end of file ----------------------------------*/
