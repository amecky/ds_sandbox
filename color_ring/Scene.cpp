#include "Scene.h"

namespace ds {

	Scene::Scene() {
	}

	Scene::~Scene()	{
	}

	void Scene::render() {
		for (auto n : _nodes) {
			n->render();
		}
	}

}