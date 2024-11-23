#!/bin/bash

x=$1
y=$2
z=$3
shield=$4

echo $x $y $z 

root -l << EOF
  cout << "Scaning" << endl;
  .x Fun4E1039Shielding.C(-1,0,${shield}); > log
  .L matscan.C
  pos_x = ${x}
  pos_y = ${y}
  pos_z = ${z}
  print() >> log
  matscan(); > T.T
  cout << "Scan finished!" << endl;
  .q
EOF

perl matscan_digest.pl T.T
