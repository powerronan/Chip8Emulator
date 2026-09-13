#include <cstdint>

class Registers {
public:
    std::uint8_t V[16] = {};        // Registers
    std::uint16_t I{};              // Special Register
    std::uint16_t pc{};             // Program Counter
    std::uint8_t sound_timer{};
    std::uint8_t delay_timer{};
    std::uint16_t sp{};             // Stack pointer
    std::uint16_t stack[16] = {};   // Stack depth
    std::uint8_t keys[16]{};        // Keypad
    std::uint16_t opcode;           // Opcode
};


