CPPFLAGS = $(shell wx-config --cflags) @SAMPLERATE_CFLAGS@ -I ../xml/ -I ../save/ `xml2-config --cflags `

LDFLAGS = $(shell wx-config --libs) -lm @SAMPLERATE_LIBS@

noinst_LIBRARIES = libwiredsamplerate.a

libwiredsamplerate_a_SOURCES =		\
		WiredSampleRate.cpp	\
		WiredSampleRate.h	\
		WiredSampleRateTypes.cpp