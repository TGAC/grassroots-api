#!/bin/sh

RUNNER=`id | sed -e 's/[^(]*(\([^)]*\)).*/\1/'`
if [ "$RUNNER" != "root" ]; then
    echo "You should run it as root"
    exit 1
fi

if test -d /sbin/init.d -o -d /etc/init.d -o -d /etc/rc.d -a `uname` != "AIX"
then
    if [ -d /sbin/init.d ]
    then
        # HP_UX
        sdir=/sbin
    elif [ -d /etc/rc.d ]
    then
        # Redhat, SuSe
        sdir=/etc/rc.d
    else
        # Solaris
        sdir=/etc
    fi
    runlev=`grep '^[ ]*[^#].*initdefault:' /etc/inittab | cut -d: -f2`
    stoplev=`expr ${runlev:=3} - 1`
    while [ $stoplev -gt 0 -a ! -d $sdir/rc${stoplev}.d -a ! -d /etc/init.d/rc${stoplev}.d ]
    do
        stoplev=`expr $stoplev - 1`
    done

    if [ -f $sdir/rc${runlev}.d/S95EGO -o -h $sdir/rc${runlev}.d/S95EGO ];then
        rm $sdir/rc${runlev}.d/S95EGO && echo "removed : $sdir/rc${runlev}.d/S95EGO"
    fi
    if [ -f $sdir/rc${runlev}.d/S95ego -o -h $sdir/rc${runlev}.d/S95ego ];then
        rm $sdir/rc${runlev}.d/S95ego && echo "removed : $sdir/rc${runlev}.d/S95ego"
    fi
    if [ -f /etc/init.d/rc${runlev}.d/S95ego -o -h /etc/init.d/rc${runlev}.d/S95ego ];then
        rm /etc/init.d/rc${runlev}.d/S95ego && echo "removed : /etc/init.d/rc${runlev}.d/S95ego"
    fi
    if [ -f $sdir/rc${stoplev}.d/K05EGO -o -h $sdir/rc${stoplev}.d/K05EGO ];then
        rm $sdir/rc${stoplev}.d/K05EGO && echo "removed : $sdir/rc${stoplev}.d/K05EGO"
    fi
    if [ -f $sdir/rc${stoplev}.d/K05ego -o -h $sdir/rc${stoplev}.d/K05ego ];then
        rm $sdir/rc${stoplev}.d/K05ego && echo "removed : $sdir/rc${stoplev}.d/K05ego"
    fi
    if [ -f /etc/init.d/rc${stoplev}.d/K05ego -o -h /etc/init.d/rc${stoplev}.d/K05ego ];then
        rm /etc/init.d/rc${stoplev}.d/K05ego && echo "removed : /etc/init.d/rc${stoplev}.d/K05ego"
    fi
    
    _hosttype=`uname` 
    if [ "$_hosttype" = "Linux" ]; then 
        chkconfig --del ego
    fi

    if [ -d $sdir/init.d -a -f $sdir/init.d/ego ];then
        rm -f $sdir/init.d/ego && echo "removed : $sdir/init.d/ego"
    elif [ -d /etc/init.d -a -f /etc/init.d/ego ];then
        rm -f /etc/init.d/ego && echo "removed : /etc/init.d/ego"
    elif [ -d $sdir/init.d -a -h $sdir/init.d/ego ];then
        rm -f $sdir/init.d/ego && echo "removed : $sdir/init.d/ego"
    elif [ -d /etc/init.d -a -h /etc/init.d/ego ];then
        rm -f /etc/init.d/ego && echo "removed : /etc/init.d/ego"    
    fi
else
    sdir=/etc
    if [ -f /etc/inittab ];then
        sed "s/^ego/:ego/" /etc/inittab > /tmp/inittab.$$
        cp /tmp/inittab.$$ /etc/inittab
    fi
    rm /etc/ego && echo "removed : /etc/ego"
fi
