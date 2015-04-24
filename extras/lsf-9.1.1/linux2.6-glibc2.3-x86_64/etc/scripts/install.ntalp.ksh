# $Id: install.ntalp.ksh,v 1.2 2012/06/08 03:37:24 sbadugu Exp $

set -e
#PATH=/usr/bin:/bin:/usr/etc
#export PATH
usage="Usage: $0 [-c] [-O] [-s] [-m perms] file1 file2"
usage2="       $0 [-c] [-O] [-s] [-m perms] file1 ... dir"
usage3="       $0 -d [-m perms] dir"

strip=:
mode=0755
dirflag=
backup=
forcedir=
cp=cp
while test x$1 != x
do
	case $1 in
	-c) cp=cp; shift;;
	-s) strip=strip; shift;;
	-d) dirflag=-d; shift;;
	-m) mode=$2; shift; shift;;
	-O) backup=-O; shift;;
	-l) cp="cp"; shift ;;
	-*) echo "Illegal argument $1"
	    echo "$usage"
	    echo "$usage2"
	    echo "$usage3"
	    exit 1 ;;
	*) break;;
	esac
done

if test x$1 = x ; then
	echo "$usage"
	echo "$usage2"
	echo "$usage3"
	exit 2
elif test x$dirflag != x ; then
    if test x$2 != x ; then
	echo $0: Only one argument if "-d" is given
	echo "$usage"
	echo "$usage2"
	echo "$usage3"
	exit 3
    else
	dest=$1
	forcedir=y
    fi
fi

# If dirflag isn't set, find the files/directories to create
if test x$dirflag = x ; then
# check our argument count
    if test $# -lt 2 ; then
	echo "$usage"
	echo "$usage2"
	echo "$usage3"
	exit 4
    fi

#find the list of files and the last argument (the destination)
    dest=
    files=
    for i
    do
	files="$files $dest"
	dest=$i
    done

    if test -d $dest -o $# -gt 2 ; then
	forcedir=y
    else
	# if last arg. ends in /, it must be a dir; create if necessary
	case $dest in
	    */) forcedir=y ;;
	    *)	;;
	esac
    fi
fi

# make the directory
# Split the destination name on all slashes and put in $1, $2 ...
# add the /x/ to avoid problems with $dest == "/"
IFS=/
set $dest
# Some shells handle this "set" very strangely.  Setting again fixes it.
IFS=" 	"
set $*
#shift		# get rid of x

# Are we starting from root?
case $dest in
    /*) existd=/ ;;
    *) existd= ;;
esac
dsep1=
while test "$1" != "" -a -d $existd$dsep1$1
do
    existd=$existd$dsep1$1
    dsep1=/
    shift
done
destd=
dsep=
while test "$1" != "" -a '(' '(' "$forcedir" = "y" ')' -o '(' "$2" != "" ')' ')'
do
    destd=$destd$dsep$1
    if test -f $existd$dsep1$destd ; then
	echo "$0: $existd$dsep1$destd is not a directory!"
	exit 1
    fi
    dsep=/
    shift
done

if test ! -z "$destd" ; then
    (
	# do this in a subshell so that the cd doesn't hurt us
	if test ! -z "$existd" ; then
	    cd $existd
	fi
	mkdir.exe -p $destd
    )
    if test -z "$existd" ; then
	dir=$destd
    elif test "$existd" != "/" ; then
	dir=$existd/$destd
    else
	dir=/$destd
    fi
else
    dir=$existd
fi

if test x$dirflag != x ; then
    chmod $mode $dir
    exit 0
fi

rmfiles=
for i in $files
do
    IFS=/
    set x/$i	# just in case $i = ""
    IFS=" 	"
    set $*
    shift		# lose the x
    for fle
    do
	j=$fle
    done
    j=$dir/$fle
    if test -f $j ; then
	if test x$backup != x ; then
	    /bin/mv -f $j $j.old
	else
	    rmfiles="$rmfiles $fle"
	fi
    else : ; fi
done
if test "$rmfiles" != "" ; then
    (cd $dir ; rm $rmfiles)
fi

if [ -d $dest ] ; then
    $cp $files $dir
    (cd $dir ; for file in $files ; do $strip `basename $file` ;
           chmod $mode `basename $file` ; done)
else
    $cp $files $dest
    (cd $dir ; $strip `basename $dest` ; chmod $mode `basename $dest`)
fi

exit 0
