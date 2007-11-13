LDFLAGS = $(shell wx-config --libs)

CPPFLAGS = $(shell wx-config --cflags) -I../xml `xml2-config --cflags` -I../engine

noinst_LTLIBRARIES = libsave.la

libsave_la_SOURCES = 			\
		SaveCenter.cpp		\
		WiredDocument.cpp	\
		../xml/WiredXml.cpp
