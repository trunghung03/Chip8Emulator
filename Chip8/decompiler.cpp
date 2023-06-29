#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>

constexpr auto BYTE = 2;

void disassembler(uint8_t* codeBuffer, int pc);
void clearDisplay();

int main(int argc, char* argv[]) {
	FILE* fp = fopen("logo.ch8", "rb");
	if (fp == NULL) {
		puts("Error: unable to open file");
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	uint8_t* buffer = static_cast<uint8_t*>(malloc(fsize));
	if (buffer == NULL) {
		puts("Error: failed to malloc");
		exit(1);
	}

	if (!fread(buffer, 1, fsize, fp) && ferror(fp)) //if fread() returns 0 and error happens
	{
		fputs("Error reading file", stderr);
	}
	else fclose(fp);

	uint16_t pc = 0;
	for (; pc < fsize; pc += BYTE) disassembler(buffer, pc);

	fclose(fp);
	return 0;
}

void disassembler(uint8_t* codeBuffer, int pc) {
	uint16_t code = (codeBuffer[pc] << 8) | codeBuffer[pc + 1];
	printf("%04x\t", pc);

	if (code == 0x00E0) printf("CLS");
	else if (code == 0x00EE) printf("RET");
	else if (code >= 0x1000 && code <= 0x1FFF) printf("JP $%04x", code & 0x0FFF); // 1nnn
	else if (code >= 0x2000 && code <= 0x2FFF) printf("CALL $%04x", code & 0x0FFF); // 2nnn

	else if (code >= 0x3000 && code <= 0x3FFF) printf("SE V%d, $%02x", (code & 0x0F00) >> 8, code & 0x00FF); // 3xkk
	else if (code >= 0x4000 && code <= 0x4FFF) printf("SNE V%d, $%02x", (code & 0x0F00) >> 8, code & 0x00FF); // 4xkk

	else if (code >= 0x5000 && code <= 0x5FFF) printf("SE V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 5xy0

	else if (code >= 0x6000 && code <= 0x6FFF) printf("LD V%d, $%02x", (code & 0x0F00) >> 8, code & 0x00FF); // 6xkk
	else if (code >= 0x7000 && code <= 0x7FFF) printf("ADD V%d, $%02x", (code & 0x0F00) >> 8, code & 0x00FF); // 7xkk

	else if (code >= 0x8000 && code <= 0x8FFF) {
		uint8_t lastDigit = (code & 0x000F);
		switch (lastDigit) {
		case 0x0:
			printf("LD V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy0
			break;
		case 0x1:
			printf("OR V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy1
			break;
		case 0x2:
			printf("AND V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy2
			break;
		case 0x3:
			printf("XOR V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy3
			break;
		case 0x4:
			printf("ADD V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy4
			break;
		case 0x5:
			printf("SUB V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy5
			break;
		case 0x6:
			printf("SHR V%d{, V%d}", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8x60
			break;
		case 0x7:
			printf("SUBN V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xy7
			break;
		case 0xE:
			printf("SHR V%d{, V%d}", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 8xyE
			break;
		default:
			printf("OP not found (%04x)", code);
			break;
		}
	} // 0x8xy0 -> 0x8xyE

	else if (code >= 0x9000 && code <= 0x9FFF) printf("SNE V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 9xy0

	else if (code >= 0xA000 && code <= 0xAFFF) printf("LD I, $%04x", code & 0xFFF); // Annn
	else if (code >= 0xB000 && code <= 0xBFFF) printf("JP V0, $%04x", code & 0xFFF); // Bnnn

	else if (code >= 0xC000 && code <= 0xCFFF) printf("RND V%d, $%02x", (code & 0x0F00) >> 8, code & 0x00FF); // Cxkk

	else if (code >= 0xD000 && code <= 0xDFFF) printf("SE V%d, V%d, $%01x", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4, (code & 0x000F)); // Dxyn

	else if (code >= 0xE09E && code <= 0xEF9E) printf("SKP V%d", (code & 0x0F00) >> 8); // Ex9E
	else if (code >= 0xE0A1 && code <= 0xEFA1) printf("SKNP V%d", (code & 0x0F00) >> 8); // ExA1

	else if (code >= 0xF007 && code <= 0xFF07) printf("LD V%d, DT", (code & 0x0F00) >> 8); // Fx07
	else if (code >= 0xF00A && code <= 0xFF0A) printf("LD V%d, K", (code & 0x0F00) >> 8); // Fx0A
	else if (code >= 0xF015 && code <= 0xFF15) printf("LD DT, V%d", (code & 0x0F00) >> 8); // Fx15
	else if (code >= 0xF018 && code <= 0xFF18) printf("LD ST, V%d", (code & 0x0F00) >> 8); // Fx18
	else if (code >= 0xF01E && code <= 0xFF1E) printf("ADD I, V%d", (code & 0x0F00) >> 8); // Fx1E
	else if (code >= 0xF029 && code <= 0xFF29) printf("LD F, V%d", (code & 0x0F00) >> 8); // Fx29
	else if (code >= 0xF033 && code <= 0xFF33) printf("LD B, V%d", (code & 0x0F00) >> 8); // Fx33
	else if (code >= 0xF055 && code <= 0xFF55) printf("LD [I], V%d", (code & 0x0F00) >> 8); // Fx55
	else if (code >= 0xF065 && code <= 0xFF65) printf("LD V%d, [I]", (code & 0x0F00) >> 8); // Fx65

	else printf("OP not found (%04x)", code);

	printf("\n");
}

struct Chip8 {
	std::array<uint8_t, 0x1000> RAM;
	std::array<uint8_t, 16> V; // General purpose register
	uint16_t I; // Memory address
	uint8_t DT; // Delay timer
	uint8_t ST; // Sound timer
	uint16_t PC; // Program counter
	uint8_t SP; // Stack pointer
	std::array<uint16_t, 16> stack; // The stack
	std::array<uint8_t, 64 * 32> graphics;

	Chip8() = default;
} chip8;


void UnimplementedInstruction() {
	printf("Error: Unimplemented Instruction: %02x", chip8.RAM[chip8.PC]);
	exit(1);
}

void emulator(uint8_t* codeBuffer, int pc) {
	uint16_t code = (codeBuffer[pc] << 8) | codeBuffer[pc + 1];
	printf("%04x\t", pc);

	// Clear the display.
	if (code == 0x00E0) clearDisplay();

	// Return from a subroutine.
	// The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
	else if (code == 0x00EE) {
		chip8.PC = chip8.stack[chip8.SP];
		chip8.SP--;
	}

	// Jump to location nnn.
	// The interpreter sets the program counter to nnn.
	else if (code >= 0x1000 && code <= 0x1FFF) {
		chip8.PC = code & 0x0FFF;
	}

	//Call subroutine at nnn.
	//The interpreter increments the stack pointer, then puts the current PC on the top of the stack.The PC is then set to nnn.
	else if (code >= 0x2000 && code <= 0x2FFF) {
		chip8.SP++;
		chip8.stack[chip8.SP] = chip8.PC;
		chip8.PC = code & 0x0FFF;
	}

	// Skip next instruction if Vx = kk.
	// The interpreter compares register Vx to kk, and if they are equal, increments the program counter by 2.
	else if (code >= 0x3000 && code <= 0x3FFF) {
		if (chip8.V[(code & 0x0F00) >> 8] == (code & 0x00FF)) {
			chip8.PC += 2;
		}
	}

	// Skip next instruction if Vx != kk.
	// The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
	else if (code >= 0x4000 && code <= 0x4FFF) {
		if (chip8.V[(code & 0x0F00) >> 8] == (code & 0x00FF)) {
			chip8.PC += 2;
		}
	}


	// Skip next instruction if Vx = Vy.
	// The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
	else if (code >= 0x5000 && code <= 0x5FFF) {
		if (chip8.V[(code & 0x0F00) >> 8] == chip8.V[(code & 0x00F0) >> 4]) {
			chip8.PC += 2;
		}
	}

	// Set Vx = kk.
	// The interpreter puts the value kk into register Vx.
	else if (code >= 0x6000 && code <= 0x6FFF) {
		chip8.V[(code & 0x0F00) >> 8] = code & 0x00FF;
	}

	// Set Vx = Vx + kk.
	// Adds the value kk to the value of register Vx, then stores the result in Vx.
	else if (code >= 0x7000 && code <= 0x7FFF) {
		chip8.V[(code & 0x0F00) >> 8] += code & 0x00FF;
	}

	else if (code >= 0x8000 && code <= 0x8FFF) {
		uint8_t lastDigit = (code & 0x000F);
		uint8_t result = 0;

		switch (lastDigit) {

			// Set Vx = Vy.
			// Stores the value of register Vy in register Vx.
		case 0x0:
			chip8.V[(code & 0x0F00) >> 8] = chip8.V[(code & 0x00F0) >> 4];
			break;

			// Set Vx = Vx OR Vy.
			// Performs a bitwise OR on the values of Vx and Vy, 
			// then stores the result in Vx. A bitwise OR compares the correponding bits from two values, 
			// and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
		case 0x1:
			chip8.V[(code & 0x0F00) >> 8] |= chip8.V[(code & 0x00F0) >> 4];
			break;

			// Set Vx = Vx AND Vy.
			// Performs a bitwise AND on the values of Vx and Vy, 
			// then stores the result in Vx.A bitwise AND compares the corrseponding bits from two values, 
			// and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
		case 0x2:
			chip8.V[(code & 0x0F00) >> 8] &= chip8.V[(code & 0x00F0) >> 4];
			break;

			// Set Vx = Vx XOR Vy.
			// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
			// An exclusive OR compares the corrseponding bits from two values, 
			// and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
		case 0x3:
			chip8.V[(code & 0x0F00) >> 8] ^= chip8.V[(code & 0x00F0) >> 4];
			break;

			// Set Vx = Vx + Vy, set VF = carry.
			// The values of Vx and Vy are added together.
			// If the result is greater than 8 bits(i.e., > 255, ) VF is set to 1, otherwise 0. 
			// Only the lowest 8 bits of the result are kept, and stored in Vx.
		case 0x4:
			result = chip8.V[(code & 0x0F00) >> 8] + chip8.V[(code & 0x00F0) >> 4];
			if (result > 255) {
				chip8.V[0xF] = 1;
			}
			else {
				chip8.V[0xF] = 0;
			}
			chip8.V[(code & 0x0F00) >> 8] = result & 0xFF;
			break;

			// Set Vx = Vx - Vy, set VF = NOT borrow.
			// If Vx > Vy, then VF is set to 1, otherwise 0. 
			// Then Vy is subtracted from Vx, and the results stored in Vx.
		case 0x5:
			if (chip8.V[(code & 0x0F00) >> 8] > chip8.V[(code & 0x00F0) >> 4]) chip8.V[0xF] = 1;
			else chip8.V[0xF] = 0;

			result = chip8.V[(code & 0x0F00) >> 8] - chip8.V[(code & 0x00F0) >> 4];
			chip8.V[(code & 0x0F00) >> 8] = result & 0xFF;
			break;

			// Set Vx = Vx SHR 1.
			// If the least - significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
		case 0x6:
			if (1 & chip8.V[(code & 0x0F00) >> 8]) chip8.V[0xF] = 1;
			else chip8.V[0xF] = 0;

			chip8.V[(code & 0x0F00) >> 8] /= 2;
			break;

			// Set Vx = Vy - Vx, set VF = NOT borrow.
			// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
		case 0x7:
			if (chip8.V[(code & 0x00F0) >> 4] > chip8.V[(code & 0x0F00) >> 8]) chip8.V[0xF] = 1;
			else chip8.V[0xF] = 0;

			chip8.V[(code & 0x0F00) >> 8] = chip8.V[(code & 0x00F0) >> 4] - chip8.V[(code & 0x0F00) >> 8];
			break;

			// Set Vx = Vx SHL 1.
			// If the most - significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
		case 0xE:
			if (1 << 7 & chip8.V[(code & 0x0F00) >> 8]) chip8.V[0xF] = 1;
			else chip8.V[0xF] = 0;
			chip8.V[(code & 0x0F00) >> 8] *= 2;
			break;
		default:
			printf("OP not found (%04x)", code);
			break;
		} // 0x8xy0 -> 0x8xyE
	}

	else if (code >= 0x9000 && code <= 0x9FFF) printf("SNE V%d, V%d", (code & 0x0F00) >> 8, (code & 0x00F0) >> 4); // 9xy0

	else if (code >= 0xA000 && code <= 0xAFFF) printf("LD I, $%04x", code & 0xFFF); // Annn
	else if (code >= 0xB000 && code <= 0xBFFF) printf("JP V0, $%04x", code & 0xFFF); // Bnnn

	// Set Vx = random byte AND kk.

	// The interpreter generates a random number from 0 to 255, 
	// which is then ANDed with the value kk.The results are stored in Vx.
	// See instruction 8xy2 for more information on AND.
	else if (code >= 0xC000 && code <= 0xCFFF) {

	}

	else if (code >= 0xD000 && code <= 0xDFFF) UnimplementedInstruction(); // Dxyn

	else if (code >= 0xE09E && code <= 0xEF9E) UnimplementedInstruction(); // Ex9E
	else if (code >= 0xE0A1 && code <= 0xEFA1) UnimplementedInstruction(); // ExA1

	// Set Vx = delay timer value.
	// The value of DT is placed into Vx.
	else if (code >= 0xF007 && code <= 0xFF07) {
		chip8.V[(code & 0x0F00) >> 8] = chip8.DT;
	}
	else if (code >= 0xF00A && code <= 0xFF0A) UnimplementedInstruction(); // Fx0A

	// Set delay timer = Vx.
	// DT is set equal to the value of Vx.
	else if (code >= 0xF015 && code <= 0xFF15) {
		chip8.DT = chip8.V[(code & 0x0F00) >> 8];
	}

	// Set sound timer = Vx.
	// ST is set equal to the value of Vx.
	else if (code >= 0xF018 && code <= 0xFF18) {
		chip8.ST = chip8.V[(code & 0x0F00) >> 8];
	}

	// Set I = I + Vx.
	// The values of I and Vx are added, and the results are stored in I.
	else if (code >= 0xF01E && code <= 0xFF1E) {
		chip8.I = (code & 0x0F00) >> 8;
	}


	else if (code >= 0xF029 && code <= 0xFF29) UnimplementedInstruction(); // Fx29

	// Store BCD representation of Vx in memory locations I, I+1, and I+2.
	// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, 
	// the tens digit at location I + 1, and the ones digit at location I + 2.
	else if (code >= 0xF033 && code <= 0xFF33) {
		chip8.RAM[chip8.I] = chip8.V[(code & 0x0F00) >> 8] % 10; // ones digit
		chip8.RAM[chip8.I + 1] = chip8.V[(code & 0x0F00) >> 8] / 10 % 10; // tens digit
		chip8.RAM[chip8.I + 2] = chip8.V[(code & 0x0F00) >> 8] / 100 % 10; // hundreds digit
	}

	// Store registers V0 through Vx in memory starting at location I.
	// The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
	else if (code >= 0xF055 && code <= 0xFF55) {
		for (uint8_t i = 0x0; i <= 0xF; i++) {
			chip8.RAM[chip8.I + i] = chip8.V[i];
		}
	}

	// Read registers V0 through Vx from memory starting at location I.
	// The interpreter reads values from memory starting at location I into registers V0 through Vx.
	else if (code >= 0xF065 && code <= 0xFF65) {
		for (uint8_t i = 0x0; i <= 0xF; i++) {
			chip8.V[i] = chip8.RAM[chip8.I + i];
		}
	}

	else printf("OP not found (%04x)", code);

	printf("\n");
}

void clearDisplay() {
	std::fill(chip8.graphics.begin(), chip8.graphics.end(), 0);
}