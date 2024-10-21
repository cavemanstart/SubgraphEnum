#! /bin/bash -e
cd ./build
#cmake ..
#make
#echo "compile completed."
cd ./toolset

./GenerateVertexPairs.out ../../dataset/BK ../../dataset/BK 1000 4 0.5
./GenerateVertexPairs.out ../../dataset/DB ../../dataset/DB 1000 4 0.5
./GenerateVertexPairs.out ../../dataset/GG ../../dataset/GG 1000 4 0.5
./GenerateVertexPairs.out ../../dataset/UP ../../dataset/UP 1000 4 0.5
./GenerateVertexPairs.out ../../dataset/SL ../../dataset/SL 200 4 0.5
./GenerateVertexPairs.out ../../dataset/LJ ../../dataset/LJ 200 4 0.5
./GenerateVertexPairs.out ../../dataset/WT ../../dataset/WT 200 4 0.5

echo "done"
