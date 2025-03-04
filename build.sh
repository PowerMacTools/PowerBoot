function make_for() {
    echo "=================="
    echo "Making $1"
    echo "=================="
    rm -rf build-$1
    mkdir -p build-$1
    cd build-$1
    cmd="cmake .. -DCMAKE_BUILD_TYPE=Release -DPLATFORM=$2 -DRETRO68_ROOT=$RETRO68_TOOLCHAIN_PATH -DCMAKE_TOOLCHAIN_FILE=$3"
    cmd2="make -j$(nproc)"
    $cmd && $cmd2
    status=$?
    cd .. 
    return $status
}
make_for "68k" "retro68" "$RETRO68_TOOLCHAIN_PATH/m68k-apple-macos/cmake/retro68.toolchain.cmake"
# make_for "PowerPC" "retroppc" "$RETRO68_TOOLCHAIN_PATH/powerpc-apple-macos/cmake/retroppc.toolchain.cmake"
