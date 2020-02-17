#! /bin/bash
dir=$1
itel=$2
itel=`printf "%02d" $itel`
for idate in `ls $dir`
do
   for ifile in `ls $dir/$idate | grep WFCTA$itel`
   do
      echo "root://eos01.ihep.ac.cn/"$dir/$idate/$ifile >> ntuple_tel$itel.txt
   done
done
