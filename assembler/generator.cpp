#include "generator.h"
#include "debug.h"

void print_error(int pos_start, int pos_end, char* error);

instruction_builder_t instruction_builder(char* name, int op1, int op2, int opcode) {
	instruction_builder_t instr = { 0 };
	strcpy(instr.name, name);
	instr.operands[0] = op1;
	instr.operands[1] = op2;
	instr.opcode = opcode;

	debugf("instr: %s (%d, %d) -> %d\n", name, op1, op2, opcode);

	return instr;
}


regsiter_name_list_t register_name(char* name, int id) {
	regsiter_name_list_t reg = { 0 };
	strcpy(reg.name, name);
	reg.id = id;

	debugf("reg: %s (%d)\n", name, id);

	return reg;
}

generator::generator(list<lexer_token_t>* tokens) : register_names(10), instruction_builders(10) {
	this->tokens = tokens;
	this->pos = -1;
	this->advance();

	register_names.add(register_name((char*)"r0", R0));
	register_names.add(register_name((char*)"r1", R1));
	register_names.add(register_name((char*)"r2", R2));
	register_names.add(register_name((char*)"r3", R3));

	// instruction_builders.add(instruction_builder((char*)"nop", NONE, NONE, build_nop_instr));

	instruction_builders.add(instruction_builder((char*) "addi", IR0, IIMM, INSTRUCTION_ADDI));
	instruction_builders.add(instruction_builder((char*) "add", IR0, IR1, INSTRUCTION_ADD));
	instruction_builders.add(instruction_builder((char*) "subi", IR0, IIMM, INSTRUCTION_SUBI));
	instruction_builders.add(instruction_builder((char*) "sub", IR0, IR1, INSTRUCTION_SUB));
	instruction_builders.add(instruction_builder((char*) "nand", IR0, IR1, INSTRUCTION_NAND));
	instruction_builders.add(instruction_builder((char*) "ldi", IR0, IIMM, INSTRUCTION_LDI));
	instruction_builders.add(instruction_builder((char*) "mov", IR0, IR1, INSTRUCTION_MOV));
	instruction_builders.add(instruction_builder((char*) "ldm", IR0, IR1, INSTRUCTION_LDM));
	instruction_builders.add(instruction_builder((char*) "ldmi", IR0, IIMM, INSTRUCTION_LDMI));
	instruction_builders.add(instruction_builder((char*) "stm", IR0, IR1, INSTRUCTION_STM));
	instruction_builders.add(instruction_builder((char*) "stmi", IR0, IIMM, INSTRUCTION_STMI));
	instruction_builders.add(instruction_builder((char*) "jzi", IR0, IIMM, INSTRUCTION_JZI));
	instruction_builders.add(instruction_builder((char*) "ji", IIMM, NONE, INSTRUCTION_JI));
	instruction_builders.add(instruction_builder((char*) "out", IR0, NONE, INSTRUCTION_OUT));
	instruction_builders.add(instruction_builder((char*) "in", IR0, NONE, INSTRUCTION_IN));
	instruction_builders.add(instruction_builder((char*) "halt", NONE, NONE, INSTRUCTION_HALT));

}


generator::~generator() {
	if (this->data) {
		free(data);
	}
}


void generator::advance() {
	this->pos++;
	if (this->pos < this->tokens->length) {
		this->current_token = &this->tokens->data[this->pos];
	}
	else {
		this->current_token = nullptr;
		// debugf("could not load token at %d\n", this->pos);
	}
}


void generator::emit(instruction_t instr) {
	uint8_t* instr_p = (uint8_t*)&instr;

	// debugf("Emitting instruction: 0x%x 0x%x 0x%x 0x%x\n", instr.opcode, instr.reg1, instr.reg2, instr.imm);

	for (int i = 0; i < sizeof(instruction_t); i++) {
		this->data[this->current_data_pos + i] = instr_p[i];
	}

	this->current_data_pos += sizeof(instruction_t);
}


