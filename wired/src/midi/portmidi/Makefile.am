SUBDIRS =		\
	porttime

LDFLAGS = 

CPPFLAGS = -DPMALSA -ggdb -Ipm_common -Iporttime

noinst_LIBRARIES = libportmidi.a

libportmidi_a_SOURCES =			\
		pm_common/pmutil.c	\
		pm_linux/pmlinux.c	\
	        pm_common/portmidi.c	\
		pm_linux/pmlinuxalsa.c



