/* ======================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ======================================================================= */
/**
* @file dss_lib.h
*
* This File contains definitions which are required to render the frames to LCD
*
* @path  domx/test/camera/inc
*
* @rev  0.1
*/
/* ----------------------------------------------------------------------------
*!
*! Revision History
*! =======================================================================
*!  Initial Version
* ========================================================================*/
struct dss_buffers {
	void *start;
	size_t length;
} ;

int open_video1();
int SendbufferToDss(int index, int vid1_fd);
int SetFormatforDSSvid(unsigned int width, unsigned int height,
				const char *image_fmt, int vid1_fd);
uint getDSSBuffers(uint count, struct dss_buffers *buffers, int vid1_fd);
