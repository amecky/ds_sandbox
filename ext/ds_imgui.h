#pragma once
#include <diesel.h>

//#define DS_IMGUI_IMPLEMENTATION

struct p2i {
	
	int x;
	int y;

	p2i() : x(0), y(0) {}
	explicit p2i(int v) : x(v), y(v) {}
	p2i(int xv, int yv) : x(xv), y(yv) {}
};

struct recti {

	int left;
	int top;
	int width;
	int height;

	recti() : left(0), top(0), width(0), height(0) {}

	recti(int l, int t, int w, int h) : left(l), top(t), width(w), height(h) {}

	ds::vec4 convert() {
		return ds::vec4(
			static_cast<float>(left),
			static_cast<float>(top),
			static_cast<float>(width),
			static_cast<float>(height)
		);
	}
};

namespace gui {

	struct IMGUISettings {
		ds::Color headerBoxColor;
		ds::Color buttonColor;
		ds::Color backgroundColor;
		ds::Color labelBoxColor;
		ds::Color enabledBoxColor;
		ds::Color disabledBoxColor;
		ds::Color activeInputBoxColor;
		ds::Color inputBoxColor;
		ds::Color boxBackgroundColor;
		ds::Color boxSelectionColor;
		ds::Color sliderColor;
		ds::Color scrollSliderColor;
		float lineSpacing;
	};

	void init(IMGUISettings* settings = 0);

	void start();

	bool begin(const char* header, int* state, int width = 200);

	bool begin(const char* header, int* state, p2i* position, int width = 200);

	void beginGroup();

	void endGroup();

	bool Button(const char* text);

	void Text(const char* text);

	void Label(const char* label, const char* text);

	void Value(const char* label, int v);

	void Value(const char* label, uint32_t v);

	void Value(const char* label, float v);

	void Value(const char* label, float v, const char* format);

	void Value(const char* label, const p2i& v);

	void Value(const char* label, bool v);

	void Value(const char* label, const ds::vec2& v);

	void Value(const char* label, const ds::vec3& v);

	void Value(const char* label, const ds::vec4& v);

	void Value(const char* label, const ds::Color& v);

	void FormattedText(const char* fmt, ...);

	void Message(const char* fmt, ...);

	bool Input(const char* label, char* str, int maxLength);

	void Input(const char* label, int* v);

	void Input(const char* label, float* v);

	void Input(const char* label, ds::vec2* v);

	void Input(const char* label, ds::vec3* v);

	void Input(const char* label, ds::vec4* v);

	void Input(const char* label, ds::Color* v);

	void StepInput(const char* label, int* v, int minValue, int maxValue, int steps);

	void Checkbox(const char* label, bool* state);

