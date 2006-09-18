#!/bin/sh

###
### classic bootstrap stuff
###
set -x

###
### cleanning part
###
#  remove autotools stuff
rm -rf config
rm -f aclocal.m4 configure config.log config.status
rm -rf autom4te*.cache
# remove libtool stuff
rm -f libtool
# remove gettext stuff
rm -f ABOUT-NLS
rm -rf intl


###
### configuration part
###
# create the config directory
mkdir config
mkdir intl

# execute autotools cmds
autopoint -f				&& \
aclocal --force	-I config/m4		&& \
libtoolize --force -c			&& \
autoconf				&& \
automake -a -f -c


# Shut up
set +x
