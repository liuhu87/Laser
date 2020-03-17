#! /bin/bash
dir=$1
iyear=$2
imonth=$3
iday=$4
itel=$5

if [[ -z $itel ]];then
   read -n1 -p "use <source create_runlist.sh directory year month day iTel>,  Press any key to continue..."
   read -n1 -p "use <source create_runlist.sh directory year month day iTel>,  Press any key to continue..."
fi

imonth=`printf "%02d" $imonth`
iday=`printf "%02d" $iday`
idate=`date -d "$iyear-$imonth-$iday 12:00:00" +%s`
idate2=$((idate+86400))
itel=`printf "%02d" $itel`

if [[ -f tel${itel}.txt ]];then
   rm tel${itel}.txt
fi

inum=0
for ith0 in `ls -l $dir | grep ^d | awk '{print $9}' | grep ^20`
do

   length0=`echo $ith0 | wc -L`
   if [[ $length0 -ne 4 ]];then
   continue
   fi
   year=$ith0

   for ith in `ls -l $dir/$year | grep ^d | awk '{print $9}'`
   do
      echo $year$ith
      length=`echo $ith | wc -L`
      if [[ $length -ne 4 ]];then
      continue
      fi
      
      month=${ith:0:2}
      day=${ith:2:2}
      #if [ ! $month -ge 10 ];then
      #   continue
      #fi
      #if [ ! $month -ge 10 ];then
      #   continue
      #fi
   
      date1=`date -d "$year-$month-$day 00:00:00" +%s`
      date2=`date -d "$year-$month-$day 23:59:00" +%s`
      if [ $date1 -gt $idate ];then
      time1=$date1
      else
      time1=$idate
      fi
   
      if [ $date2 -gt $idate2 ];then
      time2=$idate2
      else
      time2=$date2
      fi
   
      #echo "cdate = "$month" "$day
      #echo "target= "$idate" "$idate2
      #echo "now   = "$date1" "$date2
      #echo "minmax= "$time1"  "$time2
      #echo " "
   
      if [ $time1 -ge $time2 ];then
      continue
      fi
      
      #echo $inum"  "$month"  "$day":  "$ith
      
      for ith2 in `ls $dir/$ith0/$ith | grep .event.root$ | grep WFCTA$itel`
      do
         #echo $dir/$ith0/$ith/$ith2
         hour=${ith2:30:2}
         if [ $idate -gt $date1 ];then
            if [ $hour -lt 12 ];then
               continue
            fi
         else
            if [ $hour -gt 12 ];then
               continue
            fi
         fi
         #echo $hour
         
         filename=$dir/$ith0/$ith/$ith2
         filesize=`ls $filename -l | awk '{print $5}'`
         if [ $filesize -lt 10000 ];then
         continue
         fi
         
         #echo $dir/$ith0/$ith/$ith2
         echo "root://eos01.ihep.ac.cn/"$dir/$ith0/$ith/$ith2 >> tel${itel}.txt
      done
      
      inum=$((inum+1))
   
   done
done
mv tel${itel}.txt tel${itel}_$iyear$imonth$iday.txt
