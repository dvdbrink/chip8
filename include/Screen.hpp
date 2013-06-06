#pragma once

#include "gdl/sys/Window.hpp"
#include "gdl/sys/Keyboard.hpp"
#include "gdl/ogl/Context.hpp"
#include "gdl/ogl/Shader.hpp"
#include "gdl/ogl/Program.hpp"
#include "gdl/ogl/VertexBuffer.hpp"
#include "gdl/ogl/VertexArray.hpp"
#include "gdl/math/Vector2.hpp"

#include <array>
#include <unordered_map>
#include <vector>

namespace chip8
{

class Screen : public gdl::sys::Window
{
public:
	static const uint32_t width = 64;
    static const uint32_t height = 32;
    static const std::string vsStr;
    static const std::string fsStr;

public:
	Screen(const std::string&, int32_t, int32_t, uint32_t);
    virtual ~Screen();

	void clear();
	void setDirty(bool);
	bool operator()(uint32_t, uint32_t) const;
	void set(bool, uint32_t, uint32_t);
	bool isKeyDown(uint8_t);
	uint8_t waitKey();
	void update();
	void render();

private:
    gdl::ogl::Context context;
    gdl::ogl::Shader vs;
    gdl::ogl::Shader fs;
    gdl::ogl::Program prog;
    gdl::ogl::VertexBuffer vb;
    gdl::ogl::VertexArray va;
    std::vector<gdl::math::Vector2<uint32_t>> vertices;
	uint32_t scale;
	bool dirty;
	std::array<std::array<bool, height>, width> pixels;
	std::unordered_map<uint8_t, bool> keys;

private:
    uint8_t map(const gdl::sys::Keyboard::Key& key);
};

}
