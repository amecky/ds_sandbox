#include "Mesh.h"
#include <fstream>
#include <string>
#include <windows.h>
#include "utils\obj.h"

const ds::vec3 CUBE_VERTICES[8] = {
	ds::vec3(-0.5f,-0.5f,-0.5f),
	ds::vec3(-0.5f, 0.5f,-0.5f),
	ds::vec3(0.5f, 0.5f,-0.5f),
	ds::vec3(0.5f,-0.5f,-0.5f),
	ds::vec3(-0.5f,-0.5f, 0.5f),
	ds::vec3(-0.5f, 0.5f, 0.5f),
	ds::vec3(0.5f, 0.5f, 0.5f),
	ds::vec3(0.5f,-0.5f, 0.5f)
};

const int CUBE_PLANES[6][4] = {
	{ 0, 1, 2, 3 }, // front
	{ 3, 2, 6, 7 }, // right
	{ 1, 5, 6, 2 }, // top
	{ 4, 5, 1, 0 }, // left
	{ 4, 0, 3, 7 }, // bottom
	{ 7, 6, 5, 4 }, // back
};



Mesh::Mesh() {
}


Mesh::~Mesh() {
	clear();
}

void Mesh::clear() {
	for (size_t i = 0; i < _streams.size(); ++i) {
		delete[] _streams[i].data;
	}
	_streams.clear();
	_extent = ds::vec3(0.0f);
	_min = ds::vec3(0.0f);
	_max = ds::vec3(0.0f);
	_center = ds::vec3(0.0f);
}

void subDivide(ds::vec3 *&dest, const ds::vec3 &v0, const ds::vec3 &v1, const ds::vec3 &v2, int level) {
	if (level) {
		level--;
		ds::vec3 v3 = normalize(v0 + v1);
		ds::vec3 v4 = normalize(v1 + v2);
		ds::vec3 v5 = normalize(v2 + v0);

		subDivide(dest, v0, v3, v5, level);
		subDivide(dest, v3, v4, v5, level);
		subDivide(dest, v3, v1, v4, level);
		subDivide(dest, v5, v4, v2, level);
	}
	else {
		*dest++ = v0;
		*dest++ = v1;
		*dest++ = v2;
	}
}

void Mesh::createCube() {
	ds::vec3* vertices = new ds::vec3[24];
	for (int side = 0; side < 6; ++side) {
		int idx = side * 4;
		for (int i = 0; i < 4; ++i) {
			int p = idx + i;
			vertices[p] = CUBE_VERTICES[CUBE_PLANES[side][i]];
		}
	}
	addStream(AT_VERTEX, (float*)vertices, 24, 3);

	ds::vec2* uvs = new ds::vec2[24];
	for (int side = 0; side < 6; ++side) {
		int idx = side * 4;
		float u[4] = { 0.0f,0.0f,1.0f,1.0f };
		float v[4] = { 1.0f,0.0f,0.0f,1.0f };
		for (int i = 0; i < 4; ++i) {
			int p = idx + i;
			uvs[p] = ds::vec2(u[i], v[i]);
		}
	}
	addStream(AT_UV, (float*)uvs, 24, 2);

	ds::vec3* normals = new ds::vec3[24];
	for (int side = 0; side < 6; ++side) {
		int idx = side * 4;
		for (int i = 0; i < 4; ++i) {
			ds::vec3 d0 = vertices[idx + 1] - vertices[idx];
			ds::vec3 d1 = vertices[idx + 2] - vertices[idx];
			ds::vec3 c = cross(d0, d1);
			for (int i = 0; i < 4; ++i) {
				normals[idx + i] = c;
			}
		}
	}
	addStream(AT_NORMAL, (float*)normals, 24, 3);
}

