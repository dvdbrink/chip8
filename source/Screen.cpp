#include "Screen.hpp"

#include "gdl/ogl/Color.hpp"
#include "gdl/math/Matrix.hpp"

#include <thread>
#include <sstream>

namespace chip8
{

const std::string Screen::vsStr = GLSL(
    uniform mat4 projection;
    in vec2 position;
    void main(void)
    {
        gl_Position = projection * mat4(1.0) * vec4(position, 0, 1);
    }
);

const std::string Screen::fsStr = GLSL(
    uniform vec4 color;
    void main(void)
    {
        gl_FragColor = color;
    }
);

Screen::Screen(const std::string& title, int32_t left, int32_t top, uint32_t scale) :
    gdl::sys::Window(title, left, top, width*scale, height*scale),
    context(handle()),
    vs(gdl::ogl::Shader::Vertex, vsStr),
    fs(gdl::ogl::Shader::Fragment, fsStr),
    prog(vs, fs),
	scale(scale),
    dirty(true)
{
    context.setVerticalSync(true);
    context.clearColor(gdl::ogl::Color::Black);

    va.attribute(vb, {prog.attribute("position"), 2, gdl::gl::UNSIGNED_INT, 0, 0});

	clear();

	keys.insert(std::make_pair(0, false));
	keys.insert(std::make_pair(1, false));
	keys.insert(std::make_pair(2, false));
	keys.insert(std::make_pair(3, false));
	keys.insert(std::make_pair(4, false));
	keys.insert(std::make_pair(5, false));
	keys.insert(std::make_pair(6, false));
	keys.insert(std::make_pair(7, false));
	keys.insert(std::make_pair(8, false));
	keys.insert(std::make_pair(9, false));
	keys.insert(std::make_pair(10, false));
	keys.insert(std::make_pair(11, false));
	keys.insert(std::make_pair(12, false));
	keys.insert(std::make_pair(13, false));
	keys.insert(std::make_pair(14, false));
	keys.insert(std::make_pair(15, false));

    connect([&](const gdl::sys::Keyboard::KeyPressed& e){
		auto key = map(e.key);
		if (key < 16)
		{
			keys[key] = true;
		}
	});

    connect([&](const gdl::sys::Keyboard::KeyReleased& e){
		auto key = map(e.key);
		if (key < 16)
		{
			keys[key] = false;
		}
	});
}

Screen::~Screen()
{

}

void Screen::clear()
{
	for (uint32_t y = 0; y < pixels.front().size(); y++)
	{
		for (uint32_t x = 0; x < pixels.size(); x++)
		{
			pixels[x][y] = false;
		}
	}
}

void Screen::setDirty(bool d)
{
	dirty = d;
}

bool Screen::operator()(uint32_t x, uint32_t y) const
{
	return pixels[x][y];
}

void Screen::set(bool pixel, uint32_t x, uint32_t y)
{
	pixels[x][y] = pixel;
}

void Screen::update()
{
	processEvents();
}

uint8_t Screen::waitKey()
{
	while (true)
	{
		for (const auto& entry : keys)
		{
			if (entry.second)
			{
				return entry.first;
			}
		}
	}
	return 0;
}

bool Screen::isKeyDown(uint8_t key)
{
	return keys[key];
}

void Screen::render()
{
	if (dirty)
	{
        context.clear();

        prog.use();
        prog.uniform("color", gdl::ogl::Color::White);
        prog.uniform("projection", gdl::math::ortho(.0f, static_cast<float>(width*scale), static_cast<float>(height*scale), .0f, .0f, 1.0f));

        vertices.clear();
        for (uint32_t y = 0; y < pixels.front().size(); y++)
        {
            for (uint32_t x = 0; x < pixels.size(); x++)
            {
                if (pixels[x][y])
                {
                    vertices.push_back({x*scale, y*scale});
                    vertices.push_back({x*scale, y*scale+scale});
                    vertices.push_back({x*scale+scale, y*scale+scale});
                    vertices.push_back({x*scale, y*scale});
                    vertices.push_back({x*scale+scale, y*scale+scale});
                    vertices.push_back({x*scale+scale, y*scale});
                }
            }
        }
        vb.data(&vertices.front(), sizeof(gdl::math::Vector2<GLuint>) * vertices.size(), gdl::ogl::BufferUsage::Dynamic);
        va.draw(vb, gdl::ogl::PrimitiveType::Triangles, 0, vertices.size());

        context.update();

		dirty = false;
	}
}

uint8_t Screen::map(const gdl::sys::Keyboard::Key& key)
{
	switch (key)
	{
        case gdl::sys::Keyboard::Key::Q: return 0;
        case gdl::sys::Keyboard::Key::W: return 1;
        case gdl::sys::Keyboard::Key::E: return 2;
        case gdl::sys::Keyboard::Key::R: return 3;
        case gdl::sys::Keyboard::Key::T: return 4;
        case gdl::sys::Keyboard::Key::Y: return 5;
        case gdl::sys::Keyboard::Key::U: return 6;
        case gdl::sys::Keyboard::Key::I: return 7;
        case gdl::sys::Keyboard::Key::O: return 8;
        case gdl::sys::Keyboard::Key::P: return 9;
        case gdl::sys::Keyboard::Key::A: return 10;
        case gdl::sys::Keyboard::Key::S: return 11;
        case gdl::sys::Keyboard::Key::D: return 12;
        case gdl::sys::Keyboard::Key::F: return 13;
        case gdl::sys::Keyboard::Key::G: return 14;
        case gdl::sys::Keyboard::Key::H: return 15;

		default: return 16;
	}
}

}
