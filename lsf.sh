#! /bin/bash
binname=$1
runlist=$2
des_dir=$3
itel=$4
nrun=$5
run_first=$6
maxentry=$7
firstentry=$8

userid=`id`
userid=${userid%%)*}
userid=${userid##*(}
user0=${userid:0:1}
#echo $userid"  "$user0
#return 

if [ $userid == "hliu" ];then
#dir0="/eos/user/h/hliu/LaserEvent"
dir0="/scratchfs/lhaaso/hliu/buffer2"
cdir="/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/LaserEvent"
logdir="/scratchfs/lhaaso/hliu/jobout"
else
dir0="/scratchfs/ybj/$userid/Laser/data_ylq"
cdir="/scratchfs/ybj/$userid/Laser"
logdir="/scratchfs/ybj/$userid/jobout"
fi
#echo $dir0
#echo $cdir
#echo $logdir
#return

#nlsf=20
if [[ -z $binname ]];then
   read -n1 -p "use <source lsf.sh binname runlist eos_dir itel nrun run_first maxentry firstentry>,  Press any key to continue..."
   read -n1 -p "use <source lsf.sh binname runlist eos_dir itel nrun run_first maxentry firstentry>,  Press any key to continue..."
fi
if [[ -z $nrun ]]; then
   nrun=`cat $runlist | wc -l`
   run_first=0
else
   if [[ $nrun -lt 0 ]]; then
      nrun=`cat $runlist | wc -l`
      run_first=0
   fi
fi
if [[ -z $maxentry ]];then
   maxentry=-1
fi
if [[ -z $firstentry ]];then
   firstentry=0
fi
#step=$((nrun/nlsf))
step=0
if [ $step -eq 0 ]; then
   step=$(((nrun-run_first)/50))
fi
if [ $step -eq 0 ]; then
   step=1
fi

if [ ! -d $dir0/$des_dir ];then
   mkdir $dir0/$des_dir
fi

queue="ams1nd"
jobindex=0
name0="sh"
lsf_dir="lsf"${itel}
#echo ${lsf_dir}
#return
rm ${lsf_dir}/*

for (( i=$run_first; i<nrun; i=i+step ))
do
   runlast=$((i+step-1))
   if [ $runlast -ge $nrun ]; then
      runlast=$((nrun-1))
   fi

   #hostname=`cat /afs/cern.ch/work/h/huliu/Documents/test/bash/serverlist.txt | head -$((jobindex+1)) | tail -1`

     cd ${lsf_dir}
     RUN=$jobindex

     if [[ -f $dir0/$des_dir/${RUN}.root ]];then
        cd ..
        jobindex=$((jobindex+1))
        continue
     fi

     #if [[ `bjobs -q $queue | grep tr${RUN:0:8} | wc -l` -gt 0 ]];then
     #   cd ..
     #   continue
     #fi


     echo "#! /bin/bash" > $RUN.$name0
     echo "# info" >> $RUN.$name0
     echo "echo hostname \`hostname\` ..." >> $RUN.$name0
     echo "echo pwd \`pwd\`" >> $RUN.$name0
     echo "echo ROOTSYS \$ROOTSYS" >> $RUN.$name0
     echo "echo LD_LIBRARY_PATH \$LD_LIBRARY_PATH" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#set the environment and preparation for compiling and liking" >> $RUN.$name0
     echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#execute the monitor program" >> $RUN.$name0
     if [ `echo $dir0 | grep eos | wc -l` -gt 0 ];then
        #echo "time $cdir/$binname $runlist root://eos01.ihep.ac.cn/$dir0/$des_dir/${RUN}.root $i $runlast $maxentry $firstentry" >> $RUN.$name0
        #echo "time $cdir/$binname $runlist /scratchfs/lhaaso/hliu/buffer/tel${itel}/${RUN}.root $i $runlast $maxentry $firstentry" >> $RUN.$name0
        echo "time $cdir/$binname $runlist ${itel} /scratchfs/lhaaso/hliu/buffer/tel${itel}/${RUN}.root $i $runlast $maxentry $firstentry" >> $RUN.$name0
        #echo "eos cp /scratchfs/lhaaso/hliu/buffer/tel${itel}/${RUN}.root $dir0/$des_dir/${RUN}.root" >> $RUN.$name0
     else
        echo "time $cdir/$binname $runlist $dir0/$des_dir/${RUN}.root $i $runlast $maxentry $firstentry" >> $RUN.$name0
        #echo "time $cdir/$binname $runlist ${itel} $dir0/$des_dir/${RUN}.root $i $runlast $maxentry $firstentry" >> $RUN.$name0
     fi
     echo $'\n' >> $RUN.$name0

     echo "ls -ltrh" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#the end" >> $RUN.$name0

     #echo "if [ -f $RUN.root ]; then" >> clean.sh
     #echo "   rm $RUN.root" >> clean.sh
     #echo "fi" >> clean.sh

     chmod a+x $RUN.$name0
     mv $RUN.$name0 job.sh.${jobindex}

     #njobs=`hep_q -p virtual -u ${userid} | wc -l`
     #sleep_time=120
     #while [ "$njobs" -gt 350 ]
     #do
     #   echo "there are $((njobs)) jobs,too many,sleep for $sleep_time seconds at `date`"
     #   sleep $sleep_time
     #   njobs=`hep_q -p virtual -u ${userid} | wc -l`
     #done

     #hep_sub -p virtual -g lhaaso -o ${logdir}/${RUN}.out -e ${logdir}/${RUN}.err $RUN.$name0
     #source $RUN.$name0
     #echo "Processing $i : $RUN.$name0 ..."
     #ssh -o 'StrictHostKeyChecking=no' huliu@$hostname sh /afs/cern.ch/work/h/huliu/Documents/charge/lsf/$RUN.$name0 &

     #echo "RUN="$RUN"  jobindex="${jobindex}"  "$i"  "$runlast

     cd ..

   jobindex=$((jobindex+1))
done

#njobs=`bjobs | grep plot | wc -l`
#sleep_time=120
#while [ "$njobs" -gt 0 ]
#do
#   echo "there are $((njobs)) jobs,too many,sleep for $sleep_time seconds at `date`"
#   sleep $sleep_time
#   njobs=`bjobs | grep plot | wc -l`
#done
#
#cd ~/eos/ams/user/h/huliu/DST/$des_dir
#hadd -f plot.root plot_*.root
#cd ~/Documents/plot

cd ${lsf_dir}
hep_sub -p virtual -g lhaaso -o $logdir/%{ProcId}.out -e $logdir/%{ProcId}.err job.sh.%{ProcId} -n ${jobindex}
cd ..
