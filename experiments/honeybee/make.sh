git clone https://github.com/trailofbits/Honeybee.git && \
cd Honeybee && \
git checkout 5a8ab151a7fbed624b0e4741bfeb46f9cb1f27d0 && \

# apply patches
patch < ../CMakeList.txt.patch && \
cd honey_tester/unit_testing/ && \
patch  < ../../../ha_session_audit.c.patch && \
cd -  && \

# build
cd dependencies && \
./build_dependencies.sh && \
cd ../ && \

mkdir cmake_build && \
cd cmake_build && \
cmake -D CMAKE_BUILD_TYPE=Release .. && \
cmake --build . --target all 
