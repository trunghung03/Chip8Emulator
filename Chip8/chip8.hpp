#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <string>

#include "xoshiro.hpp"



constexpr auto BYTE = 2;
constexpr auto MAX_RAM = 0x1000;
constexpr auto WIDTH = 64;
constexpr auto HEIGHT = 32;
constexpr auto MAX_VRAM = WIDTH * HEIGHT;

struct Chip8 {
	std::array<uint8_t, MAX_RAM> RAM;
	std::array<uint8_t, 16> V; // General purpose register
	uint16_t I; // Memory address
	uint8_t DT; // Delay timer
	uint8_t ST; // Sound timer
	uint16_t PC; // Program counter
	uint8_t SP; // Stack pointer
	std::array<uint16_t, 16> stack; // The stack
	std::array<uint8_t, MAX_VRAM> graphics; // VRAM

	Xoshiro256 random; // Random

	Chip8() {
		RAM = {};
		V = {};
		I = 0;
		DT = 0;
		ST = 0;
		PC = 0;
		SP = 0;
		stack = {};
		graphics = {};
	}
};


void LoadROM(std::string fileName, Chip8& chip8);

void decompiler(Chip8& chip8);
void OPindex(const uint16_t& code);
void clearDisplay(Chip8& chip8);
void ROMtoRAM(uint8_t* codeBuffer, long size, Chip8 &chip8);
void emulator(Chip8& chip8);