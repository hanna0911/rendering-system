#!/usr/bin/env bash

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
mkdir -p build
cd build
cmake ..
make -j
cd ..

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
bin/PA1 testcases/scene01_basic.txt output/scene01_rc.bmp rc
bin/PA1 testcases/scene01_basic.txt output/scene01_rt.bmp rt 4
bin/PA1 testcases/scene02_cube.txt output/scene02_rc.bmp rc
bin/PA1 testcases/scene02_cube.txt output/scene02_rt.bmp rt 4
bin/PA1 testcases/scene03_sphere.txt output/scene03_rc.bmp rc
bin/PA1 testcases/scene03_sphere.txt output/scene03_rt.bmp rt 4
bin/PA1 testcases/scene04_axes.txt output/scene04_rc.bmp rc
bin/PA1 testcases/scene04_axes.txt output/scene04_rt.bmp rt 4
bin/PA1 testcases/scene05_bunny_200.txt output/scene05_rc.bmp rc
bin/PA1 testcases/scene05_bunny_200.txt output/scene05_rt.bmp rt 4
bin/PA1 testcases/scene06_bunny_1k.txt output/scene06_rc.bmp rc
bin/PA1 testcases/scene06_bunny_1k.txt output/scene06_rt.bmp rt 4
bin/PA1 testcases/scene07_shine.txt output/scene07_rc.bmp rc
bin/PA1 testcases/scene07_shine.txt output/scene07_rt.bmp rt 4
bin/PA1 testcases/scene08_bunny_1k.txt output/scene08_rc.bmp rc
bin/PA1 testcases/scene08_bunny_1k.txt output/scene08_rt.bmp rt 4
bin/PA1 testcases/scene09_arch.txt output/scene09_rc.bmp rc
bin/PA1 testcases/scene09_arch.txt output/scene09_rt.bmp rt 4
bin/PA1 testcases/scene10_faceted_gem.txt output/scene10_rc.bmp rc
bin/PA1 testcases/scene10_faceted_gem.txt output/scene10_rt.bmp rt 4