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
		int lineSpacing;
	};

	class ListBoxModel {

	public:
		ListBoxModel() : _selection(-1), _offset(0) {}
		virtual ~ListBoxModel() {}
		virtual uint32_t size() const = 0;
		virtual const char* name(uint32_t index) const = 0;
		int getSelection() const {
			return _selection;
		}
		void setSelection(int s) {
			_selection = s;
		}
		bool hasSelection() const {
			return _selection != -1;
		}
		void clearSelection() {
			_selection = -1;
		}
		int getOffset() const {
			return _offset;
		}
		void setOffset(int o) {
			_offset = o;
		}
	private:
		int _selection;
		int _offset;
	};

	void init(IMGUISettings* settings = 0);

	void start(p2i* position, int width);

	bool begin(const char* header, int* state);

	void beginGroup();

	void endGroup();

	void start_overlay();

	void end_overlay();

	bool Button(const char* text);

	void Text(const char* text);

	void Label(const char* label, const char* text);

	void Value(const char* label, int v);

	void Value(const char* label, uint32_t v);

	void Value(const char* label, float v);

	void Value(const char* label, float v, uint8_t integers, uint8_t fractions);

	void Value(const char* label, float v, const char* format);

	void Value(const char* label, const p2i& v);

	void Value(const char* label, bool v);

	void Value(const char* label, const ds::vec2& v);

	void Value(const char* label, const ds::vec3& v);

	void Value(const char* label, const ds::vec3& v, const char* format);

	void Value(const char* label, const ds::vec4& v);

	void Value(const char* label, const ds::Color& v);

	void FormattedText(const char* fmt, ...);

	void Message(const char* fmt, ...);

	bool Input(const char* label, char* str, int maxLength);

	bool TextBox(const char* label, char* str, int maxLength);

	bool Input(const char* label, int* v);

	bool Input(const char* label, float* v);

	bool Input(const char* label, ds::vec2* v);

	bool Input(const char* label, ds::vec3* v);

	void Input(const char* label, ds::vec4* v);

	void Input(const char* label, ds::Color* v);

	void StepInput(const char* label, int* v, int minValue, int maxValue, int steps);

	bool Checkbox(const char* label, bool* state);

	void RadioButtons(const char** items, int num, int* selected);

	bool ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max);

	bool ListBox(const char* label, ListBoxModel& model, int num);

	bool DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection = false);

	void Slider(const char* label, int* v, int minValue, int maxValue, int width = 200);

	void Slider(const char* label, float* v, float minValue, float maxValue, int precision = 0, int width = 200);

	void SliderAngle(const char* label, float* v, int width = 200);

	void Separator();

	void Histogram(float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	void Diagram(float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	void Diagram(const char* label, float* values, int num, float minValue, float maxValue, float step, int width = 200, int height = 100);

	bool Tabs(const char** entries, int num, int* selected);

	bool BeginMenu(const char** entries, int num, int* selected);

	bool MenuItems(const char** entries, int num, int* selected);

	void EndMenu();

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

	void setAlphaLevel(float alpha);

	const IMGUISettings& getSettings();
	
	void debug();

	void end();

	void shutdown();

}

#ifdef DS_IMGUI_IMPLEMENTATION
#include <string.h>
#include <stdarg.h>

const BYTE gui_PS_Main[] = {
	 68,  88,  66,  67, 133, 123, 244, 109,  36, 101, 150, 228,  91, 135, 209, 221,
	 54, 143,  33,  28,   1,   0,   0,   0, 112,   1,   0,   0,   3,   0,   0,   0,
	 44,   0,   0,   0, 160,   0,   0,   0, 212,   0,   0,   0,  73,  83,  71,  78,
	108,   0,   0,   0,   3,   0,   0,   0,   8,   0,   0,   0,  80,   0,   0,   0,
	  0,   0,   0,   0,   1,   0,   0,   0,   3,   0,   0,   0,   0,   0,   0,   0,
	 15,   0,   0,   0,  92,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  3,   0,   0,   0,   1,   0,   0,   0,   3,   3,   0,   0, 101,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   2,   0,   0,   0,
	 15,  15,   0,   0,  83,  86,  95,  80,  79,  83,  73,  84,  73,  79,  78,   0,
	 84,  69,  88,  67,  79,  79,  82,  68,   0,  67,  79,  76,  79,  82,   0, 171,
	 79,  83,  71,  78,  44,   0,   0,   0,   1,   0,   0,   0,   8,   0,   0,   0,
	 32,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,
	  0,   0,   0,   0,  15,   0,   0,   0,  83,  86,  95,  84,  65,  82,  71,  69,
	 84,   0, 171, 171,  83,  72,  68,  82, 148,   0,   0,   0,  64,   0,   0,   0,
	 37,   0,   0,   0,  90,   0,   0,   3,   0,  96,  16,   0,   0,   0,   0,   0,
	 88,  24,   0,   4,   0, 112,  16,   0,   0,   0,   0,   0,  85,  85,   0,   0,
	 98,  16,   0,   3,  50,  16,  16,   0,   1,   0,   0,   0,  98,  16,   0,   3,
	242,  16,  16,   0,   2,   0,   0,   0, 101,   0,   0,   3, 242,  32,  16,   0,
	  0,   0,   0,   0, 104,   0,   0,   2,   1,   0,   0,   0,  69,   0,   0,   9,
	242,   0,  16,   0,   0,   0,   0,   0,  70,  16,  16,   0,   1,   0,   0,   0,
	 70, 126,  16,   0,   0,   0,   0,   0,   0,  96,  16,   0,   0,   0,   0,   0,
	 56,   0,   0,   7, 242,  32,  16,   0,   0,   0,   0,   0,  70,  14,  16,   0,
	  0,   0,   0,   0,  70,  30,  16,   0,   2,   0,   0,   0,  62,   0,   0,   1,
};

const BYTE gui_VS_Main[] = {
	 68,  88,  66,  67, 173, 242, 138, 111, 190, 246, 106,  57, 137, 180,  96, 109, 233, 181,  39,  76,   1,   0,   0,   0,  96,   4,   0,   0,   4,   0,   0,   0,
	 48,   0,   0,   0, 100,   0,   0,   0, 216,   0,   0,   0,  80,   4,   0,   0,  73,  83,  71,  78,  44,   0,   0,   0,   1,   0,   0,   0,   8,   0,   0,   0,
	 32,   0,   0,   0,   0,   0,   0,   0,   6,   0,   0,   0,   1,   0,   0,   0,   0,   0,   0,   0,   1,   1,   0,   0,  83,  86,  95,  86,  69,  82,  84,  69,
	 88,  73,  68,   0,  79,  83,  71,  78, 108,   0,   0,   0,   3,   0,   0,   0,   8,   0,   0,   0,  80,   0,   0,   0,   0,   0,   0,   0,   1,   0,   0,   0,
	  3,   0,   0,   0,   0,   0,   0,   0,  15,   0,   0,   0,  92,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   1,   0,   0,   0,
	  3,  12,   0,   0, 101,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   0,   0,   0,   2,   0,   0,   0,  15,   0,   0,   0,  83,  86,  95,  80,
	 79,  83,  73,  84,  73,  79,  78,   0,  84,  69,  88,  67,  79,  79,  82,  68,   0,  67,  79,  76,  79,  82,   0, 171,  83,  72,  69,  88, 112,   3,   0,   0,
	 64,   0,   1,   0, 220,   0,   0,   0, 106,  72,   0,   1,  89,   0,   0,   4,  70, 142,  32,   0,   0,   0,   0,   0,   5,   0,   0,   0, 162,   0,   0,   4,
	  0, 112,  16,   0,   1,   0,   0,   0,  48,   0,   0,   0,  96,   0,   0,   4,  18,  16,  16,   0,   0,   0,   0,   0,   6,   0,   0,   0, 103,   0,   0,   4,
	242,  32,  16,   0,   0,   0,   0,   0,   1,   0,   0,   0, 101,   0,   0,   3,  50,  32,  16,   0,   1,   0,   0,   0, 101,   0,   0,   3, 242,  32,  16,   0,
	  2,   0,   0,   0, 104,   0,   0,   2,   4,   0,   0,   0, 105,   0,   0,   4,   0,   0,   0,   0,   4,   0,   0,   0,   4,   0,   0,   0,  85,   0,   0,   7,
	 18,   0,  16,   0,   0,   0,   0,   0,  10,  16,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   2,   0,   0,   0, 167,   0,   0,   9, 242,   0,  16,   0,
	  1,   0,   0,   0,  10,   0,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   8,   0,   0,   0,  70, 126,  16,   0,   1,   0,   0,   0,  14,   0,   0,   8,
	242,   0,  16,   0,   2,   0,   0,   0,  70,  14,  16,   0,   1,   0,   0,   0, 230, 142,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,  54,   0,   0,   6,
	 50,  48,  32,   0,   0,   0,   0,   0,   0,   0,   0,   0,  70,   0,  16,   0,   2,   0,   0,   0,  54,   0,   0,   5, 194,   0,  16,   0,   3,   0,   0,   0,
	 86,   1,  16,   0,   2,   0,   0,   0,   0,   0,   0,   7,  50,   0,  16,   0,   3,   0,   0,   0, 230,  10,  16,   0,   2,   0,   0,   0,  70,   0,  16,   0,
	  2,   0,   0,   0,  54,   0,   0,   6,  50,  48,  32,   0,   0,   0,   0,   0,   1,   0,   0,   0, 134,   0,  16,   0,   3,   0,   0,   0,  54,   0,   0,   6,
	 50,  48,  32,   0,   0,   0,   0,   0,   2,   0,   0,   0, 118,  15,  16,   0,   3,   0,   0,   0,  54,   0,   0,   6,  50,  48,  32,   0,   0,   0,   0,   0,
	  3,   0,   0,   0,  70,   0,  16,   0,   3,   0,   0,   0, 167,   0,   0,   9,  50,   0,  16,   0,   2,   0,   0,   0,  10,   0,  16,   0,   0,   0,   0,   0,
	  1,  64,   0,   0,   0,   0,   0,   0,  70, 112,  16,   0,   1,   0,   0,   0,   0,   0,   0,   9,  98,   0,  16,   0,   0,   0,   0,   0,   6,   1,  16,   0,
	  2,   0,   0,   0,   6, 129,  32, 128,  65,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   0,   0,  10, 114,   0,  16,   0,   2,   0,   0,   0,
	  6,  16,  16,   0,   0,   0,   0,   0,   2,  64,   0,   0,   3,   0,   0,   0,   1,   0,   0,   0,   2,   0,   0,   0,   0,   0,   0,   0,  55,   0,   0,  15,
	 50,   0,  16,   0,   1,   0,   0,   0, 150,   5,  16,   0,   2,   0,   0,   0,   2,  64,   0,   0,   0,   0,   0,  63,   0,   0,   0, 191,   0,   0,   0,   0,
	  0,   0,   0,   0,   2,  64,   0,   0,   0,   0,   0, 191,   0,   0,   0,  63,   0,   0,   0,   0,   0,   0,   0,   0,  54,   0,   0,   7,  50,  32,  16,   0,
	  1,   0,   0,   0,  70,  48,  32,   4,   0,   0,   0,   0,  10,   0,  16,   0,   2,   0,   0,   0,  56,   0,   0,   7,  50,   0,  16,   0,   1,   0,   0,   0,
	230,  10,  16,   0,   1,   0,   0,   0,  70,   0,  16,   0,   1,   0,   0,   0, 167,   0,   0,   9, 242,   0,  16,   0,   2,   0,   0,   0,  10,   0,  16,   0,
	  0,   0,   0,   0,   1,  64,   0,   0,  24,   0,   0,   0,  70, 126,  16,   0,   1,   0,   0,   0, 167,   0,   0,   9,  50,   0,  16,   0,   3,   0,   0,   0,
	 10,   0,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,  40,   0,   0,   0,  70, 112,  16,   0,   1,   0,   0,   0,  50,   0,   0,   9,  50,   0,  16,   0,
	  0,   0,   0,   0,  70,   0,  16,   0,   1,   0,   0,   0,  70,   0,  16,   0,   2,   0,   0,   0, 150,   5,  16,   0,   0,   0,   0,   0,  54,   0,   0,   5,
	194,   0,  16,   0,   3,   0,   0,   0, 166,  14,  16,   0,   2,   0,   0,   0,  54,   0,   0,   5, 242,  32,  16,   0,   2,   0,   0,   0, 230,   4,  16,   0,
	  3,   0,   0,   0,  54,   0,   0,   5,  66,   0,  16,   0,   0,   0,   0,   0,   1,  64,   0,   0,   0,   0, 128,  63,  16,   0,   0,   8,  18,  32,  16,   0,
	  0,   0,   0,   0,  70,   2,  16,   0,   0,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   1,   0,   0,   0,  16,   0,   0,   8,  34,  32,  16,   0,
	  0,   0,   0,   0,  70,   2,  16,   0,   0,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   2,   0,   0,   0,  16,   0,   0,   8, 130,  32,  16,   0,
	  0,   0,   0,   0,  70,   2,  16,   0,   0,   0,   0,   0,  70, 131,  32,   0,   0,   0,   0,   0,   4,   0,   0,   0,  54,   0,   0,   5,  66,  32,  16,   0,
	  0,   0,   0,   0,   1,  64,   0,   0,   0,   0, 128,  63,  62,   0,   0,   1,  83,  70,  73,  48,   8,   0,   0,   0,   2,   0,   0,   0,   0,   0,   0,   0,
};


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

		struct GUIConstantBuffer {
			ds::vec4 screenCenter;
			ds::matrix wvp;
		};

		struct GUIItem {
			ds::vec2 position;
			ds::vec4 textureRect;
			ds::vec2 scaling;
			ds::Color color;

		};
		
		struct UIBatchBuffer {
			GUIItem* items;
			RID renderPass;
			RID drawItem;
			RID structuredBufferId;
			GUIConstantBuffer constantBuffer;
			int max;
			int current;
		};

		struct UIContext {
			UIBuffer* buffer;
			UIBuffer* overlay_buffer;
			bool use_overlay;
			p2i startPos;
			p2i size;
			char tmpBuffer[256];
			UIBatchBuffer sprites;
			float alpha;
		};

		static void createBatchBuffer(UIContext* ctx, int max, RID textureID, ds::TextureFilters textureFilter) {
			ctx->sprites.max = max;
			ctx->sprites.current = 0;
			ctx->sprites.items = new GUIItem[max];
			ds::vec2 textureSize = ds::getTextureSize(textureID);
			ctx->sprites.constantBuffer.screenCenter = ds::vec4(static_cast<float>(ds::getScreenWidth() / 2), static_cast<float>(ds::getScreenHeight() / 2), textureSize.x, textureSize.y);

			ds::ShaderInfo vsInfo = { 0 , gui_VS_Main, sizeof(gui_VS_Main), ds::ShaderType::ST_VERTEX_SHADER };
			RID vertexShader = ds::createShader(vsInfo, "GUI_VS");
			ds::ShaderInfo psInfo = { 0 , gui_PS_Main, sizeof(gui_PS_Main), ds::ShaderType::ST_PIXEL_SHADER };
			RID pixelShader = ds::createShader(psInfo, "GUI_PS");

			ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
			RID bs_id = ds::createBlendState(blendInfo);

			RID constantBuffer = ds::createConstantBuffer(sizeof(GUIConstantBuffer), &ctx->sprites.constantBuffer);

			ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, textureFilter };
			RID ssid = ds::createSamplerState(samplerInfo);

			int indices[] = { 0,1,2,1,3,2 };
			RID idxBuffer = ds::createQuadIndexBuffer(max, indices);

			ds::StructuredBufferInfo sbInfo;
			sbInfo.cpuWritable = true;
			sbInfo.data = 0;
			sbInfo.elementSize = sizeof(GUIItem);
			sbInfo.numElements = max;
			sbInfo.gpuWritable = false;
			sbInfo.renderTarget = NO_RID;
			sbInfo.textureID = NO_RID;
			ctx->sprites.structuredBufferId = ds::createStructuredBuffer(sbInfo);

			RID basicGroup = ds::StateGroupBuilder()
				.constantBuffer(constantBuffer, vertexShader)
				.structuredBuffer(ctx->sprites.structuredBufferId, vertexShader, 1)
				.vertexBuffer(NO_RID)
				.vertexShader(vertexShader)
				.indexBuffer(idxBuffer)
				.pixelShader(pixelShader)
				.samplerState(ssid, pixelShader)
				.texture(textureID, pixelShader, 0)
				.build();

			ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_INDEXED, ds::PrimitiveTypes::TRIANGLE_LIST };

			ctx->sprites.drawItem = ds::compile(drawCmd, basicGroup, "GUIDrawItem");

			// create orthographic view
			ds::matrix orthoView = ds::matIdentity();
			ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f);
			ds::Camera camera = {
				orthoView,
				orthoProjection,
				orthoView * orthoProjection,
				ds::vec3(0,0,-1),
				ds::vec3(0,0,1),
				ds::vec3(0,1,0),
				ds::vec3(1,0,0),
				0.0f,
				0.0f,
				0.0f
			};

			ds::ViewportInfo vpInfo = { ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f };
			RID vp = ds::createViewport(vpInfo);

			ds::RenderPassInfo rpInfo = { &camera, vp, ds::DepthBufferState::DISABLED, 0, 0 };
			ctx->sprites.renderPass = ds::createRenderPass(rpInfo, "GUIOrthoPass");
			ctx->sprites.constantBuffer.wvp = camera.viewProjectionMatrix;
		}

		static void createBuffer(UIBuffer* buffer, int size) {
			int sz = size * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(recti) + sizeof(ds::vec2) + sizeof(ds::Color) + sizeof(ResizeType));
			buffer->num = 0;
			buffer->capacity = size;
			buffer->data = new char[sz];
			buffer->positions = (ds::vec2*)(buffer->data);
			buffer->sizes = (ds::vec2*)(buffer->positions + buffer->capacity);
			buffer->rectangles = (recti*)(buffer->sizes + buffer->capacity);
			buffer->scales = (ds::vec2*)(buffer->rectangles + buffer->capacity);
			buffer->colors = (ds::Color*)(buffer->scales + buffer->capacity);
			buffer->resize = (ResizeType*)(buffer->colors + buffer->capacity);
		}

		UIContext* init() {
			UIContext* ctx = new UIContext;
			ctx->buffer = new UIBuffer;
			createBuffer(ctx->buffer, 4096);
			ctx->overlay_buffer = new UIBuffer();
			ctx->use_overlay = false;
			ctx->alpha = 1.0f;
			createBuffer(ctx->overlay_buffer, 4096);			
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
			createBatchBuffer(ctx, 4096, textureID, ds::TextureFilters::POINT);
			delete[] data;

			return ctx;
		}

		void reset(UIContext* ctx) {
			ctx->buffer->num = 0;
			ctx->overlay_buffer->num = 0;
			ctx->use_overlay = false;
			ctx->size = p2i(0);
		}

		// --------------------------------------------------------
		// Extract the name. If the text contains # it will be
		// cut off at that position. 
		// --------------------------------------------------------
		static const char* extract_name(UIContext* ctx, const char* t) {
			strncpy(ctx->tmpBuffer, t, 256);
			size_t l = strlen(t);
			for (size_t i = 0; i < l; ++i) {
				if (ctx->tmpBuffer[i] == '#') {
					ctx->tmpBuffer[i] = '\0';
				}
			}
			return ctx->tmpBuffer;
		}

		static void clear_items(UIContext* ctx) {
			ctx->sprites.current = 0;
		}

		static void flush_items(UIContext* ctx) {
			if (ctx->sprites.current > 0) {
				ds::mapBufferData(ctx->sprites.structuredBufferId, ctx->sprites.items, ctx->sprites.current * sizeof(GUIItem));
				ds::submit(ctx->sprites.renderPass, ctx->sprites.drawItem, ctx->sprites.current * 6);
				ctx->sprites.current = 0;
			}
		}

		static void add_item(UIContext* ctx, const ds::vec2& position, const ds::vec4& rect, const ds::vec2& scale, const ds::Color& clr) {
			if ((ctx->sprites.current + 1) >= ctx->sprites.max) {
				flush_items(ctx);
			}
			GUIItem& sp = ctx->sprites.items[ctx->sprites.current++];
			sp.position.x = position.x;
			sp.position.y = position.y;
			sp.textureRect = rect;
			sp.scaling = scale;
			sp.color = clr;
		}

		static void draw_buffer(UIContext* ctx, UIBuffer* buffer, const p2i& startPos, const p2i& size) {
			float h = size.y - 20.0f;
			float sy = h / 128.0f;
			float w = size.x + 10.0f;
			float sx = w / 128.0f;

			float bpx = startPos.x + w * 0.5f - 10.0f;
			float bpy = startPos.y - h * 0.5f - 10.0f;
			int num = buffer->num;
			for (uint16_t i = 0; i < num; ++i) {
				ds::vec2 p = buffer->positions[i];
				recti rect = buffer->rectangles[i];
				ds::vec2 s = buffer->scales[i];
				if (buffer->resize[i] == ResizeType::RT_X) {
					rect.width = 128;
					p.x = bpx;
					s.x = sx;
				}
				else if (buffer->resize[i] == ResizeType::RT_Y) {
					rect.height = 128;
					p.y = bpy;
					s.x = sx;
					s.y = sy;
				}
				else if (buffer->resize[i] == ResizeType::RT_BOTH) {
					rect.width = 128;
					rect.height = 128;
					p.x = bpx;
					p.y = bpy;
					s.x = sx;
					s.y = sy;
				}
				add_item(ctx, p, rect.convert(), s, buffer->colors[i]);
			}
			buffer->num = 0;
		}

		void draw_buffer(UIContext* ctx) {
			clear_items(ctx);
			draw_buffer(ctx, ctx->buffer, ctx->startPos, ctx->size);
			draw_buffer(ctx, ctx->overlay_buffer, ctx->startPos, ctx->size);
			flush_items(ctx);
		}

		void add_to_buffer(UIContext* ctx, const p2i& p, const recti& rect, const ds::vec2& scale, const ds::Color& color, ResizeType resize = ResizeType::RT_NONE) {
			UIBuffer* buffer = ctx->buffer;
			if (ctx->use_overlay) {
				buffer = ctx->overlay_buffer;
			}
			if ((buffer->num + 1) >= buffer->capacity) {
				draw_buffer(ctx);
			}
			buffer->positions[buffer->num].x = p.x;
			buffer->positions[buffer->num].y = p.y;
			buffer->scales[buffer->num] = scale;
			buffer->rectangles[buffer->num] = rect;
			buffer->colors[buffer->num] = color;
			buffer->resize[buffer->num] = resize;
			++buffer->num;
		}

		// --------------------------------------------------------
		// internal add text method
		// --------------------------------------------------------
		p2i add_text(UIContext* ctx, const p2i& pos, const char* text, int xoffset = 10, const ds::Color& color = ds::Color(1.0f, 1.0f, 1.0f, 1.0f)) {
			const char* lbl = extract_name(ctx, text);
			size_t l = strlen(lbl);
			p2i p = pos;
			p.x += xoffset;
			int w = 0;
			p2i dim(0);
			for (size_t i = 0; i < l; ++i) {
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
			ds::Color tmpColor = color;
			tmpColor.a = ctx->alpha;
			add_to_buffer(ctx, pos, recti(256, 0, sz.x, sz.y), scale, tmpColor, resize);
			if (size.x > ctx->size.x) {
				ctx->size.x = size.x;
			}
		}

		void add_box(UIContext* ctx, const p2i& p, int width, int height, const ds::Color& color, ResizeType resize = ResizeType::RT_NONE) {
			add_box(ctx, p, p2i(width, height), color, resize);
		}

		void shutdown(UIContext* ctx) {
			delete[] ctx->sprites.items;
			delete[] ctx->buffer->data;
			delete ctx->buffer;
			delete[] ctx->overlay_buffer->data;
			delete ctx->overlay_buffer;
		}

	}
	static const IMGUISettings DEFAULT_SETTINGS = {
		ds::Color( 40,117,114, 255), // header
		ds::Color( 81, 83, 96, 255), // button
		ds::Color( 24, 24, 24, 255), // background
		ds::Color( 39, 39, 49, 255), // label 
		ds::Color(  0, 192, 0, 255), // enabled
		ds::Color(192,   0, 0, 255), // disabled
		ds::Color( 64, 64, 64, 255), // active input
		ds::Color( 62, 64, 76, 255), // input
		ds::Color( 38, 38, 38, 255), // box background color
		ds::Color(167, 77, 75, 255), // box selection color
		ds::Color( 96, 96, 96, 255), // slider color
		ds::Color( 80, 80, 80, 255), // scroll slider color
		5 // line spacing
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
			_tmp[0] = '\0';
			top = 0;
			for (int i = 0; i < STACK_MAX_ENTRIES; ++i) {
				ids[i] = 0;
			}
		}

		void append_int(int v, int offset) {
			int l = offset;
			if (v == 0) {
				_tmp[l++] = '0';
				_tmp[l++] = '\0';
			}
			else {
				int s = 1;
				int t = v;
				int cnt = 0;
				while (t > 0) {
					t /= 10;
					++cnt;
					s *= 10;
				}
				t = v;
				
				for (int i = 0; i < cnt; ++i) {
					s /= 10;
					int tmp = t / s;
					_tmp[l++] = 48 + tmp;
					t -= tmp * s;
				}
				_tmp[l] = '\0';
			}			
		}

		void push(int id) {
			if (top < STACK_MAX_ENTRIES) {
				append_int(id, 0);
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
				const char* t = text;
				int cnt = 0;
				while (*t) {
					_tmp[cnt++] = *t;
					++t;
				}
				t = suffix;
				while (*t) {
					_tmp[cnt++] = *t;
					++t;
				}
				_tmp[cnt] = '\0';
				append(_tmp);
				ids[++top] = fnv1a(buffer);
			}
		}

		void push(const char* text, int id) {
			if (top < STACK_MAX_ENTRIES) {
				const char* t = text;
				int cnt = 0;
				while (*t) {
					_tmp[cnt++] = *t;
					++t;
				}
				append_int(id, cnt);
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
				if (index < 255) {
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
		char inputText[256];
		char tmpBuffer[256];
		size_t caretPos;
		p2i size;
		IMGUISettings settings;
		renderer::UIContext* uiContext;
		IDStack idStack;
		int width;
		p2i menuPos;
	};

	static GUIContext* _guiCtx = 0;

	void setAlphaLevel(float alpha) {
		_guiCtx->uiContext->alpha = alpha;
	}

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
		size_t l = strlen(txt);
		p2i p(0);
		p.x = (int)l * 8;
		p.y = 14;
		return p;
	}

	// -------------------------------------------------------
	// determine text size
	// -------------------------------------------------------
	p2i limitedTextSize(const char* txt, size_t maxLength) {
		size_t l = strlen(txt);
		if (l > maxLength) {
			l = maxLength;
		}
		p2i p(0);
		p.x = (int)l * 8;
		p.y = 14;
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
		_guiCtx->menuPos = p2i(0, 0);
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
		size_t len = strlen(_guiCtx->inputText);
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
				if ((key.value >= 32 && key.value < 128) || key.value == '.' || key.value == '-') {
					if (len < 255) {
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
			sprintf_s(_guiCtx->inputText, 256, "%s", v);
			_guiCtx->caretPos = strlen(_guiCtx->inputText);
			_guiCtx->inputItem = _guiCtx->idStack.last();
			_guiCtx->activeItem = _guiCtx->idStack.last();
		}
		if (isInputActive()) {
			renderer::add_box(_guiCtx->uiContext, p, p2i(width, 16), _guiCtx->settings.activeInputBoxColor);
			if (handleTextInput(false)) {
				strncpy(v, _guiCtx->inputText, maxLength);
				ret = true;
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
			sprintf_s(_guiCtx->tmpBuffer, 256, "%s", v);
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
	void start(p2i* position, int width) {
		renderer::reset(_guiCtx->uiContext);
		_guiCtx->idStack.reset();
		ds::vec2 mp = ds::getMousePosition();
		_guiCtx->mousePosition = p2i(static_cast<int>(mp.x), static_cast<int>(mp.y));
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
		_guiCtx->width = width;
		_guiCtx->currentPos = *position;
		_guiCtx->startPos = _guiCtx->currentPos;
		_guiCtx->uiContext->startPos = _guiCtx->currentPos;
		_guiCtx->currentPos = _guiCtx->currentPos;
		p2i pos = _guiCtx->currentPos;
		pos.x -= 5;
		renderer::add_box(_guiCtx->uiContext, pos, _guiCtx->width + 5, 256, _guiCtx->settings.backgroundColor, renderer::ResizeType::RT_Y);
	}
	
	// --------------------------------------------------------
	// begin with header
	// --------------------------------------------------------
	bool begin(const char* header, int* state) {
		pushID(header);
		pushID("Box");
		checkItem(_guiCtx->currentPos, p2i(20, 20));
		if (state != 0) {
			if (isClicked()) {
				if (*state == 0) {
					*state = 1;
				}
				else {
					*state = 0;
				}
			}
		}
		popID();
		p2i np = _guiCtx->currentPos;
		np.x += 20;
		checkItem(np, p2i(_guiCtx->width - 20, 20));
		p2i pos = _guiCtx->currentPos;
		// header
		pos.x += 5;
		renderer::add_box(_guiCtx->uiContext, pos, _guiCtx->width, 20, _guiCtx->settings.headerBoxColor);
		pos.x += 30;
		renderer::add_text(_guiCtx->uiContext, pos, header);
		// open/close
		pos = _guiCtx->currentPos;
		pos.x -= 10;
		renderer::add_box(_guiCtx->uiContext, pos, 20, 20, _guiCtx->settings.buttonColor);
		if (state != 0) {
			if (*state == 0) {
				renderer::add_text(_guiCtx->uiContext, pos, "+");
			}
			else {
				renderer::add_text(_guiCtx->uiContext, pos, "-");
			}
		}
		int advance = 20 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(10, advance));
		popID();
		if (state != 0) {
			return *state == 1;
		}
		return true;
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
	// start overlay
	// --------------------------------------------------------
	void start_overlay() {
		_guiCtx->uiContext->use_overlay = true;
	}

	// --------------------------------------------------------
	// end overlay
	// --------------------------------------------------------
	void end_overlay() {
		_guiCtx->uiContext->use_overlay = false;
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
		renderer::add_text(_guiCtx->uiContext, p, text, 0);
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
	bool Checkbox(const char* label, bool* state) {
		bool ret = false;
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
			ret = true;
		}
		p = _guiCtx->currentPos;
		p.x += 20;
		renderer::add_text(_guiCtx->uiContext, p, label);
		int advance = 16 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(40, advance));
		popID();
		return ret;
	}

	void RadioButtons(const char** items, int num, int* selected) {		
		p2i p = _guiCtx->currentPos;
		int totalWidth = 0;
		for (int i = 0; i < num; ++i) {
			pushID(items[i]);
			checkItem(p, p2i(16, 16));
			renderer::add_box(_guiCtx->uiContext, p, p2i(16, 16), _guiCtx->settings.buttonColor);
			p.x += 4;
			if (*selected == i) {
				renderer::add_box(_guiCtx->uiContext, p, 8, 8, _guiCtx->settings.enabledBoxColor);
			}
			else {
				renderer::add_box(_guiCtx->uiContext, p, 8, 8, _guiCtx->settings.disabledBoxColor);
			}
			if (isClicked()) {
				*selected = i;
			}
			p.x += 10;
			p2i size = renderer::add_text(_guiCtx->uiContext, p, items[i]);
			p.x += size.x + 10;
			totalWidth += 16 + size.x + 10;
			popID();
		}
		int advance = 16 + _guiCtx->settings.lineSpacing;
		moveForward(p2i(totalWidth, advance));
		
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

	static void simple_print(char* buffer, float value, int integers, int prec) {
		char* _tmpBuffer = buffer;
		int cnt = 0;
		if (value < 0.0f) {
			_tmpBuffer[cnt++] = '-';
		}

		int start = static_cast<int>(pow(10, integers - 1));
		int t = static_cast<int>(value);
		if (t < 0) {
			t *= -1;
		}
		bool leading = true;
		for (int i = 0; i < integers; ++i) {
			int f = t / start;
			if (f > 0 || !leading) {
				_tmpBuffer[cnt++] = 48 + f;
				leading = false;
			}
			t -= f * start;
			start /= 10;
		}
		if (leading) {
			_tmpBuffer[cnt++] = '0';
		}
		_tmpBuffer[cnt++] = '.';
		double r = 0.0;
		start = pow(10, prec - 1);
		float tv = value;
		if (tv < 0.0f) {
			tv *= -1.0f;
		}
		float frac = static_cast<float>(modf(tv, &r) * (start * 10));
		t = static_cast<int>(frac);
		for (int i = 0; i < prec; ++i) {
			int f = t / start;
			if (f >= 0) {
				_tmpBuffer[cnt++] = 48 + f;
			}
			t -= f * start;
			start /= 10;
		}
		_tmpBuffer[cnt++] = '\0';
	}

	void Value(const char* label, float v, uint8_t integers, uint8_t fractions) {
		simple_print(_guiCtx->tmpBuffer, v, integers,fractions);
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
	// Value - vec3
	// --------------------------------------------------------
	void Value(const char* label, const ds::vec3& v, const char* format) {
		sprintf_s(_guiCtx->tmpBuffer, 256, format, v.x, v.y, v.z);
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
		pos.x -= 10;
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
		bool ret = InputScalar(0, str, maxLength, 500);
		p2i p = _guiCtx->currentPos;
		p.x += 560;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		popID();
		return ret;
	}

	// -------------------------------------------------------
	// input string
	// -------------------------------------------------------
	bool TextBox(const char* label, char* str, int maxLength) {
		pushID(label);
		int width = maxLength * 10;
		bool ret = InputScalar(0, str, maxLength, width);
		p2i p = _guiCtx->currentPos;
		p.x += width + 10;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		popID();
		return ret;
	}

	// -------------------------------------------------------
	// input int
	// -------------------------------------------------------
	bool Input(const char* label, int* v) {
		pushID(label);
		float f = static_cast<float>(*v);
		bool fr = InputScalar(0, &f, "%g", 150);
		*v = static_cast<int>(f);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		return fr;
	}

	// -------------------------------------------------------
	// input float
	// -------------------------------------------------------
	bool Input(const char* label, float* v) {
		pushID(label);
		bool fr = InputScalar(0, v, "%g", 150);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		return fr;
	}

	// -------------------------------------------------------
	// input vec2
	// -------------------------------------------------------
	bool Input(const char* label, ds::vec2* v) {
		pushID(label, "##X");
		bool fr = InputScalar(0, &v->x, "%g", 70);
		_guiCtx->idStack.pop();
		pushID(label, "##Y");
		bool sr = InputScalar(1, &v->y, "%g", 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 160;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		return fr || sr;
	}

	// -------------------------------------------------------
	// input vec3
	// -------------------------------------------------------
	bool Input(const char* label, ds::vec3* v) {
		pushID(label, "##X");
		bool fr = InputScalar(0, &v->x, "%g", 70);
		popID();
		pushID(label, "##Y");
		bool sr = InputScalar(1, &v->y, "%g", 70);
		popID();
		pushID(label, "##Z");
		bool tr = InputScalar(2, &v->z, "%g", 70);
		popID();
		p2i p = _guiCtx->currentPos;
		p.x += 240;
		p2i ts = renderer::add_text(_guiCtx->uiContext, p, label);
		moveForward(p2i(150 + ts.x + 10, 22));
		return fr || sr || tr;
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
		int val = static_cast<int>(*v * 255.0f);
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
		p.x -= 10;
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
			val = minValue + static_cast<int>(dx * d / width);
		}
		if (isDragging()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			val = minValue + static_cast<int>(dx * d / width);
		}
		*v = val;
		if (*v < minValue) {
			*v = minValue;
		}
		if (*v > maxValue) {
			*v = maxValue;
		}
		p.x += static_cast<float>(*v - minValue) / static_cast<float>(d) * width;
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
			*v = minValue + dx * d / width;
		}
		if (isDragging()) {
			ds::vec2 mp = ds::getMousePosition();
			float dx = mp.x - p.x;
			*v = minValue + dx * d / width;
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
		p.x += (*v - minValue) / d * width;
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
		
		int barWidth = 10;
		p.y -= height / 2;
		float delta = maxValue - minValue;
		if (delta == 0.0f) {
			delta = 1.0f;
		}
		int dd = num - 1;
		float st = width / static_cast<float>(dd);
		int bw = width / num;
		if (bw < 2) {
			bw = 2;
		}
		renderer::add_box(_guiCtx->uiContext, p, p2i(width, height), ds::Color(51, 51, 51, 255));
		char buffer[16];
		for (int i = 0; i < num; ++i) {
			float v = values[i];
			if (v > maxValue) {
				v = maxValue;
			}
			if (v < minValue) {
				v = minValue;
			}
			int current = (v - minValue) / delta;
			int yp = current * height;
			p = _guiCtx->currentPos;
			p.y -= (height - yp / 2);
			p.x += i * bw + 2;
			renderer::add_box(_guiCtx->uiContext, p, p2i(bw - 1, yp), ds::Color(192, 0, 0, 255));
		}
		float steps = delta / step;
		int stp = static_cast<int>(steps);
		float hs = height / steps;
		for (int i = 0; i < stp; ++i) {
			p = _guiCtx->currentPos;
			p.y -= (stp - i) * hs;
			renderer::add_box(_guiCtx->uiContext, p, p2i(width + barWidth, 1), ds::Color(16, 16, 16, 255));
		}
		for (int i = 0; i < stp + 1; ++i) {
			p = _guiCtx->currentPos;
			p.y -= (stp - i) * hs;
			p.x += width + 20;
			sprintf_s(buffer, 16, "%g", (step * i));
			renderer::add_text(_guiCtx->uiContext, p, buffer);
		}
		moveForward(p2i(width, height + 30));
	}

	// -------------------------------------------------------
	// Diagram
	// -------------------------------------------------------	
	void DiagramInternal(const p2i& pos, float* values, int num, float minValue, float maxValue, float step, int width, int height) {
		p2i p = pos;
		
		p.y -= height / 2;
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
			p.y = pos.y - height + static_cast<int>(yp);
			p.x += static_cast<float>(i)* st - 2.0f;
			renderer::add_box(_guiCtx->uiContext, p, p2i(4, 4), ds::Color(192, 0, 0, 255));
		}

		step = delta / 10.0f;
		int d = static_cast<int>(delta / step) + 1;
		for (int i = 0; i < d; ++i) {
			p = pos;
			float current = 1.0f - (step*i) / delta;
			float yp = current * height;
			p.y -= static_cast<int>(yp);
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
			renderer::add_box(_guiCtx->uiContext, p, p2i(20, sideHeight), ds::Color(30, 30, 30, 255));
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

			float d = 1.0f - static_cast<float>(*offset) / static_cast<float>(size - max);
			int dy = static_cast<int>(d) * (sideHeight) - 30;
			p.y = cy + dy;
			renderer::add_box(_guiCtx->uiContext, p, p2i(20, 6), _guiCtx->settings.scrollSliderColor);
			popID();
			
		}
	}
	// -------------------------------------------------------
	// ListBox
	// -------------------------------------------------------	
	bool ListBox(const char* label, const char** entries, int num, int* selected, int *offset, int max) {
		bool changed = false;
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
				if (i != *selected) {
					changed = true;
				}
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
		return false;
	}

	// -------------------------------------------------------
	// ListBox using ListBoxModel
	// -------------------------------------------------------	
	bool ListBox(const char* label, ListBoxModel& model,int max) {
		bool changed = false;
		bool scrolling = false;
		pushID(label);
		int width = 200;
		int offset = model.getOffset();
		int num = model.size();
		if (num > max) {
			scrolling = true;
		}
		prepareComboBox(_guiCtx->idStack.last(), &offset, num, max);
		int selected = model.getSelection();
		p2i p = _guiCtx->currentPos;
		int height = max * 20;
		int start = offset;
		int end = offset + max;
		if (end >= num) {
			end = num;
		}
		for (int i = start; i < end; ++i) {
			pushID(i);
			checkItem(p, p2i(width, 20));
			if (isClicked()) {
				selected = i;
			}
			if (selected == i) {
				int ww = width;
				if (!scrolling) {
					ww += 20;
				}
				renderer::add_box(_guiCtx->uiContext, p, p2i(ww, 20), _guiCtx->settings.boxSelectionColor);
			}
			renderer::add_text(_guiCtx->uiContext, p, model.name(i));
			p.y -= 20;
			popID();
		}
		model.setOffset(offset);
		if (selected != model.getSelection()) {
			changed = true;
		}
		model.setSelection(selected);
		moveForward(p2i(width, height + 4));
		popID();
		return changed;
	}

	// -------------------------------------------------------
	// Tabs
	// -------------------------------------------------------	
	bool Tabs(const char** entries, int num, int* selected) {
		bool changed = false;
		pushID("Tabs",entries[0]);
		p2i p = _guiCtx->currentPos;
		p.x -= 10;
		int tab_size = (_guiCtx->width + 10) / num;
		for (int i = 0; i < num; ++i) {
			pushID(i);
			checkItem(p, p2i(tab_size, 20));
			if (isClicked()) {
				if (*selected != i) {
					changed = true;
				}
				*selected = i;
			}
			if (*selected == i) {
				renderer::add_box(_guiCtx->uiContext, p, p2i(tab_size, 20), ds::Color(43,126,196,255));
			}
			else {
				renderer::add_box(_guiCtx->uiContext, p, p2i(tab_size, 20), ds::Color(54,61,69,255));
			}
			p.x += 5;
			renderer::add_text(_guiCtx->uiContext, p, entries[i]);
			p.x += tab_size - 5;
			popID();
		}
		popID();
		moveForward(p2i(300, 30));
		return changed;
	}

	bool BeginMenu(const char** entries, int num, int* selected) {
		start_overlay();
		bool changed = false;
		pushID("Menu", entries[0]);
		p2i p = _guiCtx->currentPos;
		p.x -= 10;
		int tab_size = (_guiCtx->width - 10) / num;
		for (int i = 0; i < num; ++i) {
			pushID(i);
			p2i ts = textSize(entries[i]);
			ts.x += 20;			
			checkItem(p, p2i(ts.x, 20));
			bool hot = isHot(p, p2i(ts.x, 20));
			if (isClicked()) {
				if (*selected != i) {
					changed = true;
				}
				*selected = i;
				_guiCtx->menuPos = p;
			}
			if (*selected == i || hot) {
				renderer::add_box(_guiCtx->uiContext, p, p2i(ts.x, 20), ds::Color(43,126,196,255));
			}
			else {
				renderer::add_box(_guiCtx->uiContext, p, p2i(ts.x, 20), ds::Color( 54, 61, 69, 255));
			}
			p.x += 5;
			renderer::add_text(_guiCtx->uiContext, p, entries[i]);
			p.x += ts.x - 5;
			popID();
		}
		popID();
		moveForward(p2i(300, 26));
		return changed;
	}

	bool MenuItems(const char** entries, int num, int* selected) {
		bool changed = false;
		pushID("MenuItems", entries[0]);
		p2i p = _guiCtx->menuPos;
		p.y -= 20;
		int height = 0;
		int tab_size = 0;
		for (int i = 0; i < num; ++i) {
			p2i ts = textSize(entries[i]);
			if (ts.x > tab_size) {
				tab_size = ts.x;
			}
		}
		tab_size += 30;
		for (int i = 0; i < num; ++i) {
			pushID(i);
			p2i ts = textSize(entries[i]);
			ts.x += 20;
			checkItem(p, p2i(ts.x, 20));
			bool hot = isHot(p, p2i(ts.x, 20));
			if (isClicked()) {
				if (*selected != i) {
					changed = true;
				}
				*selected = i;
				_guiCtx->menuPos = p;
			}			
			if (*selected == i || hot) {
				renderer::add_box(_guiCtx->uiContext, p, p2i(tab_size, 20), ds::Color(43, 126, 196, 255));
			}
			else {
				renderer::add_box(_guiCtx->uiContext, p, p2i(tab_size, 20), ds::Color(54, 61, 69, 255));
			}
			p.x += 5;
			renderer::add_text(_guiCtx->uiContext, p, entries[i]);
			p.x -= 5;
			p.y -= 20;
			height += 20;
			popID();
		}
		popID();
		return changed;
	}

	void EndMenu() {
		end_overlay();
	}

	// -------------------------------------------------------
	// DropDownBox
	// -------------------------------------------------------	
	bool DropDownBox(const char* label, const char** entries, int num, int* state, int* selected, int *offset, int max, bool closeOnSelection) {
		bool changed = false;
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
					if (*selected != i) {
						changed = true;
					}
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
		return changed;
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
		_guiCtx->size = p2i(0);
		_guiCtx->grouping = false;
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