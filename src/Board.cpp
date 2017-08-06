#include "Board.h"
#include "GameSettings.h"
#include "utils\tweening.h"
#include "utils\utils.h"
#include "utils\colors.h"
#include "utils\HUD.h"
#include <ds_imgui.h>

const static ds::vec4 TEXTURE = ds::vec4(0, 0, CELL_SIZE, CELL_SIZE);

ds::vec2 convertFromGrid(int gx, int gy) {
	return ds::vec2(STARTX + gx * CELL_SIZE, STARTY + gy * CELL_SIZE);
}

Board::Board(SpriteBatchBuffer* buffer, GameContext* context, GameSettings* settings) : _buffer(buffer) , _gameContext(context), _settings(settings) {
	_messages[0] = Message{ 0.0f, _settings->prepareTTL, 1.0f, ds::Color(255,255,255,255), ds::vec4(  200, 440, 515, 55), 0.0f, false };
	_messages[1] = Message{ 0.0f, 0.8f, 1.0f, ds::Color(255,255,255,255), ds::vec4(75, 5,  130, 55), 0.0f, false };
	_numMatches = 0;
	_numMovesLeft = 0;
	_numDroppedCells = 0;
	_numMoving = 0;
	_highlightTimer = 0.0f;
	m_Mode = BM_IDLE;
}

Board::~Board(void) {}

// -------------------------------------------------------
// Fill board
// -------------------------------------------------------
void Board::fill(int maxColors) {
	_cellCounter = 0;
	for ( int x = 0; x < MAX_X; ++x ) {		
		for ( int y = 0; y < MAX_Y; ++y ) {		
			int cid = ds::random(0, maxColors);
			int offset = offset = cid * CELL_SIZE;
			ds::vec2 p = convertFromGrid(x, y);
			MyEntry& e = m_Grid.get(x, y);
			e.color = cid;
			e.hidden = false;
			e.scale = 1.0f;
			e.state = TS_NORMAL;
			e.timer = 0.0f;
			e.ttl = ds::random(_settings->scaleUpMinTTL, _settings->scaleUpMaxTTL);
			m_Grid.set(x, y, e);
			++_cellCounter;
		}
	}
	m_Mode = BM_PREPARE;
	m_Timer = 0.0f;
	m_Counter = MAX_X * MAX_Y;
	_selectedX = -1;
	_selectedY = -1;
	_flashCount = 0;
	_numMatches = 0;	
	m_Grid.calculateValidMoves();
	activateMessage(0);
	_numDroppedCells = 0;
	_numMovesLeft = m_Grid.getNumberOfMoves();
	_numMoving = 0;
	_highlightTimer = 0.0f;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void Board::render() {
	_buffer->add(ds::vec2(512, 140), ds::vec4(0,810,840,10));
	if (m_Mode != BM_PREPARE) {
		// pieces
		for (int x = 0; x < MAX_X; ++x) {
			for (int y = 0; y < MAX_Y; ++y) {
				if (m_Grid.isUsed(x, y)) {
					MyEntry& e = m_Grid.get(x, y);
					if (!e.hidden) {
						_buffer->add(convertFromGrid(x, y), TEXTURE, ds::vec2(e.scale), 0.0f, _gameContext->colors[e.color]);
					}
				}
			}
		}

		// moving cells
		for (size_t i = 0; i < _numMoving; ++i) {
			_buffer->add(_movingCells[i].current, TEXTURE, ds::vec2(1, 1), 0.0f, _gameContext->colors[_movingCells[i].color]);
		}
	}

	for (int i = 0; i < 2; ++i) {
		const Message& msg = _messages[i];
		if (msg.active) {
			_buffer->add(ds::vec2(512, 384), msg.texture,ds::vec2(msg.scale),msg.rotation, msg.color);
		}
	}
}

// -------------------------------------------------------
// scale pieces for fade in / out
// -------------------------------------------------------
ScaleState Board::scalePieces(float elapsed, ScaleMode scaleMode) {
	int cnt = 0;
	int total = 0;
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				++total;
				MyEntry& e = m_Grid.get(x, y);
				e.timer += elapsed;
				float norm = e.timer / e.ttl;
				if (norm > 1.0f) {
					norm = 1.0f;
					++cnt;
				}
				if (scaleMode == ScaleMode::SM_UP) {
					e.scale = norm;
				}
				else {
					e.scale = 1.0f - norm;
				}
			}
		}
	}
	if (cnt == total) {
		return SCS_DONE;
	}
	return SCS_RUNNING;
}

