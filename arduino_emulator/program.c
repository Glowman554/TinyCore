#include <program.h>
#include <avr/io.h>
#include <stdint.h>
#include <usart.h>
#include <avr/eeprom.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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

void cpu_write_byte(uint8_t addr, uint8_t byte, bool mode);

void programing_mode() {
    USART0_transmit_str("READY\n");

	while (true) {
        char buf[0xff] = { 0 };
        USART0_receive_until(buf, '\n');

        if (strcmp(buf, "PING") == 0) {
        } else if(strncmp(buf, "WRITE ", 6) == 0) {
            int addr;
            char* new = parse_number(&buf[6], &addr);
            int val;
            parse_number(&new[1], &val);
            cpu_write_byte(addr, val, false);
        } else {
            goto error;
        }

        USART0_transmit_str("OK\n");
        continue;

    error:
        USART0_transmit_str("ERROR\n");
	}
}
