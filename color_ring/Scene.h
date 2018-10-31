#pragma once
#include <diesel.h>
#include <vector>
#include <SpriteBatchBuffer.h>

namespace ds {

	enum NodeState {
		NS_ACTIVE,
		NS_INACTIVE
	};

	class Node {

	public:
		Node(const char* name) : _name(name) , _state(NS_ACTIVE) {}
		virtual ~Node() {}
		const char* getName() const {
			return _name;
		}
		virtual void render() = 0;
		void setState(NodeState state) {
			_state = state;
		}
		bool isActive() const {
			return _state == NS_ACTIVE;
		}
		const NodeState& getState() const {
			return _state;
		}
		virtual void onStateChanged(NodeState state) {}
	private:
		NodeState _state;
		const char* _name;
	};

	class SpritesNode : public Node {

	public:
		SpritesNode(const char* name, RID textureID) : Node(name) {
			_sprites = new SpriteBatchBuffer(SpriteBatchDesc()
				.MaxSprites(4096)
				.Texture(textureID)
			);
			sprites::initialize(_array, 4096);
		}
		~SpritesNode() {
			delete _sprites;
		}
		void render() {}
	private:
		SpriteArray _array;
		SpriteBatchBuffer* _sprites;
	};

	class Node2D : public Node {

	public:
		Node2D(const char* name, RID textureID) : Node(name) {
			_sprites = new SpriteBatchBuffer(SpriteBatchDesc()
				.MaxSprites(4096)
				.Texture(textureID)
				);
			sprites::initialize(_array, 4096);
		}
		~Node2D() {
			delete _sprites;
		}
		void render() {}
	private:
		SpriteArray _array;
		SpriteBatchBuffer* _sprites;
	};

	typedef std::vector<Node*> Nodes;

	struct Scene {

	public:
		Scene();
		~Scene();
		void addNode(Node* n) {
			_nodes.push_back(n);
		}
		Node* getByName(const char* name) {
			for (auto n : _nodes) {
				if (strcmp(name, n->getName()) == 0) {
					return n;
				}
			}
		}
		void render();
	private:
		Nodes _nodes;
	};

}