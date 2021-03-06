ROOT=`pwd`

# clean
rm -rf libipt
rm -rf mbuild
rm -rf hongfuzz/build
rm hongfuzz/tester
rm -rf honeybee/Honeybee
rm killerbeez/*.o
rm killerbeez/tester
rm libipt/build/*
rm libxdc/tester
rm -rf libxdc/libxdc
rm ptrix/tester

git clone https://github.com/intelxed/xed.git xed
git clone https://github.com/intelxed/mbuild.git mbuild
cd xed
./mfile.py --shared install
mv kits/xed-install-base* kits/xed-install-base/
cd $ROOT

# build libipt and ptxed
echo "===== BUILD ptxed ====="
git clone https://github.com/intel/libipt.git
cd libipt
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPTXED=true -DXED_INCLUDE=../../xed/kits/xed-install-base/include/xed/ -DXED_LIBDIR=../../xed/kits/xed-install-base/lib/ ..
make
cd $ROOT

echo "===== BUILD honggfuzz ====="
cd honggfuzz
mkdir -p build
make
cd $ROOT

echo "===== BUILD killerbeez ====="
cd killerbeez
sh make.sh
cd $ROOT

echo "===== BUILD ptrix ====="
cd ptrix
make
cd $ROOT

echo "===== BUILD winaflpt ====="
cd winaflpt/
mkdir -p build
make 
cd $ROOT

echo "===== BUILD libxdc-tester ====="
cd libxdc/
sh compile.sh 
cd $ROOT

echo "===== BUILD honeybee ====="
cd honeybee/
sh make.sh
cd $ROOT