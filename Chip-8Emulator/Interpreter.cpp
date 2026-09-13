#include "Chip8.h"


void Chip8::openRom(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		for (int i = 0; i < size; i++)
		{
			memory.WriteMemory(0x200 + i, static_cast<uint8_t>(buffer[i]));
		}

		delete[] buffer;
	}
}

// Opcodes

void Chip8::OP_00E0()
{

}

void Chip8::OP_00EE()
{
	registers.sp--;
	registers.pc = registers.stack[registers.sp];
}

void Chip8::OP_1nnn()
{
	std::uint16_t address = registers.opcode & 0x0FFFu;

	registers.pc = address;
}

void Chip8::OP_2nnn()
{
	std::uint16_t address = registers.opcode & 0x0FFFu;

	registers.sp++;
	registers.stack[registers.sp] = registers.pc;

	registers.pc = address;
}

void Chip8::OP_3xkk()
{
	std::uint16_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint16_t kk = (registers.opcode & 0x00FFu);

	if (registers.V[x] == kk)
	{
		registers.pc += 2;
	}
}

void Chip8::OP_4xkk()
{
	std::uint16_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint16_t kk = (registers.opcode & 0x00FFu);

	if (registers.V[x] != kk)
	{
		registers.pc += 2;
	}
}

void Chip8::OP_5xy0()
{
	std::uint16_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint16_t y = (registers.opcode & 0x00F0u) >> 4;

	if (registers.V[x] == registers.V[y])
	{
		registers.pc += 2;
	}
}

void Chip8::OP_6xkk()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t kk = registers.opcode & 0x00FFu;

	registers.V[x] = kk;
}

void Chip8::OP_7xkk()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t kk = registers.opcode & 0x00FFu;

	registers.V[x] = registers.V[x] + kk;
}

void Chip8::OP_8xy0()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	registers.V[x] = registers.V[y];
}

void Chip8::OP_8xy1()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	registers.V[x] = registers.V[x] | registers.V[y];
}

void Chip8::OP_8xy2()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	registers.V[x] = registers.V[x] & registers.V[y];
}

void Chip8::OP_8xy3()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	registers.V[x] = registers.V[x] ^ registers.V[y];
}

void Chip8::OP_8xy4()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	std::uint16_t result = registers.V[x] + registers.V[y];

	if (result > 0xFF)
	{
		registers.V[0xF] = 1;
	}
	else
	{
		registers.V[0xF] = 0;
	}

	registers.V[x] = result & 0xFFu;
}

void Chip8::OP_8xy5()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	if (x > y)
	{
		registers.V[0xF] = 1;
	}
	else 
	{
		registers.V[0xF] = 0;
	}

	registers.V[x] = registers.V[x] - registers.V[y];
}

void Chip8::OP_8xy6()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.V[0xF] = (registers.V[x] & 0x1u);

	registers.V[x] = registers.V[x] >> 1;
}

void Chip8::OP_8xy7()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	if (y > x)
	{
		registers.V[0xF] = 1;
	}
	else
	{
		registers.V[0xF] = 0;
	}

	registers.V[x] = registers.V[y] - registers.V[x];
}

void Chip8::OP_8xyE()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.V[0xF] = (registers.V[x] & 0x80u) >> 7;

	registers.V[x] = registers.V[x] << 1;
}

void Chip8::OP_9xy0()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;

	if (registers.V[x] != registers.V[y])
	{
		registers.pc += 2;
	}
}

void Chip8::OP_Annn()
{
	std::uint16_t address = registers.opcode & 0x0FFFu;

	registers.I = address;
}

void Chip8::OP_Bnnn()
{
	std::uint16_t address = registers.opcode & 0x0FFFu;

	registers.pc = address + registers.V[0x0];
}

void Chip8::OP_Cxkk()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t kk = registers.opcode & 0x00FF;

	registers.V[x] = randByte(randGen) & kk;
}

void Chip8::OP_Dxyn()
{
}

void Chip8::OP_Ex9E()
{
}

void Chip8::OP_ExA1()
{
}

void Chip8::OP_Fx07()
{
}

void Chip8::OP_Fx0A()
{
}

void Chip8::OP_Fx15()
{
}

void Chip8::OP_Fx18()
{
}

void Chip8::OP_Fx1E()
{
}

void Chip8::OP_Fx29()
{
}

void Chip8::OP_Fx33()
{
}

void Chip8::OP_Fx55()
{
}

void Chip8::OP_Fx65()
{
}