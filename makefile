CC = arm-none-eabi-gcc #compilador
CINCLUDES = -I ./includes -I ./drivers
CFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus
LFLAGS = -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld

#archivos para compilar
SOURCES = startup.c main.c drivers/lcd.c

OBJS = $(SOURCES:.c=.o)


main.elf: $(OBJS)

	$(CC) $(CINCLUDES) $(CFLAGS) $(LFLAGS) $(OBJS) -o $@

flash: main.elf
	openocd -f openocd.cfg -c "program main.elf verify reset exit"

clean:
	rm -rf main.o startup.o
mrproper:
	make clean
	rm -rf main.elf
	
%.o: %.c
	$(CC) $(CINCLUDES) $(CFLAGS)  -c -o $@ $<
