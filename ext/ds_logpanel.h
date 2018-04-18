#pragma once
#include <stdint.h>

namespace logpanel {

	void init(uint16_t maxLines);

	void add_line(const char* message);

	uint16_t get_num_lines();

	const char* get_line(uint16_t index);

	void draw_gui(uint16_t lines);

	void shutdown();
}

#ifdef DS_LOG_PANEL

#include <string.h>
#include <ds_imgui.h>

namespace logpanel {

	const uint16_t LINE_SIZE = 128;

	// -------------------------------------------------------
	// PanelContext
	// -------------------------------------------------------
	struct PanelContext {
		char* text;
		uint16_t index;
		uint16_t capacity;
		uint16_t offset;
	};

	static PanelContext* _panelCtx = 0;

	// -------------------------------------------------------
	// init
	// -------------------------------------------------------
	void init(uint16_t maxLines) {
		_panelCtx = new PanelContext;
		_panelCtx->capacity = maxLines;
		_panelCtx->offset = 0;
		_panelCtx->text = new char[LINE_SIZE * maxLines];
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
		if (_panelCtx->index + 1 >= _panelCtx->capacity) {
			uint16_t end = LINE_SIZE * (_panelCtx->capacity - 1);
			memcpy(_panelCtx->text, _panelCtx->text + LINE_SIZE, end * sizeof(char));
			idx = (_panelCtx->capacity - 1) * LINE_SIZE;
			_panelCtx->index = _panelCtx->capacity - 1;
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
	void draw_gui(uint16_t lines) {
		uint16_t ml = lines;
		if (ml > _panelCtx->capacity) {
			ml = _panelCtx->capacity;
		}
		p2i sp(0, lines * 20 - 10);
		gui::start(&sp, ds::getScreenWidth());
		p2i p = gui::getCurrentPosition();
		p2i dp = p;
		dp.y -= (lines * 20) / 2 - 10;
		int height = lines * 20;
		gui::draw_box(dp, p2i(ds::getScreenWidth(), lines * 20), ds::Color(24, 24, 24, 255));

		if (lines < get_num_lines()) {
			int current = get_num_lines();
			p2i ep = p;
			ep.x = ds::getScreenWidth() - 20.0f;
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
			ep.x = ds::getScreenWidth() - 20.0f;
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
		int end = logpanel::get_num_lines();
		int start = end - ml;
		if (start < 0) {
			start = 0;
		}
		start -= _panelCtx->offset;
		end -= _panelCtx->offset;
		for (uint16_t i = start; i < end; ++i) {
			gui::draw_text(p, logpanel::get_line(i));
			p.y -= 20.0f;
		}
		gui::moveForward(p2i(ds::getScreenWidth(), lines * 20));
		gui::end();
	}

	// -------------------------------------------------------
	// shutdown
	// -------------------------------------------------------
	void shutdown() {
		if (_panelCtx != 0) {
			if (_panelCtx->text != 0) {
				delete[] _panelCtx->text;
			}
			delete _panelCtx;
		}
	}
}
#endif