// #define __AVR_ATmega328P__

#include <avr/io.h>
#include <stdint.h>
#include <string.h>
#include <usart.h>
#include <debug.h>
#include <avr/eeprom.h> 
#include <program.h>
#include <stdbool.h>

struct instruction {
	uint8_t reg2 : 2;
	uint8_t reg1 : 2;
	uint8_t opcode : 4;
	uint8_t imm;
} __attribute__((packed));

#define INSTRUCTION_ADDI 0b0
#define INSTRUCTION_ADD 0b1
#define INSTRUCTION_SUBI 0b10
#define INSTRUCTION_SUB 0b11
#define INSTRUCTION_NAND 0b100
#define INSTRUCTION_LDI 0b101
#define INSTRUCTION_MOV 0b110
#define INSTRUCTION_LDM 0b111
#define INSTRUCTION_LDMI 0b1000
#define INSTRUCTION_STM 0b1001
#define INSTRUCTION_STMI 0b1010
#define INSTRUCTION_JZI 0b1011
#define INSTRUCTION_JI 0b1100
#define INSTRUCTION_OUT 0b1101
#define INSTRUCTION_IN 0b1110
#define INSTRUCTION_HALT 0b1111

struct cpu_state {
    uint8_t regs[4];
    uint8_t pc;
    bool halt;
};


uint8_t data[0xff];

uint8_t cpu_fetch_byte(uint8_t addr, bool mode) {
    // debugf("fetching byte at 0x%x mode: %s", addr, mode ? "data" : "program");
    if (mode) {
        return data[addr];
    } else {
        return eeprom_read_byte((uint8_t*) addr);
    }
}

void cpu_write_byte(uint8_t addr, uint8_t byte, bool mode) {
    // debugf("writing byte at 0x%x (0x%x) mode: %s", addr, byte, mode ? "data" : "program");
    if (mode) {
        data[addr] = byte;
    } else {
	    eeprom_write_byte((uint8_t*) addr, byte);
    }
}

const char* regs[] = {
    "r0",
    "r1",
    "r2",
    "r3"
};

void core_run_tick(struct cpu_state* state) {
    if (state->halt) {
        return;
    }

    struct instruction instruction;
    *((uint8_t*) &instruction) = cpu_fetch_byte(state->pc++, false);

    uint8_t im = cpu_fetch_byte(state->pc++, false);

    switch (instruction.opcode) {
        case INSTRUCTION_ADDI:
            debugf("addi %s, %d", regs[instruction.reg1], im);
            state->regs[instruction.reg1] = state->regs[instruction.reg1] + im;
            break;
        case INSTRUCTION_ADD:
            debugf("add %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            state->regs[instruction.reg1] = state->regs[instruction.reg1] + state->regs[instruction.reg2];
            break;
        case INSTRUCTION_SUBI:
            debugf("subi %s, %d", regs[instruction.reg1], im);
            state->regs[instruction.reg1] = state->regs[instruction.reg1] - im;
            break;
        case INSTRUCTION_SUB:
            debugf("sub %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            state->regs[instruction.reg1] = state->regs[instruction.reg1] - state->regs[instruction.reg2];
            break;
        case INSTRUCTION_NAND:
            debugf("nand %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            state->regs[instruction.reg1] = ~(state->regs[instruction.reg1] & state->regs[instruction.reg2]);
            break;
        case INSTRUCTION_LDI:
            debugf("ldi %s, %d", regs[instruction.reg1], im);
            state->regs[instruction.reg1] = im;
            break;
        case INSTRUCTION_MOV:
            debugf("mov %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            state->regs[instruction.reg1] = state->regs[instruction.reg2];
            break;
        case INSTRUCTION_LDM:
            debugf("ldm %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            state->regs[instruction.reg1] = cpu_fetch_byte(state->regs[instruction.reg2], true);
            break;
        case INSTRUCTION_LDMI:
            debugf("ldmi %s, %d", regs[instruction.reg1], im);
            state->regs[instruction.reg1] = cpu_fetch_byte(im, true);
            break;
        case INSTRUCTION_STM:
            debugf("stm %s, %s", regs[instruction.reg1], regs[instruction.reg2]);
            cpu_write_byte(state->regs[instruction.reg2], state->regs[instruction.reg1], true);
            break;
        case INSTRUCTION_STMI:
            debugf("stmi %s, %d", regs[instruction.reg1], im);
            cpu_write_byte(im, state->regs[instruction.reg1], true);
            break;
        case INSTRUCTION_JZI:
            debugf("jzi %s, %d", regs[instruction.reg1], im);
            if (state->regs[instruction.reg1] == 0) {
                debugf("exec");
                state->pc = im;
            }
            break;
        case INSTRUCTION_JI:
            debugf("ji %s, %d", regs[instruction.reg1], im);
            state->pc = im;
            break;
        case INSTRUCTION_OUT:
            debugf("out %s", regs[instruction.reg1]);
            PORTB = state->regs[instruction.reg1];
            break;
        case INSTRUCTION_IN:
            debugf("in %s", regs[instruction.reg1]);
            state->regs[instruction.reg1] = PIND;
            break;
        case INSTRUCTION_HALT:
            debugf("HALT");
            state->halt = true;
            break;
    }
}

void main() {
	DDRB |= _BV(DDB5);
    DDRB &= ~_BV(DDB4); // set pin 12 input
    PORTB |= _BV(DDB4); // set pull-up for pin 12
    
	USART0_init();

    if (!(PINB & _BV(DDB4))) { // go into programing mode if pin 12 is pulled low
    	PORTB |= _BV(DDB5);
		programing_mode();
    }

    DDRB = 0xff; // set pb0 - pb7 as output
    DDRD = 0x0; // set pd0 - pd7 as input
    PORTD = 0x0; // set pd0 - pd7 no pull
    
    struct cpu_state state;
    memset(&state, 0, sizeof(struct cpu_state));
	while (!state.halt) {
        debugf("-- STATE --");
        debugf("r0: %x, r1: %x, r2: %x, r3: %x", state.regs[0], state.regs[1], state.regs[2], state.regs[3]);
        debugf("pc: %x", state.pc);
        debugf("--- END ---");
        core_run_tick(&state);
	}

    while (1) {}
}

// test/blink.hex  test/fib2.hex  test/fib.hex  test/io.hex  test/io_mirror.hex  test/memory.hex  test/memory_im.hex  test/nand.hex  test/test.hex
