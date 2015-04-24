#!/bin/sh

check_vemkd=`egosh ego info |wc -l`
if [ "$check_vemkd" -eq 0 ]
then
    exit 1 
fi

i=0
query_status=1

while [ $i -le 1 ]
do
   i=`expr $i + 1`

   for service_name in `egosh service view | grep  "Service Name" | sed -e 's/Name \{0,\}:/Name:/'  `
   do
        if [ "$service_name" != "Service" ] && [ "$service_name" != "Name:" ]
        then
            egosh service stop $service_name

            if [ $? != 0 ]
            then
               exit 1
            fi

            sleep 1
            query_status=0
        fi
   done

   if [ "$query_status" -eq 0 ] || [ "$i" -gt 1 ]
   then
       break;
   else
       echo "Can not get the service information,waiting for 10 seconds, try one more time..." 
       sleep 10
   fi

done


if [ "$query_status" -eq 0 ]
then
    i=0
    run_services=`egosh service list |grep " RUN "|wc -l`
    while [ "$i" -le 12  -a "$run_services" -gt 0 ]
    do
        sleep 5
        i=`expr $i + 1`
        run_services=`egosh service list |grep " RUN "|wc -l`
    done
fi

egosh ego shutdown -f all
