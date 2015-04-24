#!/bin/sh
#--------------------------------------------------------------------------
# This code gets the binary type when setting LSF user environment.
# It is based on found_binary_type() in scripts/funcs.sh.
#---------------------------------------------------------------------------

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
# Name: is_rms_host
# Synopsis: is_rms_host
# Environment Variables:
# Description:
#       It checks whether or not the current host is RMS host
# Return Value:
#       0 RMS host; 1 non-RMS host
#-----------------------------------
is_rms_host ()
{
    RMSQUERY="rmsquery"
    which "$RMSQUERY" >$LSNULFILE 2>&1
    if [ "$?" = "0" ]; then
        HOST_NAME=`hostname | cut -d. -f 1`
        RMS_HOST_LIST=`$RMSQUERY 'select name from nodes' 2>$LSNULFILE`
        if strstr "$RMS_HOST_LIST" "$HOST_NAME" ; then
            field1=`echo $uname_val |$AWK '{print $1}'`
            if [ "$field1" = "Linux" ]; then
                if [ -z "$RMS_VERSION" ]; then
                    RMS_VERSION=`rpm -q qsrmslibs | sed -e "s/qsrmslibs\-//g" | sed -e "s/\-.*//g"` 
                    #
                    # This is safe, in /usr/bin/rmscheckenv, it greps "not installed" as well
                    # if qsrmslibs is not installed, override the binary type set before.
                    # It's a non-rms host
                    #
                    NOT_INSTALLED=`echo $RMS_VERSION | grep "not installed" 2>$LSNULFILE`
                    if [ -n "$NOT_INSTALLED" ]; then
                        RMS_VERSION=""
                        return 1
                    fi
                fi
                export RMS_VERSION
            fi
            return 0
        fi
    fi
    return 1
} #is_rms_host


_CUR_PATH_ENV="$PATH"
PATH=/usr/bin:/bin:/usr/local/bin:/local/bin:/sbin:/usr/sbin:/usr/ucb:/usr/sbin:/usr/bsd:${PATH}

# handle difference between system V and BSD echo
# To echo "foo" with no newline, do
# echo $enf "foo" $enl

if [ "`echo -n`" = "-n" ] ; then
    enf=
    enl="\c"
else
    enf=-n
    enl=
fi
export enf enl

 LSNULFILE=/dev/null

