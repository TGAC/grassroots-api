#!/bin/sh
# $Id: funcs.sh,v 1.7 2012/08/09 22:16:13 sbadugu Exp $
#
# Variable and function definitions for installation scripts
#
# (c) Copyright International Business Machines Corp 1992-2012.
# US Government Users Restricted Rights - Use, duplication or disclosure 
# restricted by GSA ADP Schedule Contract with IBM Corp.

PATH=/usr/bin:/bin:/usr/local/bin:/local/bin:/sbin:/usr/sbin:/usr/ucb:/usr/sbin:/usr/bsd:${PATH}
LSNULFILE=/dev/null

# handle difference between system V and BSD echo
# To echo "foo" with no newline, do echo $enf "foo" $enl

if [ "`echo -n`" = "-n" ] ; then
    enf=
    enl="\c"
else
    enf=-n
    enl=
fi
export enf enl
umask 022

# Find a version of awk we can trust
for tmp in nawk  /usr/toolbox/nawk gawk awk 
do
#   This is the real test, for functions & tolower
#    if foo=`(echo FOO | $tmp 'function tl(str) { return tolower(str) } { print tl($1) }') 2>$LSNULFILE` \
#       && test "$foo" = "foo"
#   A simpler test, just for executability
    if ($tmp 'BEGIN{ } { }' < $LSNULFILE ) > $LSNULFILE 2>&1
    then
	AWK=$tmp
        break
    fi
done

if test "$AWK" = ""
then
    echo "Cannot find a correct version of awk."
    echo "Exiting ... "
    exit 1
fi
export AWK

# add this for multi platform installation
if test "$REMOTE_SH" != "y"; then

currDir=`pwd`
if [ "$PREFIX_LOG" = "" ] ; then 
    for tmp in $currDir /tmp ; do
        if `touch $tmp/install.log.$$ > $LSNULFILE 2>&1` ; then 
            `rm -f $tmp/install.log.$$ > $LSNULFILE 2>&1`
             `touch $tmp/Install.log > $LSNULFILE 2>&1`
             PREFIX_LOG=$tmp
             break
        fi
    done

    if [ "$PREFIX_LOG" = "" ] ; then 
         echo "Cannot find a directory to write Install.log to."
         echo "Exiting ..."
         exit 1
    else
        echo "Will log installation sequence in $PREFIX_LOG/Install.log"
        export PREFIX_LOG
    fi
    chmod 666 $PREFIX_LOG/Install.log >$LSNULFILE 2>&1
fi

trap "exit" 1 2 3 15

fi


# copy one item at a time and record failures
cp()
{
    cp_files=""
    prev=""
    for item in $* ; do
        case $item in
           -*)
               /bin/cp $*
               return 0
               ;;
           *)
               cp_files="$cp_files $prev"
               dest=$item
               prev=$dest
               ;;
        esac
    done

    for item in $cp_files ; do
        out=`/bin/cp $item $dest 2>&1`
        if [ "$out" = "" ] ; then
            :
        else
            FAILED_CP="$item $FAILED_CP"
        fi
    done
    return 0
}

# Display a variable with its prompt string
var_with_str()
{
(
   eval echo $\{"$1"_STR\}:
   eval echo '" "'$1='$'"$1"
)
}


# Display all the variable settings appropriate to the current install mode
list_params()
{
    echo
    echo "          Summary of Installation Settings"
    echo
    echo "Generic parameters:"
    echo
    var_with_str LSF_ENVDIR
    if do_indep && test "$NO_MANDATORY" != "y" ; then
	var_with_str LSF_CLUSTER_NAME
	var_with_str LSF_MANAGER
    fi
    if do_machdep ; then
	echo
	echo "Path settings for machine dependent files:"
	echo
	for var in LSF_MACHDEP LSF_BINDIR LSF_SERVERDIR LSF_LIBDIR XLSF_UIDDIR
	do
	    var_with_str $var
	done
    fi
    echo
    if do_indep ; then
	echo
	echo "Path settings for machine-independent files:"
	echo
	for var in LSF_INDEP LSF_CONFDIR LSB_CONFDIR LSB_SHAREDIR LSF_MANDIR LSF_INCLUDEDIR LSF_MISC XLSF_APPDIR
	do
	    var_with_str $var
	done
	echo
	echo "LSF daemon message logs:"
	echo
	var_with_str LSF_LOGDIR
	var_with_str LSF_LOG_MASK
	echo
	echo "Miscellaneous"
	echo
	for var in LSB_MAILTO LSF_STRIP_DOMAIN LSF_ROOT_REX LSF_AUTH
	do
	    var_with_str $var
	done
	if  defined $LSF_LIM_PORT ; then
	     var_with_str LSF_LIM_PORT
	     var_with_str LSF_RES_PORT
	     var_with_str LSB_MBD_PORT
	     var_with_str LSB_SBD_PORT
	fi

        if defined $LSB_DLOGD_PORT ; then
            var_with_str LSB_DLOGD_PORT
        fi

        if defined $LSB_DLOGD_HOST ; then
            var_with_str LSB_DLOGD_HOST
        fi

        if defined $LSF_LINK_PATH ; then
            var_with_str LSF_LINK_PATH
        fi
    fi
}

# Helper function: check if a variable is defined
defined()
{
(
    test "${1:-UNDEF}" != "UNDEF"
)
}

# Another helper: echo if defined
echodef()
{
(
    eval test \"$\{"$1":-UNDEF}\" != \"UNDEF\" '&&' echo \$1=\$"$1"
)
}

#----------------------------------------------------------------------
#
#  make_lsf_conf -
#     create the lsf.conf/lsf.conf.client file in LSF_SERVERDIR.
#
#     LSF_CONF is a local variable.    
#----------------------------------------------------------------------
make_lsf_conf()
{
LSF_CONF=$LSF_ENVDIR/lsf.conf
LSF_CONF_TMP=./lsf.conf

NEW_FILE=$LSF_CONF

if [ -d `dirname $NEW_FILE` ] ; then
    touch $NEW_FILE 2>$LSNULFILE
else
    make_dir `dirname $NEW_FILE`
    chmod 755 `dirname $NEW_FILE`
fi

if [ $? != 0 ] ; then
    echo
    echo "    Cannot create ${LSF_CONF}."
    echo "    Creating ${LSF_CONF_TMP} instead."
    echo "    You must copy $LSF_CONF_TMP to ${LSF_CONF} by hand."
    echo
    NEW_FILE=$LSF_CONF_TMP
    OLD_FILE=$LSF_CONF
else
    NEW_FILE=$LSF_CONF
    OLD_FILE=${LSF_CONF}.old
fi

#  Save old lsf.conf if any
oldfile=n
if [ -s  $NEW_FILE ] ; then
    echo Saving old $NEW_FILE to ${NEW_FILE}.old
    mv $NEW_FILE ${NEW_FILE}.old
    rm -f $NEW_FILE
    oldfile=y


    UNIFORM_DIR=$LSF_MACHDEP
fi

# make the new one
(
# delete this for multiplatform installation
#   make_dir $LSF_SERVERDIR
    exec > $NEW_FILE
    echo "# This file is produced automatically by lsfsetup according to"
    echo "# installation setup. Refer to the LSF Administrator's Guide"
    echo "# before changing any parameters in this file."
    echo "# Any changes to the path names of LSF files must be reflected"
    echo "# in this file. Make these changes with caution."
    echo

        echo LSF_SERVERDIR=$UNIFORM_DIR/etc
        echo LSB_SHAREDIR=$LSF_INDEP/work

    echo
    echo "# Configuration directories"
    echo LSF_CONFDIR="$LSF_CONFDIR"
    echo LSB_CONFDIR="$LSB_CONFDIR"
    echo
    echo "# Daemon log messages"
    echodef LSF_LOGDIR
    echodef LSF_LOG_MASK
    echo

    if defined "$LSB_MAILTO" || defined "$LSB_MAILPROG" ; then
	echo
	echo "# Batch mail message handling"
	echodef LSB_MAILTO
	echodef LSB_MAILPROG
    fi
    echo
    echo "# Miscellaneous"
    case "$LSF_ROOT_REX" in
	1|Y*|y*) echo LSF_ROOT_REX=y;;
	*) ;;
    esac
    echodef LSF_STRIP_DOMAIN
    echodef LSF_AUTH
    if defined  "$LSF_SERVER_HOSTS"
    then
        echo LSF_SERVER_HOSTS=\"$LSF_SERVER_HOSTS\"
    fi
    echo
    echo "# General lsfsetup variables"


	for var in LSF_INDEP LSF_MANDIR LSF_INCLUDEDIR LSF_MISC XLSF_APPDIR
	do
	    echodef $var
	done

	echo LSF_MACHDEP=$UNIFORM_DIR

        if [ "$LSF_DEFAULT_INSTALL" = 'y' ]; then
	    echo LSF_ENVDIR=$LSF_CONFDIR
        else
            echo LSF_ENVDIR=$LSF_SERVERDIR
        fi

	echo LSF_BINDIR=$UNIFORM_DIR/bin
	echo LSF_LIBDIR=$UNIFORM_DIR/lib
	echo XLSF_UIDDIR=$UNIFORM_DIR/lib/uid
    echo
    echo "# Internal variable to distinguish Default Install"
    echo LSF_DEFAULT_INSTALL=$LSF_DEFAULT_INSTALL
    
    echo
    echo "# Internal variable indicating operation mode"
    
    INSTALL_JS=""
    if [ "$INSTALL_JS" = "y" ] ; then    
	if [ "$INSTALL_BATCH" = "y" -o "$LSB_MODE" = "batch" -o "$LSB_MODE" = "batch_js" ] ; then
            echo "LSB_MODE=batch_js"
	else
            echo "LSB_MODE=js"
	fi
    elif [ "$INSTALL_BATCH" = "y" ] ; then    
	if [ "$LSB_MODE" = "" -o "$LSB_MODE" = "batch" ] ; then
            echo "LSB_MODE=batch"
	else
	    echo "LSB_MODE=batch_js"
	fi
    elif [ "$LSB_MODE" != "" ] ; then
	echo LSB_MODE="$LSB_MODE"
    fi
    

    echo
    echo "# Other variables"
    for var in $OPTHIDE
    do
	echodef $var
    done


    if defined "$LSF_LIM_PORT" ; then 
        echodef LSF_LIM_PORT
    fi
    if defined "$LSF_RES_PORT" ; then 
        echodef LSF_RES_PORT
    fi
    if defined "$LSB_MBD_PORT" ; then 
        echodef LSB_MBD_PORT
    fi
    if defined "$LSB_SBD_PORT" ; then 
        echodef LSB_SBD_PORT
    fi

);

#  pick up the missing parameters

if [ "$oldfile" = "y" ] ; then 
    if [  -f ${OLD_FILE} ] ; then
        if [ "$LSF_INSTALL_TYPE" = "INITIAL_INSTALL" -a "$UNATTENDED_INSTALL" = "y" ];
        then
            :
        else
            ( set $NEW_FILE ${OLD_FILE} ; . $sp/pickup)
        fi
    fi
fi

chmod 644 $NEW_FILE

}


# Ask a user a yes or no question, with optional default value (default
#    default is "no") and an optional help command (default none)
#
# Usage: yesno prompt [default] [help command]
# Returns: echoes Y or N, status 0 for yes, status !0 for no
#
# Assumes: $enf and $enl from the environment, for difference between BSD
# and SYSV echo
#-------------------------------------------------------------------------
yesno()
{
(
    # set the default echoed with Question: y or n
    case ${2:-N} in
	 Y* | y*) defe="y" ;;
	 *) defe="n" ;;
    esac

    HELP="(y/n"${3:+" or ? for help"}")"

    while :
    do
    	echo $enf $1 "$HELP [$defe] " $enl 1>&2
	read yn
	if [ "$NO_LOG" != "y" ] ; then
	    echo $enf $1 "$HELP [$defe] " $enl >>$PREFIX_LOG/Install.log
	    echo $yn >> $PREFIX_LOG/Install.log
	fi
	case $yn$defe in
	     Y* | y*) echo Y; return 0;;
	     N* | n*) echo N; return 1;;
	     *)  if test "$3" != ""
		 then
		     ($3 1>&2)
		 fi ;;
	esac
    done
    # paranoid
    return 1
)
}


# Ask for input until we get some; print help if it is given.
#
# Usage: ask prompt default help_command
# Output: input given, or default value if there is one and the user enters
# no value.
# 1999.06.04: Most of the prompts don't ask a question. Changed "?" to ":"
ask()
{
(
    if [ "$3" != "" ] ; then
        HELP="[? for help] "
    else
	HELP=
    fi
    if test "$2" != "" ; then
	HELP=$HELP"[$2] "
    fi

    while : ; do
        echo $enf "${1}: ${HELP}" $enl 1>&2
        read N
	if [ "$NO_LOG" != "y" ] ; then
	    echo $enf "${1}? ${HELP}" $enl >> $PREFIX_LOG/Install.log
	    echo $N >>$PREFIX_LOG/Install.log
	fi
        if test "$N" != "" -a  "$N" != "?" ; then
            echo $N
            return
        fi
        if test "$N" = "" -a "$2" != "" ; then
            echo $2
            return
        fi
        if test "$3" != "" ; then
            ($3 1>&2)
        fi
    done
)
}

# Ask for a standard LSF param with help, etc.
ask_param()
{
    eval ask \"\${$1_STR}:'
	'$1\" \"\${$1}\" ' "help $1"'
}


