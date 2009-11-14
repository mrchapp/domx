/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/*
 * FILE:   omtbAudio.h
 *
 * Brief:  AVME System Test functions for Audio driver
 *
 * <B> History: </B>
 *
 * -# June-13-2009   AVME Syste Test                 Inital Version
 *
 */

#ifndef __OMTB_AUDIO_H__
#define __OMTB_AUDIO_H__


/******************************************************************************\
*     Includes
\******************************************************************************/

#include <alsa/asoundlib.h>


/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
*     Defines
\******************************************************************************/

#define DEF_SND_DEVICE            ("plughw:0,0")
#define DEF_SAMPLE_SIZE           (16)            // Default Sample size 
#define DEF_SAMPLE_RATE           (8000)          // Default Sample Rate 
#define DEF_NUM_CHANNELS          (2)             // Default number of channels
#define Memory_DEFAULTALIGNMENT   ((UInt)(-1))

#define AUD_FRAME_SIZE(_rate, _ch, _size)       ((_rate) * (_ch) * (_size) / 8)

typedef struct
{
  char device[OMTB_MAX_LEN];             // Name of the audio device
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *hwParams;
  snd_pcm_sw_params_t *swParams;
  unsigned int sampleRate;          // Number of samples per second
  int sampleSize;                   // Size of each sample in bits
  int numChannels;                  // Number of channels 
  snd_pcm_stream_t mode;            // Operating mode - capture/play
  snd_pcm_format_t format;  
  snd_pcm_access_t access;
  int blockingMode;
  char *buff1;
  char *buff2;
  char *buffPtr;
  int usedBuffer;
  int bytesLeft;
  int buffSize;
  int frameSize;
} OmtbAudInfo;


/******************************************************************************\
*     Public Function Prototypes
\******************************************************************************/

OmtbRet OMTB_AudInit(OmtbAudInfo *aud);

OmtbRet OMTB_AudioIn(OmtbAudInfo *aud, char *buff, int size);

OmtbRet OMTB_AudioOut(OmtbAudInfo *aud, char *buff, int size);

OmtbRet OMTB_AudioControl(OmtbAudInfo *aud);

OmtbRet OMTB_AudioClose(OmtbAudInfo *aud);

OmtbRet OMTB_AudioSetSwParam(OmtbAudInfo *aud);

OmtbRet OMTB_AudioSetSampleRate(OmtbAudInfo *aud);

void OMTB_MonoToStereo(short *stereoSamples, short *monoSamples, int numSample); 


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/
