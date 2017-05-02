EXTRA_DIST = 

AM_CPPFLAGS = $(asio_CFLAGS) $(xml_CFLAGS) -I$(top_srcdir)/include

if ENABLE_ASIO
noinst_PROGRAMS = dbus_asio
endif

dbus_asio_SOURCES = dbus_asio-glue.h dbus_asio.h dbus_asio.cpp 
dbus_asio_LDADD = $(top_builddir)/src/integration/asio/libdbus-c++-asio-1.la \
	$(top_builddir)/src/libdbus-c++-1.la $(asio_LIBS) $(xml_LIBS) -lboost_system -lpthread

dbus_asio-glue.h: $(top_srcdir)/data/org.freedesktop.DBus.xml
	$(top_builddir)/tools/dbusxx-xml2cpp $^ --proxy=$@

BUILT_SOURCES = dbus_asio-glue.h
CLEANFILES = $(BUILT_SOURCES)

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)

MAINTAINERCLEANFILES = \
	Makefile.in