#--------------------------------------------------------------------------
#
#  Pick up existing lsf.conf info to minimize questions asked.
#
#  For install another host type, user is always asked for lsf.conf from a 
#  previously installed/upgraded host type (COPY)
#
#  For all operations, a locally found lsf.conf is always utilized to pickup 
#  more information.
#
#  We always pick up copied over lsf.conf first and then pickup the local
#  lsf.conf.
#
#  Usage: find_conf LOCAL|COPY
#----------------------------------------------------------------------------
find_conf()
{
unset COPY
if test "$GOT_CONFIG" != "y"
then

    if [ "$1" = "COPY" ] ; then
#       set_defaults changes $1 so we have to save it.
	COPY=y
	while : ; do
	    cat <<COPYFILE

    Please find the lsf.conf file from a previous machine that has LSF
    installed/upgraded. This file is needed in order to pick up the common 
    installation settings.

    You should be able to find the lsf.conf file from /etc of the other host 
    unless you have explicitly installed lsf.conf in a different directory. 
    The lsf.conf can also be found in the LSF_SERVERDIR where your LSF servers
    are installed.

COPYFILE
            lsfconf_copy=`ask "Enter the full path name of the lsf.conf file" """help LSFCONF"`
	    if readable "$lsfconf_copy" LSF
	    then 
		break
            else
		continue
	    fi
	done
    else  # LOCAL ####
        # Find lsf.conf and exec it to set the environment

        LSF_ENVDIR=${LSF_ENVDIR:-/etc}
        cffile=$LSF_ENVDIR/lsf.conf

        if [ -f $cffile ] ; then
 	    while : ; do
	        if [ "$COPY" = "y" ] ; then
                    echo "Do you want to include local installation informationdefined?" |tee -a $PREFIX_LOG/Install.log
		     if yesno "in $cffile" Y "help LSFCONF_LOCAL" > $LSNULFILE
		     then
		         lsfconf=$cffile
		     else
		         break
		     fi
	        else
		    if [ "$UNATTENDED_INSTALL" = "y" ]; then
			lsfconf=$cffile
		    else
        		if yesno "Use $cffile for setup" Y "help LSFCONF" >$LSNULFILE
        		then
        		    lsfconf=$cffile
			    . $lsfconf



			LSF_ENVDIR=`echo $lsfconf | $AWK '{
		  pathLength=length($0);lsfLength=length("/lsf.conf");
		  print(substr($0, 0, (pathLength - lsfLength)))}'`


        		else
        		    break
        		fi
		    fi
	        fi
	        if readable $lsfconf LSF ; then
	    	    break;
	        else
	    	    continue;
	        fi
	    done
        fi
    fi #COPY&&LOCAL

    if [ "$COPY" != "y" ] ; then
	while [ "$lsfconf" = "" ]
	do
            lsfconf=`ask "Enter the full path name of your lsf.conf file" "" "help LSFCONF"`
	    if [ ! -f "$lsfconf" ]
	    then
		echo "Cannot open $lsfconf."
		lsfconf=""
	    else
		if readable $lsfconf LSF
		then

		    . $lsfconf
		else
		    lsfconf=""
		fi
	    fi
	done
    fi

    #LSF_CLUSTER_NAME is over-written in set_defaults. Saved if previously set.
    if [ "$COPY" = "y" ] ; then

        . $lsfconf_copy
	# sub() function doesn't work in SUN
        # LSF_ENVDIR=`echo $lsfconf_copy | $AWK '{sub(/\/lsf.conf/, ""); print}'`
	LSF_ENVDIR=`echo $lsfconf | $AWK '{
		  pathLength=length($0);lsfLength=length("/lsf.conf");
		  print(substr($0, 0, (pathLength - lsfLength)))}'`

	OLD_LSF_MACHDEP=$LSF_MACHDEP
	LSF_MACHDEP=`ask_param LSF_MACHDEP`
	N=n
	while [  -d $LSF_MACHDEP ]; do
            echo $enf "$LSF_MACHDEP already exists, do you want to overwrite all files under $LSF_MACHDEP?" [n]  $enl
	    read N
	    if [ "$N" = "" ]; then
	         N=n
	    fi
	    if [ $N = 'n' -o $N = 'N' ]; then
	        LSF_MACHDEP=`ask_param LSF_MACHDEP`
	    else
	        break
	    fi
	done
	update_params $OLD_LSF_MACHDEP $LSF_MACHDEP $BINS
        cat <<WARNING_COPY

     The machine-dependent settings picked up may or may not be what you
     want for this host type. Use List Current Settings to carefully review
     your installation settings and use Change Current Settings to make
     any necessary changes.

WARNING_COPY
    echo $enf "Press Enter to continue ..." $enl
    read C
    fi

    if [ "$lsfconf" != "" ] ; then
	# we cannot trust /etc, for maybe there is another cluster. 
	# sub() function doesn't work in sun. 
        # LSF_ENVDIR=`echo $lsfconf | $AWK '{sub(/\/lsf.conf/, ""); print}'` 	
	LSF_ENVDIR=`echo $lsfconf | $AWK '{
		    pathLength=length($0);lsfLength=length("/lsf.conf");
	            print(substr($0, 0, (pathLength - lsfLength)))}'`

	. $LSF_ENVDIR/lsf.conf
	if [ "$COPY" != "y" ] ; then
	    eval `sed -n -e "s/#.*//;s/[	 ]*=[	 ]*/=/p" $lsfconf`
	else
	# do not pick up things that must be shared.
	    eval `sed -n -e "s/#.*//;s/[	 ]*=[	 ]*/=/; /LSF_ENVDIR/p; /LSF_MACHDEP/p; /LSF_BINDIR/p; /LSF_LIBDIR/p; /LSF_SERVERDIR/p; /XLSF_UIDDIR/p; /LSF_LOGDIR/p; /LSF_AUTH/p; /LSF_MANDIR/p; /XLSF_APPDIR/p; /LSF_MISC/p; /LSF_INCLUDEDIR/p" $lsfconf`
	fi
    fi

    GOT_CONFIG=y
fi
}

#----------------------------------------------------------------------------
#
# return entries in a vertical section of a configuration file
# Note: this routine is case sensitive. 
#
# Usage: vsect_entries file sectionName columnNumber
#
#-----------------------------------------------------------------------------
vsect_entries()
{
    sed -e ': top
         s/#.*//
         /\\$/{
             N
             s/\\\n//
             b top
         }
         /^[    ]$/d' < $1 \
    | $AWK "	\$1 == \"End\" && \$2 == \"$2\" {Estate=1}
		Bstate == 2 && Estate != 1 {print $\"$3\"} 
		Bstate==1 {Bstate=2} 	# for skipping one line after Begin
		\$1 == \"Begin\" && \$2 == \"$2\" {Bstate=1}"
}


#--------------------------------------------------------------------------
#
#  Find Cluster Name 
#  
#  The lsf.shared file is searched for all cluster Names define. Then the
#  local hostName is searched in all the lsf.cluster.<clusterName> files.
#
#  Usage: find_cluster
#----------------------------------------------------------------------------
find_cluster()
{
if test "$GOT_CONFIG" != "y"; then
    # first get an lsf.conf file since we need one
    find_conf LOCAL
fi
	
# initialization
LSF_SHARED_FILE=${LSF_CONFDIR}/lsf.shared

if [ -f $LSF_SHARED_FILE ] ; then
    :
else
    echo "    Cannot find the shared file"
    echo "    $LSF_SHARED_FILE".
    echo "    An lsf.shared file should have been installed in"
    echo "    $LSF_CONFDIR"
    echo "    from your first installation."
    exit 1
fi

# get all the cluster names in CLUSTERNAME section of lsf.shared

# Potential Problems inside the brackets... Caution!

clusters=`sed -e "1,/^[Bb][Ee][Gg][Ii][Nn][	  ][	  ]*[Cc][Ll][Uu][Ss][Tt][Ee][Rr]/d" -e "/^[Ee][Nn][Dd][	  ][	  ]*[Cc][Ll][Uu][Ss][Tt][Ee][Rr]/,$ d" -e "s/[Cc][Ll][Uu][Ss][Tt][Ee][Rr][Nn][Aa][Mm][Ee]//" -e "s/#.*//" -e "s/^[	  ]*//" -e "s/[	  ]*$//" $LSF_SHARED_FILE`

# get the local host name
localHost=`hostname`
noLocal=`echo $localHost | $AWK '{ FS = "." ; print NF }'`

stripLocal=`echo $localHost | sed -e "s/[.].*//"` # strip domain

i=0
for clusterName in $clusters
do
    i=`expr $i + 1`

	if grepLine=`egrep "^[	 ]*${stripLocal}[ .	]" ${LSF_CONFDIR}/lsf.cluster.$clusterName 2>&1`
	then
            foundHost=`echo $grepLine | $AWK '{ print $1 }'`
            noFields=`echo $foundHost | $AWK '{ FS = "." ; print NF }'`
            if [ "$noLocal" = "1" -o "$noFields" = "1" ] ; then 
                :
            else
                if [ "$foundHost" = "$localHost" ] ; then  
                    :
                else
                    continue
                fi
            fi
            LSF_CLUSTER_NAME=$clusterName
            break
	else
		continue
	fi
done

if defined "$LSF_CLUSTER_NAME" ; then 
	:
else
# If only one cluster file exist, using it 
    if [ "$i" = "1" -a -f ${LSF_CONFDIR}/lsf.cluster.$clusterName ]; then
        LSF_CLUSTER_NAME=$clusterName
    else 
	echo
        echo Trying to determine cluster name ...
	echo Local host name \<$localHost\> not defined in any of the
        echo ${LSF_CONFDIR}/lsf.cluster."<clusterName>" configuration files.
	echo
	echo ${LSF_CONFDIR}/lsf.cluster."<clusterName>" configuration files
        echo Please enter the name of the LSF cluster for this host.
	echo
	LSF_CLUSTER_NAME=`ask_param LSF_CLUSTER_NAME`
    fi
fi
} # end find_cluster


#---------------------------------------------------------------------------
#
# Translate old lsf.conf to new if necessary
#
#---------------------------------------------------------------------------
translate_params()
{
    if [ "$LSF_ENVDIR" = "." ] ; then
	LSF_ENVDIR=/etc
    fi
    LSF_BINDIR=${LSF_BIN:-$LSF_BINDIR}
    LSF_MANDIR=${MAN_DIR:-$LSF_MANDIR}
    LSF_INCLUDEDIR=${INCLUDE_DIR:-$LSF_INCLUDEDIR}

    NEW_LSF_MACHDEP=`dirname $LSF_BINDIR`
    NEW_LSF_INDEP=`dirname $LSF_CONFDIR`
    case "$LSF_ROOT_REX" in
	1|y*|Y*) LSF_ROOT_REX=y ;;
	*) LSF_ROOT_REX=UNDEF ;;
    esac
    LSF_AUTH=${LSF_AUTHD:-$LSF_AUTH}
 
    update_params $LSF_INDEP $NEW_LSF_INDEP $SHARED $OPTSHARE
    update_params $LSF_MACHDEP $NEW_LSF_MACHDEP LSF_MACHDEP $BINS
}

