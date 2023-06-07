#include <stdio.h>
#include <tusb.h>
#include "pico/stdlib.h"

#define PRG 12
#define DIN 15
#define CLK 14
#define LATCH 13
#define RESET 11

#define PROG_LED 25

#define LSBFIRST 1
#define MSBFIRST 2

char* parse_number(char* input, int* output) {
	int idx = 0;
	int number_system_base = 10;

	if (input[0] == '0') {
		if (input[1] == 'x') {
			number_system_base = 16;
			idx = 2;
		} else if (input[1] == 'b') {
			number_system_base = 2;
			idx = 2;
		}
	}

	int _number = 0;

	while (input[idx] != '\0') {
		if (input[idx] >= '0' && input[idx] <= '9') {
			_number = _number * number_system_base + (input[idx] - '0');
		} else if (input[idx] >= 'a' && input[idx] <= 'f') {
			_number = _number * number_system_base + (input[idx] - 'a' + 10);
		} else if (input[idx] >= 'A' && input[idx] <= 'F') {
			_number = _number * number_system_base + (input[idx] - 'A' + 10);
		} else {
			break;
		}

		idx++;
	}

	*output = _number;

	return &input[idx];
}

#define MAX_LENGTH 64
unsigned char str[MAX_LENGTH + 1];

unsigned char* readLine() {
	memset(str, 0, sizeof(str));
	unsigned char u;
	unsigned char* p;

	for(p = str, u = getchar(); u != '\n' && p - str < MAX_LENGTH; u = getchar()) {
		*p++ = u;
	}

	*p = 0;
	
	return str;
}

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
	for (uint8_t i = 0; i < 8; i++)  {
		if (bitOrder == LSBFIRST)
			gpio_put(dataPin, !!(val & (1 << i)));
		else      
			gpio_put(dataPin, !!(val & (1 << (7 - i))));

		gpio_put(clockPin, 1);
		gpio_put(clockPin, 0);            
	}
}

void sendByte(uint8_t addr, uint8_t byte) {
	shiftOut(DIN, CLK, MSBFIRST, addr); // address
    shiftOut(DIN, CLK, MSBFIRST, byte); // byte
    gpio_put(LATCH, 1);
    sleep_ms(10);
    gpio_put(LATCH, 0);
}

void gpioInitOut(int gpio) {
	gpio_init(gpio);
	gpio_set_dir(gpio, GPIO_OUT);
}

int main() {
    stdio_init_all();

	gpioInitOut(DIN);
	gpioInitOut(CLK);
	gpioInitOut(LATCH);
	gpioInitOut(RESET);
	gpioInitOut(PRG);
	gpioInitOut(PROG_LED);

	gpio_put(PROG_LED, 1);

	gpio_put(RESET, 1);

	while (!tud_cdc_connected()) {}

	while (true) {
        char* buf = readLine();

        if (strcmp(buf, "PING") == 0) {
        } else if(strncmp(buf, "WRITE ", 6) == 0) {
            int addr;
            char* new = parse_number(&buf[6], &addr);
            int val;
            parse_number(&new[1], &val);
			sendByte(addr, val);
        } else if (strcmp(buf, "RESET ON") == 0) {
			gpio_put(RESET, 0);
        } else if (strcmp(buf, "RESET OFF") == 0) {
			gpio_put(RESET, 1);
        } else if (strcmp(buf, "PRG ON") == 0) {
			gpio_put(PRG, 1);
        } else if (strcmp(buf, "PRG OFF") == 0) {
			gpio_put(PRG, 0);
        } else {
            goto error;
        }

        printf("OK\n");
        continue;

    error:
        printf("ERROR\n");
	}

}