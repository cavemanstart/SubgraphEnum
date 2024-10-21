#! /bin/bash -e
cd ./build
cmake ..
make
echo "compile completed."
cd ./enumerator

#echo "expriment on EP..."
./Enumerator.out ../../dataset/SL
./Enumerator.out ../../dataset/EP
./Enumerator.out ../../dataset/LJ
./Enumerator.out ../../dataset/WT
./Enumerator.out ../../dataset/YE
echo "done"