# dirname utility;
# some systems do not have dirname to parse path, so use this
dirname()
{
(
    case $1
    in
    /*)	dname=/ ;;
    *)	dname= ;;
    esac
    dirsep=

    IFS=/
    # avoid problems when $1 is empty or /
    set x/$1
    IFS=" 	"
    set $*
    shift		# get rid of x

    while test "$2" != ""
    do
	dname=$dname$dirsep$1
	shift
	dirsep=/
    done
    if test "$dname" = "" ; then
	echo "."
    else
	echo "$dname"
    fi
)
}


# Quick test for "are we doing machine-dependent or machine-independent"
do_indep()
{
    test "$DO_INDEP" != "n"
}

do_machdep()
{
    test "$DO_MACHDEP" != "n"
}

#--------------------------------------------------------------
#   readable: check if a file is readable or not.
#
#   When running as root, test -f and test -r always return TRUE
#   because it is confused with root on local host. Now we try
#   to actually read the file.
#
#   ARG1 : File name.
#   ARG2 : a key word to search for in the file.
#---------------------------------------------------------------
readable()
{
FNAME=$1
KEYWORD=$2

SS=`grep $KEYWORD $FNAME 2>$LSNULFILE`

if [ "$SS" = "" ] ; then
    echo
    echo "    File $FNAME does not exist, is not readable, or not valid."
    echo
    return 1
fi

return 0
}

#-------------------------------------------------------------------------
#
# man_ready: Check if man pages are installed and up-to-date.
#------------------------------------------------------------------------
man_ready()
{
    if [ -f $LSF_MANDIR/man1/lsid.1 ]
    then
	:
    else
	cat <<MAN_WARN |tee -a $PREFIX_LOG/Install.log

    `echo LSF man pages do not seem to be accessible in $LSF_MANDIR`
    from this host, added to the install list.

MAN_WARN
	return 1
    fi

    allcomps="base"

    if [ "$INSTALL_BATCH" = "y" -o "$INSTALL_JS" = "y" ] ; then
	allcomps="$allcomps batch_js"
	if [ "$INSTALL_BATCH" = "y" ] ; then
	    allcomps="$allcomps batch";
	fi
	if [ "$INSTALL_JS" = "y" ] ; then
	    allcomps="$allcomps js";
	fi
    fi

    for section in 1 3 5 8 ; do

	for comptype in $allcomps ; do

	    if [ -d $comptype/man/man${section} ] ; then

		MANS=`cd $comptype/man/man${section}; ls *.${section}`
		for file in $MANS ; do
		    DIF=`diff -b $comptype/man/man${section}/$file $LSF_MANDIR/man${section} 2>$LSNULFILE`
		    if [ "$?" != "0"  -o  "$DIF" != "" ] ; then
			cat <<MAN_OLD |tee -a $PREFIX_LOG/Install.log

    `echo "LSF man pages in $LSF_MANDIR appear to be old, added"`
    to the install list.

MAN_OLD
			return 1
		    fi
		done
	    fi

	done

    done


    return 0
}

#------------------------------------------------------------------------
#
# include_ready: Check if things in LSF_INCLUDEDIR are already installed
#              and up-to-date.
#-----------------------------------------------------------------------
include_ready()
{
    if [ -f $LSF_INCLUDEDIR/lsf/lsf.h -o -f $LSF_INCLUDEDIR/lsf/lsbatch.h ] ; then
	:
    else
	cat <<INCLUDE_WARN |tee -a $PREFIX_LOG/Install.log

    LSF header files are not installed or are not accessible in 
    `echo $LSF_MISC from this host, added to the install list.`

INCLUDE_WARN

	return 1
    fi

    DIF1=`diff -b base/include/lsf/lsf.h $LSF_INCLUDEDIR/lsf/lsf.h 2>$LSNULFILE`
    rc1=$?

    if [ "$INSTALL_BATCH" = "y" -o "$INSTALL_JS" = "y" ] ; then
        DIF2=`diff -b batch_js/include/lsf/lsbatch.h $LSF_INCLUDEDIR/lsf/lsbatch.h 2>$LSNULFILE`
    fi

    if [ $? != 0 -o "$DIF1" != "" -o $rc1 != 0 -o "$DIF2" != "" ] ; then
	cat <<INCLUDE_OLD |tee -a $PREFIX_LOG/Install.log

    `echo LSF header files in $LSF_INCLUDEDIR/lsf appear to be old, added`
    to the install list.

INCLUDE_OLD
        return 1
    fi

    return 0
}

#------------------------------------------------------------------------
#
# misc_ready: Check if things in LSF_MISC are already installed
#              and up-to-date.
#-----------------------------------------------------------------------
misc_ready()
{
    if [ -f $LSF_MISC/examples/elim.c ] ; then
	:
    else
	cat <<MISC_WARN |tee -a $PREFIX_LOG/Install.log

    Miscellaneous example files are not installed or are not accessible in 
    `echo $LSF_MISC from this host, added to the install list.`

MISC_WARN
	return 1
    fi

    for file in elim.c simsh.c ssh Makefile README ; do
	DIF=`diff -b examples/$file $LSF_MISC/examples/$file 2>$LSNULFILE`
	if [ $? != 0  -o "$DIF" != "" ] ; then
	    cat <<MISC_OLD |tee -a $PREFIX_LOG/Install.log

    `echo Miscellaneous LSF example files in $LSF_MISC/example/$file appear to `
    be old, added to the install list.

MISC_OLD
            return 1
	fi
    done

return 0
}

#------------------------------------------------------------------------
#
# xmisc_ready: Check if things in XLSF_APPDIR are already installed
#              and up-to-date.
#-----------------------------------------------------------------------
xmisc_ready()
{
    if [ "$LSHOSTTYPE" = "NT" ] ; then
        return 0
    fi

    if [ -f $XLSF_APPDIR/XLsmon ] ; then
	:
    else
	cat <<XMISC_WARN |tee -a $PREFIX_LOG/Install.log

    Miscellaneous files for xlsbatch or xlsmon are not installed or are 
    `echo not accessible in $XLSF_APPDIR from this host, added to the install`
    list.

XMISC_WARN
	return 1
    fi

    allcomps="base"

    if [ "$INSTALL_BATCH" = "y" -o "$INSTALL_JS" = "y" ] ; then
	allcomps="$allcomps batch_js"
	if [ "$INSTALL_BATCH" = "y" ] ; then
	    allcomps="$allcomps batch";
	fi
	if [ "$INSTALL_JS" = "y" ] ; then
	    allcomps="$allcomps js";
	fi
    fi

    for comptype in $allcomps ; do

        if [ -d $comptype/misc ] ; then

            #files=`cd $comptype/misc; if [ -s X* ] ; then ls X* ; fi ; \
                                      #if [ -s x* ] ; then ls x* ; fi`
            files=`cd $comptype/misc; ls x* 2> $LSNULFILE`
            ifiles=`cd $comptype/misc; ls X* 2> $LSNULFILE`
            files="$files $ifiles"

            for file in $files ; do
    	        DIF=`diff -b $comptype/misc/$file $XLSF_APPDIR 2>$LSNULFILE`
	        if [ $? != 0  -o "$DIF" != "" ] ; then
	            cat <<XMISC_OLD |tee -a $PREFIX_LOG/Install.log

    `echo "Miscellaneous LSF X applications default files in $XLSF_APPDIR"`
    appear to be old, added to the install list.

XMISC_OLD
	            return 1
	        fi
            done
        fi
    done

    return 0
}

#---------------------------------------------------------------------
#
# is_setuid --
#
#    Check if necessary binaries are installed as setuid to root.
#---------------------------------------------------------------------
is_setuid()
{
echo
echo Checking installed binaries. Please wait ...
echo
unset ERR_SUID
for _osVersion in $SelectedOSVersions ; do

    MACHTYPE=`echo $_osVersion | sed -e "s/^lsf"$LSF_VER"_//"`

    if [ "$LSF_DEFAULT_INSTALL" = "y" ]; then
	LSF_BINDIR=$LSF_INDEP/$MACHTYPE/bin
        if [ "$UPGRADE" = "y" ]; then
	    if is_AFS ; then
                LSF_BINDIR=$LSF_MACHDEP/bin
	    fi
        fi
    else
	LSF_BINDIR=$LSF_MACHDEP/bin
    fi

    for file in $SETUIDS ; do
        MODE=`ls -l $LSF_BINDIR/$file |$AWK '{print $1}'` 
        OWNER=`ls -l $LSF_BINDIR/$file |$AWK '{print $3}'`
        SUID=`echo $MODE | egrep "^-[-r][-w][s]"`

        if [ "$SUID" = ""  -a "$OWNER" != "root" ] ; then
    	    echo "    File $LSF_BINDIR/$file is not setuid and not owned by root." 
    	    ERR_SUID=y
    	    continue
        fi
    
        if [ "$SUID" = "" ] ; then
    	    echo "    File $LSF_BINDIR/$file is not setuid."
    	    ERR_SUID=y
    	    continue
        fi
    
        if [ "$OWNER" != "root" ]  ; then
    	    echo "    File $LSF_BINDIR/$file is not owned by root."
    	    ERR_SUID=y
    	    continue
        fi
    done
done

if defined "$ERR_SUID"
then
    return 1
else
    return 0
fi

}

how_to_setuid()
{
    
cat << NOT_SETUID 

    The above binaries must be setuid to root in order to run LSF
    correctly, unless you want to run LSF servers in single user mode
    (non-root).

    If you want to make the above binaries setuid to root, do the following
    by hand as root:

    `echo cd $LSF_BINDIR`
    `echo chown root $SETUIDS`
    `echo chmod 4755 $SETUIDS`

NOT_SETUID

echo $enf "Press Enter to continue ..." $enl
read X
}

#-----------------------------------------------------------------
#
# check_setuid -
#
#   setuid need not be checked if :
#       On a NT host 
#       LSF_AUTH is defined
#       EXCUSED_BIN == y
#       DO_MACHDEP = n
#---------------------------------------------------------------------
check_setuid()
{
    if [ "$LSHOSTTYPE" = "NT" ] ; then
        return 
    fi

    if test "$EXCUSED_BIN" = "y"
    then
	return
    fi

    if do_machdep
    then
	:
    else
	return
    fi

    if [ "$UNATTENDED_INSTALL" != "y" ]; then
        if is_setuid
        then
	    :
        else
	    how_to_setuid
        fi
    fi
}

#------------------------------------------------------------------------
#
# check_confdir
#
#    Check if LSF_CONFDIR/LSB_CONFDIR has been installed and accessible.
#
#-------------------------------------------------------------------------
check_confdir()
{


if [ ! -d "$LSF_CONFDIR" -o  ! -d "$LSB_CONFDIR" ]
then
    if defined "$LSF_SERVER_HOSTS"
    then
        return 0
    else
        cat << CONF_WARN |tee -a $PREFIX_LOG/Install.log

    `echo "$LSF_CONFDIR (LSF_CONFDIR) or $LSB_CONFDIR (LSB_CONFDIR)"`
     is not accessible from this host. LSF_CONFDIR and LSB_CONFDIR
       (as defined in lsf.conf) must be installed and shared by all server
       hosts in the LSF cluster.

    If you do not want to configure this host as a client-only host, you
    must run lsfsetup and choose Option 1. Install LSF Products, then choose
    Option 5. Install Additional Products to install LSF configuration files
    in a shared directory.

CONF_WARN

        if yesno "Make this host a client-only host?" Y "help LSF_CLIENTS" >$LSNULFILE
        then
            return 0
        else
            return 1
        fi
    fi
fi

return 0
}

#----------------------------------------------------------------------
#
#   check_sharedir: this should not happen.
#
#   If LSB_SHAREDIR is not really shared, then only hosts that share this
#   dir may become the master.
#----------------------------------------------------------------------
check_sharedir()
{
if test ! -d "$LSB_SHAREDIR/$LSF_CLUSTER_NAME"
then
    if defined "$LSF_SERVER_HOSTS" 
    then
        return 0
    else
        cat << SHARED_WARN

    `echo "Warning! $LSB_SHAREDIR/$LSF_CLUSTER_NAME (LSB_SHAREDIR)"`
    is not accessible from this host. LSB_SHAREDIR (as defined in lsf.conf)
    must be accessible from all hosts that could potentially serve as the
    master host (see LSF Administrator's Guide for more information).

SHARED_WARN
        if yesno "Make this host a client-only host" Y "help LSF_CLIENTS" >$LSNULFILE
        then



            return 0
        else
            if yesno "This host cannot be LSF master, continue the installation" Y "" >$LSNULFILE
	    then
		return 0
            else
		return 1
            fi
        fi
    fi
fi

return 0
}

# Create a directory and change ownership to the manager
make_sharedir()
{
    sharedir=$1
    if is_root ; then
        CHOWN=chown
    else
        CHOWN=true
    fi
    if test ! -d "$sharedir"
    then
        make_dir "$sharedir"
        find "$sharedir"/. -exec $CHOWN $LSF_MANAGER {} \;
    fi

    $CHOWN $LSF_MANAGER $sharedir
}


# Installing the LSF configuration files
make_cluster() 
{
    if [ -f $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME ]
    then
	if [ "$UNATTENDED_INSTALL" = "y" ]; then 
	    mv -f $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME $LSF_CONFDIR/lsf.cluster.${LSF_CLUSTER_NAME}.old
	else
	    if yesno "Configuration for cluster $LSF_CLUSTER_NAME already exists.  Replace it?" >$LSNULFILE
	    then
                echo "Saving $LSF_CONFDIR/lsf.shared in $LSF_CONFDIR/lsf.shared.old."
	        mv -f $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME $LSF_CONFDIR/lsf.cluster.${LSF_CLUSTER_NAME}.old
	    else
	        return 
	    fi
        fi
    fi

    if is_root ; then
	CHOWN=chown
    else
	CHOWN=true
    fi

# Create $LSF_CONFDIR if needed
    make_dir $LSF_CONFDIR
    $CHOWN $LSF_MANAGER $LSF_CONFDIR

# Create lsf.shared if necessary
    if grep -i "^ClusterName" $LSF_CONFDIR/lsf.shared >$LSNULFILE 2>&1
    then
	true
    else
	if [ -f $LSF_CONFDIR/lsf.shared ]
	then
	    echo "Saving $LSF_CONFDIR/lsf.shared in $LSF_CONFDIR/lsf.shared.old"
	    mv -f $LSF_CONFDIR/lsf.shared $LSF_CONFDIR/lsf.shared.old
	else true; fi
	cp base/conf/lsf.shared $LSF_CONFDIR/lsf.shared
	$CHOWN $LSF_MANAGER $LSF_CONFDIR/lsf.shared
	chmod 644 $LSF_CONFDIR/lsf.shared
    fi
 
# Add cluster entry to lsf.shared file if necessary
    if  sed -n -e "/^Begin Cluster/,/^End Cluster/p" <$LSF_CONFDIR/lsf.shared | grep "^$LSF_CLUSTER_NAME" >$LSNULFILE 2>&1
    then
        echo "$LSF_CONFDIR/lsf.shared already has an entry for cluster $LSF_CLUSTER_NAME."
    else
        echo "Adding entry for cluster $LSF_CLUSTER_NAME to $LSF_CONFDIR/lsf.shared."
	ed $LSF_CONFDIR/lsf.shared <<ED_SCRIPT >$LSNULFILE 2>&1
/End Cluster/i
$LSF_CLUSTER_NAME
.
w
q
ED_SCRIPT
    fi

# Create the lsf.cluster.<cluster> file
# The old lsf.cluster.<cluster> file was backed up above.
    PRODUCTSLINE=LSF_Base 
    if [ "$INSTALL_BATCH" = "y" ]; then
        PRODUCTSLINE="$PRODUCTSLINE Clusterware_Manager LSF_Manager LSF_Sched_Fairshare LSF_Sched_Preemption LSF_Sched_Parallel LSF_Sched_Resource_Reservation LSF_Sched_Advance_Reservation"
    fi
    if  [ "$INSTALL_JS" = "y" ]; then
	PRODUCTSLINE="$PRODUCTSLINE LSF_JobScheduler"
    fi
    if  [ "$INSTALL_MC" = "y" ]; then
	PRODUCTSLINE="$PRODUCTSLINE LSF_MultiCluster"
    fi 
    if  [ "$INSTALL_PARALLEL" = "y" ]; then
	PRODUCTSLINE="$PRODUCTSLINE LSF_Parallel"
    fi 
    if  [ "$INSTALL_LSMAKE" = "y" ]; then
        PRODUCTSLINE="$PRODUCTSLINE LSF_Make"
    fi
#    if  [ "$INSTALL_ANALYZER" = "y" ]; then
#        PRODUCTSLINE="$PRODUCTSLINE LSF_Analyzer"
#    fi 


    sed -e s/@MANAGER@/$LSF_MANAGER/ <base/conf/TMPL.lsf.cluster >$LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME

   ed $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME <<ED_SCRIPT2 >$LSNULFILE 2>&1
/End Parameters/i
PRODUCTS=$PRODUCTSLINE
.
w
q
ED_SCRIPT2

    $CHOWN $LSF_MANAGER $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
    chmod 644 $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME

# Create lsf.task if necessary
# format and content of this file hasn't changed, so don't move old one
    if [ -f $LSF_CONFDIR/lsf.task ]
    then
	:
    else
	cp base/conf/lsf.task $LSF_CONFDIR/lsf.task
	chmod 644 $LSF_CONFDIR/lsf.task
	$CHOWN $LSF_MANAGER $LSF_CONFDIR/lsf.task
    fi

    echo Installing lsbatch directories and configurations ...
# Install default lsbatch config files and logdir
    if [ -d $LSB_CONFDIR/$LSF_CLUSTER_NAME ] ; then
	if [ "$UNATTENDED_INSTALL" = "y" ]; then
	    rm -rf $LSB_CONFDIR/${LSF_CLUSTER_NAME}.old
	    mv -f $LSB_CONFDIR/$LSF_CLUSTER_NAME $LSB_CONFDIR/$LSF_CLUSTER_NAME.old
	else
    	    if yesno "LSF Batch configuration for cluster <$LSF_CLUSTER_NAME> already exists.  Replace it?" N "" >$LSNULFILE
    	    then
    	        echo "Saving old configuration as" $LSB_CONFDIR/${LSF_CLUSTER_NAME}.old
    	        rm -rf $LSB_CONFDIR/${LSF_CLUSTER_NAME}.old
    	        mv -f $LSB_CONFDIR/$LSF_CLUSTER_NAME $LSB_CONFDIR/$LSF_CLUSTER_NAME.old
    	    else
    	        return 0
    	    fi
	fi
    else
	true
    fi

    if [ "$INSTALL_BATCH" = "y" -o "$INSTALL_JS" = "y" ] ; then
        make_dir $LSB_CONFDIR/$LSF_CLUSTER_NAME/configdir
        cp batch_js/conf/configdir/lsb* $LSB_CONFDIR/$LSF_CLUSTER_NAME/configdir

        if [ "$INSTALL_JS" = "y" ] ; then
            cp js/conf/configdir/lsb* $LSB_CONFDIR/$LSF_CLUSTER_NAME/configdir
 	fi

        if [ "$INSTALL_BATCH" = "y" ] ; then
            cp batch/conf/configdir/lsb* $LSB_CONFDIR/$LSF_CLUSTER_NAME/configdir
 	fi
        chmod 644 $LSB_CONFDIR/$LSF_CLUSTER_NAME/configdir/*
        $CHOWN $LSF_MANAGER $LSB_CONFDIR
        find $LSB_CONFDIR/$LSF_CLUSTER_NAME/. -exec $CHOWN $LSF_MANAGER {} \;
        make_sharedir $LSB_SHAREDIR/$LSF_CLUSTER_NAME/logdir
        make_sharedir $LSB_SHAREDIR/$LSF_CLUSTER_NAME/live_confdir
        make_sharedir $LSB_SHAREDIR/$LSF_CLUSTER_NAME/lsf_indir
        make_sharedir $LSB_SHAREDIR/$LSF_CLUSTER_NAME/lsf_cmddir
        chmod 777 $LSB_SHAREDIR/$LSF_CLUSTER_NAME/lsf_indir
        chmod 777 $LSB_SHAREDIR/$LSF_CLUSTER_NAME/lsf_cmddir
    fi

}

#--------------------------------------------------------------------------
#
#  is_linux_box -
#     try to get whether it is linux or not.
#
#     if it is linux,  return 1
#     Otherwise, return 0
#
#  for Red Hat 5.1 linux(by Jin Li Oct. 9, 1998)
# 
#---------------------------------------------------------------------------

is_linux_box()
{
    uname_val=`uname -a`
    IS_LINUX="fail"
    field1=`echo $uname_val |$AWK '{print $1}'`
    case "$field1" in 
    Linux)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        if [ $version = "1" ]; then
	    IS_LINUX="linux"
        elif [  $version = "2" -o $version -gt 2 ]; then
            # we support alpha only on 2.0.x and up
            machine=`echo $uname_val | $AWK '{print $11}'`
	    IS_LINUX="linux2"
            if [ "$machine" = "alpha" ]; then
                IS_LINUX="linux2-alpha"
            else
                IS_LINUX="linux2-intel"
            fi
        fi
        ;;
    esac
    if [ $IS_LINUX = "fail" ]; then
        return 1
    else
        return 0
    fi
}


#-------------------------------------------------------------------
# Return the list of keywords for a section, converted to lower case
# Usage: sect_keys file sectionname
#-------------------------------------------------------------------
sect_keys()
{
    sed -e ': top
         y/ABCDEFGHIJKLMNOPQRSTUVWXYZ/abcdefghijklmnopqrstuvwxyz/
         s/#.*//
	 /\\$/{
	     N
	     s/\\\n//
	     b top
	 }
	 /^[ 	]$/d' < $1 \
    | $AWK "state==1 {while ( NF > 0 ) {print \$0; exit} }
            (\$1 == \"begin\" && \$2 == \"$2\") || state==1 {state=1}"
}

#------------------------------------------------
# isServerHost
#  - test if a host is a server host
#------------------------------------------------
isServerHost()
{
    local_host=`hostname`
    LSF_CLUSTER=${LSF_CONFDIR}/lsf.cluster.${LSF_CLUSTER_NAME}
    server=`cat $LSF_CLUSTER |$AWK '/HOSTNAME/,/End/'| $AWK '($1!~/^#/)&&($1!~/^HOSTNAME/)&&($1!~/^End/){print $1, $4}'|grep $local_host | $AWK '{print $2}'`
    if [ "$server" = "1" ]; then
        return 0  
    else 
        return 1  
    fi
}
 
#-----------------------------------------------------------------------
# add_host - Add a host into cluster file
#-----------------------------------------------------------------------
add_host()
{
hostIncluster=""

LSF_SHARED=${LSF_CONFDIR}/lsf.shared

if egrep "^[    ]*MODELNAME[    ][       ]*CPUFACTOR[    ][       ]*ARCHITECTURE" ${LSF_SHARED} >$LSNULFILE 2>&1
then
    defaultModelType="!"
else
    defaultModelType="DEFAULT"
fi

LSF_CLUSTER=${LSF_CONFDIR}/lsf.cluster.${LSF_CLUSTER_NAME}
LSF_CLUSTER_TMP=/${LSF_CONFDIR}/lsf.cluster.tmp
rm -f $LSF_CLUSTER_TMP

cp ${LSF_CLUSTER} ${LSF_CLUSTER_TMP}
# get the keywords for the Host section
skeys=`sect_keys $LSF_CLUSTER_TMP host`
        
_tmpfile=/tmp/.tmp.file1.$$
cp $LSNULFILE $_tmpfile 
AHOST="	# Added hosts"

#get_lsf_manager

if [ "$LSF_SERVERS" != "" ]; then
    _allhosts=$LSF_SERVERS
    SERVER_HOST=1
else
    _allhosts=$LSF_CLIENTS
    SERVER_HOST=0
fi

_i=1
_No=`echo "$_allhosts" | wc -w`

incluster=` cat $LSF_CLUSTER | $AWK '/HOSTNAME/,/End/'|$AWK '($1!~/^#/)&&($1!~/^HOSTNAME/)&&($1!~/^End/){print $1}'`

for eachHost in $incluster
do
    if [ "$hostIncluster" = "" ]; then
        hostIncluster="$eachHost"
    else
        hostIncluster="$hostIncluster $eachHost"
    fi
done
hostIncluster=" $hostIncluster "

while [ "$_i" -le "$_No" ]
do
    _cmd=`echo "awk '{print \\\$$_i}'"`
    local_host=`echo "$_allhosts" | eval $_cmd`

    case $hostIncluster in
        *" $local_host "* )
            isIncluster=y ;;
        * )
            isIncluster=n ;;
    esac

    if [ "$isIncluster" = "y" ] ; then
	:
    else  
	LINE=""
	for field in $skeys
	do
            case $field in
	    hostname)	LINE="${LINE}$local_host	" ;;


 	    model)      LINE="${LINE}$defaultModelType	" ;;
 	    type)	LINE="${LINE}$defaultModelType	" ;;
	    r|server)	LINE="${LINE}$SERVER_HOST	" ;;
	    r1m)	LINE="${LINE}3.5 " ;;
	    *)          LINE="${LINE}()   " ;;
	    esac
	done

	echo "$LINE$AHOST" >> $_tmpfile
    fi

    if [ "$_i" -eq "$_No" ]; then
	if [ $SERVER_HOST = 1 ]; then
	    _allhosts=$LSF_CLIENTS
            _i=1; _No=`echo $_allhosts | wc -w`
            SERVER_HOST=0    
	else
	    break
	fi
    else
	_i=`expr $_i + 1`
    fi
