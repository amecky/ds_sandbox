#pragma once
#include <vector>

namespace ds {

	struct p2i {

		int x;
		int y;

		p2i() : x(0), y(0) {}
		explicit p2i(int v) : x(v), y(v) {}
		p2i(int _x, int _y) : x(_x), y(_y) {}
		p2i(const p2i& other) : x(other.x), y(other.y) {}

		void operator=(const p2i& other) {
			x = other.x;
			y = other.y;
		}
		bool operator==(const p2i& other) {
			return x == other.x && y == other.y;
		}
	};



	// -------------------------------------------------------
	// Dropped Cell
	// -------------------------------------------------------
	template<class T>
	struct DroppedCell {

		p2i from;
		p2i to;
		T data;
	};
	// ------------------------------------------------
	// Grid
	// ------------------------------------------------
	template<class T>
	class Grid {

		struct GridNode {
			p2i v;
			T data;
			bool used;

			GridNode() : v(-1, -1), used(false) {}

			GridNode(const GridNode& other) : v(other.v), data(other.data), used(other.used) {}

		};

	public:
		Grid(int width, int height) : _width(width), _height(height) {
			_size = width * height;
			_data = new GridNode[_size];
			for (int x = 0; x < width; ++x) {
				for (int y = 0; y < height; ++y) {
					int index = getIndex(x, y);
					GridNode* node = &_data[index];
					node->v = p2i(x, y);
					node->used = false;
				}
			}
			_used = new p2i[_size];
			_helper = new p2i[_size];
			_numUsed = 0;
			_validMoves = 0;
			_lastColumn = _width - 1;
			_startColumn = 0;
		}

		virtual ~Grid() {
			delete[] _helper;
			delete[] _data;
			delete[] _used;
		}

		// ------------------------------------------------
		// Clears the entire grid with given object
		// ------------------------------------------------
		void clear() {
			for (int i = 0; i < _size; ++i) {
				_data[i].used = false;
			}
			_lastColumn = 0;
			_startColumn = 0;
		}

		// ------------------------------------------------
		// Clears the entire grid with given object
		// ------------------------------------------------
		void clear(const T& t) {
			for (int i = 0; i < _size; ++i) {
				_data[i].data = t;
				_data[i].used = true;
			}
			_lastColumn = 0;
			_startColumn = 0;
		}


		// ------------------------------------------------
		// Gets the object at given position
		// ------------------------------------------------
		const T& get(int x, int y) const {
			int idx = getIndex(x, y);
			return _data[idx].data;
		}

		// ------------------------------------------------
		// Gets the object at given position
		// ------------------------------------------------
		T& get(const p2i& p) {
			int idx = getIndex(p);
			return _data[idx].data;
		}

		// ------------------------------------------------
		// Gets the object at given position
		// ------------------------------------------------
		const T& get(const p2i& p) const {
			int idx = getIndex(p);
			return _data[idx].data;
		}

		// ------------------------------------------------
		// Gets the object at given position
		// ------------------------------------------------
		T& get(int x, int y) {
			int idx = getIndex(x, y);
			return _data[idx].data;
		}
		// ------------------------------------------------
		//
		// ------------------------------------------------
		void set(int x, int y, const T& t) {
			int idx = getIndex(x, y);
			if (idx != -1) {
				GridNode* node = &_data[idx];
				node->data = t;
				node->used = true;
				if (x < _startColumn) {
					_startColumn = x;
				}
			}
		}

		// ------------------------------------------------
		// remove
		// ------------------------------------------------
		bool remove(int x, int y) {
			int idx = getIndex(x, y);
			if (idx != -1 && _data[idx].used) {
				_data[idx].used = false;
				return true;
				// FIXME: if column is empty set start and last column
			}
			return false;
		}

		// ------------------------------------------------
		// fill gaps
		// ------------------------------------------------
		void fillGaps() {
			int center = _width / 2;
			for (int i = _lastColumn; i >= center; --i) {
				if (isColumnEmpty(i)) {
					shiftColumns(i + 1, 0);
				}
			}
			for (int i = _startColumn; i < center; ++i) {
				if (isColumnEmpty(i)) {
					shiftColumns(i, 1);
				}
			}
		}