void Mesh::createSphere(const int subDivLevel) {
	const int nVertices = 8 * 3 * (1 << (2 * subDivLevel));

	ds::vec3 *vertices = new ds::vec3[nVertices];

	// Tessellate a octahedron
	ds::vec3 px0(-1, 0, 0);
	ds::vec3 px1(1, 0, 0);
	ds::vec3 py0(0, -1, 0);
	ds::vec3 py1(0, 1, 0);
	ds::vec3 pz0(0, 0, -1);
	ds::vec3 pz1(0, 0, 1);

	ds::vec3 *dest = vertices;
	subDivide(dest, py0, px0, pz0, subDivLevel);
	subDivide(dest, py0, pz0, px1, subDivLevel);
	subDivide(dest, py0, px1, pz1, subDivLevel);
	subDivide(dest, py0, pz1, px0, subDivLevel);
	subDivide(dest, py1, pz0, px0, subDivLevel);
	subDivide(dest, py1, px0, pz1, subDivLevel);
	subDivide(dest, py1, pz1, px1, subDivLevel);
	subDivide(dest, py1, px1, pz0, subDivLevel);

	addStream(AT_VERTEX, (float*)vertices, nVertices, 3);

}

int Mesh::addStream(AttributeType type, float* data, int size, int components) {
	Stream s;
	s.data = data;
	s.nComponents = components;
	s.num = size;
	s.type = type;
	_streams.push_back(s);
	return _streams.size() - 1;
}

RID Mesh::assemble() {
	int maxSize = 0;
	int totalComponents = 0;
	for (size_t i = 0; i < _streams.size(); ++i) {
		totalComponents += _streams[i].nComponents;
		if (_streams[i].num > maxSize) {
			maxSize = _streams[i].num;
		}
	}
	float* data = new float[maxSize * totalComponents];
	int offset = 0;
	for (size_t i = 0; i < _streams.size(); ++i) {
		int steps = _streams[i].num;// *_streams[i].nComponents;
		//ds::log(LogLevel::LL_DEBUG, "(%d) steps %d", i, steps);
		for (int j = 0; j < steps; ++j) {
			for (int n = 0; n < _streams[i].nComponents; ++n) {
				data[j * totalComponents + n + offset] = _streams[i].data[j * _streams[i].nComponents + n];
			}
		}		
		offset += _streams[i].nComponents;
	}
	
	RID rid = ds::createVertexBuffer(ds::VertexBufferDesc()
		.BufferType(ds::BufferType::STATIC)
		.NumVertices(maxSize)
		.VertexSize(totalComponents * sizeof(float))
		.Data(data)
	);
	delete[] data;
	return rid;
}

void Mesh::calculate() {
	_min = { 10000.0f,10000.0f,10000.0f };
	_max = { 0.0f,0.0f,0.0f };

	int idx = getStreamIndex(AT_VERTEX);

	const Stream& s = _streams[idx];
	for (int i = 0; i < s.num; ++i) {
		for (int j = 0; j < s.nComponents; ++j) {
			float c = s.data[i * s.nComponents + j];
			if (c > _max.data[j]) {
				_max.data[j] = c;
			}
			if (c < _min.data[j]) {
				_min.data[j] = c;
			}
		}
	}

	for (int i = 0; i < 3; ++i) {
		_center.data[i] = (_max.data[i] + _min.data[i]) * 0.5f;
	}
	for (int i = 0; i < 3; ++i) {
		_extent.data[i] = (abs(_max.data[i]) + abs(_min.data[i]));
	}
}

void Mesh::align() {
	int idx = getStreamIndex(AT_VERTEX);
	const Stream& s = _streams[idx];
	for (int i = 0; i < s.num; ++i) {
		for (int j = 0; j < s.nComponents; ++j) {
			s.data[i * s.nComponents + j] -= _center.data[j];			
		}
	}
	_center = ds::vec3(0.0f);
}

int Mesh::getStreamIndex(AttributeType type) {
	for (size_t i = 0; i < _streams.size(); ++i) {
		if (_streams[i].type == type) {
			return i;
		}
	}
	return -1;
}

void Mesh::scaleStream(int streamID, float scale) {
	const Stream& s = _streams[streamID];
	for (int i = 0; i < s.num; ++i) {
		for (int j = 0; j < s.nComponents; ++j) {
			s.data[i * s.nComponents + j] *= scale;
		}
	}
}

