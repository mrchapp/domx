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
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_DynamicFramerate_NALConfig_bitrate_F.264",/*output file*/
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
  1,/*bit enable_advanced*/
  11,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  10/*stop frame number in case of partial encode*/
  }
};







#if 0
H264E_TestCaseParams H264_TestCaseTable[17]= {
  {
  0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_420pl_300fr_nv12.264",/*output file*/
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  10/*stop frame number in case of partial encode*/
  },
  {
  1 
  },
  {
  2
  },
  {
  3
  },
  {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_420pl_300fr_nv12.264",/*output file*/
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
  10/*stop frame number in case of partial encode*/
  },
  {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_420pl_300fr_nv12.264",/*output file*/
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
  {
 6,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_420pl_300fr_nv12.264",/*output file*/
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  10/*stop frame number in case of partial encode*/
  },
{
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_CABAC_2B_16frames.264",/*output file*/
  176,
  144,
  OMX_VIDEO_AVCProfileHigh ,
  OMX_VIDEO_AVCLevel41,
  OMX_COLOR_FormatYUV420PackedSemiPlanar,
  0,/*progressive or interlace*/
  0,/*interlace type*/
  0,/*bloopfillter*/
  1,/*bCABAC*/
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
 {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_CABAC_ZeroB_16frames.264",/*output file*/
  176,
  144,
  OMX_VIDEO_AVCProfileHigh ,
  OMX_VIDEO_AVCLevel41,
  OMX_COLOR_FormatYUV420PackedSemiPlanar,
  0,/*progressive or interlace*/
  0,/*interlace type*/
  0,/*bloopfillter*/
  1,/*bCABAC*/
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
  {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_CABAC_LF_ZeroB_16frames.264",/*output file*/
  176,
  144,
  OMX_VIDEO_AVCProfileHigh ,
  OMX_VIDEO_AVCLevel41,
  OMX_COLOR_FormatYUV420PackedSemiPlanar,
  0,/*progressive or interlace*/
  0,/*interlace type*/
  1,/*bloopfillter*/
  1,/*bCABAC*/
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
 
  {
 0,
  "input\\crew_p352x288_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\crew_p352x288_30fps2B_16frames.264",/*output file*/
  352,
  288,
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
   {
 0,
  "input\\18WintertimeRiver_p720x480_30fps_420pl_450frames.yuv",/*input file*/
  "output\\18WintertimeRiver_p720x480_ZeroB_16frames.264",/*output file*/
  720,
  480,
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
   {
 0,
 "input\\18WintertimeRiver_p720x480_30fps_420pl_450frames.yuv",/*input file*/
  "output\\18WintertimeRiver_p720x480_2B_16frames.264",/*output file*/
  720,
  480,
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
   {
 0,
  "input\\sriverbed_p1920x1080_30fps_420pl_30fr.yuv",/*input file*/
  "output\\sriverbed_p1920x1080_ZeroB_16frames.264",/*output file*/
  1920,
  1080,
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  1,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
   {
 0,
 "input\\sriverbed_p1920x1080_30fps_420pl_30fr.yuv",/*input file*/
  "output\\sriverbed_p1920x1080_2B_16frames.264",/*output file*/
  1920,
  1080,
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  0,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
  {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_full_zeroB.264",/*output file*/
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
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  0,/*test type*/
  16/*stop frame number in case of partial encode*/
  },
 {
 0,
  "input\\foreman_p176x144_30fps_420pl_300fr_nv12.yuv",/*input file*/
  "output\\foreman_p176x144_30fps_fullencode_2Bframes.264",/*output file*/
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
  3,/*max interframe interval*/
  0,/*bit enable_advanced*/
  0,/*bit enable_dynamic*/
  1,/*flag_Input_allocate buffer*/
  1,/*flag_Output_allocate buffer*/
  0,/*test type*/
  16/*stop frame number in case of partial encode*/
  }
  

  
};
#endif