	void ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max);

	void DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection = false);

	void Slider(const char* label, int* v, int minValue, int maxValue, int width = 200);

	void Slider(const char* label, float* v, float minValue, float maxValue, int precision = 0, int width = 200);

	void SliderAngle(const char* label, float* v, int width = 200);

	void Separator();

	void Histogram(float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	void Diagram(float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	void Diagram(const char* label, float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	void pushID(int id);

	void pushID(const char* text);

	void pushID(const char* text, const char* suffix);

	void pushID(const char* text, int id);

	void popID();

	p2i getCurrentPosition();

	void moveForward(const p2i& dim);

	bool isHot(const p2i& p, const p2i& dim);

	void checkItem(const p2i& p, const p2i& dim);

	bool isClicked();

	bool isDragging();

	bool isActive();

	bool isInputActive();

	p2i textSize(const char* txt);

	p2i draw_text(const p2i& position, const char* text);

	void draw_box(const p2i& position, const p2i& dimension, const ds::Color& color);

	const IMGUISettings& getSettings();
	
	void debug();

	void end();

	void shutdown();

}

#ifdef DS_IMGUI_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#include <string.h>
#include <stdarg.h>

const unsigned char DS_IMGUI_FONT[128][16] = {
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x60, 0x06, 0x1B, 0x3C, 0x6C, 0xC0, 0x01, 0x06, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0xF0, 0x60, 0x06, 0x1B, 0x66, 0x6C, 0x61, 0x03, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0xF0, 0x60, 0x86, 0x3F, 0x06, 0xB8, 0x61, 0x03, 0x06, 0x18, 0x60, 0xC0, 0x06, 0x06, 0x00 },
	{ 0x00, 0xF0, 0x00, 0x00, 0x1B, 0x0C, 0xC0, 0xC0, 0x01, 0x00, 0x0C, 0xC0, 0x80, 0x03, 0x06, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x18, 0x60, 0x60, 0x00, 0x00, 0x0C, 0xC0, 0xE0, 0x8F, 0x1F, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x30, 0x30, 0x60, 0x0F, 0x00, 0x0C, 0xC0, 0x80, 0x03, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x80, 0x3F, 0x60, 0xD8, 0x61, 0x06, 0x00, 0x0C, 0xC0, 0xC0, 0x06, 0x06, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x66, 0x68, 0x63, 0x06, 0x00, 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x00, 0x00, 0x1B, 0x3C, 0x60, 0xC3, 0x0D, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xC0, 0x01, 0x00, 0x00, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x18, 0x78, 0xC0, 0xC0, 0x03, 0x0F, 0x0C, 0xF8, 0x81, 0x83, 0x1F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x18, 0xCC, 0xE0, 0x60, 0x86, 0x19, 0x0C, 0x18, 0x80, 0x01, 0x18, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0xEC, 0xF8, 0x60, 0x86, 0x19, 0x6C, 0x18, 0xC0, 0x00, 0x0C, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0xEC, 0xC0, 0x00, 0x06, 0x18, 0x6C, 0x18, 0xE0, 0x03, 0x0C, 0x00 },
	{ 0x00, 0xF8, 0x01, 0x00, 0x06, 0xCC, 0xC0, 0x00, 0x03, 0x0E, 0x6C, 0xF8, 0x60, 0x06, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x06, 0xDC, 0xC0, 0x80, 0x01, 0x18, 0x66, 0x80, 0x61, 0x06, 0x06, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x03, 0xDC, 0xC0, 0xC0, 0x80, 0x19, 0xFE, 0x80, 0x61, 0x06, 0x03, 0x00 },
	{ 0x38, 0x00, 0x80, 0x03, 0x03, 0xCC, 0xC0, 0x60, 0x80, 0x19, 0x60, 0xC0, 0x60, 0x06, 0x03, 0x00 },
	{ 0x38, 0x00, 0x80, 0x83, 0x01, 0x78, 0xC0, 0xE0, 0x07, 0x0F, 0x60, 0x78, 0xC0, 0x03, 0x03, 0x00 },
	{ 0x30, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3C, 0xF0, 0x00, 0x00, 0x00, 0x60, 0x00, 0x60, 0x00, 0x0F, 0x7E, 0x60, 0xE0, 0x03, 0x0F, 0x00 },
	{ 0x66, 0x98, 0x01, 0x00, 0x00, 0x30, 0x00, 0xC0, 0x80, 0x19, 0xC3, 0xF0, 0x60, 0x86, 0x19, 0x00 },
	{ 0x66, 0x98, 0x81, 0x03, 0x0E, 0x18, 0x00, 0x80, 0x81, 0x19, 0xC3, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x6E, 0x98, 0x81, 0x03, 0x0E, 0x0C, 0xF8, 0x01, 0x03, 0x0C, 0xF3, 0x98, 0x61, 0x86, 0x01, 0x00 },
	{ 0x3C, 0x98, 0x01, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x06, 0xDB, 0x98, 0xE1, 0x83, 0x01, 0x00 },
	{ 0x76, 0xF0, 0x01, 0x00, 0x00, 0x0C, 0xF8, 0x01, 0x03, 0x06, 0xDB, 0xF8, 0x61, 0x86, 0x01, 0x00 },
	{ 0x66, 0xC0, 0x00, 0x00, 0x00, 0x18, 0x00, 0x80, 0x01, 0x00, 0xF3, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x66, 0x60, 0x80, 0x03, 0x0E, 0x30, 0x00, 0xC0, 0x00, 0x06, 0x03, 0x98, 0x61, 0x86, 0x19, 0x00 },
	{ 0x3C, 0x70, 0x80, 0x03, 0x0E, 0x60, 0x00, 0x60, 0x00, 0x06, 0xFE, 0x98, 0xE1, 0x03, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1E, 0xF8, 0xE1, 0x07, 0x0F, 0x66, 0xF0, 0x00, 0x86, 0x19, 0x06, 0x18, 0x63, 0x0C, 0x0F, 0x00 },
	{ 0x36, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x00, 0x86, 0x19, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0xB8, 0xE3, 0x8C, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x01, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0x58, 0xE3, 0x8D, 0x19, 0x00 },
	{ 0x66, 0xF8, 0xE0, 0x83, 0x01, 0x7E, 0x60, 0x00, 0x86, 0x07, 0x06, 0x58, 0x63, 0x8F, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x1D, 0x66, 0x60, 0x00, 0x86, 0x0D, 0x06, 0x58, 0x63, 0x8E, 0x19, 0x00 },
	{ 0x66, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x60, 0x86, 0x0D, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x36, 0x18, 0x60, 0x80, 0x19, 0x66, 0x60, 0x60, 0x86, 0x19, 0x06, 0x18, 0x63, 0x8C, 0x19, 0x00 },
	{ 0x1E, 0xF8, 0x61, 0x00, 0x1F, 0x66, 0xF0, 0xC0, 0x83, 0x19, 0x7E, 0x18, 0x63, 0x0C, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3E, 0xF0, 0xE0, 0x03, 0x0F, 0x7E, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0xE1, 0x07, 0x0F, 0x00 },
	{ 0x66, 0x98, 0x61, 0x86, 0x19, 0x18, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0x01, 0x06, 0x03, 0x00 },
	{ 0x66, 0x98, 0x61, 0x86, 0x01, 0x18, 0x98, 0x61, 0x86, 0x31, 0x2C, 0x98, 0x01, 0x06, 0x03, 0x00 },
	{ 0x66, 0x98, 0x61, 0x06, 0x03, 0x18, 0x98, 0x61, 0x86, 0x35, 0x18, 0x98, 0x01, 0x03, 0x03, 0x00 },
	{ 0x3E, 0x98, 0xE1, 0x03, 0x06, 0x18, 0x98, 0x61, 0x86, 0x35, 0x18, 0xF0, 0x80, 0x01, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x03, 0x0C, 0x18, 0x98, 0x61, 0x86, 0x35, 0x34, 0x60, 0xC0, 0x00, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x06, 0x18, 0x18, 0x98, 0x61, 0x06, 0x1B, 0x66, 0x60, 0x60, 0x00, 0x03, 0x00 },
	{ 0x06, 0x98, 0x61, 0x86, 0x19, 0x18, 0x98, 0xC1, 0x03, 0x1B, 0x66, 0x60, 0x60, 0x00, 0x03, 0x00 },
	{ 0x06, 0xF0, 0x60, 0x06, 0x0F, 0x18, 0xF0, 0x80, 0x01, 0x1B, 0x66, 0x60, 0xE0, 0x07, 0x03, 0x00 },
	{ 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 },
	{ 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x80, 0x01, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xC0, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0xF0, 0x60, 0x06, 0x00, 0x30, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0x80, 0x07, 0x00, 0x00 },
	{ 0x06, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x60, 0x00, 0xC0, 0x00, 0x00, 0x00 },
	{ 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xE0, 0x03, 0x0F, 0x7C, 0xF0, 0xC0, 0x00, 0x1F, 0x00 },
	{ 0x0C, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x61, 0x86, 0x19, 0x66, 0x98, 0xC1, 0x80, 0x19, 0x00 },
	{ 0x18, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x61, 0x86, 0x01, 0x66, 0x98, 0xE1, 0x87, 0x19, 0x00 },
	{ 0x18, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x61, 0x86, 0x01, 0x66, 0xF8, 0xC1, 0x80, 0x19, 0x00 },
	{ 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x98, 0x61, 0x86, 0x01, 0x66, 0x18, 0xC0, 0x80, 0x19, 0x00 },
	{ 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x98, 0x61, 0x86, 0x19, 0x66, 0x18, 0xC0, 0x80, 0x19, 0x00 },
	{ 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xE1, 0x03, 0x0F, 0x7C, 0xF0, 0xC0, 0x00, 0x1F, 0x00 },
	{ 0x60, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00 },
	{ 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00 },
	{ 0x00, 0xF0, 0x00, 0xC0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x60, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0x60, 0x00, 0x83, 0x01, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0x00, 0x00, 0x80, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3E, 0x78, 0xC0, 0x83, 0x19, 0x18, 0xF8, 0xE1, 0x03, 0x0F, 0x3E, 0xF0, 0x61, 0x06, 0x1F, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x19, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x87, 0x01, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x0D, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0xE1, 0x80, 0x01, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x07, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x0F, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x0D, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x18, 0x00 },
	{ 0x66, 0x60, 0x00, 0x83, 0x19, 0x18, 0x58, 0x63, 0x86, 0x19, 0x66, 0x98, 0x61, 0x00, 0x18, 0x00 },
	{ 0x66, 0xF8, 0x01, 0x83, 0x19, 0x7E, 0x18, 0x63, 0x06, 0x0F, 0x3E, 0xF0, 0x61, 0x80, 0x0F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x80, 0x01, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x30, 0xE0, 0x88, 0x1F, 0x00 },
	{ 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x18, 0x60, 0xB0, 0x8D, 0x1F, 0x00 },
	{ 0x7E, 0x98, 0x61, 0x86, 0x31, 0x66, 0x98, 0xE1, 0x07, 0x06, 0x18, 0x60, 0x10, 0x87, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x66, 0x98, 0x01, 0x06, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x3C, 0x98, 0x01, 0x03, 0x03, 0x18, 0xC0, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x18, 0x98, 0x81, 0x81, 0x01, 0x18, 0x80, 0x01, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0x61, 0x86, 0x35, 0x3C, 0x98, 0xC1, 0x00, 0x03, 0x18, 0xC0, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x0C, 0x98, 0xC1, 0x03, 0x1B, 0x66, 0x98, 0x61, 0x00, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x78, 0xF0, 0x81, 0x01, 0x1B, 0x66, 0xF0, 0xE0, 0x07, 0x06, 0x18, 0x60, 0x00, 0x80, 0x1F, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x06, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x0C, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

namespace gui {

	typedef uint32_t HashedId;

	static HashedId NULL_HASH = 0;

	const uint32_t FNV_Prime = 0x01000193; //   16777619
	const uint32_t FNV_Seed = 0x811C9DC5; // 2166136261

	inline HashedId fnv1a(const char* text, uint32_t hash = FNV_Seed) {
		const unsigned char* ptr = (const unsigned char*)text;
		while (*ptr) {
			hash = (*ptr++ ^ hash) * FNV_Prime;
		}
		return hash;
	}

	const static recti INPUT_RECT = recti(0, 128, 150, 20);
	const static recti WHITE_RECT = recti(0, 128, 128, 128);

	namespace renderer {

		enum ResizeType {
			RT_NONE,
			RT_X,
			RT_Y,
			RT_BOTH
		};

		struct UIBuffer {
			ds::vec2* positions;
			ds::vec2* sizes;
			recti* rectangles;
			ds::vec2* scales;
			ds::Color* colors;
			ResizeType* resize;
			uint16_t num;
			uint16_t capacity;
			char* data;
		};

		struct UIContext {
			UIBuffer* buffer;
			SpriteBatchBuffer* sprites;
			p2i startPos;
			p2i size;
			char tmpBuffer[256];
		};

		UIContext* init() {
			UIContext* ctx = new UIContext;
			ctx->buffer = new UIBuffer;
			int sz = 4096 * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(recti) + sizeof(ds::vec2) + sizeof(ds::Color) + sizeof(ResizeType));
			ctx->buffer->num = 0;
			ctx->buffer->capacity = 4096;
			ctx->buffer->data = new char[sz];
			ctx->buffer->positions = (ds::vec2*)(ctx->buffer->data);
			ctx->buffer->sizes = (ds::vec2*)(ctx->buffer->positions + ctx->buffer->capacity);
			ctx->buffer->rectangles = (recti*)(ctx->buffer->sizes + ctx->buffer->capacity);
			ctx->buffer->scales = (ds::vec2*)(ctx->buffer->rectangles + ctx->buffer->capacity);
			ctx->buffer->colors = (ds::Color*)(ctx->buffer->scales + ctx->buffer->capacity);
			ctx->buffer->resize = (ResizeType*)(ctx->buffer->colors + ctx->buffer->capacity);

			uint8_t* data = new uint8_t[256 * 256 * 4];
			for (int i = 0; i < 256 * 256 * 4; ++i) {
				data[i] = 255;
			}
			int index = 0;
			for (int y = 0; y < 128; ++y) {
				for (int x = 0; x < 16; ++x) {
					uint8_t t = DS_IMGUI_FONT[y][x];
					index = x * 32 + y * 256 * 4;
					for (int z = 0; z < 8; ++z) {
						if ((t >> z) & 1) {
							data[index++] = 255;
							data[index++] = 255;
							data[index++] = 255;
							data[index++] = 255;
						}
						else {
							data[index++] = 0;
							data[index++] = 0;
							data[index++] = 0;
							data[index++] = 0;
						}
					}
				}
			}

			ds::TextureInfo texInfo = { 256, 256, 4, data, ds::TextureFormat::R8G8B8A8_UNORM, ds::BindFlag::BF_SHADER_RESOURCE };
			RID textureID = ds::createTexture(texInfo);
			SpriteBatchBufferInfo sbbInfo = { 4096,textureID, ds::TextureFilters::POINT };
			ctx->sprites = new SpriteBatchBuffer(sbbInfo);
			delete[] data;

			return ctx;
		}

		void reset(UIContext* ctx) {
			ctx->buffer->num = 0;
			ctx->size = p2i(0);
		}

		// --------------------------------------------------------
		// Extract the name. If the text contains # it will be
		// cut off at that position. 
		// --------------------------------------------------------
		static const char* extract_name(UIContext* ctx, const char* t) {
			strncpy(ctx->tmpBuffer, t, 256);
			int l = strlen(t);
			for (int i = 0; i < l; ++i) {
				if (ctx->tmpBuffer[i] == '#') {
					ctx->tmpBuffer[i] = '\0';
				}
			}
			return ctx->tmpBuffer;
		}

		void draw_buffer(UIContext* ctx) {
			ctx->sprites->begin();
			float h = ctx->size.y - 20.0f;
			float sy = h / 128.0f;
			float w = ctx->size.x + 10.0f;
			float sx = w / 128.0f;

			float bpx = ctx->startPos.x + w * 0.5f;
			float bpy = ctx->startPos.y - h * 0.5f - 10.0f;

			int num = ctx->buffer->num;
			for (uint16_t i = 0; i < num; ++i) {
				ds::vec2 p = ctx->buffer->positions[i];
				if (ctx->buffer->resize[i] == ResizeType::RT_X) {
					recti rect = ctx->buffer->rectangles[i];
					rect.width = 128;
					p.x = bpx;
					ctx->sprites->add(p, rect.convert(), ds::vec2(sx, 1.0f), 0.0f, ctx->buffer->colors[i]);
				}
				else if (ctx->buffer->resize[i] == ResizeType::RT_Y) {
					recti rect = ctx->buffer->rectangles[i];
					rect.height = 128;
					p.y = bpy;
					ctx->sprites->add(p, rect.convert(), ds::vec2(sx, sy), 0.0f, ctx->buffer->colors[i]);
				}
				else if (ctx->buffer->resize[i] == ResizeType::RT_BOTH) {
					recti rect = ctx->buffer->rectangles[i];
					rect.width = 128;
					rect.height = 128;
					p.x = bpx;
					p.y = bpy;
					ctx->sprites->add(p, rect.convert(), ds::vec2(sx, sy), 0.0f, ctx->buffer->colors[i]);
				}
				else {
					ctx->sprites->add(p, ctx->buffer->rectangles[i].convert(), ctx->buffer->scales[i], 0.0f, ctx->buffer->colors[i]);
				}
			}
			ctx->buffer->num = 0;
			ctx->sprites->flush();
		}

		void add_to_buffer(UIContext* ctx, const p2i& p, const recti& rect, const ds::vec2& scale, const ds::Color& color, ResizeType resize = ResizeType::RT_NONE) {
			if ((ctx->buffer->num + 1) >= ctx->buffer->capacity) {
				draw_buffer(ctx);
			}
			ctx->buffer->positions[ctx->buffer->num] = ds::vec2(p.x, p.y);
			ctx->buffer->scales[ctx->buffer->num] = scale;
			ctx->buffer->rectangles[ctx->buffer->num] = rect;
			ctx->buffer->colors[ctx->buffer->num] = color;
			ctx->buffer->resize[ctx->buffer->num] = resize;
			++ctx->buffer->num;
		}

		// --------------------------------------------------------
		// internal add text method
		// --------------------------------------------------------
		p2i add_text(UIContext* ctx, const p2i& pos, const char* text, int xoffset = 10, const ds::Color& color = ds::Color(1.0f, 1.0f, 1.0f, 1.0f)) {
			const char* lbl = extract_name(ctx, text);
			int l = strlen(lbl);
			p2i p = pos;
			p.x += xoffset;
			int w = 0;
			p2i dim(0);
			for (int i = 0; i < l; ++i) {
				int idx = (int)lbl[i] - 32;
				if (idx >= 0 && idx < 127) {
					int c = lbl[i] - 32;
					int t = c / 12;
					int y = t * 16;
					int x = (c - t * 12) * 10;
					recti rect = recti(x, y, 8, 14);
					add_to_buffer(ctx, p, rect, ds::vec2(1, 1), color, ResizeType::RT_NONE);
					p.x += rect.width;
					w += rect.width;
					dim.x += rect.width;
					if (rect.height > dim.y) {
						dim.y = rect.height;
					}
				}
			}
			if (w > ctx->size.x) {
				ctx->size.x = w;
			}
			return dim;
		}

		// --------------------------------------------------------
		// internal add box
		// --------------------------------------------------------
		void add_box(UIContext* ctx, const p2i& p, const p2i& size, const ds::Color& color, ResizeType resize = ResizeType::RT_NONE) {
			p2i pos = p;
			pos.x += size.x / 2;
			ds::vec2 scale = ds::vec2(1.0f, 1.0f);
			p2i sz = size;
			if (size.x > WHITE_RECT.width) {
				sz.x = WHITE_RECT.width;
				scale.x = static_cast<float>(size.x) / static_cast<float>(WHITE_RECT.width);
			}
			if (size.y > WHITE_RECT.height) {
				sz.y = WHITE_RECT.height;
				scale.y = static_cast<float>(size.y) / static_cast<float>(WHITE_RECT.height);
			}
			add_to_buffer(ctx, pos, recti(256, 0, sz.x, sz.y), scale, color, resize);
			if (size.x > ctx->size.x) {
				ctx->size.x = size.x;
			}
		}

		void add_box(UIContext* ctx, const p2i& p, int width, int height, const ds::Color& color, ResizeType resize = ResizeType::RT_NONE) {
			add_box(ctx, p, p2i(width, height), color, resize);
		}

		void shutdown(UIContext* ctx) {
			delete ctx->sprites;
			delete[] ctx->buffer->data;
			delete ctx->buffer;
		}

	}
	static const IMGUISettings DEFAULT_SETTINGS = {
		ds::Color(40,117,114, 255), // header
		ds::Color(68, 68, 68, 255), // button
		ds::Color(16, 16, 16, 255), // background
		ds::Color(32, 32, 32, 255), // label 
		ds::Color(0, 192, 0, 255), // enabled
		ds::Color(192, 0, 0, 255), // disabled
		ds::Color(64,64,64,255), // active input
		ds::Color(32,32,32,255), // input
		ds::Color(38,38,38,255), // box background color
		ds::Color(167,77,75,255), // box selection color
		ds::Color(96,96,96,255), // slider color
		ds::Color(255,0,255,255), // scroll slider color
		5.0f // line spacing
	};

	// ----------------------------------------------------------
	// Very simple internal HashedId stack.
	// ----------------------------------------------------------
	static const int STACK_MAX_ENTRIES = 64;
	static const char STACK_DELIMITER = '#';

	struct IDStack {

		HashedId ids[STACK_MAX_ENTRIES];
		uint16_t top;
		char _tmp[256];
		char buffer[256];
		uint16_t index;

		IDStack() {
			reset();
		}

		void reset() {
			index = 0;
			buffer[0] = '\0';
			top = 0;
			for (int i = 0; i < STACK_MAX_ENTRIES; ++i) {
				ids[i] = 0;
			}
		}

		void push(int id) {
			if (top < STACK_MAX_ENTRIES) {
				sprintf_s(_tmp, 256, "%d", id);
				append(_tmp);
				ids[++top] = fnv1a(buffer);
			}
		}

		void push(const char* text) {
			if (top < STACK_MAX_ENTRIES) {
				append(text);
				ids[++top] = fnv1a(buffer);
			}
		}

		void push(const char* text, const char* suffix) {
			if (top < STACK_MAX_ENTRIES) {
				sprintf_s(_tmp, 256, "%s%s", text, suffix);
				append(_tmp);
				ids[++top] = fnv1a(buffer);
			}
		}

		void push(const char* text, int id) {
			if (top < STACK_MAX_ENTRIES) {
				sprintf_s(_tmp, 256, "%s%d", text, id);
				append(_tmp);
				ids[++top] = fnv1a(buffer);
			}
		}

		void push(HashedId id) {
			if (top < STACK_MAX_ENTRIES) {
				ids[top++] = id;
			}
		}

		bool empty() {
			return top == 0;
		}

		HashedId last() {
			return ids[top];
		}

		void pop() {
			if (index > 0) {
				bool found = false;
				for (int i = index; i > 0; --i) {
					if (buffer[i] == STACK_DELIMITER && buffer[i - 1] == STACK_DELIMITER && !found) {
						buffer[i - 1] = '\0';
						found = true;
						index = i - 1;
					}
				}
				if (!found) {
					index = 0;
					buffer[0] = '\0';
				}
			}
			if (top > 0) {
				--top;
			}
		}

		void append(const char* text) {
			if (index != 0) {
				buffer[index++] = STACK_DELIMITER;
				buffer[index++] = STACK_DELIMITER;
			}
			const char* p = text;
			while (*p != '\0') {
				if (index < 256) {
					buffer[index++] = *p;
				}
				++p;
			}
			buffer[index] = '\0';
		}
	};


	struct GUIContext {
		p2i mousePosition;
		bool mouseDown;
		bool clicked;
		HashedId activeItem;
		HashedId hotItem;
		HashedId inputItem;
		int keyInput[256];
		int keys[32];
		int numKeys;
		p2i startPos;
		p2i currentPos;
		bool grouping;
		char inputText[32];
		char tmpBuffer[256];
		int caretPos;
		p2i size;
		IMGUISettings settings;
		renderer::UIContext* uiContext;
		IDStack idStack;
		
	};

	static GUIContext* _guiCtx = 0;

	// -------------------------------------------------------
	// methods for state checking
	// -------------------------------------------------------

	// -------------------------------------------------------
	// check if mouse cursor is inside box
	// -------------------------------------------------------
	bool isHot(const p2i& p, const p2i& dim) {
		p2i mp = _guiCtx->mousePosition;
		int half_y = dim.y / 2;
		if (mp.x < p.x) {
			return false;
		}
		if (mp.x > (p.x + dim.x)) {
			return false;
		}
		if (mp.y < (p.y - half_y)) {
			return false;
		}
		if (mp.y > (p.y + half_y)) {
			return false;
		}
		return true;
	}

	// -------------------------------------------------------
	// check item if is hot and becomes active
	// -------------------------------------------------------
	void checkItem(const p2i& p, const p2i& dim) {
		HashedId id = _guiCtx->idStack.last();
		if (isHot(p2i(p.x, p.y), dim)) {
			_guiCtx->hotItem = id;
			if (_guiCtx->activeItem == 0 && _guiCtx->mouseDown) {
				_guiCtx->activeItem = id;
				_guiCtx->clicked = true;
			}			
		}
	}

	// -------------------------------------------------------
	// check if clicked
	// -------------------------------------------------------
	bool isClicked() {
		HashedId id = _guiCtx->idStack.last();
		if (_guiCtx->clicked && _guiCtx->hotItem == id && _guiCtx->activeItem == id) {
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// check if dragging
	// -------------------------------------------------------
	bool isDragging() {
		HashedId id = _guiCtx->idStack.last();
		return (_guiCtx->mouseDown && _guiCtx->activeItem == id && _guiCtx->hotItem == id);
	}

	// -------------------------------------------------------
	// check if item is active
	// -------------------------------------------------------
	bool isActive() {
		HashedId id = _guiCtx->idStack.last();
		return _guiCtx->activeItem == id;
	}

	// -------------------------------------------------------
	// check if input item is active
	// -------------------------------------------------------
	bool isInputActive() {
		HashedId id = _guiCtx->idStack.last();
		return _guiCtx->inputItem == id;
	}
	
	// -------------------------------------------------------
	// determine text size
	// -------------------------------------------------------
	p2i textSize(const char* txt) {
		int l = strlen(txt);
		p2i p(0);
		for (int i = 0; i < l; ++i) {
			int idx = (int)txt[i] - 32;
			if (idx >= 0 && idx < 127) {
				int c = idx / 16;
				int r = c - idx * 16;
				ds::vec4 rect = ds::vec4(r, c, 7, 10);
				p.x += rect.z;
				if (rect.w > p.y) {
					p.y = rect.w;
				}
			}
		}
		return p;
	}

	// -------------------------------------------------------
	// determine text size
	// -------------------------------------------------------
	p2i limitedTextSize(const char* txt, int maxLength) {
		int l = strlen(txt);
		if (l > maxLength) {
			l = maxLength;
		}
		p2i p(0);
		for (int i = 0; i < l; ++i) {
			int idx = (int)txt[i] - 32;
			if (idx >= 0 && idx < 127) {
				int c = idx / 16;
				int r = c - idx * 16;
				ds::vec4 rect = ds::vec4(r, c, 7, 10);
				p.x += rect.z;
				if (rect.w > p.y) {
					p.y = rect.w;
				}
			}
		}
		return p;
	}

	// -------------------------------------------------------
	// initialize GUI
	// -------------------------------------------------------
	void init(IMGUISettings* settings) {
		_guiCtx = new GUIContext;	
		_guiCtx->uiContext = renderer::init();
		for (int i = 0; i < 255; ++i) {
			_guiCtx->keyInput[i] = 0;
		}		
		_guiCtx->grouping = false;
		_guiCtx->numKeys = 0;
		if (settings != 0) {
			_guiCtx->settings = *settings;
		}
		else {
			_guiCtx->settings = DEFAULT_SETTINGS;
		}
		_guiCtx->hotItem = 0;
		_guiCtx->activeItem = 0;
		_guiCtx->inputItem = 0;
	}

	// --------------------------------------------------------
	// move current position to next position
	// --------------------------------------------------------
	void moveForward(const p2i& dim) {
		if (_guiCtx->grouping) {
			_guiCtx->currentPos.x += dim.x + 10;
		}
		else {
			_guiCtx->currentPos.y -= dim.y;
			_guiCtx->size.y += dim.y;
			_guiCtx->currentPos.x = _guiCtx->startPos.x;
		}
	}

	// -------------------------------------------------------
	// handle text input
	// -------------------------------------------------------
	// FIXME: provide numeric input only
	static bool handleTextInput(bool numeric = false) {
		bool ret = false;
		int len = strlen(_guiCtx->inputText);
		for (int i = 0; i < ds::getNumInputKeys(); ++i) {
			const ds::InputKey& key = ds::getInputKey(i);
			if (key.type == ds::IKT_SYSTEM) {
				if (key.value == ds::SpecialKeys::DSKEY_Backspace) {
					if (_guiCtx->caretPos > 0) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos - 1, _guiCtx->inputText + _guiCtx->caretPos, len - _guiCtx->caretPos);
						}
						--_guiCtx->caretPos;
						--len;
						_guiCtx->inputText[len] = '\0';

					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_LeftArrow) {
					if (_guiCtx->caretPos > 0) {
						--_guiCtx->caretPos;
					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_RightArrow) {
					if (_guiCtx->caretPos < strlen(_guiCtx->inputText)) {
						++_guiCtx->caretPos;
					}
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Home) {
					_guiCtx->caretPos = 0;
				}
				else if (key.value == ds::SpecialKeys::DSKEY_End) {
					_guiCtx->caretPos = strlen(_guiCtx->inputText);
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Enter) {
					_guiCtx->activeItem = 0;
					_guiCtx->inputItem = 0;
					ret = true;
				}
				else if (key.value == ds::SpecialKeys::DSKEY_ESC) {
					_guiCtx->activeItem = 0;
					_guiCtx->inputItem = 0;
					ret = false;
				}
				else if (key.value == ds::SpecialKeys::DSKEY_Delete) {
					if (len > 0) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos, _guiCtx->inputText + _guiCtx->caretPos + 1, len - _guiCtx->caretPos);
							--len;
							_guiCtx->inputText[len] = '\0';
						}
					}
				}
			}
			else {
				if ((key.value > 47 && key.value < 128) || key.value == '.' || key.value == '-') {
					if (len < 32) {
						if (_guiCtx->caretPos < len) {
							memmove(_guiCtx->inputText + _guiCtx->caretPos + 1, _guiCtx->inputText + _guiCtx->caretPos, len - _guiCtx->caretPos);
						}
						if (numeric) {
							if ((key.value > 47 && key.value < 58) || key.value == '.' || key.value == '-') {
								_guiCtx->inputText[_guiCtx->caretPos] = key.value;
								++len;
								++_guiCtx->caretPos;
							}
						}
						else {
							_guiCtx->inputText[_guiCtx->caretPos] = key.value;
							++len;
							++_guiCtx->caretPos;
						}
					}
				}
			}
			_guiCtx->inputText[len] = '\0';
		}
		//++len;		
		return ret;
	}
	// -------------------------------------------------------
	// input scalar
	// -------------------------------------------------------
	static bool InputScalar(int index, char* v, int maxLength, int width = 150) {
		bool ret = false;
		p2i p = _guiCtx->currentPos;
		checkItem(p, p2i(width, 20));
		if (isClicked()) {
			sprintf_s(_guiCtx->inputText, 32, "%s", v);
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			_guiCtx->inputItem = _guiCtx->idStack.last();
			_guiCtx->activeItem = _guiCtx->idStack.last();
		}
		if (isInputActive()) {
			renderer::add_box(_guiCtx->uiContext, p, p2i(width, 16), _guiCtx->settings.activeInputBoxColor);
			if (handleTextInput(false)) {
				strncpy(v, _guiCtx->inputText, maxLength);
			}
			p2i textDim = textSize(_guiCtx->inputText);
			p2i cp = p;
			p2i cursorPos = limitedTextSize(_guiCtx->inputText, _guiCtx->caretPos);
			cp.x += (width - textDim.x) /2 + cursorPos.x - 2;
			cp.y -= 6;
			renderer::add_box(_guiCtx->uiContext, cp, p2i(10, 3), ds::Color(192, 0, 192, 255));
			p.x += (width - textDim.x) / 2;
			p.y -= 1;
			renderer::add_text(_guiCtx->uiContext, p, _guiCtx->inputText, 0);
			sprintf_s(v, maxLength, "%s", _guiCtx->inputText);
		}
		else {
			sprintf_s(_guiCtx->tmpBuffer, 64, "%s", v);
			renderer::add_box(_guiCtx->uiContext, p, p2i(width, 16), _guiCtx->settings.inputBoxColor);
			p2i textDim = textSize(_guiCtx->tmpBuffer);
			p.y -= 1;
			p.x += (width - textDim.x) / 2;
			renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer, 0);
		}
		return ret;
	}

	// -------------------------------------------------------
	// internal input float
	// -------------------------------------------------------
	static bool InputScalar(int index, float* v, const char* format = "%g", int width = 100) {
		bool ret = false;
		p2i p = _guiCtx->currentPos;
		if (index > 0) {
			p.x += index * width + index * 10;
		}
		checkItem(p, p2i(width, 20));
		bool selected = isHot(p, p2i(width, 20));
		if (isClicked()) {
			sprintf_s(_guiCtx->inputText, 32, format, *v);
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			_guiCtx->activeItem = _guiCtx->idStack.last();
			_guiCtx->inputItem = _guiCtx->idStack.last();
		}
		if (isInputActive()) {
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			// draw box
			renderer::add_box(_guiCtx->uiContext, p, width, 16,_guiCtx->settings.activeInputBoxColor);
			// handle text input
			ret = handleTextInput(true);
			if (ret) {
				*v = static_cast<float>(atof(_guiCtx->inputText));
			}
			p2i textDim = textSize(_guiCtx->inputText);
			p2i cp = p;
			// draw cursor
			p2i cursorPos = limitedTextSize(_guiCtx->inputText, _guiCtx->caretPos);
			cp.x += (width - textDim.x) / 2 + cursorPos.x - 2;
			cp.y -= 6;
			renderer::add_box(_guiCtx->uiContext, cp, p2i(10, 3), ds::Color(192, 0, 192, 255));
			// draw input text
			p.x += (width - textDim.x) / 2;
			p.y -= 1;
			renderer::add_text(_guiCtx->uiContext, p, _guiCtx->inputText, 0);
		}
		else {
			sprintf_s(_guiCtx->tmpBuffer, 64, format, *v);
			p2i textDim = textSize(_guiCtx->tmpBuffer);
			renderer::add_box(_guiCtx->uiContext, p, width, 16, _guiCtx->settings.inputBoxColor);
			p.y -= 1;
			p.x += (width - textDim.x) / 2;
			renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer, 0);
		}
		return ret;
	}

	// --------------------------------------------------------
	// start
	// --------------------------------------------------------
	void start() {
		renderer::reset(_guiCtx->uiContext);
		_guiCtx->idStack.reset();
		ds::vec2 mp = ds::getMousePosition();
		_guiCtx->mousePosition = p2i(mp.x, mp.y);
		_guiCtx->hotItem = 0;
		_guiCtx->clicked = false;
		if (ds::isMouseButtonPressed(0)) {
			_guiCtx->mouseDown = true;			
		}
		else {
			_guiCtx->mouseDown = false;
		}
		_guiCtx->size = p2i(0);
		_guiCtx->grouping = false;
	}

	// --------------------------------------------------------
	// begin
	// --------------------------------------------------------
	bool begin(const char* header, int* state, int width) {
		pushID(header);
		p2i pos = _guiCtx->currentPos;
		// header
		renderer::add_box(_guiCtx->uiContext, pos, width, 20, _guiCtx->settings.headerBoxColor);
		pos.x += 30;
		renderer::add_text(_guiCtx->uiContext, pos, header);
		// open/close
		pos = _guiCtx->currentPos;
		pos.x -= 10;
		renderer::add_box(_guiCtx->uiContext, pos, 20, 20, _guiCtx->settings.buttonColor);
		pushID("Box");
		checkItem(pos, p2i(20, 20));
		if (*state == 0) {
			renderer::add_text(_guiCtx->uiContext, pos, "+");
		}
		else {
			renderer::add_text(_guiCtx->uiContext, pos, "-");
		}

		if (isClicked()) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				*state = 0;
			}
		}
		popID();
		int advance = 20 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(10, advance));
		popID();
		return *state == 1;
	}
	// --------------------------------------------------------
	// begin with header
	// --------------------------------------------------------
	bool begin(const char* header, int* state, p2i* position, int width) {
		pushID(header);
		_guiCtx->currentPos = *position;
		_guiCtx->startPos = _guiCtx->currentPos;
		_guiCtx->uiContext->startPos = _guiCtx->currentPos;
		_guiCtx->currentPos = _guiCtx->currentPos;
		pushID("Box");
		checkItem(*position, p2i(20, 20));
		if (isClicked()) {
			if (*state == 0) {
				*state = 1;
			}
			else {
				*state = 0;
			}
		}
		popID();
		p2i np = *position;
		np.x += 20;
		checkItem(np, p2i(width - 20, 20));		
		p2i pos = _guiCtx->currentPos;
		pos.x -= 5;
		renderer::add_box(_guiCtx->uiContext, pos, width, 256, _guiCtx->settings.backgroundColor, renderer::ResizeType::RT_Y);
		// header
		pos.x += 5;
		renderer::add_box(_guiCtx->uiContext, pos, width, 20, _guiCtx->settings.headerBoxColor);
		pos.x += 30;
		renderer::add_text(_guiCtx->uiContext, pos, header);
		// open/close
		pos = _guiCtx->currentPos;
		pos.x -= 10;
		renderer::add_box(_guiCtx->uiContext, pos, 20, 20, _guiCtx->settings.buttonColor);
		if (*state == 0) {
			renderer::add_text(_guiCtx->uiContext, pos, "+");
		}
		else {
			renderer::add_text(_guiCtx->uiContext, pos, "-");
		}
		
		
		if (isHot(np, p2i(width - 20, 20))) {
			if (_guiCtx->mouseDown) {
				position->x = _guiCtx->mousePosition.x - width / 2;
				position->y = _guiCtx->mousePosition.y;
			}
		}
		int advance = 20 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(10, advance));
		popID();
		return *state == 1;
	}

	// --------------------------------------------------------
	// begin grouping
	// --------------------------------------------------------
	void beginGroup() {
		_guiCtx->grouping = true;
	}

	// --------------------------------------------------------
	// end grouping
	// --------------------------------------------------------
	void endGroup() {
		_guiCtx->grouping = false;
		moveForward(p2i(10, 30));
	}

	// --------------------------------------------------------
	// Button
	// --------------------------------------------------------
	bool Button(const char* text) {
		pushID(text);
		p2i p = _guiCtx->currentPos;
		checkItem(p, p2i(120, 20));
		bool clicked = isClicked();
		renderer::add_box(_guiCtx->uiContext, p, 120, 20, _guiCtx->settings.buttonColor);
		p2i dim = p2i(120, 20);
		p2i textDim = textSize(text);
		p.x += (120 - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, p, text, 0.0f);
		dim.y = 20;
		p2i buttonPos = _guiCtx->currentPos;
		p2i m = dim;
		m.y += _guiCtx->settings.lineSpacing;
		moveForward(m);
		popID();
		return clicked;
	}

	// --------------------------------------------------------
	// Button
	// --------------------------------------------------------
	void Checkbox(const char* label, bool* state) {
		pushID(label);
		p2i p = _guiCtx->currentPos;
		checkItem(p, p2i(16, 16));
		renderer::add_box(_guiCtx->uiContext, p, p2i(16, 16),_guiCtx->settings.buttonColor);
		p.x += 4;
		if (*state) {			
			renderer::add_box(_guiCtx->uiContext, p, 8, 8, _guiCtx->settings.enabledBoxColor);
		}
		else {
			renderer::add_box(_guiCtx->uiContext, p, 8, 8, _guiCtx->settings.disabledBoxColor);
		}
		if (isClicked()) {
			*state = !*state;
		}
		p = _guiCtx->currentPos;
		p.x += 20;
		renderer::add_text(_guiCtx->uiContext, p, label);
		int advance = 16 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(40, advance));
		popID();
	}

	// --------------------------------------------------------
	// Label
	// --------------------------------------------------------
	void Label(const char* label, const char* text) {
		pushID(label);
		renderer::add_box(_guiCtx->uiContext, _guiCtx->currentPos, p2i(150, 16), _guiCtx->settings.labelBoxColor);
		p2i cp = p2i(_guiCtx->currentPos.x, _guiCtx->currentPos.y);
		checkItem(cp, p2i(150, 16));
		p2i textDim = textSize(text);
		p2i textPos = _guiCtx->currentPos;
		textPos.x += (160 - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, textPos, text, 0);
		p2i labelPos = _guiCtx->currentPos;
		labelPos.x += 160;
		p2i size = renderer::add_text(_guiCtx->uiContext, labelPos, label);
		moveForward(p2i(size.x, 22));
		popID();
	}

	// --------------------------------------------------------
	// Value - vec2
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec2& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g", v.x, v.y);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - int
	// --------------------------------------------------------
	void Value(const char* label, uint32_t v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - uint32_t
	// --------------------------------------------------------
	void Value(const char* label, int v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - float
	// --------------------------------------------------------
	void Value(const char* label, float v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g", v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - float
	// --------------------------------------------------------
	void Value(const char* label, float v, const char* format) {
		sprintf_s(_guiCtx->tmpBuffer, 256, format, v);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - vec3
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec3& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g %g", v.x, v.y, v.z);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - p2i
	// --------------------------------------------------------
	void Value(const char* label, const p2i& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d %d", v.x, v.y);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - bool
	// --------------------------------------------------------
	void Value(const char* label, bool v) {
		if (v) {
			sprintf_s(_guiCtx->tmpBuffer, 256, "true");
		}
		else {
			sprintf_s(_guiCtx->tmpBuffer, 256, "false");
		}
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - vec4
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec4& v) {
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g %g %g %g", v.x, v.y, v.z, v.w);
		Label(label, _guiCtx->tmpBuffer);
	}

	// --------------------------------------------------------
	// Value - color
	// --------------------------------------------------------
	void Value(const char* label, const ds::Color& v) {
		int r = static_cast<int>(v.r * 255.0f);
		int g = static_cast<int>(v.g * 255.0f);
		int b = static_cast<int>(v.b * 255.0f);
		int a = static_cast<int>(v.a * 255.0f);
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d %d %d %d", r, g, b, a);
		p2i p = _guiCtx->currentPos;
		p2i textDim = textSize(label);
		p.x = _guiCtx->currentPos.x + textDim.x + 200;
		renderer::add_box(_guiCtx->uiContext, p, p2i(20, 20), v);
		Label(label, _guiCtx->tmpBuffer);

	}

	// --------------------------------------------------------
	// Text
	// --------------------------------------------------------
	void Text(const char* text) {
		size_t l = strlen(text);
		p2i p = _guiCtx->currentPos;
		renderer::add_text(_guiCtx->uiContext, p, text);
		p2i size = textSize(text);
		moveForward(p2i(size.x, 22));
	}

	// --------------------------------------------------------
	// Formatted text
	// --------------------------------------------------------
	void FormattedText(const char* fmt, ...) {
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		Text(buffer);
		va_end(args);

	}

	// --------------------------------------------------------
	// Message
	// --------------------------------------------------------
	void Message(const char* fmt, ...) {
		p2i pos = _guiCtx->currentPos;
		renderer::add_box(_guiCtx->uiContext, pos, 10, 20, _guiCtx->settings.labelBoxColor, renderer::ResizeType::RT_X);
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		Text(buffer);
		va_end(args);

	}

	// -------------------------------------------------------
	// input string
	// -------------------------------------------------------
	bool Input(const char* label, char* str, int maxLength) {
		pushID(label);
		bool ret = InputScalar(0, str, maxLength, 150);
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		popID();
		return ret;
	}

	// -------------------------------------------------------
	// input int
	// -------------------------------------------------------
	void Input(const char* label, int* v) {
		pushID(label);
		float f = static_cast<float>(*v);
		InputScalar(0, &f, "%g", 150);
		*v = static_cast<int>(f);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
	}

	// -------------------------------------------------------
	// input float
	// -------------------------------------------------------
	void Input(const char* label, float* v) {
		pushID(label);
		InputScalar(0, v, "%g", 150);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
	}

	// -------------------------------------------------------
	// input vec2
	// -------------------------------------------------------
	void Input(const char* label, ds::vec2* v) {
		pushID(label, "##X");
		InputScalar(0, &v->x, "%g", 70);
		_guiCtx->idStack.pop();
		pushID(label, "##Y");
		InputScalar(1, &v->y, "%g", 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
	}

	// -------------------------------------------------------
	// input vec3
	// -------------------------------------------------------
	void Input(const char* label, ds::vec3* v) {
		pushID(label, "##X");
		InputScalar(0, &v->x, "%g", 70);
		popID();
		pushID(label, "##Y");
		InputScalar(1, &v->y, "%g", 70);
		popID();
		pushID(label, "##Z");
		InputScalar(2, &v->z, "%g", 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 240;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
	}

	// -------------------------------------------------------
	// input vec4
	// -------------------------------------------------------
	void Input(const char* label, ds::vec4* v) {
		pushID(label,"##X");
		InputScalar(0, &v->x, "%g", 70);
		popID();
		pushID(label,"##Y");
		InputScalar(1, &v->y, "%g", 70);
		popID();
		pushID(label,"##Z");
		InputScalar(2, &v->z, "%g", 70);
		popID();
		pushID(label,"##W");
		InputScalar(3, &v->w, "%g", 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 320;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void ColorSlider(float* v, char prefix, int index, int width) {
		p2i p = _guiCtx->currentPos;
		p.x += index * (width + 10);
		checkItem(p, p2i(width, 20));
		renderer::add_box(_guiCtx->uiContext, p, p2i(width, 20), _guiCtx->settings.labelBoxColor);
		// calculate offset
		int val = *v * 255.0f;
		int d = 255;
		if (isClicked()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			val = static_cast<int>(dx * d / width);
		}
		if (isDragging()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			val = static_cast<int>(dx * d / width);
		}
		if (val < 0) {
			val = 0;
		}
		if (val > 255) {
			val = 255;
		}
		p.x += static_cast<float>(val) / static_cast<float>(d) * width;
		p.x -= 4;
		renderer::add_box(_guiCtx->uiContext, p, p2i(8, 24), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		p.x += index * (width + 10);
		sprintf_s(_guiCtx->tmpBuffer, 256, "%c: %d", prefix, val);
		p2i textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer,0);
		*v = static_cast<float>(val) / 255.0f;
	}

	// -------------------------------------------------------
	// input color
	// -------------------------------------------------------
	void Input(const char* label, ds::Color* v) {
		pushID(label,"##R");
		ColorSlider(&v->r, 'R', 0, 70);
		popID();
		pushID(label, "##G");
		ColorSlider(&v->g, 'G', 1, 70);
		popID();
		pushID(label, "##B");
		ColorSlider(&v->b, 'B', 2, 70);
		popID();
		pushID(label, "##A");
		ColorSlider(&v->a, 'A', 3, 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x = _guiCtx->currentPos.x + 320;
		renderer::add_box(_guiCtx->uiContext, p, p2i(20, 20), *v);
		p.x = _guiCtx->currentPos.x + 350;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 26));
	}

	// -------------------------------------------------------
	// Separator
	// -------------------------------------------------------	
	void Separator() {
		p2i p = _guiCtx->currentPos;
		p.y += 8;
		renderer::add_box(_guiCtx->uiContext, p, p2i(10, 4), ds::Color(32, 32, 32, 255), renderer::ResizeType::RT_X);
		moveForward(p2i(10, 16));
	}

	// -------------------------------------------------------
	// int input using steps
	// -------------------------------------------------------
	void StepInput(const char* label, int* v, int minValue, int maxValue, int steps) {
		pushID(label);
		p2i p = _guiCtx->currentPos;
		pushID("-");
		checkItem(p, p2i(20, 20));
		// -
		renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
		if (isClicked()) {
			*v -= steps;
			if (*v < minValue) {
				*v = minValue;
			}
		}
		renderer::add_text(_guiCtx->uiContext, p, "-");
		popID();
		// value
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", *v);
		p2i textDim = textSize(_guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += 20;
		renderer::add_box(_guiCtx->uiContext, p, p2i(110, 20), _guiCtx->settings.labelBoxColor);
		p.x += (110 - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer);		
		// +
		pushID("+");
		p = _guiCtx->currentPos;
		p.x += 130;
		checkItem(p, p2i(20, 20));
		renderer::add_box(_guiCtx->uiContext, p,20, 20, _guiCtx->settings.buttonColor);
		if (isClicked()) {
			*v += steps;
			if (*v > maxValue) {
				*v = maxValue;
			}
		}
		renderer::add_text(_guiCtx->uiContext, p, "+");
		// label
		p.x += 30;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 30, 30));
		popID();
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void Slider(const char* label, int* v, int minValue, int maxValue, int width) {
		pushID(label);
		p2i p = _guiCtx->currentPos;
		checkItem(p, p2i(width, 20));
		renderer::add_box(_guiCtx->uiContext, _guiCtx->currentPos, p2i(width, 20), _guiCtx->settings.labelBoxColor);
		// calculate offset
		int d = maxValue - minValue;
		int val = *v;
		if (isClicked()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			val = static_cast<int>(dx * d / width);
		}
		if (isDragging()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			val = static_cast<int>(dx * d / width);
		}
		*v = val;
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		p.x += static_cast<float>(*v) / static_cast<float>(d) * width;
		renderer::add_box(_guiCtx->uiContext, p, p2i(8, 24), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		sprintf_s(_guiCtx->tmpBuffer, 256, "%d", *v);
		p2i textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += width + 10;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(width + ts.x + 20, 28));
		popID();
	}

	// -------------------------------------------------------
	// Slider
	// -------------------------------------------------------	
	void Slider(const char* label, float* v, float minValue, float maxValue, int precision, int width) {
		pushID(label);
		p2i p = _guiCtx->currentPos;
		checkItem(p, p2i(width, 20));
		renderer::add_box(_guiCtx->uiContext, _guiCtx->currentPos, p2i(width, 20), _guiCtx->settings.buttonColor);
		// calculate offset
		float d = maxValue - minValue;
		if (isClicked()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			*v = dx * d / width;
		}
		if (isDragging()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			*v = dx * d / width;
		}
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		if (precision > 0) {
			float prec = 10.0f * static_cast<float>(precision);
			*v = roundf(*v * prec) / prec;
		}
		p.x += *v / d * width;
		renderer::add_box(_guiCtx->uiContext, p, p2i(8, 28), _guiCtx->settings.sliderColor);
		p = _guiCtx->currentPos;
		sprintf_s(_guiCtx->tmpBuffer, 256, "%g", *v);
		p2i textDim = textSize(_guiCtx->tmpBuffer);
		p.x += (width - textDim.x) / 2;
		renderer::add_text(_guiCtx->uiContext, p, _guiCtx->tmpBuffer);
		p = _guiCtx->currentPos;
		p.x += width + 10;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(width + ts.x + 20, 28));
		popID();
	}

	void SliderAngle(const char* label, float* v, int width) {
		int d = static_cast<int>(*v / ds::TWO_PI * 360.0f);
		Slider(label, &d, 0, 360, width);
		*v = d / 360.0f * ds::TWO_PI;
	}

	// -------------------------------------------------------
	// Histogram
	// -------------------------------------------------------	
	void Histogram(float* values, int num, float minValue, float maxValue, float step, int width, int height) {
		p2i p = _guiCtx->currentPos;
		
		float barWidth = 10.0f;
		p.y -= height / 2.0f;
		float delta = maxValue - minValue;
		if (delta == 0.0f) {
			delta = 1.0f;
		}
		float st = width / static_cast<float>(num - 1);
		float bw = width / static_cast<float>(num);
		renderer::add_box(_guiCtx->uiContext, p, p2i(width + barWidth, height), ds::Color(51, 51, 51, 255));
		p.x += width + 20.0f;
		p.y += height / 2.0f;
		char buffer[16];
		sprintf_s(buffer, 16, "%g", maxValue);
		renderer::add_text(_guiCtx->uiContext, p, buffer);
		p.y -= height;
		sprintf_s(buffer, 16, "%g", minValue);
		renderer::add_text(_guiCtx->uiContext, p, buffer);
		for (int i = 0; i < num; ++i) {
			float v = values[i];
			if (v > maxValue) {
				v = maxValue;
			}
			float current = (v - minValue) / delta;
			float yp = current * height;
			p = _guiCtx->currentPos;
			p.y -= (height - yp / 2);
			p.x += i * bw + 2;
			renderer::add_box(_guiCtx->uiContext, p, p2i(bw - 4, yp), ds::Color(192, 0, 0, 255));
		}
		step = delta / 10.0f;
		int d = static_cast<int>(delta / step) + 1;
		for (int i = 0; i < d; ++i) {
			p = _guiCtx->currentPos;
			float current = 1.0f - (step*i) / delta;
			int yp = current * height;
			p.y -= yp;
			renderer::add_box(_guiCtx->uiContext, p, p2i(width + barWidth, 1), ds::Color(16, 16, 16, 255));
		}
		moveForward(p2i(width, height + 30));
	}

	// -------------------------------------------------------
	// Diagram
	// -------------------------------------------------------	
	void DiagramInternal(const p2i& pos, float* values, int num, float minValue, float maxValue, float step, int width, int height) {
		p2i p = pos;
		
		p.y -= height / 2.0f;
		float delta = (maxValue - minValue);
		if (delta == 0.0f) {
			delta = 1.0f;
		}
		float st = width / static_cast<float>(num - 1);
		renderer::add_box(_guiCtx->uiContext, p, p2i(width, height), ds::Color(51, 51, 51, 255));
		p.x += width + 20;
		p.y += height / 2;
		char buffer[16];
		sprintf_s(buffer, 16, "%g", maxValue);
		renderer::add_text(_guiCtx->uiContext, p, buffer);
		p.y -= height;
		sprintf_s(buffer, 16, "%g", minValue);
		renderer::add_text(_guiCtx->uiContext, p, buffer);

		for (int i = 0; i < num; ++i) {
			float v = values[i];
			if (v > maxValue) {
				v = maxValue;
			}
			if (v < minValue) {
				v = minValue;
			}
			float norm = (v - minValue) / delta;
			float yp = norm * height;
			p = pos;
			p.y = pos.y - height + yp;
			p.x += static_cast<float>(i)* st - 2.0f;
			renderer::add_box(_guiCtx->uiContext, p, p2i(4, 4), ds::Color(192, 0, 0, 255));
		}

		step = delta / 10.0f;
		int d = static_cast<int>(delta / step) + 1;
		for (int i = 0; i < d; ++i) {
			p = pos;
			float current = 1.0f - (step*i) / delta;
			float yp = current * height;
			p.y -= yp;
			renderer::add_box(_guiCtx->uiContext, p, p2i(width, 1), ds::Color(16, 16, 16, 255));
		}
		moveForward(p2i(width, height + 30));
	}

	void Diagram(float* values, int num, float minValue, float maxValue, float step, int width, int height) {
		DiagramInternal(_guiCtx->currentPos, values, num, minValue, maxValue, step, width, height);
	}

	void Diagram(const char* label, float* values, int num, float minValue, float maxValue, float step, int width, int height) {
		p2i p = _guiCtx->currentPos;
		renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(10, 20));
		DiagramInternal(p, values, num, minValue, maxValue, step, width, height);
	}
	
	// -------------------------------------------------------
	// prepare combo box
	// -------------------------------------------------------
	void prepareComboBox(int id, int* offset, int size, int max) {
		int width = 200;
		p2i p = _guiCtx->currentPos;
		int height = max * 20;
		p.y -= height / 2 - 10;
		// background
		renderer::add_box(_guiCtx->uiContext, p, p2i(width + 20, height), _guiCtx->settings.boxBackgroundColor);
		if (size > max) {
			// up
			p = _guiCtx->currentPos;
			p.x += width;
			pushID("up");
			checkItem(p, p2i(20, 20));
			renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
			renderer::add_text(_guiCtx->uiContext, p, "-");
			if (isClicked()) {
				*offset -= 1;
				if (*offset < 0) {
					*offset = 0;
				}
			}
			int sideHeight = height - 40;
			p.y -= sideHeight / 2;
			p.y -= 10;
			int cy = p.y;
			renderer::add_box(_guiCtx->uiContext, p, p2i(20, sideHeight), ds::Color(20, 20, 20, 255));
			popID();
			
			// down
			pushID("down");
			p.y = _guiCtx->currentPos.y - (max - 1) * 20;
			renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
			renderer::add_text(_guiCtx->uiContext, p, "+");
			checkItem(p, p2i(20, 20));
			if (isClicked()) {
				if ((*offset + max) < size) {
					*offset += 1;
				}
			}
			float d = 1.0f - static_cast<float>(*offset) / static_cast<float>(max);
			int dy = d * (sideHeight) - 20;
			p.y = cy + dy;
			renderer::add_box(_guiCtx->uiContext, p, p2i(20, 6), _guiCtx->settings.scrollSliderColor);
			popID();
			
		}
	}
	// -------------------------------------------------------
	// ListBox
	// -------------------------------------------------------	
	void ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max) {
		pushID(label);
		prepareComboBox(_guiCtx->idStack.last(), offset, num, max);
		int width = 200;
		p2i p = _guiCtx->currentPos;
		int height = max * 20;
		int start = *offset;
		int end = *offset + max;
		if (end >= num) {
			end = num;
		}
		for (int i = start; i < end; ++i) {
			pushID(i);
			checkItem(p, p2i(width, 20));
			if ( isClicked()) {
				*selected = i;
			}
			if (*selected == i) {
				renderer::add_box(_guiCtx->uiContext, p, p2i(width, 20), _guiCtx->settings.boxSelectionColor);
			}
			renderer::add_text(_guiCtx->uiContext, p, entries[i]);
			p.y -= 20;
			popID();
		}
		moveForward(p2i(width, height + 4));
		popID();
	}

	// -------------------------------------------------------
	// DropDownBox
	// -------------------------------------------------------	
	void DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection) {
		pushID(label);
		p2i p = _guiCtx->currentPos;
		if (*state == 0) {
			renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
			renderer::add_text(_guiCtx->uiContext, p, "+");
			pushID("+");
			checkItem(p, p2i(20, 20));
			if (isClicked()) {
				*state = 1;
			}
			popID();
		}
		else {
			renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
			renderer::add_text(_guiCtx->uiContext, p, "-");
			pushID("-");
			checkItem(p, p2i(20, 20));
			if (isClicked()) {
				*state = 0;
			}
			popID();
		}
		p.x += 20;
		renderer::add_box(_guiCtx->uiContext, p, p2i(200, 20), _guiCtx->settings.labelBoxColor);
		if (*selected >= 0 && *selected < num) {
			renderer::add_text(_guiCtx->uiContext, p, entries[*selected]);
		}
		else {
			renderer::add_text(_guiCtx->uiContext, p, "{Select entry}");
		}
		p.x += 180;
		renderer::add_box(_guiCtx->uiContext, p, 20, 20, _guiCtx->settings.buttonColor);
		renderer::add_text(_guiCtx->uiContext, p, "x");
		pushID("x");
		checkItem(p, p2i(20, 20));
		if ( isClicked()) {
			*selected = -1;
		}
		popID();
		p.x += 30;
		renderer::add_text(_guiCtx->uiContext, p, label);
		
		moveForward(p2i(300, 20));
		if (*state == 1) {
			prepareComboBox(_guiCtx->idStack.last(), offset, num, max);
			int width = 200;
			// if there is no scroll border then expand it
			if (num <= max) {
				width += 20;
			}
			p2i p = _guiCtx->currentPos;
			int height = max * 20;
			int start = *offset;
			int end = *offset + max;
			if (end >= num) {
				end = num;
			}
			for (int i = start; i < end; ++i) {
				pushID(i);
				checkItem(p, p2i(width, 20));
				if ( isClicked()) {
					*selected = i;
					if (closeOnSelection) {
						*state = 0;
					}
				}
				if (*selected == i) {
					renderer::add_box(_guiCtx->uiContext, p, p2i(width, 20), _guiCtx->settings.boxSelectionColor);
				}
				renderer::add_text(_guiCtx->uiContext, p, entries[i]);
				p.y -= 20;
				popID();
			}
			moveForward(p2i(width, height + 4));
		}
		else {
			moveForward(p2i(300, 4));
		}
		popID();
	}

	// --------------------------------------------------------
	// push int as ID
	// --------------------------------------------------------
	void pushID(int id) {
		_guiCtx->idStack.push(id);
	}

	// --------------------------------------------------------
	// push text as ID
	// --------------------------------------------------------
	void pushID(const char* text) {
		_guiCtx->idStack.push(text);
	}

	// --------------------------------------------------------
	// push text and suffix as ID
	// --------------------------------------------------------
	void pushID(const char* text, const char* suffix) {
			_guiCtx->idStack.push(text,suffix);
	}

	// --------------------------------------------------------
	// push text and int as ID
	// --------------------------------------------------------
	void pushID(const char* text, int id) {
		_guiCtx->idStack.push(text,id);
	}

	// --------------------------------------------------------
	// pop ID
	// --------------------------------------------------------
	void popID() {
		_guiCtx->idStack.pop();
	}

	// --------------------------------------------------------
	// draw text and return the actual size
	// --------------------------------------------------------
	p2i draw_text(const p2i& position, const char* text) {
		return renderer::add_text(_guiCtx->uiContext, position, text);
	}

	// --------------------------------------------------------
	// draw box
	// --------------------------------------------------------
	void draw_box(const p2i& position, const p2i& dimension, const ds::Color& color) {
		renderer::add_box(_guiCtx->uiContext, position, dimension, color);
	}

	// --------------------------------------------------------
	// get IMGUI settings
	// --------------------------------------------------------
	const IMGUISettings& getSettings() {
		return _guiCtx->settings;
	}

	// --------------------------------------------------------
	// get current position
	// --------------------------------------------------------
	p2i getCurrentPosition() {
		return _guiCtx->currentPos;
	}

	// --------------------------------------------------------
	// debug FIXME: RAD
	// --------------------------------------------------------
	void debug() {
		Value("Mouse", _guiCtx->mousePosition);
		Value("Button", _guiCtx->mouseDown);
		Value("Hot", _guiCtx->hotItem);
		Value("Active", _guiCtx->activeItem);
	}
	// --------------------------------------------------------
	// end - render all draw calls
	// --------------------------------------------------------
	void end() {
		_guiCtx->uiContext->size.y = _guiCtx->size.y;
		renderer::draw_buffer(_guiCtx->uiContext);
		_guiCtx->numKeys = 0;
		if (!_guiCtx->mouseDown && _guiCtx->activeItem != 0) {
			_guiCtx->activeItem = 0;
		}	
		if (_guiCtx->mouseDown && _guiCtx->activeItem != _guiCtx->inputItem) {
			_guiCtx->inputItem = 0;
		}
	}

	// --------------------------------------------------------
	// shutdown GUI
	// --------------------------------------------------------
	void shutdown() {
		if (_guiCtx != 0) {
			renderer::shutdown(_guiCtx->uiContext);
			delete _guiCtx->uiContext;
			delete _guiCtx;
		}
	}

}

#endif