#!/bin/bash
# https://e906-gat1.fnal.gov/data-summary/e1039/run.php?run_begin=6090&run_end=6100
DIR_IN=/data2/e1039/dst
FN_OUT=list_input.txt

function ParseOneRun {
    local -r DIR_IN=$1
    local -r RUN=$2
    local -r RUN6=$(printf '%06d' $RUN)
    test -d $DIR_IN/run_$RUN6 || return
    for FNAME in $DIR_IN/run_$RUN6/run_${RUN6}_spill_*_spin.root ; do
	BNAME=$(basename $FNAME)
	SPILL9=${BNAME:17:9}
	SPILL=$(( 10#$SPILL9 ))
    	echo "$RUN $SPILL $FNAME"
    done
}

#for RUN in 6096 6097 6098 6099 ; do
#    ParseOneRun $DIR_IN $RUN
#done >$FN_OUT

for (( RUN = 6178 ; RUN >= 5462 ; RUN-- )) ; do
#for (( RUN = 6096 ; RUN <= 6099 ; RUN++ )) ; do
    echo "Run $RUN" >/dev/stderr
    ParseOneRun $DIR_IN $RUN
done >$FN_OUT
