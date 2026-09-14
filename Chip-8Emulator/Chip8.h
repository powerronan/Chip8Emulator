#pragma once
#include "Memory.h"
#include "Registers.h"
#include <chrono>
#include <random>
#include <fstream>


class Chip8
{
public:
	Memory memory;
	Registers registers;

	static constexpr int FONT_SET_SIZE = 80;

	std::uint8_t fontSet[FONT_SET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	Chip8() :
		randGen(std::chrono::system_clock::now().time_since_epoch().count())
	{
		registers.pc = memory.INTERP_OFFSET;

		for (int i = 0; i < FONT_SET_SIZE; i++)
		{
			memory.WriteMemory(memory.FONT_SIZE_START + i, fontSet[i]);
		}

		randByte = std::uniform_int_distribution<unsigned int>(0, 255);

		table[0x0] = &Chip8::Table0;
		table[0x1] = &Chip8::OP_1nnn;
		table[0x2] = &Chip8::OP_2nnn;
		table[0x3] = &Chip8::OP_3xkk;
		table[0x4] = &Chip8::OP_4xkk;
		table[0x5] = &Chip8::OP_5xy0;
		table[0x6] = &Chip8::OP_6xkk;
		table[0x7] = &Chip8::OP_7xkk;
		table[0x8] = &Chip8::Table8;
		table[0x9] = &Chip8::OP_9xy0;
		table[0xA] = &Chip8::OP_Annn;
		table[0xB] = &Chip8::OP_Bnnn;
		table[0xC] = &Chip8::OP_Cxkk;
		table[0xD] = &Chip8::OP_Dxyn;
		table[0xE] = &Chip8::TableE;
		table[0xF] = &Chip8::TableF;

		for (size_t i = 0; i <= 0xE; i++)
		{
			table0[i] = &Chip8::OP_NULL;
			table8[i] = &Chip8::OP_NULL;
			tableE[i] = &Chip8::OP_NULL;
		}

		table0[0x0] = &Chip8::OP_00E0;
		table0[0xE] = &Chip8::OP_00EE;

		table8[0x0] = &Chip8::OP_8xy0;
		table8[0x1] = &Chip8::OP_8xy1;
		table8[0x2] = &Chip8::OP_8xy2;
		table8[0x3] = &Chip8::OP_8xy3;
		table8[0x4] = &Chip8::OP_8xy4;
		table8[0x5] = &Chip8::OP_8xy5;
		table8[0x6] = &Chip8::OP_8xy6;
		table8[0x7] = &Chip8::OP_8xy7;
		table8[0xE] = &Chip8::OP_8xyE;

		tableE[0x1] = &Chip8::OP_ExA1;
		tableE[0xE] = &Chip8::OP_Ex9E;

		for (size_t i = 0; i <= 0x65; i++)
		{
			tableF[i] = &Chip8::OP_NULL;
		}

		tableF[0x07] = &Chip8::OP_Fx07;
		tableF[0x0A] = &Chip8::OP_Fx0A;
		tableF[0x15] = &Chip8::OP_Fx15;
		tableF[0x18] = &Chip8::OP_Fx18;
		tableF[0x1E] = &Chip8::OP_Fx1E;
		tableF[0x29] = &Chip8::OP_Fx29;
		tableF[0x33] = &Chip8::OP_Fx33;
		tableF[0x55] = &Chip8::OP_Fx55;
		tableF[0x65] = &Chip8::OP_Fx65;
	}

	std::default_random_engine randGen;

	std::uniform_int_distribution<unsigned int> randByte;
	void openRom(char const* filename);

	void Table0()
	{
		((*this).*(table0[registers.opcode & 0x000Fu]))();
	}

	void Table8()
	{
		((*this).*(table8[registers.opcode & 0x000Fu]))();
	}

	void TableE()
	{
		((*this).*(tableE[registers.opcode & 0x000Fu]))();
	}

	void TableF()
	{
		((*this).*(tableF[registers.opcode & 0x00FFu]))();
	}

	void OP_NULL()
	{
	}

	void Cycle();

	// Opcodes
	void OP_00E0(); // Clear display
	void OP_00EE(); // Return from Subroutine
	void OP_1nnn(); // Jump to address
	void OP_2nnn(); // Call address
	void OP_3xkk(); // Skip next instruction if Vx = kk
	void OP_4xkk(); // Skip next instruction if Vx != kk
	void OP_5xy0(); // Skip next instruction if Vx = Vy
	void OP_6xkk(); // Set Vx = kk
	void OP_7xkk(); // Set Vx = Vx + kk
	void OP_8xy0(); // Set Vx = Vy
	void OP_8xy1(); // Set Vx = Vx | Vy
	void OP_8xy2(); // Set Vx = Vx & Vy
	void OP_8xy3(); // Set Vx = Vx ^ Vy
	void OP_8xy4(); // Set Vx = Vx + Vy, set VF = carry
	void OP_8xy5(); // Set Vx = Vx - Vy, set VF = Not borrow
	void OP_8xy6(); // Set Vx = Vx SHR 1
	void OP_8xy7(); // Set Vx = Vy - Vx, set VF = Not borrow
	void OP_8xyE(); // Set Vx = Vx SHL 1
	void OP_9xy0(); // Skip next instruction if Vx != Vy
	void OP_Annn(); // Set I = nnn
	void OP_Bnnn(); // Jump to location nnn + V0
	void OP_Cxkk(); // Set Vx = random byte AND kk
	void OP_Dxyn(); // Display n-byte sprite starting at memory location I at Vx, Vy, set VF = collision
	void OP_Ex9E(); // Skip next instruction if key with the value of Vx is pressed
	void OP_ExA1(); // Skip next instruction if key with the value of Vx is NOT pressed
	void OP_Fx07(); // Set Vx = delay timer value
	void OP_Fx0A(); // Wait for a key press, store the value of the key in Vx
	void OP_Fx15(); // Set delay timer = Vx
	void OP_Fx18(); // Set sound timer = Vx
	void OP_Fx1E(); // Set I = I + Vx
	void OP_Fx29(); // Set I = location of sprite for digital Vx
	void OP_Fx33(); // Store BCD representation of Vx in memory locations I, I+1 and I+2
	void OP_Fx55(); // Store registers V0 through Vx in memory starting at location I
	void OP_Fx65(); // Read registers V0 through Vx from memory starting at location I

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1];
	Chip8Func table0[0xE + 1];
	Chip8Func table8[0xE + 1];
	Chip8Func tableE[0xE + 1];
	Chip8Func tableF[0x65 + 1];
};