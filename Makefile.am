## Process this file with automake to produce Makefile.in

SUBDIRS = \
	mmplatform/src \
	system/domx \
	video/omx_proxy_component/src
	

lib_LTLIBRARIES = \
	libdomx.la \
	libOMX.TI.DUCATI1.VIDEO.H264E.la \
	libOMX.TI.DUCATI1.VIDEO.H264D.la \
	libOMX.TI.DUCATI1.VIDEO.CAMERA.la \
	libOMX.TI.DUCATI1.IMAGE.JPEGD.la \
	libOMX.TI.DUCATI1.MISC.SAMPLE.la


# core DOMX:
libdomx_la_SOURCES      = 
libdomx_la_LDFLAGS      = -no-undefined
libdomx_la_LIBADD       = mmplatform/src/libsrc.la system/domx/libsrc.la

# proxy components:
libOMX_TI_DUCATI1_VIDEO_H264E_la_SOURCES  =
libOMX_TI_DUCATI1_VIDEO_H264E_la_LDFLAGS  = -no-undefined
libOMX_TI_DUCATI1_VIDEO_H264E_la_LIBADD   = video/omx_proxy_component/src/libh264e.la libdomx.la

libOMX_TI_DUCATI1_VIDEO_H264D_la_SOURCES  =
libOMX_TI_DUCATI1_VIDEO_H264D_la_LDFLAGS  = -no-undefined
libOMX_TI_DUCATI1_VIDEO_H264D_la_LIBADD   = video/omx_proxy_component/src/libh264d.la libdomx.la

libOMX_TI_DUCATI1_VIDEO_CAMERA_la_SOURCES =
libOMX_TI_DUCATI1_VIDEO_CAMERA_la_LDFLAGS = -no-undefined
libOMX_TI_DUCATI1_VIDEO_CAMERA_la_LIBADD  = video/omx_proxy_component/src/libcamera.la libdomx.la

libOMX_TI_DUCATI1_IMAGE_JPEGD_la_SOURCES  =
libOMX_TI_DUCATI1_IMAGE_JPEGD_la_LDFLAGS  = -no-undefined
libOMX_TI_DUCATI1_IMAGE_JPEGD_la_LIBADD   = video/omx_proxy_component/src/libjpegd.la libdomx.la

libOMX_TI_DUCATI1_MISC_SAMPLE_la_SOURCES  =
libOMX_TI_DUCATI1_MISC_SAMPLE_la_LDFLAGS  = -no-undefined
libOMX_TI_DUCATI1_MISC_SAMPLE_la_LIBADD   = video/omx_proxy_component/src/libsample.la libdomx.la

# test apps:
#if BUILD_TESTS
#  # TODO:  add test apps..
#  SUBDIRS               += test
#  bin_PROGRAMS           = foo
#  foo_SOURCES            =
#  foo_LDFLAGS            = -no-undefined
#  foo_LDADD              = test/libfoo.la
#endif