done

    $LSEXEDITOR ${LSF_CLUSTER_TMP} << EXEND2 >$LSNULFILE 2>&1
    /^[	 ]*End[	  ]*Host
    -1
    r $_tmpfile
    wq
EXEND2

 # Get rid of the "Added hosts" comments, if any 

    $LSEXEDITOR ${LSF_CLUSTER_TMP} << EXEND3 >$LSNULFILE 2>&1
    /# Added hosts$/s/[ 	]*# Added hosts$//
    wq
EXEND3

    cp ${LSF_CLUSTER} ${LSF_CLUSTER}.old
    cp ${LSF_CLUSTER_TMP} ${LSF_CLUSTER}
    stat=$?
    if is_root ; then
        CHOWN=chown
    else
        CHOWN=true
    fi
    $CHOWN ${LSF_MANAGER} ${LSF_CLUSTER} ${LSF_CLUSTER}.old
    chmod 644 ${LSF_CLUSTER}.old ${LSF_CLUSTER}
    if [ "$?" != "0"  -o  "${stat}" != "0" ] ; then
        echo "cp or chmod $LSF_CLUSTER failed"
	kill -2 $$
        exit
    fi
    rm ${LSF_CLUSTER_TMP}
} # add_host

#-------------------------------------------------------------------
# do_hostsetup --
#
#  Do hostsetup directly from lsfsetup command. 
#  $1 is full pathname of lsf.conf
#  $2 is cluster name
#-------------------------------------------------------------------
do_hostsetup()
{
    lsfconf="$1"
    clustername="$2"
    if [ -f "$lsfconf" ]; then
	GOT_CONFIG=y
	. $lsfconf
    else
	while : ; do
            N=`ask "Enter the pathname of your installed lsf.conf file" "" "help LSFCONF"`
	    if [ -d "$N" ] ; then
		echo "   $N is not a file."
		help LSFCONF
		continue
	    fi
	    if [ ! -f "$N" ] ; then
		echo "   Cannot find file $N."
		continue
	    fi
	    .  $N
	    lsfconf=$N
	    GOT_CONFIG=y
	    break
	done
    fi

    LSF_CLUSTER=${LSF_CONFDIR}/lsf.cluster.${clustername} 
    while [ ! -f "$LSF_CLUSTER" ]; do 
         if [ "$clustername" != "" ] ; then
            echo "$LSF_CLUSTER does not exist."
	 fi
	 clustername=`ask_param LSF_CLUSTER_NAME`
	 LSF_CLUSTER=${LSF_CONFDIR}/lsf.cluster.${clustername}
	 echo
    done 
    LSF_CLUSTER_NAME=$clustername

    . $sp/hostsetup; hostsetup
}

#----------------------------------------------
# Compares 2 lists of features:
#
# compare_feature <cluster file features> <license file features>
# returns 0 <true> is same 1 <false> otherwise
#
#--------------------------------------------
compare_feature()
{

   new_list=`echo $1 | sed 's/^FEATURES=//'`
   new_list=`echo $new_list | sed 's/^PRODUCTS=//'`
   lic_list=$2
   # convert old feature names into new ones
   comp_list=""
   for user_feat in $new_list
     do
          case "$user_feat" in
	  lsf_base) comp_list="$comp_list LSF_Base" ;;
	  clusterware_manager) comp_list="$comp_list Clusterware_Manager" ;;
	  lsf_manager) comp_list="$comp_list LSF_Manager" ;;
	  lsf_sched_fairshare) comp_list="$comp_list LSF_Sched_Fairshare" ;;
	  lsf_sched_preemption) comp_list="$comp_list LSF_Sched_Preemption" ;;
	  lsf_sched_parallel) comp_list="$comp_list LSF_Sched_Parallel" ;;
	  lsf_sched_resource_reservation) comp_list="$comp_list LSF_Sched_Resource_Reservation" ;;
	  lsf_multicluster) comp_list="$comp_list LSF_MultiCluster" ;;
	  lsf_sched_advance_reservation) comp_list="$comp_list LSF_Sched_Advance_Reservation" ;;
	  lsf_license_maximizer) comp_list="$comp_list LSF_License_Maximizer" ;;
	  lsf_make) comp_list="$comp_list LSF_Make" ;;
	  lsf_parallel) comp_list="$comp_list LSF_Parallel" ;;
	  lsf_data) comp_list="$comp_list LSF_Data" ;;
	  lsf_sched_topology) comp_list="$comp_list LSF_Sched_Topology" ;;
	  lsf_js) comp_list="$comp_list LSF_JobScheduler" ;;
	  lsf_mc) comp_list="$comp_list LSF_MultiCluster" ;;
	  *) comp_list="$comp_list $user_feat" ;;
	  esac
     done

   lic_i=0
   for lic_feat in $lic_list ; do 
       lic_i=`expr $lic_i + 1`
   done

   user_i=0
   for user_feat in $comp_list ; do 
       user_i=`expr $user_i + 1`
   done

   # check if there is the same no. of arguments first
   if [ "$lic_i" = "$user_i" ] ; then 
       :   
   else
       return 1
   fi 

   for lic_feat in $lic_list
     do
         SAME="n"
         for user_feat in $comp_list
           do
              if [ "$lic_feat" = "$user_feat" ] ; then
                  SAME="y" 
                  break
              fi
           done      
           if [ "$SAME" = "n" ] ; then
                return 1
           fi
     done
     return 0
}

