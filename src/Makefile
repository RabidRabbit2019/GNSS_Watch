TOOLS=/home/vzaytsev/tools/riscv-gcc/bin/

all: a.elf

a.elf: libbaremetal/lib/libc.a libbaremetal/lib/libm.a
	$(TOOLS)riscv64-unknown-elf-gcc -DCK_SYS_VALUE=64000000 \
          startup.S \
          n200_func.c \
          display.c \
          fonts/font_bmp.c \
          images/zic_utils.c \
          fonts/font_24_26.c \
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
          main.c \
          -Os -s -flto -march=rv32imac_zicsr -mabi=ilp32 -ffunction-sections -fdata-sections \
          -nostartfiles -ffreestanding -nostdlib -nodefaultlibs \
          -I./libbaremetal/include \
          -I./images \
          -I./fonts \
          -L./libbaremetal/lib -Wl,-gc-sections -Wl,-TGD32VF103C8Tx_FLASH.ld \
          -o a.elf

libbaremetal/lib/libc.a:
	$(MAKE) -C ./libbaremetal

libbaremetal/lib/libm.a:
	$(MAKE) -C ./libbaremetal

clean:
	rm -f a.elf
	$(MAKE) -C ./libbaremetal clean
