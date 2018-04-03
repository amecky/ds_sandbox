#pragma once
#include <vector>
#include <typeinfo> 

struct BaseStream {
	int size;
	int capacity;

	BaseStream() : size(0), capacity(0) {}
};

template<class T>
struct DataStream : BaseStream {

	typedef T type;
	type tt;
	T* data;
	

	DataStream() : BaseStream() , data(0)  {}

	~DataStream() {
		if (data) {
			delete[] data;
		}
	}

	void alloc(int size) {
		data = new T[size];
		capacity = size;
	}

	void add(const T& t) {
		data[size++] = t;
	}
};


struct ECSContext {

};

const static uint32_t INVALID_ID = UINT32_MAX;

struct ComponentData {

	uint32_t ID;
	const std::type_info* typeInfo;

	ComponentData(const std::type_info* type) : ID(INVALID_ID) {
		typeInfo = type;
	}
};

struct Position : ComponentData {

	float x, y, z;

	Position() : ComponentData(&typeid(Position)), x(0.0f), y(0.0f), z(0.0f) {}
};

struct Rotation : ComponentData {

	float x, y, z;

	Rotation() : ComponentData(&typeid(Rotation)), x(0.0f), y(0.0f), z(0.0f) {}
};

struct Velocity : ComponentData {

	float x, y, z;

	Velocity() : ComponentData(&typeid(Velocity)), x(0.0f), y(0.0f), z(0.0f) {}
};

struct StreamDefinition {
	const std::type_info* typeInfo;
	size_t size;
};

struct MyMoveComponent {

	DataStream<Position> positions;
	DataStream<Rotation> rotations;
	DataStream<Velocity> velocities;

};

struct MyBaseSystem {

	void create(StreamDefinition* definitions, int num) {

	}

	void create(const std::type_info** typeInfos, int num) {

	}
};

struct MyMoveSystem : MyBaseSystem {

	struct Data {
		Position* positions;
	};

	void registerGroup() {
		const std::type_info* typeInfos[] = { &typeid(Position), &typeid(Velocity) };
		create(typeInfos,2);
	}

};

struct MoveData {
	float x;
	float y;
	float z;
	float velocity;
};

struct TransformData {
	int v;
};

class BaseComponent {

public:
	const std::type_info* typeInfo;
};

template<class T>
class AbstractComponent : public BaseComponent {

public:
	typedef T type;
	type dataType;
	AbstractComponent() {
		typeInfo = &typeid(T);
	}
	

	virtual int add(const T& t) = 0;

	virtual bool contains(int id) const = 0;

	virtual T get(int id) = 0;

};



class MoveComponent : public AbstractComponent<MoveData> {

private:
	DataStream<float> x;
	DataStream<float> y;
	DataStream<float> z;
	DataStream<float> velocity;

	
public:
	MoveComponent() {
		x.alloc(1024);
		y.alloc(1024);
		z.alloc(1024);
		velocity.alloc(1024);
	}
	int add(const MoveData& md) {
		x.add(md.x);
		y.add(md.y);
		z.add(md.z);
		velocity.add(md.velocity);
		return x.size - 1;
	}

	bool contains(int id) const {
		return true;
	}

	MoveData get(int id) {
		return{ x.data[id],y.data[id],z.data[id],velocity.data[id] };
	}
};

class TransformComponent : public AbstractComponent<TransformData> {

private:
	DataStream<int> v;

public:
	TransformComponent() {
		v.alloc(1024);
	}

	int add(const TransformData& md) {
		v.add(md.v);
		return v.size - 1;
	}

	bool contains(int id) const {
		return true;
	}

	TransformData get(int id) {
		return{ v.data[id] };
	}

};

struct Mapping {
	int entity;
	int component;
	int component_id;
};

struct ComponentDataArray {

};

class ECS;

class BaseSystem {

public:
	virtual void tick(float dt) = 0;
	void addStreams(const std::type_info** typeInfos, int num) {

	}
	virtual void registerStreams(ECS* ecs) = 0;
	template<typename U>
	DataStream<U>* getStream(const std::type_info* typeInfo) {

		DataStream<U>* stream = new DataStream<U>;
		return stream;
	}
};

class MoveSystem : public BaseSystem {

public:
	struct MoveSystemData {
		DataStream<MoveData> moves;
		DataStream<TransformData> transforms;
	};
	MoveSystemData data;

	void registerStreams(ECS* ecs) {
		//data.moves = *ecs->getStream<MoveData>(&typeid(MoveData));
		//data.transforms = *ecs->getStream<TransformData>(&typeid(TransformData));
	}
	void tick(float dt) {
		
		for (int i = 0; i < data.moves.size; ++i) {
			data.moves.data[i].x += data.moves.data[i].velocity * dt;
		}
	}
};

class ECS {

public:
	template<typename U>
	void setData(const U& u) {
		for (size_t i = 0; i < _components.size(); ++i) {
			if (typeid(U) == *_components[i]->typeInfo) {
				AbstractComponent<U>* ac = (AbstractComponent<U>*)_components[i];
				ac->add(u);
			}
		}
	}
	void addComponent(BaseComponent* component) {
		_components.push_back(component);
	}
	void addComponent(BaseComponent* component, StreamDefinition* defs, int num) {
		_components.push_back(component);
	}
	void addSystem(BaseSystem* system, const std::type_info** typeInfos, int num) {
		int matches = 0;
		for (int j = 0; j < num; ++j) {
			for (size_t i = 0; i < _components.size(); ++i) {
				if (*typeInfos[j] == *_components[i]->typeInfo) {
					++matches;
				}
			}
		}
		printf("matches %d\n", matches);
		system->registerStreams(this);
	}

	template<typename U>
	DataStream<U>* getStream(const std::type_info* typeInfo) {

		DataStream<U>* stream = new DataStream<U>;
		return stream;
	}
private:
	std::vector<BaseComponent*> _components;
	std::vector<BaseStream*> _streams;
};