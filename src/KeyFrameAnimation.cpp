#include "KeyFrameAnimation.h"
#include "utils\tweening.h"
#include <fstream>

namespace animation {

	// ------------------------------------------------------------------
	// read animation as text file
	// ------------------------------------------------------------------
	bool load_text(const char* name, KeyFrameAnimation* animation) {
		animation->clear();
		std::ifstream file(name);
		if (!file) {
			return false;
		}
		char cmd[256] = { 0 };
		for (;; ) {
			file >> cmd;
			if (!file) {
				break;
			}
			if (*cmd == '#') {

			}
			else if (strcmp(cmd, "s:") == 0) {
				float t, sx, sy;
				file >> t;
				file.ignore();
				file >> sx;
				file.ignore();
				file >> sy;
				animation->addScaling(t, ds::vec2(sx, sy));
			}
			else if (strcmp(cmd, "r:") == 0) {
				float t, r;
				file >> t;
				file.ignore();
				file >> r;
				float rot = r / 360.0f * ds::TWO_PI;
				animation->addRotation(t, rot);
			}
			else if (strcmp(cmd, "t:") == 0) {
				float t, tx, ty;
				file >> t;
				file.ignore();
				file >> tx;
				file.ignore();
				file >> ty;
				animation->addTranslation(t, ds::vec2(tx, ty));
			}
			else if (strcmp(cmd, "n:") == 0) {
				file >> animation->name;
			}
		}
		return true;
	}

	// ------------------------------------------------------------------
	// save animation as text file
	// ------------------------------------------------------------------
	bool save_text(const char* name, KeyFrameAnimation* animation) {
		std::ofstream file(name);
		if (!file) {
			return false;
		}
		file << "# scale\n";
		const AnimationTimeline& st = animation->scalingTimeline;
		for (int i = 0; i < st.num; ++i) {
			file << "s: " << st.entries[i].start;
			file << " , " << st.entries[i].value.x << " , " << st.entries[i].value.y;
			file << "\n";
		}
		file << "# rotation\n";
		const AnimationTimeline& rt = animation->rotationTimeline;
		for (int i = 0; i < rt.num; ++i) {
			file << "r: " << rt.entries[i].start;
			float rot = rt.entries[i].value.x / ds::TWO_PI * 360.0f;
			file << " , " << rot;
			file << "\n";
		}
		file << "# translation\n";
		const AnimationTimeline& tt = animation->translationTimeline;
		for (int i = 0; i < tt.num; ++i) {
			file << "t: " << tt.entries[i].start;
			file << " , " << tt.entries[i].value.x << " , " << tt.entries[i].value.y;
			file << "\n";
		}
		return true;
	}

	struct AnimationImportData {
		char name[32];
		ds::vec2 pos;		
		ds::vec4 rect;
		ds::vec2 scale;
		float rotation;
		int parent;
		uint16_t id;
	};

	// ------------------------------------------------------------------
	// load animation object
	// ------------------------------------------------------------------
	bool load_animation_object(const char* name, AnimationObject* object) {
		std::vector<AnimationImportData> importData;
		std::ifstream file(name);
		if (!file) {
			return false;
		}
		int current = -1;
		char cmd[256] = { 0 };
		for (;; ) {
			file >> cmd;
			if (!file) {
				break;
			}
			if (*cmd == '#') {

			}
			else if (strcmp(cmd, "pos:") == 0) {
				float t, sx, sy;
				file >> sx;
				file.ignore();
				file >> sy;
				importData[current].pos = ds::vec2(sx, sy);
			}
			else if (strcmp(cmd, "rect:") == 0) {
				float l, t, w, h;
				file >> l;
				file.ignore();
				file >> t;
				file.ignore();
				file >> w;
				file.ignore();
				file >> h;
				importData[current].rect = ds::vec4(l, t, w, h);
			}
			else if (strcmp(cmd, "scale:") == 0) {
				float sx,sy;
				file >> sx;
				file.ignore();
				file >> sy;
				importData[current].scale = ds::vec2(sx, sy);
			}
			else if (strcmp(cmd, "rotation:") == 0) {
				float r;
				file >> r;
				importData[current].rotation = r / 360.0f * ds::TWO_PI;
			}
			else if (strcmp(cmd, "parent:") == 0) {
				int p;
				file >> p;
				importData[current].parent = p;
			}
			else if (strcmp(cmd, "n:") == 0) {
				char tmp[32];
				AnimationImportData data;
				data.pos = ds::vec2(0.0f);
				data.scale = ds::vec2(1.0f);
				data.rotation = 0.0f;
				file >> tmp;
				sprintf_s(data.name, "%s", tmp);
				importData.push_back(data);
				current = importData.size() - 1;				
			}
		}
		for (size_t i = 0; i < importData.size(); ++i) {
			AnimationImportData& data = importData[i];
			data.id = object->add(data.name, data.pos, data.rect);				
			AnimationPart& part = object->getPart(data.id);
			part.baseRotation = data.rotation;
			part.baseScale = data.scale;
		}
		for (size_t i = 0; i < importData.size(); ++i) {
			AnimationImportData& data = importData[i];
			if (data.parent != -1) {
				const AnimationImportData& p = importData[data.parent];				
				AnimationPart& part = object->getPart(data.id);
				part.parent = importData[data.parent].id;
			}
		}
		return true;
	}
}