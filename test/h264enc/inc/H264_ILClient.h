/*
*   @file  H264D_ILClient.h
*   This file contains the structures and macros that are used in the Application which tests the OpenMAX component
*
*  @path \WTSD_DucatiMMSW\omx\khronos1.1\omx_h264_dec\test
*
*  @rev 1.0
*/

#ifndef _H264_ILClient_
#define _H264_ILClient_


/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/**----- system and platform files ----------------------------**/

//#include <ti/sysbios/knl/Task.h>
//#include <xdc/runtime/System.h>

#include <stdio.h>
#include <string.h>

#include <timm_osal_interfaces.h>

#define H264ENC_TRACE_ENABLE
#ifdef H264ENC_TRACE_ENABLE
/*overwriting the default trcae flags*/
#define TIMM_OSAL_DEBUG_TRACE_DETAIL 0
#define TIMM_OSAL_DEBUG_TRACE_LEVEL 5
#endif
#include <timm_osal_trace.h>

/**-------program files ----------------------------------------**/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Video.h>
#include <OMX_IVCommon.h>
#include <OMX_TI_Video.h>
#include <OMX_TI_Index.h>
#include <OMX_TI_Common.h>



#define GOTO_EXIT_IF(_CONDITION,_ERROR) { \
            if((_CONDITION)) { \
                TIMM_OSAL_Error("%s : %s : %d :: ", __FILE__, __FUNCTION__,__LINE__);\
                TIMM_OSAL_Info(" Exiting because: %s \n", #_CONDITION); \
                eError = (_ERROR); \
                goto EXIT; \
            } \
}


#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)   \
 TIMM_OSAL_Memset((_s_), 0x0, sizeof(_name_)); 			\
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       	\
    (_s_)->nVersion.s.nStep   = 0x0;

/*Advanced Settings*/
typedef struct{
	OMX_U32 	nStartofSequence;	
	OMX_U32 	nEndofSequence;	
	OMX_U32 	nIDR;
	OMX_U32 	nIntraPicture;
	OMX_U32 	nNonIntraPicture;	
}NALUSettings;


typedef struct{
	OMX_U8 nNumSliceGroups;
	OMX_U8 nSliceGroupMapType; 
	OMX_VIDEO_SLICEGRPCHANGEDIRTYPE eSliceGrpChangeDir; 
	OMX_U32 nSliceGroupChangeRate; 
	OMX_U32 nSliceGroupChangeCycle; 
	OMX_U32 nSliceGroupParams[H264ENC_MAXNUMSLCGPS] ;
}FMOSettings;


typedef struct{
	OMX_VIDEO_BLOCKSIZETYPE eMinBlockSizeP;
	OMX_VIDEO_BLOCKSIZETYPE eMinBlockSizeB;
}MEBlockSize;


typedef struct{
	OMX_VIDEO_INTRAREFRESHTYPE eRefreshMode;
	OMX_U32 nRate;
}IntraRefreshSettings;


typedef struct{
	OMX_BOOL bAspectRatioPresent;
	OMX_VIDEO_ASPECTRATIOTYPE eAspectRatio;
	OMX_BOOL bFullRange;
}VUISettings;


typedef struct{
	OMX_U32	 nLumaIntra4x4Enable;	
	OMX_U32 nLumaIntra8x8Enable;	
	OMX_U32 nLumaIntra16x16Enable; 
	OMX_U32 nChromaIntra8x8Enable;
	OMX_VIDEO_CHROMACOMPONENTTYPE eChromaComponentEnable;
}IntrapredictionSettings;

typedef struct{	
	OMX_VIDEO_DATASYNCMODETYPE inputDataMode;
	OMX_VIDEO_DATASYNCMODETYPE outputDataMode;
	OMX_U32 numInputDataUnits;
	OMX_U32 numOutputDataUnits;
}H264EDataSyncSettings;



/*Dynamic Params Settings*/
typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nFramerate[10];	
}FrameRate;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nBitrate[10];
}BitRate;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nMVAccuracy[10];
	OMX_U32	 nHorSearchRangeP[10];
	OMX_U32	 nVerSearchRangeP[10];
	OMX_U32	 nHorSearchRangeB[10];
	OMX_U32	 nVerSearchRangeB[10];
}MESearchRange;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_BOOL ForceIFrame[10];
}ForceFrame;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nQpI[10];
	OMX_U32 nQpMaxI[10];
	OMX_U32 nQpMinI[10];
	OMX_U32 nQpP[10];
	OMX_U32 nQpMaxP[10];
	OMX_U32 nQpMinP[10];
	OMX_U32 nQpOffsetB[10];
	OMX_U32 nQpMaxB[10];
	OMX_U32	 nQpMinB[10];
}QpSettings;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nIntraFrameInterval[10];
}IntraFrameInterval;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nNaluSize[10];
}NALSize;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_VIDEO_AVCSLICEMODETYPE eSliceMode[10];
	OMX_U32	 nSlicesize[10];
}SliceCodingSettings;


