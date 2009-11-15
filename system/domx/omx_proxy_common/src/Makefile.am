## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = omx_proxy_common.c
libsrc_la_LIBADD        = \
	$(MMOSAL_LIBS) \
	$(OMXCORE_LIBS) \
	$(D2CMAP_LIBS) \
	$(MEMMGR_LIBS) \
	$(SYSLINK_LIBS) \
	@LTLIBOBJS@
libsrc_la_CFLAGS        = \
	-I$(top_srcdir)/system/domx \
	-I$(top_srcdir)/system/domx/omx_rpc/inc \
	$(MMOSAL_CFLAGS) \
	$(OMXCORE_CFLAGS) \
	$(D2CMAP_CFLAGS) \
	$(MEMMGR_CFLAGS) \
	$(SYSLINK_CFLAGS)

