#pragma once
#include <diesel.h>
#include <ds_sprites.h>
#include <ds_imgui.h>
#include <vector>

namespace editor {

	struct SpriteEntry {
		char name[20];
		ds::vec4 rect;
	};

	class SpriteList : public gui::ListBoxModel {

	public:
		SpriteList() : gui::ListBoxModel() {}
		virtual ~SpriteList() {}
		uint32_t size() const {
			return _entries.size();
		}

		const char* name(uint32_t index) const {
			return _entries[index].name;
		}
		
		const ds::vec4& getRect(int s) const {
			return _entries[s].rect;
		}
		void add(const char* name, const ds::vec4& rect) {
			SpriteEntry entry;
			sprintf(entry.name, "%s", name);
			entry.rect = rect;
			_entries.push_back(entry);
		}
		const char* getName(int index) const {
			return _entries[index].name;
		}
		void clear() {
			_entries.clear();
		}
	private:
		std::vector<SpriteEntry> _entries;
	};

	struct SpriteEditorContext {		
		RID textureId;
		ds::vec2 textureOffset;
		ds::vec4 rect;
		float scale;
		SpriteList sprites;
		int selection;
		char name[20];
		bool dragging;
		ds::vec2 draggingPos;
	};

	void render(SpriteEditorContext* ctx);

	void renderGUI(SpriteEditorContext* ctx);

}

