CC=arm-linux-gnueabi-gcc
CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv6 -msoft-float -fPIC -mapcs-frame
LD=arm-linux-gnueabi-ld
LDFLAGS=-N -Ttext=0x10000

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

kernel.elf: kernel.o bootstrap.o 

run: kernel.elf 
	qemu-system-arm -M versatilepb -cpu arm1176 -nographic -kernel kernel.elf
clean:
	$(RM) *.o *.elf
