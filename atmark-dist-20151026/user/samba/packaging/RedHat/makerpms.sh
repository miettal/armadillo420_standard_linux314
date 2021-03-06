#!/bin/sh
# Copyright (C) John H Terpstra 1998
# Updated for RPM 3 by Jochen Wiedmann, joe@ispsoft.de
#
USERID=`id -u`
GRPID=`id -g`

rpm3var () {
    echo "rpm3var start $1" >>/tmp/log
    var=`rpm --showrc \
         | awk "/-[0-9]+[:=][[:blank:]]+$1[[:blank:]]/ {print \\$3}"`
    echo "var=$var" >>/tmp/log
    while test -n "`echo $var | egrep '%{[_a-zA-Z]+}'`"; do
       v=`echo $var | sed 's/.*%{\([_a-zA-Z]\+\)}.*/\1/'`
       echo "Loop: v=$v" >>/tmp/log
       w="`rpm3var $v`"
       var=`echo $var | sed "s,%{\\([_a-zA-Z]\\+\\)},$w,g"`
       echo "Loop: var=$var" >>/tmp/log
    done
    echo "rpm3var stop $1 $var" >>/tmp/log
    echo $var
}

case `rpm --version | awk '{print $3}'` in
    2.*)
       RPMDIR=`rpm --showrc | awk '/^rpmdir/ { print $3}'`
       SPECDIR=`rpm --showrc | awk '/^specdir/ { print $3}'`
       SRCDIR=`rpm --showrc | awk '/^sourcedir/ { print $3}'`
       ;;
    3.*)
       RPMDIR=`rpm3var _rpmdir`
       SPECDIR=`rpm3var _specdir`
       SRCDIR=`rpm3var _sourcedir`
       ;;
    *)
       echo "Unknown RPM version: `rpm --version`"
       exit 1
       ;;
esac

( cd ../../.. ; chown -R ${USERID}.${GRPID} ${SRCDIR}/samba-2.0.7 )
( cd ../../.. ; tar czvf ${SRCDIR}/samba-2.0.7.tar.gz samba-2.0.7 )

cp -a *.spec $SPECDIR
cp -a *.patch smb.* samba.log $SRCDIR
cd $SPECDIR
rpm -ba -v samba2.spec
