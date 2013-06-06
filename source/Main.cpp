#include "Cpu.hpp"

#include "gdl/log/Emit.hpp"

#include <string>
#include <memory>
#include <stdexcept>

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		try
		{
			const std::string title{"Chip8"};
			const int defaultScale = 10;
            chip8::Cpu cpu{
                std::make_shared<chip8::Screen>(title, 100, 100, (argv[2] ? std::stoi(argv[2]) : defaultScale)),
                std::make_shared<chip8::Memory<4096>>(argv[1])
			};
			cpu.start();
		}
		catch (const std::invalid_argument&)
		{
            gdl::log::out<gdl::log::Error>() << "Scale argument is invalid" << gdl::log::ENDL;
			return 1;
		}
	}
	else
	{
        gdl::log::out<gdl::log::Error>() << "Missing argument(s). Expected: <string:rom> <optional:uint:scale>" << gdl::log::ENDL;
		return 1;
	}
	return 0;
}
