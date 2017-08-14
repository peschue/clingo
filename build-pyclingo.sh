
mkdir -p build/pyclingo/
cmake -H./ -B./build/pyclingo/ -DCMAKE_BUILD_TYPE=Release -DCLINGO_BUILD_APPS=OFF -DCLINGO_REQUIRE_PYTHON=ON -DPYTHON_EXECUTABLE=/usr/bin/python3
cmake --build build/pyclingo/