#--------------------------------------------------------------------
#
# Update the FEATURES line in lsf.cluster if necessary
#
# Usage: check_features <license file>
#
#--------------------------------------------------------------------
check_features()
{        
    FEATURELINE="lsf_base"

    for feat in lsf_base clusterware_manager lsf_manager lsf_make lsf_client lsf_float_client lsf_sched_fairshare lsf_sched_preemption lsf_sched_parallel lsf_sched_resource_reservation lsf_multicluster lsf_license_maximizer lsf_sched_advance_reservation lsf_parallel lsf_data lsf_sched_topology lsf_after_hours ; do
        f=`grep "^FEATURE $feat " $1`

        if [ "$f" != "" ] ; then
	    FEATURELINE="$FEATURELINE $feat"
        fi
    done


    if [ "$LSF_CLUSTER_NAME" = "" ]; then
    	find_cluster
    fi


    if [ "$LSF_MANAGER" = "" ] ; then 
	 LSF_CLUSTER=${LSF_CONFDIR}/lsf.cluster.$LSF_CLUSTER_NAME
	 get_lsf_manager
    fi 
    LSF_ADMIN=`echo $LSF_MANAGER | $AWK '{print $1}'`
    if [ "$LSF_ADMIN" = "" ] ; then
        # Shouldn't come here 
        echo 
        echo "Unrecoverable error. LSF administrator is not defined."
        echo "Exiting ... "
        echo 
        exit 1
    fi

    # determine how many floating client licenses
    f=`grep "^FEATURE lsf_float_client " $1`
    if [ "$f" != "" ] ; then
        FLOATINGCLIENTS=`echo $f | $AWK '{print $6}'`
        # go throught the INCREMENT keys
        grep '^INCREMENT lsf_float_client ' $1 > /tmp/floatIncrKeys.tmp
        NUMINCR=`wc -l /tmp/floatIncrKeys.tmp | $AWK '{ print $1 }'`
        while [ "$NUMINCR" -gt "0" ]
        do
            LINE=`sed $NUMINCR!d /tmp/floatIncrKeys.tmp`
            LINE=`echo "$LINE" | $AWK '{ print $6 }'`
            FLOATINGCLIENTS=`expr $FLOATINGCLIENTS + $LINE`
            NUMINCR=`expr $NUMINCR - 1`
        done
        rm -f /tmp/floatIncrKeys.tmp
    else
        FLOATINGCLIENTS="0"
    fi
    # determine current FLOAT_CLIENTS value
    f=`grep "^FLOAT_CLIENTS" < $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME`
    USER_FLOATINGCLIENTS=`echo $f | sed "s/FLOAT_CLIENTS//" | sed "s/=//" | sed 's/^[ \t]*//;s/[ \t]*$//'`
    if [ "$USER_FLOATINGCLIENTS" = "" ] ; then
        USER_FLOATINGCLIENTS="0"
    fi
    if [ "$USER_FLOATINGCLIENTS" -lt "1" ] ; then
        USER_FLOATINGCLIENTS="0"
    fi

    USER_FEATURES=`grep "^FEATURES" < $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME`
    if [ "$USER_FEATURES" = "" ] ; then 
        USER_FEATURES=`grep "^PRODUCTS" < $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME` 
    fi 

    MODIFY="y"
    MODIFY_HELP=""

    if [ "$MODIFY" != "n" ] ; then

	comp=""
        TMPFEATLINE=$FEATURELINE
        FEATURELINE=""
        jsAddLater=n

        for feat in $TMPFEATLINE
	do
	    case "$feat" in
	    lsf_base) comp="$comp Base"
                FEATURELINE="$FEATURELINE LSF_Base"
                ;;
            clusterware_manager) comp="$comp Clusterware_Manager"
                FEATURELINE="$FEATURELINE Clusterware_Manager"
                ;;
            lsf_manager) comp="$comp Manager"
                FEATURELINE="$FEATURELINE LSF_Manager"
                ;;
            lsf_sched_fairshare) comp="$comp Sched_Fairshare"
                FEATURELINE="$FEATURELINE LSF_Sched_Fairshare"
                ;;
            lsf_sched_preemption) comp="$comp Sched_Preemption"
                FEATURELINE="$FEATURELINE LSF_Sched_Preemption"
                ;;
            lsf_sched_parallel) comp="$comp Sched_Parallel"
                FEATURELINE="$FEATURELINE LSF_Sched_Parallel"
                ;;
            lsf_sched_resource_reservation) comp="$comp Sched_Resource_Reservation"
                FEATURELINE="$FEATURELINE LSF_Sched_Resource_Reservation"
                ;;
            lsf_multicluster) comp="$comp MultiCluster"
                FEATURELINE="$FEATURELINE LSF_MultiCluster"
                ;;
            lsf_sched_advance_reservation) comp="$comp Sched_Advance_Reservation"
                FEATURELINE="$FEATURELINE LSF_Sched_Advance_Reservation"
                ;;
            lsf_license_maximizer) comp="$comp License_Maximizer"
                FEATURELINE="$FEATURELINE LSF_License_Maximizer"
                ;;
            lsf_parallel) comp="$comp Parallel"
                FEATURELINE="$FEATURELINE LSF_Parallel"
                ;;
            lsf_data) comp="$comp Data"
                FEATURELINE="$FEATURELINE LSF_Data"
                ;;
            lsf_sched_topology) comp="$comp Sched_Topology"
                FEATURELINE="$FEATURELINE LSF_Sched_Topology"
                ;;
            lsf_make) comp="$comp Make"
		echo
		if test "$UNATTENDED_INSTALL" = "y"; then
		    FEATURELINE="$FEATURELINE LSF_Make"
		else

		if yesno "Are all the hosts in the cluster going to be Make servers"  Y > $LSNULFILE ; then
		    FEATURELINE="$FEATURELINE LSF_Make"
                else
		    makeAddLater=y
                fi

		fi
		;;
	    *) ;;
	    esac
	done

        if compare_feature "$USER_FEATURES" "$FEATURELINE" ; then 
            FEATURES_FLAG="0"
        else
            FEATURES_FLAG="1"
        fi

        if [ "$FLOATINGCLIENTS" != "$USER_FLOATINGCLIENTS" ] ; then
            FLOATINGCLIENT_FLAG="1"
        else
            FLOATINGCLIENT_FLAG="0"
        fi
        
        if  [ "$FEATURES_FLAG" -eq "1" ] || [ "$FLOATINGCLIENT_FLAG" -eq "1" ]
        then
           cat << FEATURE_HELP

        Your license key contains features for the following LSF products:

		$comp

	Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
        file needs to be modified to include the following line in order that 
        the features specified in the line are enabled for the entire cluster:

		PRODUCTS=$FEATURELINE

FEATURE_HELP
           if [ "$FLOATINGCLIENTS" -gt "0" ] ; then
               cat << CLIENTFLOAT_HELP
        Your license key contains features for floating clients. If 
        you wish to enable this feature you must edit your
        $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file
        manually and uncomment the "FLOAT_CLIENTS" line.

        WARNING: When the LSF floating client license feature is
        enabled, ANY host will be able to submit jobs to the cluster.
        This is a security risk. If you decide to enable this feature
        it is recommended that you add the parameter
        FLOAT_CLIENTS_ADDR_RANGE. See the LSF Administrator's Guide
        for more details.

CLIENTFLOAT_HELP
           fi

           if [ "$MODIFY" = "y" ] ; then
	       cat << MODIFY_HELP
	Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
        file currently contains the following PRODUCTS line:

		$USER_FEATURES

MODIFY_HELP
               if [ "$USER_FLOATINGCLIENTS" -gt "0" ] ; then
                   cat << HASFLOATINGCLIENTS
        Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
        file currently has $USER_FLOATINGCLIENTS floating clients
        configured.

HASFLOATINGCLIENTS
                   if [ "$FLOATINGCLIENTS" -eq "0" ] ; then
                       cat << REMOVINGFLOATS
        The FLOAT_CLIENTS line will be commented out from your
        $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file
        since no license for this feature was found.

REMOVINGFLOATS
                   fi
               fi
	   fi

           if test "$UNATTENDED_INSTALL" = "y" || yesno "Do you want to modify $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME?" Y ""  > $LSNULFILE
	   then
	       if [ "$MODIFY" = "a" ] ; then
                   echo "Appending PRODUCTS line to $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME ..."
                   echo
                   echo "Begin Parameters" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                   echo "PRODUCTS=$FEATURELINE" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                   
                   if [ "$FLOATINGCLIENTS" -gt "0" ] ; then
                       echo "# WARNING: When the LSF floating client license feature is" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                       echo "# enabled, ANY host will be able to submit jobs to the cluster." >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                       echo "# This is a security risk. It is recommended that you add the" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                       echo "# parameter FLOAT_CLIENTS_ADDR_RANGE. See the LSF Administrator's" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                       echo "# Guide for more details." >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                       echo "#FLOAT_CLIENTS= $FLOATINGCLIENTS" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                   fi
                   
                   echo "End Parameters" >> $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
	       else
                   echo "Modifying PRODUCTS line in $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME ..."
                   echo
                   sed "s/^PRODUCTS.*/PRODUCTS=$FEATURELINE/" < $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME > /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                   sed "s/^FEATURES.*/PRODUCTS=$FEATURELINE/" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$

                   # comment out original FLOAT_CLIENTS line
                   mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                   sed "s/^FLOAT_CLIENTS/#FLOAT_CLIENTS/" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                   
                   if [ "$FLOATINGCLIENTS" -gt "0" ] ; then
                       # add warning
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/# WARNING: When the LSF floating client license feature is/;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/# enabled, ANY host will be able to submit jobs to the cluster./;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/# This is a security risk. It is recommended that you add the/;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/# parameter FLOAT_CLIENTS_ADDR_RANGE. See the LSF Administrator's/;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/# Guide for more details./;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                       # add new FLOAT_CLIENTS line
                       mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                       sed "/End Parameters/{x;s/.*/#FLOAT_CLIENTS= $FLOATINGCLIENTS/;G;}" < /${LSF_CONFDIR}/lsf.cluster1.tmp.$$ > /${LSF_CONFDIR}/lsf.cluster.tmp.$$
                   fi

                   rm -f /${LSF_CONFDIR}/lsf.cluster1.tmp.$$
                   mv /${LSF_CONFDIR}/lsf.cluster.tmp.$$ $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
                   chown $LSF_ADMIN $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
	           chmod 644 $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME
	       fi
	   fi
        fi

#       fi
    fi

#
# JS installation has been removed from lsfsetup from 4.2
#
#    if [ "$jsAddLater" = "y" ] ; then 
#        cat << MODIFY_JSRES
#
#        Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file 
#        needs to be further modified manually to include LSF_JobScheduler
#        in the resource part of the Hosts section for all your JobScheduler 
#        servers. For example:
#        
#        Begin   Host
#        HOSTNAME  model    ...   RESOURCES    #Keywords
#        apple     SPARCIPC ...   (LSF_JobScheduler bsd) #JobScheduler Server
#        ...
#        End     Host
#
#
#MODIFY_JSRES
#
#    fi

#
# Analyzer installation has been removed from 4.2
#
#    if [ "$analyzerAddLater" = "y" ] ; then
#	cat << MODIFY_ANAL
#
#	Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file
#	needs to be further modified manually to include LSF_Analyzer
#	in the resource part of the Hosts section for all your Analyzer 
#        servers. For example:
#
#	Begin   Host
#	HOSTNAME  model    ...   RESOURCES  
#	apple     SPARCIPC ...   (LSF_Analyzer bsd)
#	...
#	End     Host
#
#MODIFY_ANAL
#    fi

    if [ "$makeAddLater" = "y" ] ; then
	cat << MODIFY_MAKE

	Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file
	needs to be further modified manually to include LSF_Make
	in the resource part of the Hosts section for all your Make 
        servers. For example:

	Begin   Host
	HOSTNAME  model    ...   RESOURCES   
	apple     SPARCIPC ...   (LSF_Make bsd)
	...
	End     Host

MODIFY_MAKE
    fi

#
# Parallel installation has been removed from lsfsetup from 4.2
#
#    if [ "$parallelAddLater" = "y" ] ; then
#        cat << MODIFY_PARA
#
#	Your $LSF_CONFDIR/lsf.cluster.$LSF_CLUSTER_NAME file
#	needs to be further modified manually to include LSF_Parallel
#	in the resource part of the Hosts section for all your Parallel
#        servers. For example:
#
#	Begin   Host
#	HOSTNAME  model    ...   RESOURCES
#	apple     SPARCIPC ...   (LSF_Parallel bsd)
#	...
#        End     Host
#MODIFY_PARA
#    fi
}
# check_features


#-------------------------------------------------------------
#
# get_lsf_manager() --
# 
#   From cluster file get lsf manager name
#-------------------------------------------------------------