// -------------------------------------------------------
// activate message
// -------------------------------------------------------
void Board::activateMessage(int idx) {
	Message& msg = _messages[idx];
	msg.timer = 0.0f;
	msg.active = true;
	msg.scale = 1.0f;
	msg.rotation = 0.0f;
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void Board::update(float elapsed) {

	// update message
	for (int i = 0; i < 2; ++i) {
		Message& msg = _messages[i];
		if (msg.active) {
			msg.timer += elapsed;
			if (msg.timer >= msg.ttl) {
				msg.active = false;
				msg.timer = 0.0f;
			}
			msg.scale = 1.0f + sin(msg.timer / msg.ttl * ds::PI) * _settings->messageScale;
		}
	}
	
	if (m_Mode == BM_FILLING) {
		if ( scalePieces(elapsed,ScaleMode::SM_UP) == ScaleState::SCS_DONE) {
			m_Mode = BM_READY;
			activateMessage(1);
			_highlightTimer = 0.0f;
		}
	}

	else if (m_Mode == BM_PREPARE) {
		m_Timer += elapsed;
		if (m_Timer >= _settings->prepareTTL) {
			m_Timer = 0.0f;
			m_Mode = BM_FILLING;
		}
	}

	else if (m_Mode == BM_FLASHING) {
		m_Timer += elapsed;
		if (m_Timer > _settings->flashTTL) {
			m_Mode = BM_READY;
			m_Timer = 0.0f;
			m_Grid.remove(_matches, _numMatches,true);
			_numDroppedCells = 0;
			_numMoving = 0;
			_numDroppedCells = m_Grid.dropCells(_droppedCells,TOTAL);
			for (size_t i = 0; i < _numDroppedCells; ++i) {
				const ds::DroppedCell<MyEntry>& dc = _droppedCells[i];
				ds::p2i to = dc.to;
				MyEntry& e = m_Grid.get(to.x, to.y);
				e.hidden = true;
				MovingCell m;
				m.x = to.x;
				m.y = to.y;
				m.color = e.color;
				m.start = convertFromGrid(dc.from.x, dc.from.y);
				m.end = convertFromGrid(to.x, to.y);
				_movingCells[_numMoving++] = m;
			}
			if (_numDroppedCells > 0) {
				m_Mode = BM_MOVING;
				m_Timer = 0.0f;
			}			
		}
		
	}
	else if (m_Mode == BM_MOVING) {
		m_Timer += elapsed;
		if (m_Timer >= _settings->droppingTTL) {
			m_Mode = BM_READY;
			m_Timer = 0.0f;
			for (size_t i = 0; i <_numMoving; ++i) {
				MovingCell& m = _movingCells[i];
				MyEntry& e = m_Grid.get(m.x, m.y);
				e.hidden = false;
			}
			_numMoving = 0;
		}
		else {
			if (m_Timer < _settings->droppingTTL) {
				float norm = m_Timer / _settings->droppingTTL;
				for (size_t i = 0; i < _numMoving; ++i) {
					MovingCell& m = _movingCells[i];
					m.current = tweening::interpolate(&tweening::linear, m.start, m.end, m_Timer, _settings->droppingTTL);
				}
			}
		}
	}
	else if (m_Mode == BM_READY) {
		ds::vec2 mousePos = ds::getMousePosition();
		int mx = -1;
		int my = -1;
		if ( input::convertMouse2Grid(&mx,&my)) {
			if (mx != _selectedX || my != _selectedY) {
				_selectedX = mx;
				_selectedY = my;
				MyEntry& me = m_Grid(mx, my);
				if (me.state == TS_NORMAL) {
					me.timer = 0.0f;
					me.state = TS_WIGGLE;
				}
			}
		}
	}

	else if (m_Mode == BM_CLEARING) {
		if (scalePieces(elapsed, ScaleMode::SM_DOWN) == ScaleState::SCS_DONE) {
			m_Mode = BM_IDLE;
		}
	}
	
	// update all pieces
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				MyEntry& e = m_Grid.get(x, y);
				if (e.state == TS_SHRINKING) {
					e.timer += elapsed;
					if (e.timer >= _settings->flashTTL) {
						e.state = TS_NORMAL;
						e.scale = 1.0f;
						--_flashCount;
					}
					else {
						float norm = e.timer /_settings->flashTTL;
						e.scale = 1.0f - norm * 0.9f;
					}
				}
				else if (e.state == TS_WIGGLE) {
					e.timer += elapsed;
					if (e.timer >= _settings->wiggleTTL) {
						e.state = TS_NORMAL;
						e.scale = 1.0f;
					}
					else {
						float norm = e.timer / _settings->wiggleTTL;
						e.scale = 1.0f + sin(norm * ds::TWO_PI * 2.0f) * _settings->wiggleScale;
					}
				}
			}
		}
	}

	if (m_Mode == BM_READY) {
		// check if we should help the player and give him a hint
		_highlightTimer += elapsed;
		if (_highlightTimer > _settings->highlightTime) {
			highlightBlock();
			_highlightTimer = 0.0f;
		}
	}
}

