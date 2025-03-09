### CHIP CONFIG ###
MMCU := atmega328p
F_CPU := 16000000UL

### COMPILATION FLAGS ###
CC := avr-gcc
CFLAGS = -O1 -mmcu=$(MMCU) -Wall -Wextra -Werror -D F_CPU=$(F_CPU) -D UART_BAUDRATE=$(UART_BAUDRATE) -I $(INCLUDE_DIR)
OBJCOPY := avr-objcopy
AVRDUDE := avrdude

PROGRAMMER := arduino
UART_BAUDRATE := 115200


### GENNERAL CONFIG ###
INCLUDE_DIR := ./includes/

SRC_DIR := src
SRC_FILES := uart.c \
			 i2c.c \
		     main.c

OBJ_DIR := .obj
OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))


### RULES ###
all : hex flash

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


hex : $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o main.bin
	$(OBJCOPY) -O ihex main.bin main.hex

flash: hex
	$(AVRDUDE) -c $(PROGRAMMER) -p m328p -b $(UART_BAUDRATE) -U flash:w:main.hex -P /dev/ttyUSB0

both: flash
	$(AVRDUDE) -c $(PROGRAMMER) -p m328p -b $(UART_BAUDRATE) -U flash:w:main.hex -P /dev/ttyUSB1

clean :
	rm main.hex main.bin
