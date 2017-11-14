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
	int addStream(AttributeType type, float* data, int size, int components);
	RID createInputLayout(RID vertexShaderId);
	int getCount() const;
	void save(const char* fileName);
	void load(const char* fileName);
	void loadBin(const char* fileName, bool scale = true);
private:
	std::vector<Stream> _streams;
};

