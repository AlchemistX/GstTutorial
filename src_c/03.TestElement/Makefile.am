plugin_LTLIBRARIES = libgsttest.la

# sources used to compile this plug-in
libgsttest_la_SOURCES = \
	gstfakeadec.c \
	plugin.c

# compiler and linker flags used to compile this plugin, set in configure.ac
libgsttest_la_CFLAGS = $(GST_CFLAGS)
libgsttest_la_LIBADD = $(GST_LIBS)
libgsttest_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgsttest_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = \
	gstfakeadec.h
