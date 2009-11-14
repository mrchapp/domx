/*
*   @file  H264_ILClien_BasicSettings.c
*   This file contains the structures and macros that are used in the Application which tests the OpenMAX component
*
*  @path \WTSD_DucatiMMSW\omx\khronos1.1\omx_h264_dec\test
*
*  @rev 1.0
*/

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/**----- system and platform files ----------------------------**/

/**-------program files ----------------------------------------**/
#include "H264_ILClient.h"


H264E_TestCaseParams H264_TestCaseTable[1]= {
  {
  0,
  "./input/foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "./output/foreman_p176x144_30fps_420pl_300fr_nv12.264",/*output file*/
  176,
  144,
  OMX_VIDEO_AVCProfileHigh ,
  OMX_VIDEO_AVCLevel41,
  OMX_COLOR_FormatYUV420PackedSemiPlanar,
  0,/*progressive or interlace*/
  0,/*interlace type*/
  0,/*bloopfillter*/
  0,/*bCABAC*/
  0,/*bFMO*/
  0,/*bconstIntrapred*/
  0,/*slicegrps*/
  0,/*slicegrpmap*/
  0,/*slice mode*/
  0,/*rate cntrl*/
  1,/*transform block size*/
  2,/*Encoder preset*/
  4,/*RC Preset*/
  0,/*Bit Stream Select*/
  1,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  0,/*flag_Input_allocate buffer*/
  0,/*flag_Output_allocate buffer*/
  1,/*test type*/
  4/*stop frame number in case of partial encode*/
  }
};



