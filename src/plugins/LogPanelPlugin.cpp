#include "LogPanelPlugin.h"
#include "ApiRegistry.h"
#include <string.h>
#include <ds_imgui.h>
#include <stdlib.h>
#include <Windows.h>

const uint16_t LINE_SIZE = 128;

// -------------------------------------------------------
// PanelContext
// -------------------------------------------------------
struct PanelContext {
	char* text;
	uint16_t index;
	uint16_t capacity;
	uint16_t total;
	uint16_t offset;
};

static PanelContext* _panelCtx = 0;

// -------------------------------------------------------
// init
// -------------------------------------------------------
void init(uint16_t maxLines) {
	_panelCtx = (PanelContext*)malloc(sizeof(PanelContext));
	_panelCtx->capacity = maxLines;
	_panelCtx->offset = 0;
	_panelCtx->total = LINE_SIZE * maxLines;
	//_panelCtx->text = new char[LINE_SIZE * maxLines];
	_panelCtx->text = (char*)malloc(sizeof(char) * _panelCtx->total);
	_panelCtx->index = 0;
}

// -------------------------------------------------------
// get number of lines
// -------------------------------------------------------
uint16_t get_num_lines() {
	return _panelCtx->index;
}

// -------------------------------------------------------
// get line
// -------------------------------------------------------
const char* get_line(uint16_t index) {
	return _panelCtx->text + index * LINE_SIZE;
}

// -------------------------------------------------------
// add line
// -------------------------------------------------------
void add_line(const char* message) {
	char b[128];
	uint16_t nr = 0;
	uint16_t idx = _panelCtx->index * LINE_SIZE;
	//if (_panelCtx->index < 16) {
		if (_panelCtx->index + 1 >= _panelCtx->capacity) {
			//uint16_t end = _panelCtx->total - LINE_SIZE;
			//memcpy(_panelCtx->text, _panelCtx->text + LINE_SIZE, end * sizeof(char));
			idx = (_panelCtx->capacity - 1) * LINE_SIZE;
			_panelCtx->index = _panelCtx->capacity - 1;
		}
		sprintf(b, "%d %d\n", idx, _panelCtx->index);
		OutputDebugString(b);
		const char* tmp = message;
		while (*tmp != 0) {
			if (nr < LINE_SIZE) {
				_panelCtx->text[idx + nr] = *tmp;
				++nr;
			}
			++tmp;
		}
		_panelCtx->text[idx + nr] = '\0';
		++_panelCtx->index;
	//}
}

// -------------------------------------------------------
// draw gui panel
// -------------------------------------------------------
void draw_gui(uint16_t lines) {
	uint16_t ml = lines;
	if (ml > _panelCtx->capacity) {
		ml = _panelCtx->capacity;
	}
	p2i sp(0, lines * 20 - 10);
	gui::begin(sp);
	p2i p = gui::getCurrentPosition();
	p2i dp = p;
	dp.y -= (lines * 20) / 2 - 10;
	int height = lines * 20;
	gui::draw_box(dp, p2i(900, lines * 20), ds::Color(48, 48, 48, 64));

	if (lines < get_num_lines()) {
		int current = get_num_lines();
		p2i ep = p;
		ep.x += 880.0f;
		gui::pushID("up");
		gui::checkItem(ep, p2i(20, 20));
		gui::draw_box(ep, p2i(20, 20), gui::getSettings().buttonColor);
		gui::draw_text(ep, "-");
		if (gui::isClicked()) {
			++_panelCtx->offset;
			if (_panelCtx->offset + lines >= current) {
				_panelCtx->offset = current - lines;
			}
		}
		gui::popID();

		ep = p;
		ep.x += 880.0f;
		ep.y -= (lines - 1) * 20;
		gui::pushID("down");
		gui::checkItem(ep, p2i(20, 20));
		gui::draw_box(ep, p2i(20, 20), gui::getSettings().buttonColor);
		gui::draw_text(ep, "+");
		if (gui::isClicked()) {
			if (_panelCtx->offset > 0) {
				--_panelCtx->offset;
			}
		}
		gui::popID();
		ep.y = p.y - height / 2 + 10;
		gui::draw_box(ep, p2i(20, height - 40), gui::getSettings().sliderColor);

		float d = 1.0f - static_cast<float>(_panelCtx->offset) / static_cast<float>(current - lines);
		int dy = d * (height - 60);
		ep.y = p.y - dy - 20;
		gui::draw_box(ep, p2i(20, 20), gui::getSettings().scrollSliderColor);

	}
	int end = get_num_lines();
	int start = end - ml;
	if (start < 0) {
		start = 0;
	}
	start -= _panelCtx->offset;
	end -= _panelCtx->offset;
	for (uint16_t i = start; i < end; ++i) {
		gui::draw_text(p, get_line(i));
		p.y -= 20.0f;
	}
	gui::moveForward(p2i(900, lines * 20));
	gui::end();
}

static struct log_panel_plugin INSTANCE = { add_line, get_num_lines,get_line,draw_gui };

void load_logpanel_plugin(struct ds_api_registry* registry) {
	init(16);
	registry->add(LOG_PANEL_PLUGIN_NAME, &INSTANCE);	
}

void unload_logpanel_plugin(struct ds_api_registry* registry) {
	if (_panelCtx != 0) {
		if (_panelCtx->text != 0) {
			free(_panelCtx->text);
		}
		free(_panelCtx);
	}
}