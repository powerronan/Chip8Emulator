#include <cstdint>
#include <array>

class Memory {
public:
    static const std::uint16_t MEM_SIZE = 0x1000;
    const std::uint16_t INTERP_OFFSET = 0x200;
    const std::uint16_t FONT_SIZE_START = 0x50;

    std::uint8_t ReadMemory(std::uint16_t address) const;
    void WriteMemory(std::uint16_t address, std::uint8_t value);
private:
    std::array<std::uint8_t, MEM_SIZE> data = {};
};