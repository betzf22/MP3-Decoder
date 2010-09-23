
configure_all() {
    rm -rf build
    mkdir build
    cd build
    
    cmake -G "Unix Makefiles" ..
    cd ..
}

build_all() {
    cd build
    make
    if [ ! -f "test/test.mp3" ]; then
	cp ../test/test.mp3 test/test.mp3
    fi
    cd ..
}

test_all() {
    cd build/test
    ./test
}

if [ $# -eq 0 ] || [ "$1" = "" ]; then
    configure_all
    build_all
elif [ "$1" = "configure" ]; then
    configure_all
elif [ "$1" = "test" ]; then
    test_all
elif [ "$1" = "btest" ]; then
    build_all
    test_all
fi

