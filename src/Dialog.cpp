#include "Dialog.h"
#include <vector>
#include "Constants.h"
#include <stdarg.h>
#include "utils\utils.h"
#include "utils\tweening.h"
#include "GameSettings.h"
#include "utils\HUD.h"

namespace dialog {

	struct DrawCall {
		ds::vec2 pos;
		ds::vec4 rect;
	};

	struct GUIContext {
		SpriteBatchBuffer* buffer;
		std::vector<DrawCall> calls;
		bool clicked;
		bool buttonPressed;
	};

	static GUIContext* _guiCtx = 0;

	// -------------------------------------------------------
	// check if mouse cursor is inside box
	// -------------------------------------------------------
	static bool isCursorInside(const ds::vec2& p, const ds::vec2& dim) {
		ds::vec2 mp = ds::getMousePosition();
		if (mp.x < (p.x - dim.x * 0.5f)) {
			return false;
		}
		if (mp.x >(p.x + dim.x * 0.5f)) {
			return false;
		}
		if (mp.y < (p.y - dim.y * 0.5f)) {
			return false;
		}
		if (mp.y >(p.y + dim.y * 0.5f)) {
			return false;
		}
		return true;
	}

	// -------------------------------------------------------
	// handle mouse interaction
	// -------------------------------------------------------
	static bool isClicked(const ds::vec2& pos, const ds::vec2& size) {
		if (_guiCtx->clicked) {
			ds::vec2 p = pos;
			//p.x += size.x * 0.5f;
			if (_guiCtx->clicked && isCursorInside(p, size)) {
				return true;
			}
		}
		return false;
	}

	void init(SpriteBatchBuffer* buffer) {
		_guiCtx = new GUIContext;
		_guiCtx->buffer = buffer;
		_guiCtx->clicked = false;
		_guiCtx->buttonPressed = false;
	}

	void begin() {
		_guiCtx->calls.clear();
		if (_guiCtx->clicked) {
			_guiCtx->clicked = false;
		}
		if (ds::isMouseButtonPressed(0)) {
			_guiCtx->buttonPressed = true;
		}
		else {
			if (_guiCtx->buttonPressed) {
				_guiCtx->clicked = true;
			}
			_guiCtx->buttonPressed = false;
		}
	}

	bool Button(const ds::vec2& pos, const ds::vec4& rect) {
		DrawCall call;
		call.pos = pos;
		call.rect = rect;
		_guiCtx->calls.push_back(call);
		ds::vec2 dim = ds::vec2(rect.z, rect.w);
		return isClicked(pos, dim);
	}

	void Image(const ds::vec2& pos, const ds::vec4& rect) {
		DrawCall call;
		call.pos = pos;
		call.rect = rect;
		_guiCtx->calls.push_back(call);
	}

	void Text(const ds::vec2& pos, const char* text, bool centered) {
		int l = strlen(text);
		ds::vec2 p = pos;
		ds::vec2 size = font::textSize(text);
		if (centered) {
			p.x = (1024.0f - size.x) * 0.5f;
		}
		float lw = 0.0f;
		for (int i = 0; i < l; ++i) {
			ds::vec4 r = font::get_rect(text[i]);
			p.x += lw * 0.5f + r.z * 0.5f;
			DrawCall call;
			call.pos = p;
			call.rect = r;
			_guiCtx->calls.push_back(call);
			lw = r.z;
		}
	}

	void FormattedText(const ds::vec2& pos, bool centered, const char* fmt, ...) {
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsprintf(buffer, fmt, args);
		ds::vec2 size = font::textSize(buffer);
		ds::vec2 p = pos;
		if (centered) {
			p.x = (1024.0f - size.x) * 0.5f;
		}
		Text(p, buffer, centered);
		va_end(args);
		
	}

	void end() {
		for (size_t i = 0; i < _guiCtx->calls.size(); ++i) {
			const DrawCall& call = _guiCtx->calls[i];
			_guiCtx->buffer->add(call.pos, call.rect);
		}
	}

	void shutdown() {
		if (_guiCtx != 0) {
			delete _guiCtx;
		}
	}

}

enum FloatInDirection {
	FID_LEFT,
	FID_RIGHT
};

