#! /bin/bash
dir=$1
itel=$2
itel=`printf "%02d" $itel`
if [ -f all_tel${itel}.txt ];then
   rm all_tel${itel}.txt
fi

for idir0 in `ls -l $dir | grep ^d | awk '{print $9}' | grep ^20`
do

   #echo $idir0
   for ith in `ls -l $dir/$idir0 | grep ^d | awk '{print $9}'`
   do
      length=`echo $ith | wc -L`
      if [ $length -ne 4 ];then
         continue
      fi

      month=${ith:0:2}
      day=${ith:2:2}
      if [ $idir0 -eq 2019 ];then
         if [ $month -le 10 ];then
            if [ $month -lt 10 ];then
               continue
            fi
            if [ $day -lt 25 ];then
               continue
            fi
         fi
      fi
      
      echo $idir0/$ith"  "$month"  "$day
      for ith2 in `ls $dir/${idir0}/$ith | grep .event.root$ | grep WFCTA${itel}`
      do
      #echo $dir/$idir0/$ith/$ith2
      
         filename=$dir/$idir0/$ith/$ith2
         #echo $filename

         #if [ `echo $ith2 | grep .FULL.DAQ_TEST. | wc -l` -le 0 ];then
         #   if [ `echo $ith2 | grep "WFCTA$itel" | wc -l` -le 0 ];then
         #      continue;
         #   fi
         #fi

         filesize=`ls $filename -l | awk '{print $5}'`
         if [ $filesize -lt 10000 ];then
         continue
         fi
      
         echo "root://eos01.ihep.ac.cn/"$dir/$idir0/$ith/$ith2 >>all_tel${itel}_v4.txt
      done
   
   done
done