get_lsf_manager()
{
    LSF_MANAGER=`sed -n "/^[ 	]*[Bb]egin[	 ]*[Cc]luster[Mm]anager/,/^[	 ]*[Ee]nd[	 ]*[Cc]luster[Mm]anager/p" ${LSF_CLUSTER} | grep "^[ 	]*Manager" | $AWK 'BEGIN{FS = "="} {print $2}'`

    if [ "$LSF_MANAGER" = "" ] ; then
        LSF_MANAGER=`sed -n "/^[ 	]*[Bb]egin[	 ]*[Cc]luster[Aa]dmins/,/^[	 ]*[Ee]nd[	 ]*[Cc]luster[Aa]dmins/p" ${LSF_CLUSTER} | grep -i "^[ 	]*Administrators" | $AWK 'BEGIN{FS = "="} {print $2}'`    
    fi

    if defined "$LSF_MANAGER" ; then
        echo LSF cluster administrator is $LSF_MANAGER.
	export LSF_MANAGER
        sleep 3
    else
	echo "Cannot find LSF cluster administrator definition in ${LSF_CLUSTER}."
        echo
	exit
    fi

} # get_lsf_manager()

#----------------------------------------------------------------
# get_db_host
# 
#    get a host name to install Database server and set
#    LSF_DB_HOST = hostname
#
#    Note: The functure is unused.
#----------------------------------------------------------------
get_db_host()
{

if [ "$INSTALL_ANALYZER" = y  -a "$LSF_DB_HOST" = "" ] ; then
    echo 
    echo "Please choose one host to run LSF Analyzer database server." |tee -a $PREFIX_LOG/Install.log
    echo "The host must have at least 200 MB free local disk space."
    echo 
    while [ 1 ] 
    do 
         LSF_DB_HOST=`ask "Enter the hostname" ""`

         echo "get_db_host: Possibly using ypmatch $LSF_DB_HOST hosts.byname to search for host." >> $PREFIX_LOG/Install.log
	     
         if (grep "$LSF_DB_HOST" /etc/hosts \
	     || ypmatch $LSF_DB_HOST hosts.byname \
             || nismatch name=$LSF_DB_HOST hosts.org_dir ) >$LSNULFILE 2>&1
         then
             break 
         elif test "" != "`(nslookup $LSF_DB_HOST 2>&1) | grep '\*\*\*' 2>&1`"
         then
             echo "Cannot find \"${LSF_DB_HOST}\" in /etc/hosts, NIS(YP), or DNS."
             if yesno "Are you sure \"${LSF_DB_HOST}\" is a valid host name?" Y "" >$LSNULFILE
             then
  	         break
             else 
	         continue
             fi
          fi
          break
     done
     export LSF_DB_HOST
fi

}

showCopyright()
{
    if [ "$LSFQA" != "qa" ]; then
        more scripts/license_agreement.txt
    fi
}

SystemType()
{
# Name: SystemType: return the type of the system
# Synopsis: SystemType
# Description:
#   This function determine the type of the system on which it is executing 
#   and returns one of following strings:
#   	AIX	DECOSF	HP	SCO	SGI	SOLARIS
#   	SUNBSD	ULTRIX	LINUX   CRAY

#   ULTRIX is returned for Digital Equipment's older BSD systems and DECOSF 
#   is returned for their OSF based systems.

#   SUNBSD is returned for Sun BSD systems (versions 4.*) and SOLARIS is 
#   returned for Sun System V sytems. (version 5.*)

#   CRAY UNICOS. The systems tested 
#      1. edge.cray.com    = CRAY SV1 Unicos    10.0, 
#      2. subzero.cray.com = CRAY TS  Unicos    10.0hs, and 
#      3. hubble.cray.com  = CRAY T3E Unicos/mk 2.0.5 
#   give a code snNNNN, where N is a decimal digit, 
#   in response to the system name query "uname -s"
   
#   If this function is unable to determine the type of the system, it will
#   return an empty string.

    _HOSTNAME=`hostname | sed 's/\..*//'`

    case `uname -s` in
    AIX)	echo AIX ;;
    Darwin)	echo MACOSX ;;
    HP-UX)	echo HP ;;
    Linux)      echo LINUX ;;
    IRIX*)	echo SGI ;;
    OSF1)	echo DECOSF ;;
    ULTRIX)	echo ULTRIX ;;
    SUPER-UX)   echo SUPER-UX ;;
    UNIX_System_V) echo UNIX_System_V ;;
    SunOS) 	case `uname -r` in
		    4*) echo SUNSBD ;;
		    5*) echo SOLARIS ;;
 		esac
		;;
    $_HOSTNAME) case `uname -m` in
		    IP*)  echo SGI ;;
		    i386) echo SCO ;;
		esac
		;;
    sn*)        case `uname -m` in
                    CRAY*) echo CRAY;;
                esac
                ;;
    esac
}

SpaceAvail()
{ 
DIRECTORY=$1
SYSTEM=`SystemType`

case $SYSTEM in
    UNIX_System_V)
	SPACE=`df -k "$DIRECTORY" | sed -n '2p' | awk '{print $2}'`
	FileSysType=`df -k $DIRECTORY | sed -n '2p'| awk '{print $1}'`
	;;
    SUPER-UX)
        SPACE=`df -c "$DIRECTORY" | sed -n '2p' | awk '{print $4}'`
        DISK=`df -c .|sed -n '2p'|awk '{print $1}'`
        FileSysType=`mount | grep "$DISK" |awk '{print $5}'`
        ;;
    UTRIX)
        SPACE=`df $DIRECTORY | sed -n '3p' | awk '{print $4}'`
        FileSysType=`df $DIRECTORY | sed -n '3p' | awk '{print $1}'`
        ;;
    LINUX|SUNBSD|MACOSX) 	
        SPACE=`df $DIRECTORY | sed -n '2p' | awk '{print $4}'` 
        FileSysType=`df $DIRECTORY | sed -n '2p' | awk '{print $1}'`

#Reason:In Linux, if the first field is longer,and the rest fields will be moved
#       to the next line.
        if [ "$SPACE" = "" ] ; then 
            SPACE=`df $DIRECTORY | sed -n '3p' | awk '{print $3}'` 
        fi
        ;; 
    SOLARIS|DECOSF)
        SPACE=`df -k $DIRECTORY | sed -n '2p' | awk '{print $4}'` 
        FileSysType=`df -k $DIRECTORY | sed -n '2p' | awk '{print $1}'`
        ;; 
    SGI)
        SPACE=`df -k $DIRECTORY | sed -n '2p' | awk '{print $5}'`
        FileSysType=`df -k $DIRECTORY | sed -n '2p' | awk '{print $1}'`
        ;;
    HP)
        BLOCKS=`df -f $DIRECTORY >$LSNULFILE 2>&1`
        if [ $? != "0" ]; then
	    SPACE=`bdf -b $DIRECTORY | tail -1 | awk '{print $(NF-2)}'`
            FileSysType=`bdf -b $DIRECTORY | sed -n '2p' | awk '{print $1}'`
        else
            BLOCKS=`df -f $DIRECTORY | sed -e 's/.*: *//' -e 's/ .*//'`
            FileSysType=`df -f $DIRECTORY | awk '{print $1}'`
            SPACE=`expr $BLOCKS / 2`
        fi
        ;;
    AIX)
        BLOCKS=`df $DIRECTORY | sed -n '2p' | awk '{print $3}'`
        FileSysType=`df $DIRECTORY | sed -n '2p' | awk '{print $1}'`
        SPACE=`expr $BLOCKS / 2`
        ;;
  CRAY)
         # The -P option of "df" ensures a standard (Posix) info format:
         #   1              2            3       4           5         6
         #   Filesystem     1024-blocks  Used    Available   Capacity  Mounted on
         #
         SPACE=`df -kP $DIRECTORY | sed -n '2p' | awk '{print $4}'`
         FileSysType=`df -kP $DIRECTORY | sed -n '2p' | awk '{print $1}'`
         ;;
    *)
        SPACE="" 
        ;;
esac

if [ "$FileSysType" = "AFS" ]; then
    SPACE=`fs diskfree $DIRECTORY | sed -n '2p' | awk '{print $4}'`
    echo $SPACE
elif [ "$FileSysType" = "DFS" ]; then
    QUOTA=`fts lsquota $DIRECTORY | sed -n '2p' | awk '{print $2}'`
    usedQUOTA=`fts lsquota $DIRECTORY | sed -n '2p' | awk '{print $3}'`
    SPACE=`expr $QUOTA - $usedQUOTA`
    echo $SPACE
else
    echo $SPACE
fi
}

uncom_untar()
{
f1="${LSF_DIST_DIR}/$1.tar.Z"
f2="$1.tar.Z"
f3="$1.tar"


if [ -f $f1 ] ; then

    cp "$f1" .

    if [ "$?" -eq 0 ]; then
        :
    else
        echo "Can not prepare distribution files!" | tee -a $PREFIX_LOG/Install.log
        echo "LSF installation exiting..." | tee -a $PREFIX_LOG/Install.log
        kill $$
        exit
    fi

    uncompress  -c "$f2" | tar xvf - >$LSNULFILE  2>&1

    if [ "$?" -eq 0 ]; then
        :
    else
        echo "Can not prepare distribution files!" | tee -a $PREFIX_LOG/Install.log
        echo "LSF installation exiting..." | tee -a $PREFIX_LOG/Install.log
        kill $$
        exit
    fi

    rm "$f2"
else

    cp "${LSF_DIST_DIR}/$1.tar" .

    if [ "$?" -eq 0 ]; then
        :
    else
        echo "Can not prepare distribution files!" | tee -a $PREFIX_LOG/Install.log
        echo "LSF installation exiting..." | tee -a $PREFIX_LOG/Install.log
        kill $$
        exit
    fi

    tar xvf "$f3" >$LSNULFILE  2>&1

    if [ "$?" -eq 0 ]; then
        :
    else
        echo "Can not prepare distribution files!" | tee -a $PREFIX_LOG/Install.log
        echo "LSF installation exiting..." | tee -a $PREFIX_LOG/Install.log
        kill $$
        exit
    fi

    rm "$f3"
fi


}

# uncompress and untar a tar file
# Usage: uncompress_untar file_number_in_SelectedOSVersions
uncompress_untar()
{
_tf="$1"
_FileNumber=`grep "^$_tf:" $TABLE | cut -d ':' -f2`
cd $LSF_WORKING_DIR

CurrentBinVersion=$_tf
export CurrentBinVersion

if [ -d ${LSF_DIST_DIR}/$_tf ]; then

    ln -s  ${LSF_DIST_DIR}/$_tf ./$_tf


else
	echo
        echo "Extracting $_tf distribution tar files ..."
	echo

	make_dir "$_tf" 
	if [ ! -d "$_tf" ] ; then
            echo "Can not create $_tf!" | tee -a $PREFIX_LOG/Install.log
            echo "LSF installation exiting..." | tee -a $PREFIX_LOG/Install.log
            kill $$
            exit
	fi

	cd $_tf

	if [ "$_FileNumber" = "1" ]; then
	    _TarFile=$_tf ; uncom_untar $_TarFile
	else
	    _TarFile=`echo "${_tf}.part1"` ; uncom_untar $_TarFile
	    _TarFile=`echo "${_tf}.part2"` ; uncom_untar $_TarFile
	fi

fi
}

# Create a directory
# Do it all in () to not step on the positional parameters $1, $2, ...
is_root()
{

    if [ "$LSHOSTTYPE" = "NT" ] ; then
        return 0
    fi

    if  [ `uname` = ConvexOS ] ; then
        if [ `whoami` = root ] ; then
            return 0
        else
            return 1
        fi
    else
        if [ `id | sed -e 's/[^(]*(\([^)]*\)).*/\1/'` = root ] ; then
            return 0
        else
            return 1
        fi
    fi
}           


