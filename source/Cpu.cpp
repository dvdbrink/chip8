#include "Cpu.hpp"

#include <sstream>

namespace chip8
{

Cpu::Cpu(std::shared_ptr<Screen> screen, std::shared_ptr<Memory<4096>> memory) :
	running(false),
	delayTimer(0),
	soundTimer(0),
	indexRegister(0),
	programCounter(0x200),
	screen(screen),
	memory(memory),
    random(seed()),
    context(device),
    file("resource/sfx/ping_pong_8bit_beeep.ogg")
{
    buffer.write(file);
    source.buffer(buffer);

    screen->connect([&](const gdl::sys::Closed&){stop();});

	registerOpcode(0x00E0, 0xFFFF, [&](const Instruction&){
		screen->clear();
		programCounter += 2;
	});

	registerOpcode(0x00EE, 0xFFFF, [&](const Instruction&){
		programCounter = stack.top() + 2;
		stack.pop();
	});

	registerOpcode(0x1000, 0xF000, [&](const Instruction& instruction){
		programCounter = instruction.nnn;
	});

	registerOpcode(0x2000, 0xF000, [&](const Instruction& instruction){
		stack.push(programCounter);
		programCounter = instruction.nnn;
	});

	registerOpcode(0x3000, 0xF000, [&](const Instruction& instruction){
		if (registers[instruction.x] == instruction.nn)
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0x4000, 0xF000, [&](const Instruction& instruction){
		if (registers[instruction.x] != instruction.nn)
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0x5000, 0xF00F, [&](const Instruction& instruction){
		if (registers[instruction.x] == registers[instruction.y])
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0x6000, 0xF000, [&](const Instruction& instruction){
		registers[instruction.x] = instruction.nn;
		programCounter += 2;
	});

	registerOpcode(0x7000, 0xF000, [&](const Instruction& instruction){
		registers[instruction.x] += instruction.nn;
		programCounter += 2;
	});

	registerOpcode(0x8000, 0xF00F, [&](const Instruction& instruction){
		registers[instruction.x] = registers[instruction.y];
		programCounter += 2;
	});

	registerOpcode(0x8001, 0xF00F, [&](const Instruction& instruction){
		registers[instruction.x] |= registers[instruction.y];
		programCounter += 2;
	});

	registerOpcode(0x8002, 0xF00F, [&](const Instruction& instruction){
		registers[instruction.x] &= registers[instruction.y];
		programCounter += 2;
	});

	registerOpcode(0x8003, 0xF00F, [&](const Instruction& instruction){
		registers[instruction.x] ^= registers[instruction.y];
		programCounter += 2;
	});

	registerOpcode(0x8004, 0xF00F, [&](const Instruction& instruction){
		auto res = registers[instruction.x] + registers[instruction.y];
		registers[0xF] = res > 255;
		registers[instruction.x] = res & 0xFF;
		programCounter += 2;
	});

	registerOpcode(0x8005, 0xF00F, [&](const Instruction& instruction){
		registers[0xF] = registers[instruction.x] > registers[instruction.y];
		registers[instruction.x] -= registers[instruction.y];
		programCounter += 2;
	});

	registerOpcode(0x8006, 0xF00F, [&](const Instruction& instruction){
		registers[0xF] = registers[instruction.x] % 2;
		registers[instruction.x] >>= 1;
		programCounter += 2;
	});

	registerOpcode(0x8007, 0xF00F, [&](const Instruction& instruction){
		registers[0xF] = registers[instruction.y] > registers[instruction.x];
		registers[instruction.x] = registers[instruction.y] - registers[instruction.x];
		programCounter += 2;
	});

	registerOpcode(0x800E, 0xF00F, [&](const Instruction& instruction){
		registers[0xF] = registers[instruction.x] >> 7;
		registers[instruction.x] <<= 1;
		programCounter += 2;
	});

	registerOpcode(0x9000, 0xF00F, [&](const Instruction& instruction){
		if (registers[instruction.x] != registers[instruction.y])
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0xA000, 0xF000, [&](const Instruction& instruction){
		indexRegister = instruction.nnn;
		programCounter += 2;
	});

	registerOpcode(0xB000, 0xF000, [&](const Instruction& instruction){
		programCounter = instruction.nnn + registers[0];
	});

	registerOpcode(0xC000, 0xF000, [&](const Instruction& instruction){
		registers[instruction.x] = (random() % 256) & instruction.nn;
		programCounter += 2;
	});

	registerOpcode(0xD000, 0xF000, [&](const Instruction& instruction){
		registers[0xF] = 0;

		const auto xCoordinate = registers[instruction.x];
		const auto yCoordinate = registers[instruction.y];
		const uint8_t width = 8;
		const uint8_t height = instruction.n;

		for (uint32_t y = 0; y < height; y++)
		{
			auto pixel = memory->at(indexRegister + y);
			for (uint32_t x = 0; x < width; x++)
			{
				if ((pixel & (0x80 >> x)) != 0)
				{
					if (screen->operator()(xCoordinate+x, yCoordinate+y))
					{
						registers[0xF] = 1;
					}
					auto v = screen->operator()(xCoordinate+x, yCoordinate+y)^1;
					screen->set(v, xCoordinate+x, yCoordinate+y);
					if (v) screen->setDirty(true);
				}
			}
		}

		programCounter += 2;
	});

	registerOpcode(0xE09E, 0xF0FF, [&](const Instruction& instruction){
		if (screen->isKeyDown(registers[instruction.x]))
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0xE0A1, 0xF0FF, [&](const Instruction& instruction){
		if (!screen->isKeyDown(registers[instruction.x]))
		{
			programCounter += 2;
		}
		programCounter += 2;
	});

	registerOpcode(0xF007, 0xF0FF, [&](const Instruction& instruction){
		registers[instruction.x] = delayTimer;
		programCounter += 2;
	});

	registerOpcode(0xF00A, 0xF0FF, [&](const Instruction& instruction){
		registers[instruction.x] = screen->waitKey();
		programCounter += 2;
	});

	registerOpcode(0xF015, 0xF0FF, [&](const Instruction& instruction){
		delayTimer = registers[instruction.x];
		programCounter += 2;
	});

	registerOpcode(0xF018, 0xF0FF, [&](const Instruction& instruction){
		soundTimer = registers[instruction.x];
		programCounter += 2;
	});

	registerOpcode(0xF01E, 0xF0FF, [&](const Instruction& instruction){
		indexRegister += registers[instruction.x];
		programCounter += 2;
	});

	registerOpcode(0xF029, 0xF0FF, [&](const Instruction& instruction){
		indexRegister = registers[instruction.x] * 5;
		programCounter += 2;
	});

	registerOpcode(0xF033, 0xF0FF, [&](const Instruction& instruction){
		auto dec = registers[instruction.x];
		memory->at(indexRegister + 2) = dec % 10; // ones
		memory->at(indexRegister + 1) = (dec % 100) / 10; // tens
		memory->at(indexRegister + 0) = (dec % 1000) / 100; // hundreds
		programCounter += 2;
	});

	registerOpcode(0xF055, 0xF0FF, [&](const Instruction& instruction){
		for (uint32_t i = 0; i <= instruction.x; ++i)
		{
			memory->at(indexRegister + i) = registers[i];
		}
		programCounter += 2;
	});

	registerOpcode(0xF065, 0xF0FF, [&](const Instruction& instruction){
		for (uint32_t i = 0; i <= instruction.x; ++i)
		{
			registers[i] = memory->at(indexRegister + i);
		}
		programCounter += 2;
	});
}

void Cpu::start()
{
	running = true;
	while (running)
	{
		executeOpcode(fetchOpcode(programCounter));
	}
}

void Cpu::stop()
{
	running = false;
}

void Cpu::registerOpcode(const uint32_t opcode, const uint32_t mask, const std::function<void(const Instruction&)>&& function)
{
	opcodes.push_back({opcode, mask, std::move(function)});
}

uint32_t Cpu::fetchOpcode(const uint16_t i) const
{
	uint32_t opcode = memory->at(i);
	opcode <<= 8;
	opcode += memory->at(i+1);
	return opcode;
}

void Cpu::executeOpcode(const uint32_t instruction)
{
	for (auto& opcode : opcodes)
	{
		if ((instruction & opcode.mask) == opcode.value)
		{
			opcode({
				(instruction & 0x0F00) >> 8,
				(instruction & 0x00F0) >> 4,
				(instruction & 0x000F),
				(instruction & 0x00FF),
				(instruction & 0x0FFF)
			});

			updateTimers();

			screen->update();
			screen->render();

			return;
		}
	}

	std::stringstream message;
	message << "Unimplemented opcode: " << std::hex << instruction;
	throw std::runtime_error(message.str());
}

void Cpu::updateTimers()
{
	if (delayTimer > 0)
	{
		delayTimer--;
	}
	if (soundTimer > 0)
	{
		soundTimer--;
        source.play();
	}
}

}
