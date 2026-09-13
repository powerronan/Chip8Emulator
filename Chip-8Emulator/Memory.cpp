#include "Memory.h"

std::uint8_t Memory::ReadMemory(std::uint16_t address) const
{
    return data[address & 0xFFF];
}

void Memory::WriteMemory(std::uint16_t address, std::uint8_t value)
{
    data[address & 0xFFF] = value;
}
