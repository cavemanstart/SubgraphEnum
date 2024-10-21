#! /bin/bash -e
#cmake ..
#make
echo "compile completed."
./Enumerator.out ../../dataset/BK
./Enumerator.out ../../dataset/DB
./Enumerator.out ../../dataset/UP
./Enumerator.out ../../dataset/GG
./Enumerator.out ../../dataset/EP
./Enumerator.out ../../dataset/SL
./Enumerator.out ../../dataset/LJ
./Enumerator.out ../../dataset/WT

echo "done"
