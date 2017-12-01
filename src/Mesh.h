#pragma once
#include <diesel.h>
#include <stdint.h>
#include <vector>

enum AttributeType {
	AT_VERTEX,
	AT_NORMAL,
	AT_UV,
	AT_COLOR,
	AT_TANGENT
};

struct Stream {
	float *data;
	uint32_t num;
	int nComponents;
	AttributeType type;
};

typedef struct color_definition_t {
	ds::Color color;
	uint32_t hash;
} color_definition;

typedef struct entry_t {
	ds::vec3 pos;
	ds::vec2 uvs;
	ds::vec3 n;
	ds::Color color;
} entry;

typedef struct face_t {
	entry entries[16];
	int num;
} face;

class Mesh {

public:
	Mesh();
	~Mesh();
	void clear();
	void createCube();
	void calculateTangents(int posStream, int uvStream);
	void createSphere(const int subDivLevel);
	void scaleStream(int streamID, float scale);
	RID assemble();
	void calculate();
	int addStream(AttributeType type, float* data, int size, int components);
	int getStreamIndex(AttributeType type);
	RID createInputLayout(RID vertexShaderId);
	void align();
	uint32_t getCount() const;
	void save(const char* fileName);
	void load(const char* fileName);
	void loadObj(const char* dir, const char* fileName);
	void loadBin(const char* fileName, bool scale = true);
	const ds::vec3& getExtent() const {
		return _extent;
	}
	const ds::vec3& getCenter() const {
		return _center;
	}
	const ds::vec3& getMin() const {
		return _min;
	}
	const ds::vec3& getMax() const {
		return _max;
	}
private:
	int loadMaterial(const char* dir, const char* fileName, color_definition* defs, int max);
	std::vector<Stream> _streams;
	ds::vec3 _extent;
	ds::vec3 _center;
	ds::vec3 _min;
	ds::vec3 _max;
};

