#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <print>

constexpr int WIDTH = 64;
constexpr int HEIGHT = 32;

struct Chip8 {
	uint8_t memory[4096]{};
	uint8_t V[16]{};
	uint16_t I{ 0 };
	uint16_t pc{ 0x200 };
	uint8_t display[WIDTH * HEIGHT]{};
};

std::vector<char> readBinaryFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::println(stderr, "failed to open ROM: {}", filename);
		return {};
	}

	auto fileSize = std::filesystem::file_size(filename);
	if (fileSize >= (4096 - 0x200)) {
		std::println(stderr, "ROM is too large ({} bytes)", fileSize);
		return {};
	}

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	return buffer;
}

int main(int argc, char* argv[]) {
	std::string romPath = (argc > 1) ? argv[1] : "IBM Logo.ch8";

	std::vector<char> rom = readBinaryFile(romPath);
	if (rom.empty()) {
		return 1;
	}

	Chip8 chip;
	std::copy(rom.begin(), rom.end(), &chip.memory[0x200]);

	for (int cycle = 0; cycle < 30; ++cycle) {
		if (chip.pc >= 4095) {
			std::println(stderr, "PC out of bounds: 0x{:04X}", chip.pc);
			break;
		}

		uint16_t opcode = (static_cast<uint16_t>(chip.memory[chip.pc]) << 8) | chip.memory[chip.pc + 1];
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t nn = opcode & 0x00FF;
		uint8_t y = (opcode & 0x00F0) >> 4;
		uint8_t n = opcode & 0x0F;

		switch (opcode >> 12) {
			case 0x0:
				if (opcode == 0x00E0) {
					std::fill(std::begin(chip.display), std::end(chip.display), 0);
				}
				chip.pc += 2;
				break;

			case 0x1:
				chip.pc = opcode & 0x0FFF;
				break;

			case 0x3:
				if (chip.V[x] == nn) {
					chip.pc += 2;
				}
				chip.pc += 2;
				break;

			case 0x4:
				if (chip.V[x] != nn) {
					chip.pc += 2;
				}
				chip.pc += 2;
				break;

			case 0x6:
				chip.V[x] = nn;
				chip.pc += 2;
				break;

			case 0x7:
				chip.V[x] += nn;
				chip.pc += 2;
				break;

			case 0x8:
				switch (n) {
					case 0x0:
						chip.V[x] = chip.V[y];
						break;
					case 0x1:
						chip.V[x] = chip.V[x] | chip.V[y];
						break;
					default:
						std::println(stderr, "unknown alu sub opcode: 0x{:04X}", opcode);
						break;
				}
				chip.pc += 2;
				break;

			case 0xA:
				chip.I = opcode & 0x0FFF;
				chip.pc += 2;
				break;

			case 0xD:
				chip.V[0xF] = 0;

				for (int row = 0; row < n; ++row) {
					uint8_t sprite_byte = chip.memory[chip.I + row];
					for (int col = 0; col < 8; ++col) {
						if ((sprite_byte & (0x80 >> col)) != 0) {
							int px = (chip.V[x] + col) % WIDTH;
							int py = (chip.V[y] + row) % HEIGHT;
							int screen_index = (py * WIDTH) + px;

							if (chip.display[screen_index] == 1) {
								chip.V[0xF] = 1;
							}

							chip.display[screen_index] ^= 1;
						}
					}
				}
				chip.pc += 2;
				break;

			default:
				std::println(stderr, "unknown opcode: 0x{:04X}", opcode);
				chip.pc += 2;
				break;
		}
	}

	std::println("\n--Display output--");
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			std::print("{}", chip.display[y * WIDTH + x] ? '#' : ' ');
		}
		std::println("");
	}

	return 0;
}