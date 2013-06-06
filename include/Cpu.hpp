#pragma once

#include "Screen.hpp"
#include "Memory.hpp"

#include "gdl/oal/Device.hpp"
#include "gdl/oal/Context.hpp"
#include "gdl/oal/File.hpp"
#include "gdl/oal/Buffer.hpp"
#include "gdl/oal/Source.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <vector>
#include <stack>
#include <memory>
#include <random>

namespace chip8
{

struct Instruction
{
	const uint32_t x;
	const uint32_t y;
	const uint32_t n;
	const uint32_t nn;
	const uint32_t nnn;

	Instruction(const uint32_t x, const uint32_t y, const uint32_t n, const uint32_t nn, const uint32_t nnn) :
		x(x), y(y), n(n), nn(nn), nnn(nnn) {}
};

struct Opcode
{
	const uint32_t value;
	const uint32_t mask;
	const std::function<void(const Instruction&)> function;

	Opcode(const uint32_t value, const uint32_t mask, const std::function<void(const Instruction&)>&& function) :
		value(value), mask(mask), function(std::move(function)) {}

	inline void operator()(const Instruction& instruction)
	{
		function(instruction);
	}

	inline bool operator==(const uint32_t opcode)
	{
		return (opcode & mask) == value;
	}

	inline bool operator!=(const uint32_t opcode)
	{
		return (opcode & mask) != value;
	}
};

class Cpu
{
public:
	Cpu(std::shared_ptr<Screen>, std::shared_ptr<Memory<4096>>);

	void start();
	void stop();

private:
	void registerOpcode(const uint32_t, const uint32_t, const std::function<void(const Instruction&)>&&);
	uint32_t fetchOpcode(const uint16_t) const;
	void executeOpcode(const uint32_t);
	void updateTimers();

private:
	bool running;
	uint32_t delayTimer;
	uint32_t soundTimer;
	uint16_t indexRegister;
	uint16_t programCounter;
	std::array<uint8_t, 16> registers;
	std::stack<uint16_t> stack;
	std::vector<Opcode> opcodes;
	std::shared_ptr<Screen> screen;
	std::shared_ptr<Memory<4096>> memory;
	std::random_device seed;
	std::default_random_engine random;
    gdl::oal::Device device;
    gdl::oal::Context context;
    gdl::oal::File file;
    gdl::oal::Buffer buffer;
    gdl::oal::Source source;
};

}
