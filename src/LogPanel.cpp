#include "LogPanel.h"
#include <string.h>
#include <ds_imgui.h>

namespace logpanel {

	const uint16_t NUM_LINES = 16;
	const uint16_t LINE_SIZE = 128;

	// -------------------------------------------------------
	// PanelContext
	// -------------------------------------------------------
	struct PanelContext {
		char text[NUM_LINES * LINE_SIZE];
		uint16_t index;
	};

	static PanelContext* _panelCtx = 0;

	// -------------------------------------------------------
	// init
	// -------------------------------------------------------
	// FIXME: int maxLines -> maximum number of lines to store
	void init() {
		_panelCtx = new PanelContext;
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
		uint16_t nr = 0;
		uint16_t idx = _panelCtx->index * LINE_SIZE;
		if (_panelCtx->index + 1 >= NUM_LINES) {
			uint16_t end = LINE_SIZE * (NUM_LINES - 1);
			memcpy(_panelCtx->text, _panelCtx->text + LINE_SIZE, end * sizeof(char));
			idx = (NUM_LINES - 1) * LINE_SIZE;
			_panelCtx->index = NUM_LINES - 1;
		}
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
	}

	// -------------------------------------------------------
	// draw gui panel
	// -------------------------------------------------------
	void draw_gui(int lines) {
		p2i sp(0, lines * 20 - 10);
		gui::begin(sp);
		p2i p = gui::getCurrentPosition();
		p2i dp = p;
		dp.y -= (lines * 20) / 2 - 10;
		gui::draw_box(dp, p2i(900, lines * 20), ds::Color(32, 32, 32, 64));
		// FIXME: add scrolling and offset
		int end = logpanel::get_num_lines();
		int start = end - lines;
		if (start < 0) {
			start = 0;
		}		
		for (uint16_t i = start; i < end; ++i) {
			gui::draw_text(p, logpanel::get_line(i));
			p.y -= 20.0f;
		}
		gui::moveForward(p2i(900, lines * 20));
		gui::end();
	}

	// -------------------------------------------------------
	// shutdown
	// -------------------------------------------------------
	void shutdown() {
		if (_panelCtx != 0) {
			delete _panelCtx;
		}
	}
}