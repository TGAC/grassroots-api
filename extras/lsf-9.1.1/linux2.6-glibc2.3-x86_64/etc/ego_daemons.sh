#!/bin/sh
#
# Start and stop EGO daemons, System V / OSF version
# Make sure we're running a shell that understands functions
#
# The following is for the Linux chkconfig utlility
# chkconfig: 35 99 01 
# description:  Load Sharing Facility
#
# The following is for the Linux insserv utility
### BEGIN INIT INFO
# Provides: ego
# Required-Start: $remote_fs
# Required-Stop: $remote_fs
# Default-Start: 3 5
# Default-Stop: 0 1 2 6
# Description: Start EGO daemons
### END INIT INFO

kill_daemons() {
    # make sure all old daemons are dead first
    # This would be "/etc/killall lim " if everyone had
    # killall
    MACHINETYPE=`uname -a | cut -d" " -f 5`
    lim_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/lim" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    pem_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/pem" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    vemkd_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/vemkd" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    egosc_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/egosc" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    named_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/named" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    wsg_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/wsgserver" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    rs_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "$EGO_SERVERDIR/rs" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    if [ -f @EGO_TOP@/gui/@EGOVERSION@/etc/@BINARY_TYPE@/wsm ]; then
        wsm_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@/gui/@EGOVERSION@/etc/@BINARY_TYPE@/wsm" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u` 
    fi
	#plc process id
	if [ -f @EGO_TOP@/perf/@EGOVERSION@/etc/plc.sh ]; then
		plc_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@/perf/@EGOVERSION@/etc/plc.sh" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
		plc_java_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@" | egrep "perf.dataloader.Main" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
	fi
	#purge process id
	if [ -f @EGO_TOP@/perf/@EGOVERSION@/etc/purger.sh ]; then
		purger_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@/perf/@EGOVERSION@/etc/purger.sh" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
		purger_java_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@" | egrep "perf.purger.MainApp" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
	fi
	#derby DB id
	if [ -f @EGO_TOP@/perf/@EGOVERSION@/etc/startNetworkServer.sh ]; then
		derby_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@/perf/@EGOVERSION@/etc/startNetworkServer.sh" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
		derby_java_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@" | egrep "org.apache.derby.drda.NetworkServerControl" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
	fi
	#jobdt process id
	if [ -f @EGO_TOP@/perf/lsf/7.0/etc/jobdt.sh ]; then
		jobdt_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@/perf/lsf/7.0/etc/jobdt.sh" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
		jobdt_java_pid=`(ps -ef; ps auxww) 2>/dev/null | egrep "@EGO_TOP@" | egrep "com.platform.perf.datatransformer.MainApp" | sed -n "/grep/d;s/^ *[a-z]* *\([0-9]*\).*/\1/p" | sort -u`
	fi

    for PIDS in $lim_pid $vemkd_pid $egosc_pid $wsm_pid $rs_pid $plc_pid $plc_java_pid $purger_pid $purger_java_pid $derby_pid $derby_java_pid $wsg_pid $named_pid $pem_pid $jobdt_java_pid $jobdt_pid
    do
        kill $PIDS > /dev/null 2>&1
    done
}

heads_daemons() {
	for _serverdir_pair in `env | grep SERVERDIR | grep -v EGO_SERVERDIR`
	do
		_serverdir_name=`echo $_serverdir_pair | cut -f1 -d"="`
		_serverdir_value=`echo $_serverdir_pair | cut -f2 -d"="`
		_filename="`echo "$_serverdir_name" | sed "s/_SERVERDIR//" | tr A-Z a-z`_daemons.sh"
		#special handle lsf_daemon
		if [ "$_filename" = "lsf_daemons.sh" ]; then
			_filename="lsf_daemons"
		fi
		#run daemon script to start or stop related daemons
		if [ -f "$_serverdir_value/$_filename" ]; then
			$_serverdir_value/$_filename $1
		fi
	done
}

EXIT_OK=0
EXIT_ERR=1  # Failed to startup EGO
EXIT_NA=2    # EGO not configured

EGO_TOP=@EGO_TOP@
EGO_VERSION=@EGOVERSION@

EGO_BINDIR=@EGO_TOP@/$EGO_VERSION/@BINARY_TYPE@/bin
EGO_SERVERDIR=@EGO_TOP@/$EGO_VERSION/@BINARY_TYPE@/etc
EGO_LIBDIR=@EGO_TOP@/$EGO_VERSION/@BINARY_TYPE@/lib
EGO_ESRVDIR=@EGO_TOP@/eservice
PATH=${EGO_BINDIR}:${EGO_SERVERDIR}:${PATH}
EGO_CONFDIR=@EGO_TOP@/kernel/conf
LD_LIBRARY_PATH=@EGO_TOP@/$EGO_VERSION/@BINARY_TYPE@/lib
. ${EGO_TOP}/profile.platform
export EGO_BINDIR EGO_SERVERDIR EGO_LIBDIR EGO_ESRVDIR PATH EGO_CONFDIR LD_LIBRARY_PATH

case "$1" in
  'stop')
	$EGO_BINDIR/egosh ego shutdown -f
	sleep 10
	kill_daemons
	heads_daemons stop
	if [ -f /etc/redhat-release -o -f /etc/centos-release ]; then
		rm -f /var/lock/subsys/ego
	fi
	exit $EXIT_OK
        ;;

  'start')
        if [ x$EGO_CONFDIR = x ]; then
            EGO_CONF=@EGO_TOP@/kernel/conf/ego.conf
        else
            EGO_CONF=$EGO_CONFDIR/ego.conf
        fi

        if [ -f $EGO_CONF ]; then
            kill_daemons

            # Get the location of the EGO daemons
            . $EGO_CONF
            # Export this env.variable to notify EGO daemons the loc. of
            # ego.conf
            export EGO_CONFDIR

            # For default install ( remove link )
            if [ "x$EGO_SERVERDIR" = "x" ]; then
                . ${EGO_CONFDIR}/profile.ego
            fi


            # If they are really there, start them
            if [ -f $EGO_SERVERDIR/lim ] ; then
                $EGO_SERVERDIR/lim
                if [ -f /etc/redhat-release -o -f /etc/centos-release ]; then
                    touch /var/lock/subsys/ego
                fi

                #start other heads
                heads_daemons start	
                
                exit $EXIT_OK
            fi
            exit $EXIT_ERR
         fi

         exit $EXIT_NA
        ;;
    *)
        echo "Usage: $0 { start | stop }"
        exit $EXIT_ERR
        ;;
esac


