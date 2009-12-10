## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = ipc_setup.c mmplatform.c
libsrc_la_LIBADD        = @LTLIBOBJS@ $(SYSLINK_LIBS) $(MMOSAL_LIBS)
libsrc_la_CFLAGS        = -I$(top_srcdir)/system/domx $(OMXCORE_CFLAGS) $(SYSLINK_CFLAGS) $(MMOSAL_CFLAGS)

