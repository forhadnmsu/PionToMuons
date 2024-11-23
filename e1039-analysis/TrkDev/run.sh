#!/bin/bash

job=$1
nevent=$2
nmu=$3

base_dir=`pwd`
work_dir=$base_dir/$job
mkdir -p $work_dir

cd $work_dir 
ln -sf $base_dir/*.C .
ln -sf $base_dir/*.cfg .
ln -sf $base_dir/*.opts .
ln -sf $base_dir/*.mac .
( nice time root -l -q Fun4PrgTrkDev.C\($nevent,$nmu\) ) >& log
cd $base_dir
