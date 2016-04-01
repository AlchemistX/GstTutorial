bin_PROGRAMS = 02_GStreamerConcepts-@GST_API_VERSION@

noinst_PROGRAMS = 02_GStreamerConcepts-@GST_API_VERSION@-debug

bin_SCRIPTS =

AM_CFLAGS = $(GST_ALL_CFLAGS) $(GST_PBUTILS_CFLAGS) $(GST_VIDEO_CFLAGS)
LDADD = $(GST_PBUTILS_LIBS) $(GST_LIBS) $(GST_VIDEO_LIBS)

02_GStreamerConcepts_@GST_API_VERSION@_SOURCES = 02.GStreamerConcepts.c
02_GStreamerConcepts_@GST_API_VERSION@_CFLAGS = $(GIO_CFLAGS) $(AM_CFLAGS)
02_GStreamerConcepts_@GST_API_VERSION@_LDADD = $(GIO_LIBS) $(LDADD)

02_GStreamerConcepts_@GST_API_VERSION@_debug_SOURCES = 02.GStreamerConcepts.c
02_GStreamerConcepts_@GST_API_VERSION@_debug_CFLAGS = $(GIO_CFLAGS) $(AM_CFLAGS)
02_GStreamerConcepts_@GST_API_VERSION@_debug_LDADD = $(GIO_LIBS) $(LDADD)
02_GStreamerConcepts_@GST_API_VERSION@_debug_LDFLAGS = -no-install

