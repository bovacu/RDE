cd build
clang++ ../engine/src/rde.cpp -DRDE_AUDIO_MODULE -DRDE_PHYSICS_MODULE -I../engine/include -I../engine/src -I../external -I../external/linux/include -o libRDE.o
ar r libRDE.o libRDE.a
rm libRDE.o
cd ..