# Find a version of awk we can trust
AWK=""
for tmp in nawk  /usr/toolbox/nawk gawk awk 
do
#   This is the real test, for functions & tolower
#    if foo=`(echo FOO | $tmp 'function tl(str) { return tolower(str) } { print tl($1) }') 2>$LSNULFILE` \
#       && test "$foo" = "foo"
#   A simpler test, just for executability
    if ($tmp 'BEGIN{ } { }') < $LSNULFILE > $LSNULFILE 2>&1  
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
 
    uname_val=`uname -a`
    BINARY_TYPE="fail"
    export BINARY_TYPE
    field1=`echo $uname_val |$AWK '{print $1}'`
    case "$field1" in 
    UNIX_System_V)
	 BINARY_TYPE="uxp"
	 ;;

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
            # check for rms
	    # Kite#31479
	    if [ ! -z "`/usr/sbin/rcmgr get SC_MS`" ]; then
		BINARY_TYPE="alpha5-rms"
	    fi
         fi
	     ;;
    HI-UX) 
	   BINARY_TYPE="hppa"
 	   ;;
   HP-UX) 
        # spp need to run uname -C to find the version
        uname -C 2>$LSNULFILE
        if [ $? = 1 ]; then 
            version=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
	    if [ $version = "09" ] ; then
	        BINARY_TYPE="hppa9"
            elif [ $version = "10" ] ; then
                subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[3]}'`
                if [ $subver = "10" ]; then
                    BINARY_TYPE="hppa10"
                elif [ $subver = "20" ]; then
                    BINARY_TYPE="hppa10.20"
                fi
            elif [ $version = "11" ] ; then
                machine=`echo $uname_val | $AWK '{print $5}'`
                if [ "$machine" = "ia64" ]; then
                   BINARY_TYPE="hpuxia64"
                else
                   kernel=`/usr/bin/getconf KERNEL_BITS`
                   if [ "$kernel" = "32" ] ; then
                        BINARY_TYPE="hppa11-32"
                   else
                        BINARY_TYPE="hppa11-64"
                   fi
                fi
            else 
                echo "Cannot figure out the HP version."
		exit 1
            fi 
         else
	    version=`uname -C -a |  $AWK '{split($3, a, "."); print a[1]}'` 
	    if [ $version = "5" ]; then
	        BINARY_TYPE="spp"
            elif  [ $version = "4" ]; then
	        BINARY_TYPE="spp4"
             fi
         fi
        ;;
    Linux)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        if [ $version = "1" ]; then
	    BINARY_TYPE="linux"
        elif [  $version = "2" ]; then
            subver=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]"."a[2]}'`
            libcver=`ls /lib/libc-* 2> /dev/null`
            libcver=`echo $libcver | $AWK '{split($1, a, "."); print a[3]}'`

            if [ $subver = "2.0" ]; then
	        BINARY_TYPE="linux2"
                machine=`echo $uname_val | $AWK '{print $11}'`
                if [ "$machine" = "alpha" ]; then
                    BINARY_TYPE="linux2-alpha"
                else
                    BINARY_TYPE="linux2-intel"
                fi
            elif [  $subver = "2.2" ]; then
                BINARY_TYPE="linux2.2"

                smp=`echo $uname_val | $AWK '{print $5}'`
                if [ "$smp" = "SMP" ]; then
                    machine=`echo $uname_val | $AWK '{print $12}'`
                else
                    machine=`echo $uname_val | $AWK '{print $11}'`
                fi

                if [ "$machine" = "alpha" ]; then
                    BINARY_TYPE="linux2.2-glibc2.1-alpha"
                elif [ "$machine" = "sparc" -o "$machine" =  "sparc64" ]; then
		    BINARY_TYPE="linux2.2-glibc2.1-sparc"
                elif [ "$machine" = "ppc" ]; then
		    BINARY_TYPE="linux2.2-glibc2.1-powerpc"
		else
                    BINARY_TYPE="linux2.2-glibc2.1-x86"
                fi

            elif [  $subver = "2.4" ]; then
                  BINARY_TYPE="linux2.4"

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
                     # check for rms
                     is_rms_host
                     if [ "$?" = "0" ]; then
                        BINARY_TYPE="linux2.4-glibc2.2-ia64-rms${RMS_VERSION}"
                     fi
                  elif [ "$machine" = "ppc64" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-ppc64"
                  elif [ "$machine" = "s390" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390-32"
                  elif [ "$machine" = "s390x" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-s390x-64"
                  elif [ "$machine" = "alpha" ]; then
                     BINARY_TYPE="linux2.4-glibc2.2-alpha"
                     # check for rms
                     is_rms_host
                     if [ "$?" = "0" ]; then
                        BINARY_TYPE="linux2.4-glibc2.2-alpha-rms${RMS_VERSION}"
                     fi
                  else
		     echo $uname_val | $AWK '{print $3}' | grep "_Scyld" > $LSNULFILE 2>&1
		     if [ "$?" = "0" ]; then
                         BINARY_TYPE="linux2.4-glibc2.2-x86-scyld"
		     else
                       BINARY_TYPE="fail"
                       _libcver=`/lib/libc.so.6 | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
                       if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.4-glibc2.1-x86"
                       elif [ "$_libcver" = "2" ]; then
                          BINARY_TYPE="linux2.4-glibc2.2-x86"
                       elif [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.4-glibc2.3-x86"
                       fi
                       # check for rms
                       is_rms_host
                       if [ "$?" = "0" ]; then
                           BINARY_TYPE="${BINARY_TYPE}-rms${RMS_VERSION}"
                       fi
		     fi 
                  fi
            elif [  $subver = "2.6" ]; then
               BINARY_TYPE="fail"
               smp=`echo $uname_val | $AWK '{print $5}'`
               if [ "$smp" = "SMP" ]; then
                     machine=`echo $uname_val | $AWK '{print $12}'`
               else
                     machine=`echo $uname_val | $AWK '{print $11}'`
               fi
               case "$machine" in
                   x86_64)
                       _libcver=`/lib/libc.so.6 | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
                       if [ "$_libcver" = "1" ]; then
                          BINARY_TYPE="linux2.6-glibc2.1-amd64"
                       elif [ "$_libcver" = "2" ]; then
                          BINARY_TYPE="linux2.6-glibc2.2-amd64"
                       elif [ "$_libcver" = "3" ]; then
                          BINARY_TYPE="linux2.6-glibc2.3-amd64"
                       fi
                       ;;
                   i[3456]86)
                       _libcver=`/lib/libc.so.6 | grep "GNU C Library" | $AWK '{print substr($0,match($0,/version/))}' | $AWK '{print $2}' | $AWK -F. '{print $2}'| sed 's/,//'`
                       if [ "$_libcver" = "1" ]; then
                           BINARY_TYPE="linux2.6-glibc2.1-x86"
                       elif [ "$_libcver" = "2" ]; then
                           BINARY_TYPE="linux2.6-glibc2.2-x86"
                       elif [ "$_libcver" = "3" ]; then
                           BINARY_TYPE="linux2.6-glibc2.3-x86"
                       fi
                       ;;
               esac
            fi
        fi
         ;;
    UNIX_SV)
        BINARY_TYPE="mips-nec"
         ;;
    Darwin)
        BINARY_TYPE="macosx"
         ;;
    NEWS-OS)
	BINARY_TYPE="mips-sony"
 	;;
    AIX)
        version=`echo $uname_val | $AWK '{print $4}'`
        release=`echo $uname_val | $AWK '{print $3}'`
        if [ $version = "4" -a $release -ge 1 ]; then
           if [ $release -lt 2 ]; then
                BINARY_TYPE="aix4"
           elif [ $release -ge 2 ]; then
                 BINARY_TYPE="aix4.2"
           fi
        elif [ $version = "5" -a $release -ge 1 ]; then
            #On AIX, only in 64bit mode, you can see shlap64 process
            #check shlap64 process is safer than check kernel_bit
            if [ `ps -e -o "comm" |grep shlap64` ]; then
               BINARY_TYPE="aix-64"
            else
	        BINARY_TYPE="aix-32"
            fi
	elif [ $version = "3" ]; then
            BINARY_TYPE="aix3"
  	fi
	;;
    IRIX*)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        release=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        if [ $version = "6" -a $release -ge "2" ]; then
	    mls=`sysconf MAC`
	    if [ "$mls" = "1" ]
	    then
        	if [ $version = "6" -a $release -eq "5" ]; then
                    modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    if [ -z "$modification" ]; then
                        BINARY_TYPE="trix6"
                    else
                        if [ $modification -ge "8" ]; then
                            BINARY_TYPE="trix6.5.8"
                        else
                            BINARY_TYPE="trix6"
                        fi
                    fi 
		else
		    BINARY_TYPE="trix6"
                fi
	    else
        	if [ $version = "6" -a $release -eq "5" ]; then
                    modification=`/sbin/uname -R | awk '{split($2, a, "."); print a[3]}' | awk -F'[.a-zA-Z]' '{print $1}'`
                    if [ -z "$modification" ]; then
                        BINARY_TYPE="sgi6.5"
                    else
                        if [ $modification -ge "8" ]; then
                            BINARY_TYPE="sgi6.5.8"
                        else
                            BINARY_TYPE="sgi6.5"
                        fi
                    fi
	        elif [ $version = "6" -a $release -eq "4" ]; then
	            BINARY_TYPE="sgi6"
	        elif [ $version = "6" -a $release -eq "2" ]; then
	            BINARY_TYPE="sgi6"
		fi
	    fi
         else
	    BINARY_TYPE="sgi5"
 	fi
	;;
    SunOS)
        version=`echo $uname_val | $AWK '{split($3, a, "."); print a[1]}'`
        minorver=`echo $uname_val | $AWK '{split($3, a, "."); print a[2]}'`
        machine=`echo $uname_val | $AWK '{print $5}'`
        if [ $version = "4" ]; then
	    BINARY_TYPE="sunos4"
 	else
            if [ "$machine" = "i86pc" ]; then
                BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
                if [ "$BIT64" -eq "0" ] ; then
                    if [ "$minorver" = "7" -o "$minorver" = "8" -o "$minorver" = "9" -o "$minorver" = "10" ] ; then
                        BINARY_TYPE="x86-sol7"
                    else
                        BINARY_TYPE="x86-sol2"
                    fi
                else
                    if [ "$minorver" = "10" ] ; then
                        BINARY_TYPE="x86-64-sol10"
                    fi
                fi
            else
                BINARY_TYPE="sparc-sol2"
                if [ "$minorver" -ge "7" -a "$minorver" -le "9" ]; then
	       	    BIT64=`/usr/bin/isainfo -vk | grep -c '64.bit' 2> /dev/null`
	            if [ "$BIT64" -eq "0" ]; then
		        BINARY_TYPE="sparc-sol7-32"
	            else
		        BINARY_TYPE="sparc-sol7-64"
	            fi
                elif [ "$minorver" -ge "10" ]; then
                    BINARY_TYPE="sparc-sol10-64"
                fi
            fi
 	fi
	;;
    SUPER-UX)
	lastfield=`echo $uname_val | $AWK '{print $NF}'`
	if [ $lastfield = "SX-3" ]; then
	    BINARY_TYPE="sx3"
 	elif [  $lastfield = "SX-4" ]; then
	    BINARY_TYPE="sx4"
        elif [  $lastfield = "SX-5" ]; then
            BINARY_TYPE="sx5"
        elif [  $lastfield = "SX-6" ]; then
            BINARY_TYPE="sx6"
        fi
	;;
    ULTRIX)
        BINARY_TYPE="ultrix"
         ;;
    ConvexOS)
        BINARY_TYPE="convex"
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
        elif [ $lastfield = "C90" ]; then
   	    BINARY_TYPE="crayc90"
 	elif  [ $lastfield = "J90" ]; then
	    BINARY_TYPE="crayj90"
	elif  [ $lastfield = "SV1" ]; then
	    BINARY_TYPE="craysv1"
	elif  [ $lastfield = "T3E" ]; then
	    BINARY_TYPE="crayt3e"
        elif  [ $lastfield = "crayx1" ]; then
            BINARY_TYPE="crayx1"
         fi
	;;
    esac
    if [ $BINARY_TYPE = "fail" ]; then
        echo "Cannot get binary type."
    fi
