#include "EmitterQueue.h"
#include "BackgroundGrid.h"
#include "Cubes.h"

void EmitterQueue::tick(float dt) {
	int cnt = 0; 
	while (cnt < _numEntries) {
		QueueEntry& entry = _entries[cnt];
		entry.timer -= dt;
		if (entry.timer <= 0.0f) {
			_cubes->create(_grid->grid_to_screen(entry.x, entry.y), 3);
			_grid->highlight(entry.x, entry.y,BGF_ONE);
			entry = _entries[_numEntries--];
		}
		else {
			++cnt;
		}
	}
}

void EmitterQueue::emitt(int x, int y) {
	QueueEntry& entry = _entries[_numEntries++];
	entry.x = x;
	entry.y = y;
	entry.timer = 2.0f;
	entry.type = 0;
	_grid->highlight(entry.x, entry.y, BGF_PULSE);
}