		// ------------------------------------------------
		// Remove grid points
		// ------------------------------------------------
		void remove(p2i* points, int num, bool shift) {
			for (int i = 0; i < num; ++i) {
				const p2i& gp = points[i];
				remove(gp.x, gp.y);
			}
			fillGaps();
			calculateValidMoves();
		}

		const int width() const {
			return _width;
		}

		const int height() const {
			return _height;
		}

		// ------------------------------------------------
		// is valid
		// ------------------------------------------------
		bool isValid(int x, int y) const {
			return getIndex(x, y) != -1;
		}

		// ------------------------------------------------
		// Checks if cell is used
		// ------------------------------------------------
		bool isUsed(int x, int y) const {
			int idx = getIndex(x, y);
			if (idx != -1) {
				return _data[idx].used;
			}
			return false;
		}

		// ------------------------------------------------
		// find all matching neighbours
		// ------------------------------------------------
		int findMatchingNeighbours(int x, int y, p2i* ret, int max) {
			int cnt = 0;
			if (isUsed(x, y)) {
				cnt = simpleFindMatching(x, y, ret, max);
			}
			return cnt;
		}

		// ------------------------------------------------
		// fill row
		// ------------------------------------------------
		void fillRow(int row, const T& t) {
			if (row >= 0 && row < _height) {
				for (int x = 0; x < _width; ++x) {
					int idx = getIndex(x, row);
					if (idx != -1) {
						_data[idx].data = t;
						_data[idx].used = true;
					}
				}
				_startColumn = 0;
				_lastColumn = _width - 1;
			}
		}

		// ------------------------------------------------
		// copy row
		// ------------------------------------------------
		void copyRow(int oldRow, int newRow) {
			for (int x = 0; x < _width; ++x) {
				int oldIndex = getIndex(x, oldRow);
				int newIndex = getIndex(x, newRow);
				_data[newIndex] = _data[oldIndex];
			}
		}

		// ------------------------------------------------
		// copy column
		// ------------------------------------------------
		void copyColumn(int oldColumn, int newColumn) {
			for (int y = 0; y < _height; ++y) {
				int oldIndex = getIndex(oldColumn, y);
				int newIndex = getIndex(newColumn, y);
				if (_data[oldIndex].used) {
					_data[newIndex].data = _data[oldIndex].data;
					_data[newIndex].used = true;
				}
				else {
					_data[newIndex].used = false;
				}
			}
		}

		int findLastUsedColumn() {
			int ret = _width - 1;
			for (int i = ret; i >= 0; --i) {
				if (!isColumnEmpty(i)) {
					return ret;
				}
			}
			return ret;
		}
		// ------------------------------------------------
		// shift columns
		// ------------------------------------------------
		void shiftColumns(int startColumn, int direction) {
			if (startColumn >= 0 && startColumn < _lastColumn) {
				int sx = startColumn - 1;
				if (sx < 0) {
					sx = 0;
				}
				if (direction == 0) {
					for (int x = sx; x < _lastColumn; ++x) {
						copyColumn(x + 1, x);
					}
					clearColumn(_lastColumn);
					--_lastColumn;
				}
				else {
					for (int x = startColumn; x > _startColumn; --x) {
						copyColumn(x - 1, x);
					}
					clearColumn(_startColumn);
					++_startColumn;
				}
			}
		}

		// ------------------------------------------------
		// clear column
		// ------------------------------------------------
		void clearColumn(int column) {
			if (column >= 0 && column < _width) {
				for (int y = 0; y < _height; ++y) {
					int idx = getIndex(column, y);
					_data[idx].used = false;
				}
			}
		}
		// ------------------------------------------------
		// fill column
		// ------------------------------------------------
		void fillColumn(int column, const T& t) {
			for (int y = 0; y < _height; ++y) {
				int idx = getIndex(column, y);
				_data[idx].data = t;
				_data[idx].used = true;
			}
		}