make_dir()
{
(
    dest=$1
    # Split the destination name on all slashes and put in $1, $2 ...
    # add the /x/ to avoid problems with $dest == "/"
    _temp=/x/$dest
    IFS=/
    set $_temp 
    # Some shells handle this "set" very strangely.  Setting again fixes it.
    IFS="       "
    set $*
    shift               # get rid of x

    # Are we starting from root?
    case $dest in
        /*) existd=/ ;;
        *) existd= ;;
    esac
    dsep1=                                                  
    while test "$1" != "" -a -d "$existd$dsep1$1"
    do
        existd="$existd$dsep1$1"
        dsep1=/
        shift
    done
    destd=
    dsep=
    while test "$1" != ""
    do
        destd=$destd$dsep$1
        if test -f "$existd$dsep1$destd" ; then
           echo "$0: $existd$dsep1$destd is not a directory."
           exit 1
        fi
        dsep=/
        shift
    done
    if test ! -z "$existd" ; then
        cd $existd
    fi
    if test ! -z "$destd" ; then
        echo                                        
        echo "    Creating $dest ..."
        echo
        mkdir -p $destd 
    fi
)
}
#-----------------------------------------------------------------
#
#   writable: Check if a directory is writable or not.
#---------------------------------------------------------------
writable()
{
    DIRNAME=$1
    TESTFILE=$2

#   find out the longest existing component in path
    if [ -d "$DIRNAME" ] ; then
        if [ "$TESTFILE" != ""  -a  -f $DIRNAME/$TESTFILE ] ; then
            touch $DIRNAME/$TESTFILE > $LSNULFILE 2>&1
            if [ $? != 0 ] ; then
                echo Cannot overwrite  $DIRNAME/$TESTFILE
                echo Please check file permissions.
                return 1
            fi
        fi
    else
        while  [ "$DIRNAME" != "/"  -a "$DIRNAME" != "." ] ; do
            DIRNAME=`dirname $DIRNAME`
            if [ -d $DIRNAME ] ; then
                break;                     
            fi
        done
    fi
# check permissions ( "test -w" does not work correctly on HPUX, so touch)
    touch $DIRNAME/.lsf.$$ >$LSNULFILE 2>&1
    if [ $? = 0 ] ; then
        rm $DIRNAME/.lsf.$$
        return 0
    else
        echo Cannot create or write $DIRNAME
        echo Please check file permissions.
        return 1
    fi
}                                              

#------------------------------------------------------------------
# is_local --
#
#     If $1 is a local directory, return 0
#     Otherwise, return 1
#------------------------------------------------------------------
is_local()
{
     # Basically, only two formats about df in all platforms.
     # One is (fs:pathname) in df output.
     # Another is fs:pathname in df output

     df_value=`df $1 2>$LSNULFILE | grep -v "Filesystem"`

     if [ "$df_value"x = x ]; then
         #HP 9 df will fail, try bdf
         df_value=`bdf  $1 2>$LSNULFILE`
         if [ "$df_value"x = x ]; then
             echo "Error running df command."
             echo
             return 1
         fi                   
     fi

     _ht=`uname`
     case $_ht in
     Linux) 
 	dirname=`echo $df_value | $AWK '{print $1}'`
	;;
     *) 
	dirname=`echo $df_value |$AWK -F\( '{print $2}' |$AWK -F\) '{print $1}'`
	;;
     esac

     if [ "$dirname"x = x ]; then
         #Not first format, try another.
         fs=`echo $df_value | sed  -n -e '/:/p'`
         if [ "$fs"x = x ]; then
             return 0
         else
             return 1
         fi
     else
         # dirname is fs:pathname (mounted) or pathname (local)
         fs=`echo $dirname | sed  -n -e '/:/p'`
         if [ "$fs"x = x ]; then
             return 0
         else
             return 1
         fi
     fi
}
                            
#------------------------------------------------------------------
# Name: strstr
# Synopsis: strstr $SRC_STR $SUB_STR
# Description:
#       check whether $SUB_STR is in $SRC_STR or not
#       grep -w does not work on SGI or HP.
# Return Value:
#       0 yes 1 no
#------------------------------------------------------------------
strstr ()
{
_src_string="$1"
_sub_string="$2"

if [ "$_src_string" = "" -o "$_sub_string" = "" ]; then
    return 1
fi

for _item in $_src_string
do
    if [ "$_item" = "$_sub_string" ]; then
        return 0
    fi
done

return 1
} # strstr

#-----------------------------------
# Name: is_slurm_host
# Synopsis: is_slurm_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is a slurm host
#       check HP-hptc/normal linux with slurm installed.
#       Currently we only check for HP-hptc: /var/lsf/lsfslurm
#       in the output
# Return Value:
#       0 slurm host; 1 non-slurm host
#-----------------------------------
is_slurm_host ()
{
    _slurm_signature_file="/var/lsf/lsfslurm"

    if [ -f  "$_slurm_signature_file" ]; then
        # HP hptc release
        return 0
    fi
    return 1
} # is_slurm_host

#-----------------------------------
# Name: is_rms_host
# Synopsis: is_rms_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is RMS host
# Return Value:
#        0 RMS host; 1 non-RMS host
#-----------------------------------
is_rms_host ()
{
    RMSQUERY="rmsquery"
    which "$RMSQUERY" 2>"$LSNULFILE"

    if [ "$?" = "0" ]; then
       HOST_NAME=`hostname | cut -d. -f 1`
       RMS_HOST_LIST=`$RMSQUERY 'select name from nodes' 2>$LSNULFILE`
       if strstr "$RMS_HOST_LIST" "$HOST_NAME" ; then
          return 0
       fi
    fi
    return 1 

} #is_rms_host

#-----------------------------------
# Name: is_bproc_host
# Synopsis: is_bproc_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is bproc host
# Return Value:
#       0 bproc host; 1 non-bproc host
#-----------------------------------
is_bproc_host ()
{
    if [ -f /usr/lib/libbproc.so ]; then 
        return 0
    else
        return 1
    fi
} # is_bproc_host 


#--------------------------------------------------------------------------
#
#  found_binary_type -
#     try to get release binary type from uname -a.
#
#  NOTE:
#     When we support a new platform, we should  modify this function
#     so that it can find correct binary type.
#
#     if success,  return 0 and set type to BINARY_TYPE.
#     Otherwise, return 1
#---------------------------------------------------------------------------      
found_binary_type()
{

    uname_val="$1"
    BINARY_TYPE="fail"
    HOST_TYPE=DEFAULT
    REDHAT_TYPE="isnotredhat" 
    export BINARY_TYPE
    export HOST_TYPE
    field1=`echo $uname_val |$AWK '{print $1}'`
    case "$field1" in
    OSF1)
           version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
           if [ $version = "V5" ] ; then
               BINARY_TYPE="alpha5"
           else
              if [ $version = "T5" ] ; then
                 BINARY_TYPE="alpha5"
               else
                  BINARY_TYPE="alpha"
               fi
           fi

           if [ "$BINARY_TYPE" = "alpha5" ]; then
	       is_rms_host
               if [ "$?" = "0" ]; then
                   BINARY_TYPE="alpha5-rms"
               fi
           fi

           HOST_TYPE="DigitalUNIX" ;;
    HI-UX)
           BINARY_TYPE="hppa"
           HOST_TYPE="HITACHI"
           ;;
   HP-UX)
        # spp need to run uname -C to find the version
        uname -C 2>$LSNULFILE
        if [ $? = 1 ]; then
            version=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
            if [ $version = "09" ] ; then
                BINARY_TYPE="hppa9"                                
            elif [ $version = "10" ] ; then
                subVersion=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
                if [ $subVersion = "10" ]; then
                    BINARY_TYPE="hppa10"
                elif [ $subVersion = "20" ]; then
                    BINARY_TYPE="hppa10.20"
                fi
            elif [ $version = "11" ] ; then
                curLocalHost=`hostname`
                if [ "$curLocalHost" != "$_host" ]; then
                    kernel=`$RSH $_host /usr/bin/getconf KERNEL_BITS`
                else
                    kernel=`/usr/bin/getconf KERNEL_BITS`
                fi
              
                if [ "$kernel" = "32" ] ; then
                   BINARY_TYPE="hppa11-32"
                else
                   BINARY_TYPE="hppa11-64"
                fi
            else
                
                echo "Cannot figure out the HP version."
                exit 1
            fi
            HOST_TYPE="HPPA"
        else
            version=`uname -C -a |  $AWK '{split($3, a, "."); print a[1]}'`
            if [ $version = "5" ]; then
                BINARY_TYPE="spp"
            elif  [ $version = "4" ]; then
                BINARY_TYPE="spp4"
                HOST_TYPE="SPP4"
            fi                               
            HOST_TYPE="EXEMPLAR"
        fi
        ;;
    Linux)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        if [ $version = "1" ]; then
            BINARY_TYPE="linux"
        elif [ $version = "2" -o $version -gt 2 ]; then
              subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]"."a[2]}'`

	      curLocalHost=`hostname`

              if [ "$curLocalHost" != "$_host" ]; then
                  libcver=`$RSH $_host "ls /lib/libc-*" 2>/dev/null`
                  redhat=`$RSH $_host "ls /etc/redhat-release" 2>/dev/null`
              else
                  libcver=`ls /lib/libc-* 2> /dev/null`
                  redhat=`ls /etc/redhat-release 2>/dev/null`
              fi

              libcver=`echo $libcver | $AWK '{split($1, a, "."); print a[3]}'`

              if [ $subver = "2.0" ]; then
                  BINARY_TYPE="linux2"

                  machine=`echo $uname_val | $AWK '{print $11}'`
                  if [ "$machine" = "alpha" ]; then
                      BINARY_TYPE="linux2-alpha"
                  else
                      BINARY_TYPE="linux2-intel"
                  fi

              elif [  "$subver" = "2.2" ]; then
                  BINARY_TYPE="linux2.2"

                  smp=`echo $uname_val | $AWK '{print $5}'`
                  if [ "$smp" = "SMP" ]; then
			machine=`echo $uname_val | $AWK '{print $12}'`
		  else
			machine=`echo $uname_val | $AWK '{print $11}'`		  	
		  fi
		
                  if [ "$machine" = "alpha" ]; then
                      BINARY_TYPE="linux2.2-glibc2.1-alpha"
                  elif [  "$machine" = "sparc64" -o "$machine" = "sparc" ]; then

		      BINARY_TYPE="linux2.2-glibc2.1-sparc"

		  elif [  "$machine" = "ppc" ]; then

		     BINARY_TYPE="linux2.2-glibc2.1-powerpc"

		  else

                     BINARY_TYPE="linux2.2-glibc2.1-x86"
                  fi
                elif [ "$subver" = "2.4" ]; then

# ia64 is the family of 64-bit CPUs from Intel. We shouldn't need a new
# distribution for each processor
                  smp=`echo $uname_val | $AWK '{print $5}'`
                  if [ "$smp" = "SMP" ]; then
                        machine=`echo $uname_val | $AWK '{print $12}'`
                  else
                        machine=`echo $uname_val | $AWK '{print $11}'`
                  fi
		  if [ "$machine" = "ia64" ]; then
		     BINARY_TYPE="linux2.4-glibc2.2-ia64"
                     is_rms_host
                     if [ "$?" = "0" ]; then
		         BINARY_TYPE="linux2.4-glibc2.2-ia64-rms"
		     fi
		     # check sgi-release and see whether this is a sn-ipf 
                     if [ -f /etc/sgi-release ]; then
                         cat /etc/sgi-release | grep "ProPack" > $LSNULFILE 2>&1
			 if [ "$?" = "0" ]; then
                             BINARY_TYPE="linux2.4-glibc2.2-sn-ipf"
			 fi
 		     fi
                  elif [ "$machine" = "ppc64" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-ppc64"
                  elif [ "$machine" = "s390" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390-32"
                  elif [ "$machine" = "s390x" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390x-64"
                  elif [ "$machine" = "armv5l" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-armv5l"
                  elif [ "$machine" = "alpha" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-alpha"
                     is_rms_host
                     if [ "$?" = "0" ]; then
                         BINARY_TYPE="linux2.4-glibc2.2-alpha-rms"
                     fi
                  elif [ "$machine" = "x86_64" ]; then
                       _libcver=`/lib/libc.so.6 | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
                       _cputype=`cat /proc/cpuinfo | grep -i vendor | $AWK '{print $3}' | uniq`
                       if [ "$_libcver" = "2" -o "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.4-glibc2.2-x86_64"
                       fi
                       is_slurm_host
                       if [ "$?" = "0" ]; then
                          BINARY_TYPE="linux2.4-glibc2.3-x86_64-slurm"
                       fi
                  else
		     echo $uname_val | $AWK '{print $3}' | grep "_Scyld" > $LSNULFILE 2>&1
		     if [ "$?" = "0" ]; then
                        BINARY_TYPE="linux2.4-glibc2.2-x86-scyld"
		     else
                       _libcver=`/lib/libc.so.6 | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`

                       if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.4-glibc2.1-x86"
                       elif [ "$_libcver" = "2" ]; then
                          BINARY_TYPE="linux2.4-glibc2.2-x86"
			  if [ -d "/proc/vmware" ]; then
			      BINARY_TYPE="linux2.4-glibc2.2-x86-esx2.5"
			  fi
		       else
                          BINARY_TYPE="linux2.4-glibc2.3-x86"
                       fi

                        is_rms_host
                        if [ "$?" = "0" ]; then   
                            BINARY_TYPE="linux2.4-glibc2.1-x86-rms"
                        fi
                        is_bproc_host
                        if [ "$?" = "0" ]; then   
                            BINARY_TYPE="${BINARY_TYPE}-bproc"
			    if [ -f /usr/lib/libbproc.so.4 ]; then
			        BINARY_TYPE="${BINARY_TYPE}4"
                            fi
                        fi
		     fi
                  fi
               fi
        fi                            
        HOST_TYPE="LINUX"
        ;;
    UNIX_System_V)
	BINARY_TYPE="uxp"
	HOST_TYPE="vpp"
	;;
    UNIX_SV)
        BINARY_TYPE="mips-nec"
        HOST_TYPE="NECEWS"
        ;;
    Darwin)
        BINARY_TYPE="macosx"
        HOST_TYPE="MAC OS X"
        ;;
    NEWS-OS)
        BINARY_TYPE="mips-sony"
        HOST_TYPE="SONY"
        ;;
    AIX)
        version=`echo $uname_val | $AWK '{print $4}'`
        release=`echo $uname_val | $AWK '{print $3}'`
        if [ $version = "4" -a $release -ge 1 ]; then
            if [ $release -lt 2 ]; then
                BINARY_TYPE="aix4"
                HOST_TYPE="IBMAIX4"
            elif [ $release -ge 2 ]; then
                BINARY_TYPE="aix4.2"
                HOST_TYPE="IBMAIX4"
            fi
        elif [ $version = "5" -a $release -ge 1 ]; then
            #On AIX, only in 64bit mode, you can see shlap64 process
            #check shlap64 process is safer than check kernel_bit
            if [ `ps -e -o "comm" |grep shlap64` ]; then
                BINARY_TYPE="aix-64"
            else
                BINARY_TYPE="aix-32"
            fi
            HOST_TYPE="IBMAIX"
        elif [ $version = "3" ]; then
            BINARY_TYPE="aix3"
            HOST_TYPE="IBMAIX3"
        fi
        ;;                                                    
    IRIX*)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        release=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}' | cut -f1 -d'-'`
        if [ $version = "6" -a $release -ge "2" ]; then
            curLocalHost=`hostname`

            if [ "$curLocalHost" != "$_host" ]; then
                mls=`$RSH $_host sysconf MAC`
            else
	        mls=`sysconf MAC`
            fi

	    if [ "$mls" = "1" ]
	    then
		# TRIX installation
		BINARY_TYPE="trix6"
	    else
	        if [ $version = "6" -a $release -eq "5" ]; then

                    curLocalHost=`hostname`

                    if [ "$curLocalHost" != "$_host" ]; then
		        modification=`$RSH $_host /sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    else
		        modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    fi
                    
                    if [ -z "$modification" ]; then
	                BINARY_TYPE="sgi6.5"
	                HOST_TYPE="SGI65"
                    else
		        if [ $modification -ge "8" ]; then 
	                    BINARY_TYPE="irix6.5.24"
	                    HOST_TYPE="SGI65"
		        else
	                    BINARY_TYPE="sgi6.5"
	                    HOST_TYPE="SGI65"
		        fi
                     fi
		elif [ $version = "6" -a $release -eq "2" ]; then
		    BINARY_TYPE="sgi6"
		elif [ $version = "6" -a $release -ge "4" ]; then
		    BINARY_TYPE="sgi6"
		fi
	    fi

	    if [ $version = "6" -a $release -eq "2" ]; then
		HOST_TYPE="SGI6"
	    elif [ $version = "6" -a $release -ge "4" ]; then
		HOST_TYPE="SGI6"
	    fi
        else
            BINARY_TYPE="sgi5"
            HOST_TYPE="SGI5"
        fi
        ;;
    SunOS)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        minorver=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        machine=`echo $uname_val | $AWK '{print $5}'`
        if [ $version = "4" ]; then
            BINARY_TYPE="sunos4"
            HOST_TYPE="SUN41"
        else
            if [ "$machine" = "i86pc" ]; then
                curLocalHost=`hostname`
                if [ "$curLocalHost" != "$_host" ]; then
                    BIT64=`$RSH $_host /usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                else
                    BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                fi

                if [ "$BIT64" -eq "0" ] ; then

                    if [ "$minorver" = "7" -o "$minorver" = "8" -o "$minorver" = "9" -o "$minorver" = "10" ] ; then
                        BINARY_TYPE="x86-sol7"
                    else
                        BINARY_TYPE="x86-sol2"
                    fi
                else

                    if [ "$minorver" = "10" -o "$minorver" -gt "10" ] ; then
                        BINARY_TYPE="x86-64-sol10"
                    fi
                fi
            else
                BINARY_TYPE="sparc-sol2"                           
                if [ "$minorver" -ge "7" -a "$minorver" -le "9" ]; then
                    curLocalHost=`hostname`

                    if [ "$curLocalHost" != "$_host" ]; then
			BIT64=`$RSH $_host /usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                    else
                        BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                    fi
			
                    if [ "$BIT64" -eq "0" ]; then
                        BINARY_TYPE="sparc-sol7-32"
                    else  
                        BINARY_TYPE="sparc-sol7-64"
                    fi
                fi
            fi
            if [ "$machine" = "sun4H" ]; then
                HOST_TYPE="HALSOL"
            else
                HOST_TYPE="SUNSOL"
            fi
        fi
        ;;
    SUPER-UX)
        lastfield=`echo $uname_val | $AWK '{print $NF}'`
        if [ $lastfield = "SX-3" ]; then
            BINARY_TYPE="sx3"
            HOST_TYPE="NECSX3"
        elif [  $lastfield = "SX-4" ]; then
            BINARY_TYPE="sx4"                  
            HOST_TYPE="NECSX4"
	elif [ $lastfield = "SX-5" ]; then
	    BINARY_TYPE="sx5"
	    HOST_TYPE="NECSX5"
	elif [ $lastfield = "SX-6" ]; then
	    BINARY_TYPE="sx6"
	    HOST_TYPE="NECSX6"
        elif [ $lastfield = "SX-8" ]; then
            BINARY_TYPE="sx8"
            HOST_TYPE="NECSX8"
        fi
        ;;
    ULTRIX)
        BINARY_TYPE="ultrix"
        HOST_TYPE="ULTRIX"
        ;;
    ConvexOS)
        BINARY_TYPE="convex"
        HOST_TYPE="CONVEX"
        ;;
    *)
        # cray machine
        lastfield=`echo $uname_val | $AWK '{print $NF}'`
        if [ $lastfield = "TS" ]; then
            SUBTITLE=`target -s | cut -d= -f2`
            if [ "$SUBTITLE" = " CRAY-TS-IEEE" ]; then
                BINARY_TYPE="crayt90-ieee"
            else
                BINARY_TYPE="crayt90"
            fi
            HOST_TYPE="CRAYT"
        elif [ $lastfield = "C90" ]; then
            BINARY_TYPE="crayc90"
            HOST_TYPE="CRAYC"
        elif  [ $lastfield = "J90" -o $lastfield = "Y-MP" ]; then
            BINARY_TYPE="crayj90"
            HOST_TYPE="CRAYJ"
        elif [ $lastfield = "SV1" ]; then
            BINARY_TYPE="craysv1"
            HOST_TYPE="CRAYSV1"
        elif [ $lastfield = "T3E" ]; then
            BINARY_TYPE="crayt3e"
            HOST_TYPE="CRAYT3E"
        elif [ $lastfield = "crayx1" ]; then
            BINARY_TYPE="crayx1"
            HOST_TYPE="CRAYX1"
        fi
        ;;
    esac
    export BINARY_TYPE
    if [ $BINARY_TYPE = "fail" ]; then
        return 1
    else
        return 0
    fi
}                                     

modify_conf()
{
_conf=$1
_param=$2
_value=$3

#_oldConf="$1.old"
_oldConf="$1.$$"

mv $_conf $_oldConf


if [ ! -f "$_oldConf" ]; then
    echo "$_oldConf does not exist!"
    echo "Can not modify lsf.conf!"
    return
fi

_done=n

exec 3<&0 <$_oldConf
while read LINE
do
    case $LINE in
    *"$_param"*) 
	test "$_value" = "DELETE" && { _done=y; continue; }
	echo $_param=$_value >> $_conf; _done=y 
	;;
    *) 	echo $LINE >> $_conf 
	;;
    esac
done
exec 0<&3 3<&-

rm -f $_oldConf

test "$_value" = "DELETE" && return
test "$_done" = "n" && echo $_param=$_value >> $_conf 
}

#
#-----------------------------------------
#
# make_os_flag --
#
# ARGUMENTS:
# 
# PRE-CONDITION:
#
# DESCRIPTION:
#
# SIDE_EFFECTS:
#
# RETURN:
#
#-----------------------------------------
#
make_os_flag()
{
if [ -f ${LSF_SERVERDIR}/osversion ]; then
    rm ${LSF_SERVERDIR}/osversion
fi

touch ${LSF_SERVERDIR}/osversion

osFlagFile=${LSF_SERVERDIR}/osversion

echo "# This file is a flag of OS version." >> "$osFlagFile"
echo "# Please do not remove this file," >> "$osFlagFile"
echo "# otherwise, LSF install program can" >> "$osFlagFile"
echo "# not work properly." >> "$osFlagFile"
echo "$SelectedOSVersions" | sed -e "s/^[ 	]*//" | sed -e "s/^lsf"$LSF_VER"_//" >> "$osFlagFile"

}



#-----------------------------------------
#
# is_full_path --
#
# ARGUMENTS: $1: a path name 
# 
# PRE-CONDITION:
#
# DESCRIPTION: test whether a inputed path name
#              is a full path
#
# SIDE_EFFECTS:
#
# RETURN: 0: is full paht; 1: is not full path
#
#-----------------------------------------
#
is_full_path()
{

testPathName=$1

firstChar=`expr "$testPathName" : '\(.\).*'`

if [ "$firstChar" = "/" ]
then
    return 0
else 
    return 1
fi

}

#
#-----------------------------------------
#
# ask_dir --
#
# ARGUMENTS: $1: a parameter name of a directory
# 
# PRE-CONDITION:
#
# DESCRIPTION: This function asks the user 
#     to input a full paht name of a parameter
#     and does following tests:
#     1, Is the path name a full path?
#     2, Is the path name an existent file?
#
# SIDE_EFFECTS:
#
# RETURN: 
#
#-----------------------------------------
#
ask_dir()
{
varName=$1

while :
do
    pathName=`eval ask_param \$varName`

            
    if is_full_path $pathName; then
        :
    else
        echo "Parameter $varName requires a full path name." 1>&2 
        echo "$pathName is not a full path name." 1>&2
        echo "Please input a full path name." 1>&2
        continue
    fi
            
    if [ -f "$pathName" ]; then
        echo "Parameter $varName cannot be a file." 1>&2
        echo "It must be a directory." 1>&2
        echo "$pathName is an existent file." 1>&2
        continue
    else
        :
    fi
             
    break
done 

echo $pathName

return
}

#
#-----------------------------------------
#
# check_dir --
#
# ARGUMENTS: $1: a LSF param name
# 
# PRE-CONDITION:
#
# DESCRIPTION: This function checks whether
#     the directory referrenced by the given
#     param is empty and the free space this
#     directory can use.
#
# SIDE_EFFECTS:
#
# RETURN:
#
#-----------------------------------------
#
check_dir()
{
testVar=$1

testPath=`eval echo \\$$testVar`

case $testVar in
    LSF_MACHDEP )
        eval check_dir_empty \$$testVar 
        eval check_dir_space \$$testVar
        ;;
    LSF_INDEP )
        eval check_dir_empty \$$testVar
        eval check_dir_space \$$testVar
        ;;
    * ) 
        case $testPath in
            ${LSF_MACHDEP}/* )
                realChk="n"
                return ;;
            ${LSF_INDEP}/* )
                realChk="n"
                return ;;
            * ) 
                eval check_dir_empty \$$testVar
                eval check_dir_space \$$testVar
                realChk="y"
                ;;
        esac
        ;;
esac

return
}

#
#-----------------------------------------
#
# check_dir_space --
#
# ARGUMENTS: $1: full path name of a directory
# 
# PRE-CONDITION:
#
# DESCRIPTION: This function checks how much
#     disk space can be used by the given 
#     directory. 
#
# SIDE_EFFECTS:
#
# RETURN:
#
#-----------------------------------------
#
check_dir_space()
{

checkPath=$1

required_space "$OSVersionSelected"
LSF_WORKING_SPACE=$?

testBuild="n"

if [ -d $checkPath ]; then
    :
else
    make_dir $checkPath
    testBuild="y"
fi

_freeBytes=`SpaceAvail $checkPath`

if [ "$_freeBytes" != "" ]; then
    _freeSpace=`expr $_freeBytes / 1024`
fi

if [ "$_freeBytes" != "" ]; then

    cat << CHECK_DIR_SPACE_FOUND

         $checkPath has ${_freeSpace}M free space.

CHECK_DIR_SPACE_FOUND

    if [ "$_freeSpace" -le "${LSF_WORKING_SPACE}" ]; then
	 cat << SPACE_IS_NOT_AVAILABLE

	 Please make sure that the available space of $checkPath
         is enough for all the files.

         For referrence:
         the average size of the binary files of each host type is 100 MB,
         the size of all machine independent files is 3 MB.

  Please select another directory.

SPACE_IS_NOT_AVAILABLE

	return 101
   fi

else
    cat << CHECK_DIR_SPACE_NOT_FOUND

         lsfsetup cannot determine the available space of $checkPath
         Please make sure that the available space of $checkPath
         is greater than ${LSF_WORKING_SPACE} MB.

  Please select another directory.

CHECK_DIR_SPACE_NOT_FOUND
	return 102
fi

cat << CHECK_DIR_IS_OK
	The target directory is OK.
CHECK_DIR_IS_OK

if [ "$testBuild" = "y" ]; then
    rmdir $checkPath 2>$LSNULFILE
fi

return
}


#
#-----------------------------------------
#
# check_dir_empty --
#
# ARGUMENTS: $1: full path name of a directory
# 
# PRE-CONDITION:
#
# DESCRIPTION: This function checks whether
#     the directory is empty.
#
# SIDE_EFFECTS:
#
# RETURN:
#
#-----------------------------------------
#
check_dir_empty()
{

checkPath=$1


if [ -d $checkPath ]; then
    :
else
    echo
    echo "$checkPath will be built."
    echo
    return
fi

_ht=`uname`
if [ "$_ht" = "Darwin" ] ; then
    contains=`ls $checkPath | wc -w | sed -e 's/[^0-9]//g'`
else
    contains=`ls $checkPath | wc -w | sed -e 's/^[	 ]*//'`
