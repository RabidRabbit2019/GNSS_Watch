TOOLS=

all: a.elf

a.elf: libbaremetal/lib/libc.a libbaremetal/lib/libm.a
	$(TOOLS)riscv64-unknown-elf-gcc -DCK_SYS_VALUE=64000000 \
          startup.S \
          n200_func.c \
          display.c \
          fonts/font_bmp.c \
          images/zic_utils.c \
          fonts/font_28_32.c \
          fonts/font_110_110.c \
          images/flag2x2.c \
          images/alarm_clock.c \
          images/gnss_no.c \
          images/gnss_ok.c \
          images/settings.c \
          init.c \
          gnss.c \
          render.c \
          rtc.c \
          bme280.c \
          xpt2046.c \
          settings.c \
          main.c \
          -Os -g -march=rv32imac_zicsr -mabi=ilp32 -ffunction-sections -fdata-sections \
          -fstrict-volatile-bitfields -fno-strict-aliasing -fno-common -fno-builtin-printf \
          -nostartfiles -ffreestanding -nostdlib -nodefaultlibs \
          -I./libbaremetal/include \
          -I./images \
          -I./fonts \
          -L./libbaremetal/lib -Wl,-gc-sections -Wl,-TGD32VF103C8Tx_FLASH.ld \
          -L/usr/lib/gcc/riscv64-unknown-elf/12.2.0/rv32imac/ilp32 \
          -o out.elf

libbaremetal/lib/libc.a:
	$(MAKE) -C ./libbaremetal

libbaremetal/lib/libm.a:
	$(MAKE) -C ./libbaremetal

clean:
	rm -f out.elf
	$(MAKE) -C ./libbaremetal clean
