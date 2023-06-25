#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdint>
#include <cstdlib>

constexpr auto BYTE = 2;

void disassembler(uint8_t* codeBuffer, int pc);

int main(int argc, char* argv[]) 
{
    FILE* fp = fopen("logo.ch8", "rb");
    if (fp == NULL) 
    {
        puts("Error: unable to open file");
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    uint8_t* buffer = static_cast<uint8_t*>(malloc(fsize));
    if (buffer == NULL)
    {
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

void disassembler(uint8_t* codeBuffer, int pc) 
{
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

    else if (code >= 0x8000 && code <= 0x8FFF)
    {
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
    uint8_t RAM[0x1000];
    uint8_t V[16]; // General purpose register
    uint16_t I; // Memory address
    uint8_t DT; // Delay timer
    uint8_t ST; // Sound timer
    uint16_t PC; // Program counter
    uint8_t SP; // Stack pointer
    uint16_t stack[16]; // The stack

    Chip8() = default;
} chip8;


void UnimplementedInstruction(Chip8& state) {
    printf("Error: Unimplemented Instruction: %02x", state.RAM[state.PC]);
    exit(1);
}

void emulator(uint8_t* codeBuffer, int pc)
{

}