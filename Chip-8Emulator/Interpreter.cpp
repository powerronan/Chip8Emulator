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

void Chip8::Cycle()
{
	// Fetch
	registers.opcode = (memory.ReadMemory(registers.pc) << 8u) | memory.ReadMemory(registers.pc + 1);

	// Increment the PC before we execute anything
	registers.pc += 2;

	// Decode and Execute
	((*this).*(table[(registers.opcode & 0xF000u) >> 12]))();

	// Decrement the delay timer if it's been set
	if (registers.delay_timer > 0)
	{
		registers.delay_timer--;
	}

	// Decrement the sound timer if it's been set
	if (registers.sound_timer > 0)
	{
		registers.sound_timer--;
	}
}

// Opcodes

void Chip8::OP_00E0()
{
	memset(registers.video, 0, sizeof(registers.video));
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

	registers.stack[registers.sp] = registers.pc;
	registers.sp++;

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

	if (registers.V[x] > registers.V[y])
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

	if (registers.V[y] > registers.V[x])
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
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t y = (registers.opcode & 0x00F0u) >> 4;
	std::uint8_t height = registers.opcode & 0x000Fu;

	std::uint8_t xPos = registers.V[x] % 64;
	std::uint8_t yPos = registers.V[y] % 32;

	registers.V[0xF] = 0;

	for (unsigned int row = 0; row < height; row++)
	{
		std::uint8_t spriteByte = memory.ReadMemory(registers.I + row);

		for (unsigned int col = 0; col < 8; col++)
		{
			std::uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &registers.video[(yPos + row) * 64 + (xPos + col)];

			if (spritePixel)
			{
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers.V[0xF] = 1;
				}

				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t key = registers.V[x];

	if (registers.keys[key])
	{
		registers.pc += 2;
	}
}

void Chip8::OP_ExA1()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t key = registers.V[x];

	if (!registers.keys[key])
	{
		registers.pc += 2;
	}
}

void Chip8::OP_Fx07()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.V[x] = registers.delay_timer;
}

void Chip8::OP_Fx0A()
{
	uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	if (registers.keys[0])
	{
		registers.V[x] = 0;
	}
	else if (registers.keys[1])
	{
		registers.V[x] = 1;
	}
	else if (registers.keys[2])
	{
		registers.V[x] = 2;
	}
	else if (registers.keys[3])
	{
		registers.V[x] = 3;
	}
	else if (registers.keys[4])
	{
		registers.V[x] = 4;
	}
	else if (registers.keys[5])
	{
		registers.V[x] = 5;
	}
	else if (registers.keys[6])
	{
		registers.V[x] = 6;
	}
	else if (registers.keys[7])
	{
		registers.V[x] = 7;
	}
	else if (registers.keys[8])
	{
		registers.V[x] = 8;
	}
	else if (registers.keys[9])
	{
		registers.V[x] = 9;
	}
	else if (registers.keys[10])
	{
		registers.V[x] = 10;
	}
	else if (registers.keys[11])
	{
		registers.V[x] = 11;
	}
	else if (registers.keys[12])
	{
		registers.V[x] = 12;
	}
	else if (registers.keys[13])
	{
		registers.V[x] = 13;
	}
	else if (registers.keys[14])
	{
		registers.V[x] = 14;
	}
	else if (registers.keys[15])
	{
		registers.V[x] = 15;
	}
	else
	{
		registers.pc -= 2;
	}
}

void Chip8::OP_Fx15()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.delay_timer = registers.V[x];
}

void Chip8::OP_Fx18()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.sound_timer = registers.V[x];
}

void Chip8::OP_Fx1E()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	registers.I += registers.V[x];
}

void Chip8::OP_Fx29()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t hexSprite = registers.V[x];

	registers.I = memory.FONT_SIZE_START + (5 * hexSprite);
}

void Chip8::OP_Fx33()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;
	std::uint8_t BCD_Rep = registers.V[x];

	memory.WriteMemory(registers.I + 2, (BCD_Rep % 10));
	BCD_Rep /= 10;

	memory.WriteMemory(registers.I + 1, (BCD_Rep % 10));
	BCD_Rep /= 10;

	memory.WriteMemory(registers.I, (BCD_Rep % 10));
}

void Chip8::OP_Fx55()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	for (std::uint8_t i = 0; i <= x; i++)
	{
		memory.WriteMemory(registers.I + i, registers.V[x]);
	}
}

void Chip8::OP_Fx65()
{
	std::uint8_t x = (registers.opcode & 0x0F00u) >> 8;

	for (std::uint8_t i = 0; i <= x; i++)
	{
		registers.V[i] = memory.ReadMemory(registers.I + i);
	}
}