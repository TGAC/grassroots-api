#!/bin/sh

RUNNER=`id | sed -e 's/[^(]*(\([^)]*\)).*/\1/'`
if [ "$RUNNER" != "root" ]; then
    echo "You should run it as root"
    exit 1
fi

LAUNCHDIR=`dirname $0`
LOG=/tmp/setrc.log
if [ "$LAUNCHDIR" = "" ]; then
    echo LAUNCHDIR not defined. Cannot create rc.
    exit 1
fi

hosttype=`uname`
if ( test "$hosttype" != "AIX" ) && ( test -d /sbin/init.d -o -d /etc/init.d -o -d /etc/rc.d ); then
    # get the name of the init directory
    if [ -d /sbin/init.d ]
    then
        sdir=/sbin
    elif [ -d /etc/rc.d ]
    then
        sdir=/etc/rc.d
    elif [ -d /etc/init.d ]
    then
        sdir=/etc/init.d
    else
        sdir=/etc
    fi
    #*********************

    # Get the run level from /etc/inittab
    _runlev=`grep '^[ ]*[^#].*initdefault' /etc/inittab | cut -d: -f2`
    _runlev=`expr ${_runlev:=3}`
    
    # get the name of the init directory
    _sdir=""
    _found="N"
    for _dir in "/etc/init.d" "/etc/rc.d" "/sbin/init.d" "/etc"
    do
        if [ -d "$_dir" ] ; then

            if [ "$_dir" = "/sbin/init.d" ] ; then
                _sdir=/sbin
            else
                _sdir=$_dir
            fi

            _stoplev=`expr ${_runlev:=3} - 1`

            while [ ${_stoplev} -gt 0 -a ! -d "${_sdir}/rc${_stoplev}.d" ]
            do
                _stoplev=`expr ${_stoplev} - 1`
            done

            #if [ -d "$_sdir/rc${_runlev}.d" -a -d "${_sdir}/rc${_stoplev}.d" -a -d "${_sdir}/init.d" ]; then
            if ( test -d "$_sdir/rc${_runlev}.d" && test -d "${_sdir}/rc${_stoplev}.d" ) && ( test -d "${_sdir}/init.d" || test "${_sdir}" = "/etc/init.d" ); then
                _found="Y"
                break
            fi
        fi
    done
    
    if [ "$_sdir" = "" -o "$_found" = "N" ]; then
        echo "error ERROR_6005 setup_rc"
        return 1
    fi
    
    if [ "$_sdir" = "/etc/init.d" ]; then
        _init_d=/etc/init.d
    else
        _init_d=${_sdir}/init.d
    fi

    _hosttype=`uname`
    _linux_distrib="none"
   
    # if host type is Linux, then adjust stop level
    if [ $_hosttype = "Linux" ]; then

        if [ -f /etc/redhat-release -o -f /etc/centos-release ]; then
            _linux_distrib="Redhat";
        elif [ -f /etc/SuSE-release ]; then
            _linux_distrib="SuSE";
        fi

        # if run level is 3 or 5, then cp S95ego & K05ego to the rc${_runlev}.d directory
        if [ $_runlev = 3 -o $_runlev = 5 ]; then
            _stoplev=$_runlev
        fi
    fi

    # remove existing ego rc files
    if [ -h "${_sdir}/rc${_runlev}.d/S95ego" ]; then
        rm -f ${_sdir}/rc${_runlev}.d/S95ego
    fi
    if [ -h "${_sdir}/rc${_stoplev}.d/K05ego" ]; then
        rm -f ${_sdir}/rc${_stoplev}.d/K05ego
    fi
    if [ -f "${_init_d}/ego" ]; then
        rm -f ${_init_d}/ego
    fi
    
    #if run level is 5, then cp S95ego & K05ego to rc3.d
    if  [ $_linux_distrib != "Redhat" -a $_linux_distrib != "SuSE" ]; then
      if [ $_hosttype = "Linux" -a $_runlev = 5 -a -d "${_sdir}/rc3.d" ]; then
        #remove existing rc3 scripts 
        if [ -h "${_sdir}/rc3.d/S95ego" ]; then
            rm -f ${_sdir}/rc3.d/S95ego
        fi
        if [ -h "${_sdir}/rc3.d/K05ego" ]; then
            rm -f ${_sdir}/rc3.d/K05ego
        fi

        # create link for rc3
        if ln -f -s ${_init_d}/ego ${_sdir}/rc3.d/S95ego && ln -f -s ${_init_d}/ego ${_sdir}/rc3.d/K05ego
        then
            echo "ln -f -s ${_init_d}/ego ${_sdir}/rc3.d/S95ego" >>$LOG
            echo "ln -f -s ${_init_d}/ego ${_sdir}/rc3.d/K05ego" >>$LOG
        else
            echo "startup setup failed: cannot install $_init_d/ego" 
            return 1
        fi
      fi 
    #setup rc script in run level
    if  cp -f $EGO_SERVERDIR/ego_daemons.sh ${_init_d}/ego &&
        ln -f -s ${_init_d}/ego ${_sdir}/rc${_runlev}.d/S95ego &&
        ln -f -s ${_init_d}/ego ${_sdir}/rc${_stoplev}.d/K05ego
    then        
        echo "cp -f $EGO_SERVERDIR/ego_daemons.sh ${_init_d}/ego" >> $LOG
        echo "ln -f -s ${_init_d}/ego ${_sdir}/rc${_runlev}.d/S95ego" >> $LOG
        echo "ln -f -s ${_init_d}/ego ${_sdir}/rc${_stoplev}.d/K05ego" >> $LOG
        _done=yes
    else
        echo "startup setup failed: cannot install $_init_d/ego"
        return 1
    fi

    #add rc script in Redhat or Suse
    elif [ $_linux_distrib = "Redhat" -o $_linux_distrib = "SuSE" ]; then
        cp -f $EGO_SERVERDIR/ego_daemons.sh ${_init_d}/ego;
        chkconfig --add ego;
        if [ "$?" = "0" ]; then
            :
        else
            exit 1
        fi

    # SuSe requires an extra insserv it seems.
    if [ $_linux_distrib = "SuSE" ]; then
        insserv ego;
        if [ "$?" = "0" ]; then
            :
        else
            exit 1
        fi
    fi
    
    _done=yes
    fi
else
    # this is AIX, modify /etc/inittab directly
    cp $EGO_SERVERDIR/ego_daemons.sh /etc/ego
    echo "ego:2:wait:/etc/ego start >/dev/console 2>&1" >> /etc/inittab
    _done=yes
fi

if [ "$_done" = "yes" ]; then
    echo "egosetrc succeeds"
fi