		// ------------------------------------------------
		//
		// ------------------------------------------------
		T& operator() (int x, int y) {
			int index = getIndex(x, y);
			return _data[index].data;
		}

		// ------------------------------------------------
		//
		// ------------------------------------------------
		const T& operator() (int x, int y) const {
			int index = getIndex(x, y);
			return _data[index].data;
		}

		// ------------------------------------------------
		// Checks if cell is free
		// ------------------------------------------------
		bool isFree(int x, int y) const {
			int idx = getIndex(x, y);
			if (idx != -1) {
				return !_data[idx].used;
			}
			return false;
		}

		// -------------------------------------------------------
		// is column empty
		// -------------------------------------------------------
		bool isColumnEmpty(int col) const {
			if (col < _width) {
				for (int i = 0; i < _height; ++i) {
					if (isUsed(col, i)) {
						return false;
					}
				}
			}
			return true;
		}

		// -------------------------------------------------------
		// is row empty
		// -------------------------------------------------------
		bool isRowEmpty(int row) const {
			int count = 0;
			for (int i = 0; i < _width; ++i) {
				if (!isFree(i, row)) {
					++count;
				}
			}
			return count == 0;
		}

		// -------------------------------------------------------
		// swap
		// -------------------------------------------------------
		void swap(const p2i& first, const p2i& second) {
			int fi = getIndex(first);
			int si = getIndex(second);
			GridNode n = _data[fi];
			_data[fi] = _data[si];
			_data[si] = n;
		}


		// ------------------------------------------------
		// Drop row
		// ------------------------------------------------
		void dropRow(int x) {
			for (int y = (_height - 1); y >= 0; --y) {
				dropCell(x, y);
			}
		}

		// ------------------------------------------------
		// Drop cell
		// ------------------------------------------------
		void dropCell(int x, int y) {
			int idx = getIndex(x, y);
			if (isUsed(x, y)) {
				int finalY = 0;
				for (int yp = y + 1; yp < _height; ++yp) {
					if (isFree(x, yp)) {
						++finalY;
					}
					else {
						break;
					}
				}
				if (finalY != 0) {
					int nidx = getIndex(x, finalY);
					_data[nidx].data = _data[idx].data;
					_data[nidx].used = true;
					_data[idx].used = false;
				}
			}
		}

		// -------------------------------------------------------
		// Drop cells - remove empty cells in between
		// -------------------------------------------------------
		int dropCells(DroppedCell<T>* droppedCells, int num) {
			int cnt = 0;
			for (int x = 0; x < _width; ++x) {
				for (int y = 0; y < _height - 1; ++y) {
					if (isFree(x, y)) {
						int sy = y + 1;
						while (isFree(x, sy) && sy < _height - 1) {
							++sy;
						}
						if (isUsed(x, sy) && cnt < num) {
							DroppedCell<T> dc;
							dc.data = get(x, sy);
							dc.from = p2i(x, sy);
							dc.to = p2i(x, y);
							droppedCells[cnt++] = dc;
							set(x, y, get(x, sy));
							remove(x, sy);
						}
					}
				}
			}
			return cnt;
		}

		// ------------------------------------------------
		// is valid
		// ------------------------------------------------
		bool isValid(const p2i& p) const {
			return getIndex(p) != -1;
		}

		// ------------------------------------------------
		// get max row
		// ------------------------------------------------
		int getMaxRow() const {
			for (int y = _height - 1; y >= 0; --y) {
				if (!isRowEmpty(y)) {
					return y;
				}
			}
			return 0;
		}

		// ------------------------------------------------
		// get max column
		// ------------------------------------------------
		int getMaxColumn() const {
			for (int x = 0; x < _width; ++x) {
				if (isColumnEmpty(x)) {
					return x;
				}
			}
			return _width;
		}

		// ------------------------------------------------
		// get number of valid moves
		// ------------------------------------------------
		int getNumberOfMoves() const {
			return _validMoves;
		}