// -------------------------------------------------------
// highlight a matching block to help player
// -------------------------------------------------------
void Board::highlightBlock() {
	int num = m_Grid.getMatchingBlock(_matches, TOTAL);
	for (int i = 0; i < num; ++i) {
		const ds::p2i& m = _matches[i];
		MyEntry& me = m_Grid(m.x, m.y);
		if (me.state == TS_NORMAL) {
			me.timer = 0.0f;
			me.state = TS_WIGGLE;
		}
	}
}
// -------------------------------------------------------
// start clearing board
// -------------------------------------------------------
void Board::clearBoard() {
	m_Mode = BM_CLEARING;
	m_Timer = 0.0f;
	for (int x = 0; x < MAX_X; ++x) {
		for (int y = 0; y < MAX_Y; ++y) {
			if (!m_Grid.isFree(x, y)) {
				MyEntry& e = m_Grid.get(x, y);
				e.timer = 0.0f;
				e.ttl = ds::random(_settings->clearMinTTL, _settings->clearMaxTTL);
			}
		}
	}
}

// -------------------------------------------------------
// move all columns to fill the empty gaps
// -------------------------------------------------------
void Board::move() {
	m_Grid.fillGaps();
}

// -------------------------------------------------------
// Select
// -------------------------------------------------------
bool Board::select(Score* score) {
	if ( m_Mode == BM_READY ) {
		_highlightTimer = 0.0f;
		int cx = -1;
		int cy = -1;
		if (input::convertMouse2Grid(&cx,&cy)) {
			MyEntry& me = m_Grid(cx, cy);
			_numMatches = m_Grid.findMatchingNeighbours(cx,cy,_matches,TOTAL);
			if (_numMatches > 1 ) {
				m_Timer = 0.0f;
				m_Mode = BM_FLASHING;
				score->points += _numMatches * _numMatches  * 10;
				if (_numMatches > score->highestCombo) {
					score->highestCombo = _numMatches;
				}
				score->itemsCleared += _numMatches;
				_cellCounter -= _numMatches;
				score->piecesLeft = _cellCounter;
				for ( size_t i = 0; i < _numMatches; ++i ) {
					const ds::p2i& gp = _matches[i];
					MyEntry& c = m_Grid.get(gp.x, gp.y);
					c.state = TS_SHRINKING;
					c.timer = 0.0f;
					++_flashCount;
				}
				return true;
			}
		}
	}
	return false;
}

void Board::debug() {
	p2i start = gui::getCurrentPosition();
	p2i p = start;
	//std::string dbgTxt;
	//char buffer[32];
	for (int y = m_Grid.height() - 1; y >= 0; --y) {
		//dbgTxt.clear();
		for (int x = 0; x < m_Grid.width(); ++x) {
			ds::p2i gp(x, y);
			if (m_Grid.isUsed(x, y)) {
				const MyEntry& c = m_Grid.get(gp.x, gp.y);			
				gui::draw_box(p, p2i(6, 6), _gameContext->colors[c.color]);
				//sprintf_s(buffer, 32, "%2d ", c.color);
			}
			else {
				gui::draw_box(p, p2i(6, 6), ds::Color(255, 255, 255, 255));
				//sprintf_s(buffer, 32, " x ");
			}
			//dbgTxt.append(buffer);
			p.x += 10;
		}
		p.x = start.x;
		p.y -= 10;
		//gui::FormattedText("%2d: %s", y, dbgTxt.c_str());
	}
	gui::moveForward(p2i(200, 150));
}