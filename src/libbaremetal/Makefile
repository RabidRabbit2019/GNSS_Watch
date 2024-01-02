all: lib/libc.a lib/libm.a


lib/libc.a: src/libc/gmtime_r.o src/libc/memset.o src/libc/bzero.o
	riscv64-unknown-elf-ar cr lib/libc.a src/libc/gmtime_r.o src/libc/memset.o src/libc/bzero.o
	riscv64-unknown-elf-ranlib lib/libc.a

src/libc/gmtime_r.o: src/libc/gmtime_r.c
	riscv64-unknown-elf-gcc src/libc/gmtime_r.c -I./include -Os -flto -s -ffunction-sections -fdata-sections -nostartfiles -ffreestanding -nostdlib -nodefaultlibs -march=rv32imac -mabi=ilp32 -c -o src/libc/gmtime_r.o
	
src/libc/memset.o: src/libc/memset.c
	riscv64-unknown-elf-gcc src/libc/memset.c -I./include -Os -flto -s -ffunction-sections -fdata-sections -nostartfiles -ffreestanding -nostdlib -nodefaultlibs -march=rv32imac -mabi=ilp32 -c -o src/libc/memset.o

src/libc/bzero.o: src/libc/bzero.c
	riscv64-unknown-elf-gcc src/libc/bzero.c -I./include -Os -flto -s -ffunction-sections -fdata-sections -nostartfiles -ffreestanding -nostdlib -nodefaultlibs -march=rv32imac -mabi=ilp32 -c -o src/libc/bzero.o
	
lib/libm.a: src/libm/dummy_libm.o
	riscv64-unknown-elf-ar cr lib/libm.a src/libm/dummy_libm.o
	riscv64-unknown-elf-ranlib lib/libm.a

src/libm/dummy_libm.o: src/libm/dummy_libm.c
	riscv64-unknown-elf-gcc src/libm/dummy_libm.c -Os -flto -s -march=rv32imac -mabi=ilp32 -c -o src/libm/dummy_libm.o


clean:
	rm -f lib/libc.a  \
	lib/libm.a \
	src/libc/gmtime_r.o \
	src/libm/dummy_libm.o \
	src/libc/memset.o \
	src/libc/bzero.o