fi

if [ "$contains" != "0" ]; then
    emptyDir="n"
else
    emptyDir="y"
fi

if [ "$emptyDir" = "y" ]; then
    echo
    echo "$checkPath is empty."
    echo
else
    echo
    echo "$checkPath is not empty. The new installation"
    echo "may fail or overwrite a previous installation."
    echo
fi

return
}

#-----------------------------------------------------------------------
#This function computes the space requirement after a user selects 
#the LSF binary file package(s).
#-----------------------------------------------------------------------
required_space()
{

if [ "$1" = "" ]; then
	return 0
fi

total_space_required=0
space_required=0

for _os_version in $1
do
	_os_name=`echo $_os_version | sed -e "s/lsf"$LSF_VER"_//"`
	case $_os_name in
	   aix*)
		space_required=100
		;;
	   alpha*)
		space_required=125
		;;
	   cray*)
		space_required=150
		;;
	   hppa*)
		space_required=140
		;;
	   linux* | redhat*)
		space_required=125
		;;
	   sgi*)
		space_required=125
		;;
	   sparc-sol*)
		space_required=230
		;;
  	   *)
		space_required=230
		;;
	esac
	total_space_required=`expr $total_space_required + $space_required`
done
echo "To install all platforms you selected, you need "$total_space_required"M disk space on your file system."
return $total_space_required
}

is_AFS()
{
    filesys=`df $LSF_MACHDEP|tail -1 |$AWK '{print $1}'`

    if [ "$filesys" = "AFS" ] ; then
	return 0
    else
	return 1
	fi
}

