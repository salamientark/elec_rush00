### CHIP CONFIG ###
MMCU := atmega328p
F_CPU := 16000000UL

### COMPILATION FLAGS ###
CC := avr-gcc
OBJCOPY := avr-objcopy
AVRDUDE := avrdude

PROGRAMMER := arduino
UART_BAUDRATE := 115200

### GENNERAL CONFIG ###
SRC_DIR := src
SRC_FILES := uart.c \
			main.c

OBJ_DIR := .obj
OBJ_FILES := $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))


### RULES ###
all : hex flash

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -O1 -mmcu=$(MMCU) -Wall -Wextra -Werror -D F_CPU=$(F_CPU) -D UART_BAUDRATE=$(UART_BAUDRATE) -c $< -o $@


hex : $(OBJ_FILES)
	$(CC) -O1 -mmcu=$(MMCU) -Wall -Wextra -Werror -D F_CPU=$(F_CPU) -D UART_BAUDRATE=$(UART_BAUDRATE) $(OBJ_FILES) -o main.bin
	$(OBJCOPY) -O ihex main.bin main.hex

flash: hex
	$(AVRDUDE) -c $(PROGRAMMER) -p m328p -b $(UART_BAUDRATE) -U flash:w:main.hex -P /dev/ttyUSB0

clean :
	rm main.hex main.bin
