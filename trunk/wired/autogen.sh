#!/bin/sh

#echo -n "Running gettextize ...."
#gettextize -f -c 1>/dev/null 2>&1
#echo " done"
echo -n "Running aclocal ......."
aclocal --force 1>/dev/null 2>&1	&& \
echo " done"
echo -n "Running libtoolize ...."
libtoolize --force -c  1>/dev/null 2>&1 && \
echo " done"
echo -n "Running autoconf ......"
autoconf  1>/dev/null 2>&1		&& \
echo " done"
echo -n "Running automake ......"
automake -a -f -c 1>/dev/null 2>&1
echo " done"