void Mesh::calculateTangents(int posStream, int uvStream) {
	const Stream& s = _streams[uvStream];
	const Stream& sp = _streams[posStream];
	int steps = s.num / 4;
	ds::vec3* tangents = new ds::vec3[s.num];
	for (int i = 0; i < steps; ++i) {
		// Shortcuts for vertices
		ds::vec3 vp0 = ds::vec3(sp.data[i * 4 * 3 + 0], sp.data[i * 4 * 3 + 1], sp.data[i * 4 * 3 + 2]);
		ds::vec3 vp1 = ds::vec3(sp.data[i * 4 * 3 + 3], sp.data[i * 4 * 3 + 4], sp.data[i * 4 * 3 + 5]);
		ds::vec3 vp2 = ds::vec3(sp.data[i * 4 * 3 + 6], sp.data[i * 4 * 3 + 7], sp.data[i * 4 * 3 + 8]);

		// Shortcuts for vertices
		ds::vec2 uv0 = ds::vec2(s.data[i * 4 * 2 + 0], s.data[i * 4 * 2 + 1]);
		ds::vec2 uv1 = ds::vec2(s.data[i * 4 * 2 + 2], s.data[i * 4 * 2 + 3]);
		ds::vec2 uv2 = ds::vec2(s.data[i * 4 * 2 + 4], s.data[i * 4 * 2 + 5]);

		// Edges of the triangle : postion delta
		ds::vec3 deltaPos1 = vp1 - vp0;
		ds::vec3 deltaPos2 = vp2 - vp0;

		// UV delta
		ds::vec2 deltaUV1 = uv1 - uv0;
		ds::vec2 deltaUV2 = uv2 - uv0;
		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		ds::vec3 tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r);
		tangents[i * 4] = tangent;
		tangents[i * 4 + 1] = tangent;
		tangents[i * 4 + 2] = tangent;
		tangents[i * 4 + 3] = tangent;
	}
	addStream(AT_TANGENT, (float*)tangents, s.num, 3);
}

RID Mesh::createInputLayout(RID vertexShaderId) {
	ds::InputLayoutDefinition* decl = new ds::InputLayoutDefinition[_streams.size()];
	for (size_t i = 0; i < _streams.size(); ++i) {
		if (_streams[i].type == AT_VERTEX) {
			decl[i] = { "POSITION", 0, ds::BufferAttributeType::FLOAT3 };
		}
		else if (_streams[i].type == AT_UV) {
			decl[i] = { "TEXCOORD", 0, ds::BufferAttributeType::FLOAT2 };
		}
		else if (_streams[i].type == AT_NORMAL) {
			decl[i] = { "NORMAL", 0, ds::BufferAttributeType::FLOAT3 };
		}
		else if (_streams[i].type == AT_TANGENT) {
			decl[i] = { "TANGENT" , 0 ,ds::BufferAttributeType::FLOAT3 };
		}
		else if (_streams[i].type == AT_COLOR) {
			decl[i] = { "COLOR", 0, ds::BufferAttributeType::FLOAT4 };
		};
	}
	ds::InputLayoutInfo layoutInfo = { decl, _streams.size(), vertexShaderId };
	RID rid = ds::createInputLayout(layoutInfo);
	delete[] decl;
	return rid;
}

uint32_t Mesh::getCount() const {
	uint32_t cnt = 0;
	int offset = 0;
	for (size_t i = 0; i < _streams.size(); ++i) {
		int steps = _streams[i].num;
		if (steps > cnt) {
			cnt = steps;
		}
	}
	return cnt;
}
/*
BatchID Model::addBatch(const uint startIndex, const uint nIndices) {
	Batch batch;

	batch.startIndex = startIndex;
	batch.nIndices = nIndices;
	//	batch.startVertex = startVertex;
	//	batch.nVertices = nVertices;
	batch.startVertex = 0;
	batch.nVertices = 0;

	return batches.add(batch);
}
*/

void Mesh::save(const char* dir, const char* name) {
	char fileName[256];
	sprintf_s(fileName, "%s\\%s.bin", dir, name);
	FILE* fp = fopen(fileName, "wb");
	if (fp) {
		int nr = _streams.size();
		fwrite(&nr, sizeof(int), 1, fp);
		for (size_t i = 0; i < _streams.size(); ++i) {
			const Stream& s = _streams[i];
			fwrite(&s.num, sizeof(uint32_t), 1, fp);
			fwrite(&s.nComponents, sizeof(int), 1, fp);
			fwrite(&s.type, sizeof(AttributeType), 1, fp);
			fwrite(s.data, sizeof(float), s.num * s.nComponents, fp);
		}
		fclose(fp);
	}
}

