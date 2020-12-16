HEADER_DIR = $(top_srcdir)/include/dbus-c++

if ENABLE_ASIO

lib_LTLIBRARIES = \
	libdbus-c++-asio-1.la

libdbus_c___asio_1_la_SOURCES = \
	asio-integration.cpp

libdbus_c___asio_1_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-Wno-unused-parameter

libdbus_c___asio_1_la_LIBADD = \
	$(dbus_LIBS) \
	$(BOOST_ASIO_LIBS)

AM_CPPFLAGS = \
	$(dbus_CFLAGS) \
	$(BOOST_CFLAGS)

libdbus_c___asio_1_HEADERS = \
	$(HEADER_DIR)/asio-integration.h

libdbus_c___asio_1dir = \
	$(includedir)/dbus-c++-1/dbus-c++/

EXTRA_DIST = \
	$(libdbus_c___asio_1_HEADERS)

endif
