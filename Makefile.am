# Process this file with automake to produce Makefile.in

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
libdomx_la_LIBADD       = mmplatform/src/libipc_setup.la system/domx/libsrc.la

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
if BUILD_TESTS
  SUBDIRS += test/camera test/h264dec test/h264play test/h264enc test/jpegdec test/sample_proxy

  bin_PROGRAMS = omx_camera_test omx_h264dec_test omx_h264play_test omx_h264enc_test omx_jpegdec_test omx_sample_test

  omx_camera_test_SOURCES      =
  omx_camera_test_LDFLAGS      = -no-undefined
  omx_camera_test_LDADD        = test/camera/libsrc.la libdomx.la

  omx_h264play_test_SOURCES     =
  omx_h264play_test_LDFLAGS     = -no-undefined
  omx_h264play_test_LDADD       = test/h264play/libsrc.la libdomx.la

  omx_h264dec_test_SOURCES     =
  omx_h264dec_test_LDFLAGS     = -no-undefined
  omx_h264dec_test_LDADD       = test/h264dec/libsrc.la libdomx.la

  omx_h264enc_test_SOURCES     =
  omx_h264enc_test_LDFLAGS     = -no-undefined
  omx_h264enc_test_LDADD       = test/h264enc/libsrc.la libdomx.la

  omx_jpegdec_test_SOURCES     =
  omx_jpegdec_test_LDFLAGS     = -no-undefined
  omx_jpegdec_test_LDADD       = test/jpegdec/libsrc.la libdomx.la

  omx_sample_test_SOURCES      =
  omx_sample_test_LDFLAGS      = -no-undefined
  omx_sample_test_LDADD        = test/sample_proxy/libsrc.la libdomx.la
endif