void Mesh::load(const char* fileName) {
	clear();
	FILE* fp = fopen(fileName, "rb");
	if (fp) {
		int nr = 0;
		fread(&nr, sizeof(int), 1, fp);
		for (size_t i = 0; i < nr; ++i) {
			uint32_t num = 0;
			fread(&num, sizeof(uint32_t), 1, fp);
			int nc = 0;
			fread(&nc, sizeof(int), 1, fp);
			AttributeType type;
			fread(&type, sizeof(AttributeType), 1, fp);
			float* data = new float[num * nc];
			fread(data, sizeof(float), num * nc, fp);
			addStream(type, data, num, nc);
		}
		fclose(fp);
	}
	calculate();
	align();
}

void Mesh::loadBin(const char* fileName, bool scale) {
	clear();
	FILE* fp = fopen(fileName, "rb");
	if (fp) {
		int total = 0;
		fread(&total, sizeof(int), 1, fp);
		ds::vec3 center = { 0.0f,0.0f,0.0f };
		fread(&center.x, sizeof(float), 1, fp);
		fread(&center.y, sizeof(float), 1, fp);
		fread(&center.z, sizeof(float), 1, fp);
		ds::vec3 extent = { 0.0f,0.0f,0.0f };
		fread(&extent.x, sizeof(float), 1, fp);
		fread(&extent.y, sizeof(float), 1, fp);
		fread(&extent.z, sizeof(float), 1, fp);
		float mm = 0.0f;
		for (int i = 0; i < 3; ++i) {
			if (extent.data[i] > mm) {
				mm = extent.data[i];
			}
		}
		float s = 1.0f / mm;
		ds::matrix sm = ds::matScale(ds::vec3(s, s, s));
		ds::matrix cm = ds::matTranslate(ds::vec3(-center.x, -center.y, -center.z));
		ds::vec3* positions = new ds::vec3[total];
		ds::Color* colors = new ds::Color[total];
		ds::vec3* normals = new ds::vec3[total];
		for (int i = 0; i < total; ++i) {
			fread(&positions[i].x, sizeof(float), 1, fp);
			fread(&positions[i].y, sizeof(float), 1, fp);
			fread(&positions[i].z, sizeof(float), 1, fp);




			fread(&colors[i].r, sizeof(float), 1, fp);
			fread(&colors[i].g, sizeof(float), 1, fp);
			fread(&colors[i].b, sizeof(float), 1, fp);
			fread(&colors[i].a, sizeof(float), 1, fp);
			fread(&normals[i].x, sizeof(float), 1, fp);
			fread(&normals[i].y, sizeof(float), 1, fp);
			fread(&normals[i].z, sizeof(float), 1, fp);
			if (scale) {
				positions[i] = sm * positions[i];
			}
		}
		addStream(AT_VERTEX, (float*)positions, total, 3);		
		addStream(AT_COLOR, (float*)colors, total, 4);
		addStream(AT_NORMAL, (float*)normals, total, 3);
		fclose(fp);

		calculate();
	}
}

void getFileTime(const char* fileName, FILETIME& time) {
	WORD ret = -1;
	// no file sharing mode
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		// Retrieve the file times for the file.
		GetFileTime(hFile, NULL, NULL, &time);
		CloseHandle(hFile);
	}
}

void Mesh::loadData(const char* fileName, ds::matrix* world, bool force) {
	DBG_LOG("loading object: '%s'", fileName);
	bool load_obj = false;
	char objName[256];
	sprintf_s(objName, "obj\\%s.obj",fileName);
	FILETIME objTime;
	getFileTime(objName, objTime);
	char binName[256];
	sprintf_s(binName, "models\\%s.bin", fileName);
	HANDLE hFile = CreateFile(binName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		FILETIME binTime;
		getFileTime(binName, binTime);
		int t = CompareFileTime(&objTime, &binTime);
		if (t == 1) {
			load_obj = true;
		}
	}
	else {
		load_obj = true;
	}
	if (force) {
		load_obj = true;
	}
	if (load_obj) {
		sprintf_s(objName, "%s.obj", fileName);
		DBG_LOG("loading obj file '%s'", objName);
		obj::load("obj", objName, this, world);
		calculate();	
		if (world == 0) {
			align();
		}
		save("models", fileName);
	}
	else {
		load(binName);
	}
	DBG_LOG("extent: %3.2f %3.2f %3.2f", _extent.x, _extent.y, _extent.z)
}