typedef struct{
	OMX_S32 nFrameNumber[10];
	OMX_U32 nWidth[10];
	OMX_U32 nHeight[10];
}PixelInfo;





typedef struct H264E_TestCaseParamsAdvanced
{
	NALUSettings NALU;
	FMOSettings FMO;
	MEBlockSize MEBlock;
	IntraRefreshSettings IntraRefresh;
	VUISettings VUI;
	IntrapredictionSettings IntraPred;
	H264EDataSyncSettings DataSync;
}H264E_TestCaseParamsAdvanced;

typedef struct H264E_TestCaseParamsDynamic
{
	FrameRate DynFrameRate;
	BitRate DynBitRate;
	MESearchRange DynMESearchRange;
	ForceFrame DynForceFrame;
	QpSettings DynQpSettings;
	IntraFrameInterval DynIntraFrmaeInterval;
	SliceCodingSettings DynSliceSettings;
	PixelInfo DynPixelInfo;

}H264E_TestCaseParamsDynamic;


typedef enum H264E_TestType{
    FULL_RECORD = 0,
    PARTIAL_RECORD= 1
} H264E_TestType;


typedef struct H264E_TestCaseParams
{
	OMX_U8 TestCaseId; 
	char InFile[100];
	char OutFile[100];
	OMX_U32 width;
	OMX_U32 height;
	OMX_VIDEO_AVCPROFILETYPE profile;
	OMX_VIDEO_AVCLEVELTYPE level;
	OMX_COLOR_FORMATTYPE inputChromaFormat;
	OMX_VIDEO_FRAMECONTENTTYPE InputContentType;
	OMX_VIDEO_INTERLACE_CODINGTYPE InterlaceCodingType; 
	OMX_BOOL bLoopFilter;
	OMX_BOOL bCABAC;
	OMX_BOOL bFMO;
	OMX_BOOL bConstIntraPred;
	OMX_U8 nNumSliceGroups;
	OMX_U8 nSliceGroupMapType;
	OMX_VIDEO_AVCSLICEMODETYPE eSliceMode;
	OMX_VIDEO_CONTROLRATETYPE RateControlAlg;
	OMX_VIDEO_TRANSFORMBLOCKSIZETYPE TransformBlockSize;
	OMX_VIDEO_ENCODING_MODE_PRESETTYPE EncodingPreset;
	OMX_VIDEO_RATECONTROL_PRESETTYPE RateCntrlPreset;
	OMX_VIDEO_AVCBITSTREAMFORMATTYPE BitStreamFormat;
	OMX_U32 maxInterFrameInterval;	
	OMX_U32 nBitEnableAdvanced;
	OMX_U32 nBitEnableDynamic;
	OMX_BOOL bInAllocatebuffer;
	OMX_BOOL bOutAllocatebuffer;
	OMX_U8 TestType;
	OMX_U8 StopFrameNum;	
}H264E_TestCaseParams;

typedef struct H264E_ILClient{
	OMX_HANDLETYPE pHandle;
	OMX_STATETYPE eState;
	OMX_ERRORTYPE eAppError;
	
	OMX_BUFFERHEADERTYPE	** pInBuff;
	OMX_BUFFERHEADERTYPE	**pOutBuff;

	OMX_PTR IpBuf_Pipe;
	OMX_PTR OpBuf_Pipe;

	FILE* fIn;
	FILE* fOut;
	H264E_TestCaseParams* H264_TestCaseParams;

}H264E_ILClient;


typedef struct OMX_H264E_DynamicConfigs{
	OMX_CONFIG_FRAMERATETYPE tFrameRate;
	OMX_VIDEO_CONFIG_BITRATETYPE tBitRate;
	OMX_VIDEO_CONFIG_MESEARCHRANGETYPE tMESearchrange;
	OMX_CONFIG_INTRAREFRESHVOPTYPE tForceframe;
	OMX_VIDEO_CONFIG_QPSETTINGSTYPE tQPSettings;
	OMX_VIDEO_CONFIG_AVCINTRAPERIOD tAVCIntraPeriod;
	OMX_VIDEO_CONFIG_SLICECODINGTYPE  tSliceSettings;
	OMX_VIDEO_CONFIG_PIXELINFOTYPE tPixelInfo;
}OMX_H264E_DynamicConfigs;


#endif /*_H264_ILClient_ */