		// ------------------------------------------------
		// pick a random matching block 
		// ------------------------------------------------
		int getMatchingBlock(p2i* ret, int max) {
			int idx = static_cast<int>(ds::random(0.0f, static_cast<float>(_numUsed)));
			p2i p = _used[idx];
			return findMatchingNeighbours(p.x, p.y, ret, max);
		}

		// ------------------------------------------------
		// calculate valid moves
		// ------------------------------------------------
		void calculateValidMoves() {
			_numUsed = 0;
			_validMoves = 0;
			for (int x = 0; x < _width; ++x) {
				for (int y = 0; y < _height; ++y) {
					if (!isAlreadyProcessed(p2i(x, y), _used, _numUsed)) {
						if (isUsed(x, y)) {
							int num = simpleFindMatching(x, y, _helper, _size);
							if (num > 1) {
								++_validMoves;
								for (int i = 0; i < num; ++i) {
									const p2i& c = _helper[i];
									if (!isAlreadyProcessed(c, _used, _numUsed)) {
										_used[_numUsed++] = c;
									}
								}
							}
						}
					}
				}
			}
		}

	protected:
		virtual bool isMatch(const T& first, const T& right) = 0;

	private:
		// ------------------------------------------------
		// check if point is already in the used list
		// ------------------------------------------------
		bool isAlreadyProcessed(const p2i& p, p2i* array, int num) {
			for (int i = 0; i < num; ++i) {
				if (array[i] == p) {
					return true;
				}
			}
			return false;
		}

		// ------------------------------------------------
		// internal findMatching
		// ------------------------------------------------
		int simpleFindMatching(int x, int y, p2i* ret, int max) {
			_numMatches = 0;
			int idx = getIndex(x, y);
			if (idx != -1) {
				const GridNode& providedNode = _data[idx];
				if (isUsed(x - 1, y)) {
					findMatching(x - 1, y, providedNode, ret, max);
				}
				if (isUsed(x + 1, y)) {
					findMatching(x + 1, y, providedNode, ret, max);
				}
				if (isUsed(x, y - 1)) {
					findMatching(x, y - 1, providedNode, ret, max);
				}
				if (isUsed(x, y + 1)) {
					findMatching(x, y + 1, providedNode, ret, max);
				}
			}
			return _numMatches;
		}

		// ------------------------------------------------
		// internal findMatching
		// ------------------------------------------------
		void findMatching(int x, int y, const GridNode& providedNode, p2i* ret, int max) {
			int idx = getIndex(x, y);
			if (idx != -1) {
				const GridNode& currentNode = _data[idx];
				if (currentNode.used && isMatch(currentNode.data, providedNode.data)) {
					if (!isAlreadyProcessed(p2i(currentNode.v), ret, _numMatches)) {
						if (_numMatches < max) {
							ret[_numMatches++] = p2i(currentNode.v);
						}
						if (isUsed(x - 1, y)) {
							findMatching(x - 1, y, currentNode, ret, max);
						}
						if (isUsed(x + 1, y)) {
							findMatching(x + 1, y, currentNode, ret, max);
						}
						if (isUsed(x, y - 1)) {
							findMatching(x, y - 1, currentNode, ret, max);
						}
						if (isUsed(x, y + 1)) {
							findMatching(x, y + 1, currentNode, ret, max);
						}
					}
				}
			}
		}

		// ------------------------------------------------
		// Returns the index into the array if valid or -1
		// ------------------------------------------------
		int getIndex(int x, int y) const {
			if (x < 0 || x >= _width) {
				return -1;
			}
			if (y < 0 || y >= _height) {
				return -1;
			}
			return y * _width + x;
		}

		// ------------------------------------------------
		// Returns the index into std::vector if valid or -1
		// ------------------------------------------------
		int getIndex(const p2i& p) const {
			return getIndex(p.x, p.y);
		}

		int _width;
		int _height;
		int _size;
		int _lastColumn;
		int _startColumn;
		GridNode* _data;
		p2i* _used;
		p2i* _helper;
		int _numUsed;
		int _numMatches;
		int _validMoves;
	};

}

