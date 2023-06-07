#pragma once

#include <stdint.h>

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
#ifdef _MSC_VER
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif


#define R0 0 // x 0b0000
#define R1 1 // x 0b0001
#define R2 2 // x 0b0010
#define R3 3 // x 0b0011

typedef PACK(struct instruction {
	uint8_t reg2 : 2;
	uint8_t reg1 : 2;
	uint8_t opcode : 4;
	uint8_t imm;
}) instruction_t;

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