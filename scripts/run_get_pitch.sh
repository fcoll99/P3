#!/bin/bash

# Put here the program (maybe with path)
GETF0="get_pitch"

if [ -z "$1" ]; then
    pow_min=52
else
    pow_min=$1
fi
if [ -z "$2" ]; then
    r1norm_min=0.945
else
    r1norm_min=$2
fi
if [ -z "$3" ]; then
    rmaxnorm_min=0.47
else
    rmaxnorm_min=$3
fi

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    echo "$GETF0 $fwav $ff0 -$pow_min $r1norm_min $rmaxnorm_min ----"
    $GETF0 $fwav $ff0 $pow_min $r1norm_min $rmaxnorm_min > /dev/null || (echo "Error in $GETF0 $fwav $ff0 $pow_min $r1norm_min $rmaxnorm_min "; exit 1)
done
pitch_evaluate pitch_db/train/*f0ref
exit 0
