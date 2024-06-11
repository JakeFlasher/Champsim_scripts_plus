build:
make QEMU_DIR=path_to_qemu

qemu build:
qemu configure options: --enable-plugins
e.g.:
mkdir build && cd build/ && ../configure --target-list=x86_64-linux-user --disable-werror --enable-plugins && ninja

run:
qemu-x86_64 -d plugin -D log.txt --plugin ./build/libicount.so -- /bin/ls

dependency:
capstone next
