#pragma once
#include "Grid.h"
#include <diesel.h>
#include <SpriteBatchBuffer.h>
#include "Constants.h"

struct Score;
struct GameSettings;
struct GameContext;
// -------------------------------------------------------
// tile state
// -------------------------------------------------------
enum TileState {
	TS_NORMAL,
	TS_WIGGLE,
	TS_SHRINKING
};

// -------------------------------------------------------
// our specialized grid entry
// -------------------------------------------------------
struct MyEntry {
	int color;
	bool hidden;
	float scale;
	float timer;
	float ttl;
	TileState state;
};

// -------------------------------------------------------
// moving cell
// -------------------------------------------------------
struct MovingCell {

	int x;
	int y;
	ds::vec2 start;
	ds::vec2 end;
	ds::vec2 current;
	int color;
};

// -------------------------------------------------------
// Color grid
// -------------------------------------------------------
class ColorGrid : public ds::Grid<MyEntry> {

public:
	ColorGrid() : ds::Grid<MyEntry>(MAX_X, MAX_Y) {}
	virtual ~ColorGrid() {}
	bool isMatch(const MyEntry& first, const MyEntry& right) {
		return first.color == right.color;
	}
};

enum ScaleState {
	SCS_IDLE,
	SCS_RUNNING,
	SCS_DONE
};
// -------------------------------------------------------
// Board
// -------------------------------------------------------
class Board {

struct Message {
	float timer;
	float ttl;
	float scale;
	ds::Color color;
	ds::vec4 texture;
	float rotation;
	bool active;
};

enum BoardMode {
	BM_PREPARE,
	BM_FILLING,
	BM_FLASHING,
	BM_MOVING,
	BM_READY,
	BM_CLEARING,
	BM_IDLE
};

enum ScaleMode {
	SM_UP,
	SM_DOWN
};

public:
	Board(SpriteBatchBuffer* buffer, GameContext* context, GameSettings* settings);
	virtual ~Board();
	void fill(int maxColors);
	bool select(Score* score);
	void move();
	int getMovesLeft() {
		return 100;
	}
	void update(float elasped);
	void render();
	bool isReady() const {
		return m_Mode != BM_FILLING;
	}
	void clearBoard();
	int getNumberOfMoves() {
		return m_Grid.getNumberOfMoves();
	}
	void highlightBlock();
	void debug();
private:
	void activateMessage(int idx);
	ScaleState scalePieces(float elapsed, ScaleMode scaleMode);
	ColorGrid m_Grid;
	ds::DroppedCell<MyEntry> _droppedCells[TOTAL];
	int _numDroppedCells;
	MovingCell _movingCells[TOTAL];
	int _numMoving;
	BoardMode m_Mode;
	float m_Timer;
	int m_Counter;
	GameSettings* _settings;
	int _flashCount;

	int _cellCounter;
	int _selectedX;
	int _selectedY;
	SpriteBatchBuffer* _buffer;
	Message _messages[2];
	int _numMovesLeft;
	ds::p2i _matches[TOTAL];
	int _numMatches;
	float _highlightTimer;
	GameContext* _gameContext;
};

