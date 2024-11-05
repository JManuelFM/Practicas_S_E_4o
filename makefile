CC = arm-none-eabi-gcc #compilador
AS = arm-none-eabi-as -mthumb -mcpu=cortex-m0plus
CINCLUDES = -I ./includes -I ./drivers
CFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus
LFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld

#archivos para compilar
SOURCES = startup.c main.c drivers/lcd.c

SOURCES_ASM = divide.s

OBJS = $(SOURCES:.c=.o)


main.elf: $(OBJS)

	$(CC) $(CINCLUDES) $(CFLAGS) $(LFLAGS) $(OBJS) divide.o -o $@

flash: main.elf
	openocd -f openocd.cfg -c "program main.elf verify reset exit"

clean:
	rm -rf main.o startup.o divide.o
mrproper:
	make clean
	rm -rf main.elf
	
%.o: %.c
	$(AS) -o divide.o $(SOURCES_ASM)
	$(CC) $(CINCLUDES) $(CFLAGS)  divide.o -c -o $@ $<
