## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = \
	omx_rpc_config.c \
	omx_rpc_platform.c \
	omx_rpc_skel.c \
	omx_rpc_stub.c \
	omx_rpc_utils.c \
	omx_rpc.c

libsrc_la_LIBADD        = \
	$(MMOSAL_LIBS) \
	$(OMXCORE_LIBS) \
	$(SYSLINK_LIBS) \
	@LTLIBOBJS@

libsrc_la_CFLAGS        = \
	-I$(top_srcdir)/system/domx \
	-I$(top_srcdir)/system/domx/omx_rpc/inc \
	$(MMOSAL_CFLAGS) \
	$(OMXCORE_CFLAGS) \
	$(SYSLINK_CFLAGS)

