# Compilador + flags
CC = arm-none-eabi-gcc
CFLAGS = -I./includes -I./drivers -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z256VLL4 #para crear objetos
LDFLAGS =  --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld #para crear binario ejecutable

# Configuración de OpenOCD
OPENOCD = openocd
OPENOCD_CFG = openocd.cfg
OPENOCD_TARGET = target_name

# Archivos led_blinky
SRCS_LED = led_blinky.c startup.c drivers/board.c drivers/clock_config.c \
drivers/fsl_clock.c drivers/fsl_gpio.c drivers/fsl_common.c drivers/pin_mux.c \
drivers/system_MKL46Z4.c drivers/fsl_debug_console.c drivers/fsl_smc.c \
drivers/fsl_log.c drivers/fsl_io.c drivers/fsl_lpsci.c drivers/fsl_uart.c \
drivers/fsl_str.c drivers/fsl_ftfx_cache.c drivers/fsl_ftfx_controller.c \
drivers/fsl_ftfx_flash.c drivers/fsl_assert.c drivers/pin_mux.c

# Archivos hello_world
SRCS_HELLO = hello_world.c startup.c drivers/board.c drivers/clock_config.c \
drivers/fsl_clock.c drivers/fsl_gpio.c drivers/fsl_common.c drivers/pin_mux.c \
drivers/system_MKL46Z4.c drivers/fsl_assert.c drivers/fsl_debug_console.c \
drivers/fsl_smc.c drivers/fsl_io.c drivers/fsl_uart.c drivers/fsl_str.c \
drivers/fsl_log.c drivers/fsl_ftfx_cache.c drivers/fsl_lpsci.c

# Object files
OBJS_LED = $(SRCS_LED:.c=.o)
OBJS_HELLO = $(SRCS_HELLO:.c=.o)

# Binarios
BINARY_LED = led_blinky.elf
BINARY_HELLO = hello_world.elf

# Default
all:  $(BINARY_HELLO) $(BINARY_LED)

# Crear binario led_blinky
$(BINARY_LED): $(OBJS_LED)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "led_blinky binary created successfully"

# Crear binario hello_world
$(BINARY_HELLO): $(OBJS_HELLO)
	@$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "hello_world binary created successfully"

# Flash led_blinky
flash_led: $(BINARY_LED)
	@$(OPENOCD) -f $(OPENOCD_CFG) -c "program $(BINARY_LED) verify reset exit"
	@echo "led_blinky flashed successfully"

# Flash hello_world
flash_hello: $(BIN_HELLO)
	@$(OPENOCD) -f $(OPENOCD_CFG) -c "program $(BINARY_HELLO) verify reset exit"
	@echo "hello_world flashed successfully"

# Borrar objetos y binarios (limpieza)
clean:
	@rm -f $(OBJS_LED) $(OBJS_HELLO) $(BINARY_LED) $(BINARY_HELLO)
	@echo "object files and binaries removed"

# Regla para objetos
%.o: %.c
	@$(CC) $(CFLAGS)  -c -o $@ $<