static int floatButton(float time, float ttl, FloatInDirection dir) {
	if (time <= ttl) {
		if (dir == FloatInDirection::FID_LEFT) {
			return tweening::interpolate(tweening::easeOutElastic, -200, 512, time, ttl);
		}
		else {
			return tweening::interpolate(tweening::easeOutElastic, 1020, 512, time, ttl);
		}
	}
	return 512;
}

const static int LOGO_Y_POS = 600;

HighscoreDialog::HighscoreDialog(GameSettings* settings) : _settings(settings) , _timer(0.0f) {

}

HighscoreDialog::~HighscoreDialog() {

}

void HighscoreDialog::start() {
	_timer = 0.0f;
	_mode = 0;
	_offsetTimer = 0.0f;
}

int HighscoreDialog::tick(float dt) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	_timer += dt;
	if (_timer <= _settings->logoSlideTTL) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, _timer, _settings->logoSlideTTL);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 560, 560, 55));
	int dx = floatButton(_timer, _settings->logoSlideTTL, FloatInDirection::FID_LEFT);
	dialog::Image(ds::vec2(512, 500), ds::vec4(540, 160, 400, 30));
	if (_mode == 0) {
		dialog::Text(ds::vec2(512, 500), "ZEN Modus");
	}
	else {
		dialog::Text(ds::vec2(512, 500), "Timer Modus");
	}
	_offsetTimer += dt;
	if (_offsetTimer > 1.0f) {
		_offsetTimer -= 1.0f;		
		_offset += 5;
		if (_offset >= 10) {
			_offset = 0;
			_mode = (_mode + 1) & 1;
		}
	}
	char buffer[128];
	for (int i = 0; i < 5; ++i) {
		int idx = _offset + _mode * 10;
		dialog::Image(ds::vec2(512, 450 - i * 50), ds::vec4(0, 825, 700, 36));
		sprintf(buffer, "%d. Name 66800", (_offset + i + 1));
		dialog::Text(ds::vec2(512, 450 - i * 50), buffer);
	}
	if (dialog::Button(ds::vec2(dx, 160), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

static const char* GAME_OVER_LABELS[] = { "Pieces cleared", "Time", "Highest combo", "Total Score" };
// ---------------------------------------------------------------
// show game over menu
// ---------------------------------------------------------------
int showGameOverMenu(const Score& score, float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(200, 500, 540, 55));

	int y = 540;
	int x = 150;
	for (int i = 0; i < 4; ++i) {
		dialog::Image(ds::vec2(x + 200, y - i * 60), ds::vec4(540, 160, 450, 40));
		dialog::Image(ds::vec2(x + 550, y - i * 60), ds::vec4(540, 160, 200, 40));
		dialog::Text(ds::vec2(x, y - i * 60), GAME_OVER_LABELS[i], false);
	}

	dialog::FormattedText(ds::vec2(x + 540, y), false, "%d", score.itemsCleared);
	dialog::FormattedText(ds::vec2(x + 480, y - 60), false, "%02d %02d", score.minutes, score.seconds);
	dialog::FormattedText(ds::vec2(x + 540, y - 120), false, "%d", score.highestCombo);
	dialog::FormattedText(ds::vec2(x + 460, y - 180), false, "%06d", score.points);

	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 260), ds::vec4(0, 70, 260, 60))) {
		ret = 1;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 170), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

// ---------------------------------------------------------------
// show main menu
// ---------------------------------------------------------------
int showMainMenu(float time, float ttl) {
	int ret = 0;
	dialog::begin();
	int dy = LOGO_Y_POS;
	if (time <= ttl) {
		dy = tweening::interpolate(tweening::easeOutElastic, 1000, LOGO_Y_POS, time, ttl);
	}
	dialog::Image(ds::vec2(512, dy), ds::vec4(225, 5, 770, 55));
	dialog::Image(ds::vec2(512, 35), ds::vec4(0, 955, 530, 12));
	int dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 420), ds::vec4(0, 70, 260, 60))) {
		ret = 1;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_RIGHT);
	if (dialog::Button(ds::vec2(dx, 290), ds::vec4(270, 70, 260, 60))) {
		ret = 3;
	}
	dx = floatButton(time, ttl, FloatInDirection::FID_LEFT);
	if (dialog::Button(ds::vec2(dx, 160), ds::vec4(270, 130, 260, 60))) {
		ret = 2;
	}
	dialog::end();
	return ret;
}

