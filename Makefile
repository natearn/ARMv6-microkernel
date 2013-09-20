CC=arm-linux-gnueabi-gcc
CFLAGS=-ansi -pedantic -Wall -Wextra -march=armv6 -msoft-float -fPIC -mapcs-frame -ffreestanding
LD=arm-linux-gnueabi-ld
LDFLAGS=-N -Ttext=0x10000

.SUFFIXES: .o .elf
.o.elf:
	$(LD) $(LDFLAGS) -o $@ $^

.SUFFIXES: .s .o
.s.o:
	$(CC) $(CFLAGS) -o $@ -c $^

kernel.o: pipe.o nameserver.o string.o pipe.h kernel.h syscall.h nameserver.h string.h

kernel.elf: kernel.o bootstrap.o syscall.o pipe.o nameserver.o string.o

run: kernel.elf 
	qemu-system-arm -M versatilepb -cpu arm1176 -nographic -kernel kernel.elf
clean:
	$(RM) *.o *.elf
