#include "InstancedObjViewer.h"
#include <ds_imgui.h>
#include "..\..\shaders\AmbientMultipleLightning_VS_Main.h"
#include "..\..\shaders\AmbientMultipleLightning_PS_Main.h"
#include "..\utils\tweening.h"
#include "..\..\shaders\RepeatingGrid_VS_Main.h"
#include "..\..\shaders\RepeatingGrid_PS_MainXZ.h"

InstancedObjViewer::InstancedObjViewer() : TestApp() {
}

InstancedObjViewer::~InstancedObjViewer() {
	delete _fpsCamera;
	delete _material;
}

ds::RenderSettings InstancedObjViewer::getRenderSettings() {
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "InstancedObjViewer";
	rs.clearColor = ds::Color(0.01f, 0.01f, 0.01f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	return rs;
}

// ----------------------------------------------------
// init
// ----------------------------------------------------
bool InstancedObjViewer::init() {
	
	_fpsCamera = new FPSCamera(&_camera);
	_fpsCamera->setPosition(ds::vec3(0, 3, -6), ds::vec3(0.0f, 0.0f, 0.0f));

	_material = new InstancedAmbientLightningMaterial;

	_lightDir[0] = ds::vec3(1.0f, 0.5f, 1.0f);
	_lightDir[1] = ds::vec3(-1.0f, 0.5f, 1.0f);
	_lightDir[2] = ds::vec3(0.0f, 0.5f, 1.0f);
	
	create_instanced_render_item(&_renderItem, "cube", _material, 256);

	ds::vec3 POSITIONS[] = { ds::vec3(-2.0f,0.0f,-2.0f),ds::vec3(-2.0f,0.0f,2.0f),ds::vec3(0.0f),ds::vec3(2.0f,0.0f,-2.0f),ds::vec3(2.0f,0.0f,2.0f) };
	for (int i = 0; i < 5; ++i) {
		_ids[i] = add_instance(&_renderItem);
		instance_item& item = get_instance(&_renderItem, _ids[i]);
		item.transform.position = POSITIONS[i];
		item.transform.position.y = _renderItem.mesh->getExtent().y * item.transform.scale.y * 0.5f;
	}
	
	_grid = new SimpleGrid;
	_grid->init(SGD_XZ, 8.0f, 0.7f, 0.2f);

	_selected = -1;
	
	return true;
}

// ----------------------------------------------------
// tick
// ----------------------------------------------------
void InstancedObjViewer::tick(float dt) {
	_fpsCamera->update(dt);
}

// ----------------------------------------------------
// render
// ----------------------------------------------------
void InstancedObjViewer::render() {

	_grid->render(_basicPass, _camera.viewProjectionMatrix);
	
	draw_instanced_render_item(&_renderItem, _basicPass, _camera.viewProjectionMatrix);
}

// ----------------------------------------------------
// renderGUI
// ----------------------------------------------------
void InstancedObjViewer::renderGUI() {
	int state = 1;
	gui::start();
	p2i sp = p2i(10, 760);
	if (gui::begin("Object", &state, &sp, 320)) {
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("Center", _renderItem.mesh->getCenter());
		gui::Value("Extent", _renderItem.mesh->getExtent());
		gui::Value("Min", _renderItem.mesh->getMin());
		gui::Value("Max", _renderItem.mesh->getMax());
		gui::Input("Selected", &_selected);
		if (_selected != -1) {
			instance_item& item = get_instance(&_renderItem, _ids[_selected]);
			if (gui::Input("Scale", &item.transform.scale)) {
				item.transform.position.y = _renderItem.mesh->getExtent().y * item.transform.scale.y * 0.5f;
			}
		}
	}
	gui::end();
}

