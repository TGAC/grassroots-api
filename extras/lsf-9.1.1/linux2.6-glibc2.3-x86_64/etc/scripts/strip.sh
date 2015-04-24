#! /bin/sh

PGM=strip.sh

mystrip()
{
  m=`file $1 | grep executable | grep -v script`
  if [ "#$m" != "#" ]; then
    b="`basename $1`"
    for i in $_ignore; do
      if [ "#$b" = "#$i" ]; then
        echo "$PGM: ..... ignore $1"
        return
      fi
    done
    if [ "#$_listonly" = "#yes" ]; then
      echo "$PGM: will strip $1"
    else
      echo "$PGM: strip $_stripparm $1"
      strip $_stripparm $1
    fi
    return 0
  else
    echo "$PGM: ..... skip $1"
  fi
}

usage()
{
cat << USAGE

Usage       : strip.sh [-h] [-l] [-v ignore_files ] path [ path ... ]

Description

    strip.sh discards all symbols from the object files objfile by calling strip.
    Only object files will be tried to strip. Error messages normal cause by
    using such as "strip *" will be avoid.

Parameters and options

    path 
        File or directory.
        If path is a file, strip the file. 
        If path is a directory, strip files in the directory.
        If there are more than one path, only file will be stripped.

    -h
       show this message

    -v ignore_files
        Do not strip these files.
        Use "file file ..." to specify mutiple files.

    -l 
        Only list file that will be stripped. Don't do real strip.

Example

    strip.sh lim
    strip.sh bin		   # strip all files under bin/
    strip.sh bin/s*		   # strip all files start with "s" under bin/
    strip.sh -v pam batch/bin      # strip all files under batch/bin/ except pam
    strip.sh -v "eauth eck" bin/   # strip all files under bin/ except eauth and eck

USAGE
exit 0
}

if [ $# -lt 1 -o "#$1" = "#-h" ]; then
  usage
fi

if [ "#$1" = "#-l" ]; then
  shift
  _listonly="yes"
fi

_ignore="";
if [ "#$1" = "#-v" ]; then
  shift
  _ignore=$1
  shift
fi

if [ $# -eq 1 ]; then
  if [ -d $1 ]; then
    echo "$PGM: cd $1"
    cd $1
    files=`ls -1`
  else
    files=$1
  fi
else
  files="$@"
fi

_stripparm=""
if [ "`uname`" = "AIX" ]; then
  if [ "#`(strip 2>&1) | grep 32_64`" != "#" ]; then
    _stripparm="-X 32_64"
  fi
fi

for f in $files; do
  mystrip $f
done
