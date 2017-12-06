#include "EmitterQueue.h"
#include "BackgroundGrid.h"
#include "..\warp\WarpingGrid.h"
#include "Cubes.h"

void EmitterQueue::tick(float dt, ds::EventStream* events) {
	int cnt = 0; 
	while (cnt < _numEntries) {
		QueueEntry& entry = _entries[cnt];
		entry.timer -= dt;
		if (entry.timer <= 0.0f) {
			events->add(102, &entry, sizeof(QueueEntry));
			//_cubes->create(_grid->convert_grid_coords(entry.x, entry.y), 2);
			//_grid->highlight(entry.x, entry.y,BGF_ONE);
			//_grid->highlight(entry.x, entry.y);
			entry = _entries[_numEntries - 1];
			--_numEntries;
		}
		else {
			++cnt;
		}
	}
}

void EmitterQueue::emitt(int x, int y, ds::EventStream* events) {
	QueueEntry& entry = _entries[_numEntries++];
	entry.x = x;
	entry.y = y;
	entry.timer = 0.2f;
	entry.type = 0;
	//_grid->highlight(entry.x, entry.y, BGF_PULSE);
	//_grid->highlight(entry.x, entry.y);
	//_grid->applyForce(entry.x, entry.y, ds::vec3(0.0f, 0.0f, 0.2f));
}