bool generator::gen() {
	uint16_t address = 0;
	list<label_t> labels = list<label_t>(10);

	debugf("Doing pass 1...\n");

	while (this->current_token) {
		if (this->current_token->data.type != ID) {
			print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID");
			return true;
		}

		lexer_token_t tmp = this->current_token->data;
		this->advance();

		if (this->current_token && this->current_token->data.type == COLLON) {
			debugf("label %s at 0x%x\n", tmp.data_s, address);

			label_t label = { 0 };
			strcpy(label.name, tmp.data_s);
			label.address = address;
			labels.add(label);

			this->advance();
		}
		else {
			list<instruction_builder_t>::node* n = this->instruction_builders.find<char*>([](char* instr, list<instruction_builder_t>::node* n) {
				return strcmp(n->data.name, instr) == 0;
			}, tmp.data_s);

			if (n == nullptr) {
				char buf[256] = { 0 };
				sprintf(buf, "Instruction %s not found", tmp.data_s);
				print_error(tmp.pos_start, tmp.pos_end, buf);
				return true;
			}

			for (int i = 0; i < 2; i++) {
				switch (n->data.operands[i]) {
					case NONE:
						continue;
						break;
					case IR0:
					case IR1:
						if (this->current_token->data.type != ID) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID");
							return true;
						}
						break;
					case IIMM:
						if (this->current_token->data.type != ID && this->current_token->data.type != NUMBER) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID or NUMBER");
							return true;
						}
						if (this->current_token->data.type == ID) {
							if (strcmp(this->current_token->data.data_s, "addr") == 0) {
								this->advance();
								if (this->current_token->data.type != LPAREN) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected '('");
									return true;
								}
								this->advance();
								if (this->current_token->data.type != ID && this->current_token->data.type != NUMBER) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID or NUMBER");
									return true;
								}
								this->advance();
								if (this->current_token->data.type != RPAREN) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ')'");
									return true;
								}
							}
							else {
								print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected 'addr'");
								return true;
							}
						}
						break;
				}

				if (i == 0 && n->data.operands[1] != NONE) {
					this->advance();
					if (this->current_token->data.type != COMMA) {
						print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ','");
						return true;
					}
				}
				this->advance();
			}

			address += sizeof(instruction_t);
		}
	}

	this->pos = -1;
	this->advance();

	data = (uint8_t*) malloc(address);
	data_len = address;
	current_data_pos = 0;

	debugf("Doing pass 2...\n");

	while (this->current_token) {
		if (this->current_token->data.type != ID) {
			print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID");
			return true;
		}

		lexer_token_t tmp = this->current_token->data;
		this->advance();

		if (this->current_token && this->current_token->data.type == COLLON) {
			this->advance();
		}
		else {
			list<instruction_builder_t>::node* n = this->instruction_builders.find<char*>([](char* instr, list<instruction_builder_t>::node* n) {
				return strcmp(n->data.name, instr) == 0;
			}, tmp.data_s);

			if (n == nullptr) {
				char buf[256] = { 0 };
				sprintf(buf, "Instruction %s not found", tmp.data_s);
				print_error(tmp.pos_start, tmp.pos_end, buf);
				return true;
			}

			int ir0 = 0;
			int ir1 = 0;
			int iimm = 0;

			for (int i = 0; i < 2; i++) {
				switch (n->data.operands[i]) {
					case NONE:
						continue;
						break;

					case IR0:
					{
						if (this->current_token->data.type != ID) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID");
							return true;
						}

						list<regsiter_name_list_t>::node* r = this->register_names.find<char*>([](char* name, list<regsiter_name_list_t>::node* n) {
							return strcmp(name, n->data.name) == 0;
						}, this->current_token->data.data_s);

						if (r == nullptr) {
							char buf[256] = { 0 };
							sprintf(buf, "Unknown register: %s", this->current_token->data.data_s);
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, buf);
							return true;
						}

						ir0 = r->data.id;
					}
					break;

					case IR1:
					{
						if (this->current_token->data.type != ID) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID");
							return true;
						}

						list<regsiter_name_list_t>::node* r = this->register_names.find<char*>([](char* name, list<regsiter_name_list_t>::node* n) {
							return strcmp(name, n->data.name) == 0;
						}, this->current_token->data.data_s);

						if (r == nullptr) {
							char buf[256] = { 0 };
							sprintf(buf, "Unknown register: %s", this->current_token->data.data_s);
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, buf);
							return true;
						}

						ir1 = r->data.id;
					}
					break;

					case IIMM:
						if (this->current_token->data.type != ID && this->current_token->data.type != NUMBER) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID or NUMBER");
							return true;
						}
						if (this->current_token->data.type != ID && this->current_token->data.type != NUMBER) {
							print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID or NUMBER");
							return true;
						}
						if (this->current_token->data.type == ID) {
							if (strcmp(this->current_token->data.data_s, "addr") == 0) {
								this->advance();
								if (this->current_token->data.type != LPAREN) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected LPAREN");
									return true;
								}
								this->advance();
								if (this->current_token->data.type != ID && this->current_token->data.type != NUMBER) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected ID or NUMBER");
									return true;
								}

								uint16_t num = 0;

								if (this->current_token->data.type == NUMBER) {
									num = this->current_token->data.data_i;
								}
								else {
									list<label_t>::node* l = labels.find<char*>([](char* name, list<label_t>::node* n) {
										return strcmp(name, n->data.name) == 0;
									}, this->current_token->data.data_s);

									if (l == nullptr) {
										char buf[256];
										sprintf(buf, "Label %s not found", this->current_token->data.data_s);
										print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, buf);
										return true;
									}
									num = l->data.address;
								}

								iimm = num;
								
								this->advance();
								if (this->current_token->data.type != RPAREN) {
									print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected RPAREN");
									return true;
								}
							}
							else {
								print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected 'lo' or 'hi' or 'addr'");
								return true;
							}
						}
						else {
							iimm = this->current_token->data.data_i;
						}
						break;
				}

				if (i == 0 && n->data.operands[1] != NONE) {
					this->advance();
					if (this->current_token->data.type != COMMA) {
						print_error(this->current_token->data.pos_start, this->current_token->data.pos_end, (char*)"Expected COMMA");
						return true;
					}
				}
				this->advance();
			}

			instruction_t i = (instruction_t) {
				.reg2 = (uint8_t) ir1,
				.reg1 = (uint8_t) ir0,
				.opcode = (uint8_t) n->data.opcode,
				.imm = (uint8_t) iimm
			};
			emit(i);
		}
	}

	return false;
}


uint8_t* generator::get_data() {
	return this->data;
}


uint16_t generator::get_data_len() {
	return this->data_len;
}
