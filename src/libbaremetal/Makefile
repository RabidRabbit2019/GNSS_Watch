all: lib/libc.a lib/libm.a


lib/libc.a: src/libc/dummy_libc.o src/libc/gmtime_r.o src/libc/memset.o src/libc/bzero.o src/libc/snprintf.o src/libc/strstr.o src/libc/memmem.o src/libc/memcmp.o src/libc/memchr.o src/libc/strchr.o src/libc/strchrnul.o src/libc/strlen.o src/libc/strnlen.o
	riscv64-unknown-elf-ar cr lib/libc.a src/libc/dummy_libc.o src/libc/gmtime_r.o src/libc/memset.o src/libc/bzero.o src/libc/snprintf.o src/libc/strstr.o src/libc/memmem.o src/libc/memcmp.o src/libc/memchr.o src/libc/strchr.o src/libc/strchrnul.o src/libc/strlen.o src/libc/strnlen.o
	riscv64-unknown-elf-ranlib lib/libc.a

src/libc/dummy_libc.o: src/libc/dummy_libc.c
	riscv64-unknown-elf-gcc src/libc/dummy_libc.c -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/dummy_libc.o

src/libc/gmtime_r.o: src/libc/gmtime_r.c
	riscv64-unknown-elf-gcc src/libc/gmtime_r.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/gmtime_r.o
	
src/libc/memset.o: src/libc/memset.c
	riscv64-unknown-elf-gcc src/libc/memset.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/memset.o

src/libc/bzero.o: src/libc/bzero.c
	riscv64-unknown-elf-gcc src/libc/bzero.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/bzero.o

src/libc/snprintf.o: src/libc/snprintf.c
	riscv64-unknown-elf-gcc src/libc/snprintf.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/snprintf.o

src/libc/strstr.o: src/libc/strstr.c
	riscv64-unknown-elf-gcc src/libc/strstr.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/strstr.o

src/libc/memmem.o: src/libc/memmem.c
	riscv64-unknown-elf-gcc src/libc/memmem.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/memmem.o

src/libc/memcmp.o: src/libc/memcmp.c
	riscv64-unknown-elf-gcc src/libc/memcmp.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/memcmp.o

src/libc/memchr.o: src/libc/memchr.c
	riscv64-unknown-elf-gcc src/libc/memchr.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/memchr.o

src/libc/strchr.o: src/libc/strchr.c
	riscv64-unknown-elf-gcc src/libc/strchr.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/strchr.o

src/libc/strchrnul.o: src/libc/strchrnul.c
	riscv64-unknown-elf-gcc src/libc/strchrnul.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/strchrnul.o

src/libc/strlen.o: src/libc/strlen.c
	riscv64-unknown-elf-gcc src/libc/strlen.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/strlen.o

src/libc/strnlen.o: src/libc/strnlen.c
	riscv64-unknown-elf-gcc src/libc/strnlen.c -I./include -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libc/strnlen.o

lib/libm.a: src/libm/dummy_libm.o
	riscv64-unknown-elf-ar cr lib/libm.a src/libm/dummy_libm.o
	riscv64-unknown-elf-ranlib lib/libm.a

src/libm/dummy_libm.o: src/libm/dummy_libm.c
	riscv64-unknown-elf-gcc src/libm/dummy_libm.c -Os -s --freestanding -march=rv32imac -mabi=ilp32 -c -o src/libm/dummy_libm.o


clean:
	rm -f lib/libc.a  \
	lib/libm.a \
	src/libc/dummy_libc.o \
	src/libc/gmtime_r.o \
	src/libm/dummy_libm.o \
	src/libc/memset.o \
	src/libc/bzero.o \
	src/libc/snprintf.o \
	src/libc/strstr.o \
	src/libc/memmem.o \
	src/libc/memcmp.o \
	src/libc/memchr.o \
	src/libc/strchr.o \
	src/libc/strchrnul.o \
	src/libc/strnlen.o \
	src/libc/strlen.o
