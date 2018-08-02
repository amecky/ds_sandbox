#include <diesel.h>

const static int MAX_PARTICLES = 16384;

struct GPUParticle {
	ds::vec2 position;
	ds::vec2 velocity;
	ds::vec2 acceleration;
	ds::vec2 timer;
	ds::vec2 scale;
	ds::Color startColor;
	ds::Color endColor;
	float rotation;
	float rotationSpeed;
	ds::vec4 textureRect;
};

struct ParticleConstantBuffer {
	ds::vec4 screenDimension;
	ds::matrix wvp;
};

struct ParticleDesc {
	ds::vec2 position;
	ds::vec2 velocity;	
	ds::vec2 acceleration;
	float ttl;
	ds::vec2 scale;
	float rotation;
	float rotationSpeed;
	ds::Color startColor;
	ds::Color endColor;
	ds::vec4 textureRect;
};

// -------------------------------------------------------
// Particle array
// -------------------------------------------------------
struct ParticleArray {

	ds::vec2* positions;
	ds::vec2* velocities;
	ds::vec3* timers;
	ds::vec2* scales;
	ds::vec2* accelerations;
	float* rotations;
	float* rotationSpeeds;
	ds::Color* startColors;
	ds::Color* endColors;
	ds::vec4* textureRects;
	char* buffer;

	uint32_t count;
	uint32_t countAlive;

	ParticleArray() : count(0), countAlive(0), buffer(0), positions(0), velocities(0), timers(0), scales(0), accelerations(0), rotations(0), rotationSpeeds(0), startColors(0), endColors(0), textureRects(0) {}

	~ParticleArray() {
		if (buffer != 0) {
			delete[] buffer;
		}
	}

	void initialize(unsigned int maxParticles) {
		int size = maxParticles * (sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(ds::vec3) + sizeof(ds::vec2) + sizeof(ds::vec2) + sizeof(float) + sizeof(float) + 2 * sizeof(ds::Color) + sizeof(ds::vec4));
		buffer = new char[size];
		positions = (ds::vec2*)(buffer);
		velocities = (ds::vec2*)(positions + maxParticles);
		timers = (ds::vec3*)(velocities + maxParticles);
		scales = (ds::vec2*)(timers + maxParticles);
		accelerations = (ds::vec2*)(scales + maxParticles);
		rotations = (float*)(accelerations + maxParticles);
		rotationSpeeds = (float*)(rotations + maxParticles);
		startColors = (ds::Color*)(rotationSpeeds + maxParticles);
		endColors = (ds::Color*)(startColors + maxParticles);
		textureRects = (ds::vec4*)(endColors + maxParticles);
		count = maxParticles;
		countAlive = 0;
	}

	void swapData(uint32_t a, uint32_t b) {
		if (a != b) {
			positions[a] = positions[b];
			velocities[a] = velocities[b];
			timers[a] = timers[b];
			scales[a] = scales[b];
			accelerations[a] = accelerations[b];
			rotations[a] = rotations[b];
			rotationSpeeds[a] = rotationSpeeds[b];
			startColors[a] = startColors[b];
			endColors[a] = endColors[b];
			textureRects[a] = textureRects[b];
		}
	}

	void kill(uint32_t id) {
		if (countAlive > 0) {
			swapData(id, countAlive - 1);
			--countAlive;
		}
	}

	void wake(uint32_t id) {
		if (countAlive < count) {
			++countAlive;
		}
	}

};

class ParticleSystem {


};

class ParticleManager {

public:
	ParticleManager(RID textureID);
	RID createSystem();
	void add(const ds::vec2& pos);
	void add(ParticleDesc* descriptors, int num);
	void tick(float dt);
	void render(RID renderPass, const ds::matrix& viewProjectionMatrix);
	int numAlive() const {
		return _array.countAlive;
	}
private:
	GPUParticle _vertices[MAX_PARTICLES];
	ParticleArray _array;
	RID _drawItem;
	RID _structuredBufferId;
	ParticleConstantBuffer _constantBuffer;
};