OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

MCU = atmega328p
F_CPU=16000000
BAUD=9600

CFLAGS = -DSILENT -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD) -Iinclude -c
# CFLAGS += -DDEBUG

OUTPUT = arduino_emulator.hex

$(OUTPUT): $(OBJS)
	avr-gcc -mmcu=$(MCU) $^ -o $@.elf
	avr-objcopy -O ihex -R .eeprom $@.elf $@

%.o: %.c
	@echo CC $<
	@avr-gcc $(CFLAGS) $< -o $@

DEVICE=/dev/ttyACM0

upload: $(OUTPUT)
	avrdude -c arduino -p m328p -P $(DEVICE) -b 115200 -U flash:w:$<

monitor:
	screen $(DEVICE) $(BAUD)

clean:
	rm $(OBJS) *.hex *.elf
