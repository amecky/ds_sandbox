#include "SandboxApp.h"
#include <SpriteBatchBuffer.h>
#include <Windows.h>
#include <stb_image.h>
#include <ds_imgui.h>
#include "Material.h"

ds::BaseApp *app = new SandboxApp();

SandboxApp::SandboxApp() : ds::BaseApp() {
	_settings.screenWidth = 1280;
	_settings.screenHeight = 720;
	_settings.windowTitle = "Flow";
	_settings.useIMGUI = true;
	_settings.clearColor = ds::Color(16, 16, 16, 255);
	_settings.guiToggleKey = 'O';
	_gameContext = new GameContext;
}


SandboxApp::~SandboxApp() {
	delete _flowFieldScene;
	delete _towerTestScene;
	delete _gameContext->particleContext->particleSystem;
	delete _gameContext->particleContext;
	delete _gameContext->ambientMaterial;
	delete _gameContext->instancedAmbientmaterial;
	delete _gameContext;
}

ds::vec3 ComputePosition(const ds::vec3& eyePos, ds::vec3 pos, float size, ds::vec2 vPos) {
	// Create billboard (quad always facing the camera)
	ds::vec3 toEye = normalize(eyePos - pos);
	ds::vec3 up = ds::vec3(0.0f, 1.0f, 0.0f);
	ds::vec3 right = cross(toEye, up);
	up = cross(toEye, right);
	pos += (right * size * vPos.x) - (up * size * vPos.y);
	return pos;
}
// ---------------------------------------------------------------
// initialize
// ---------------------------------------------------------------
void SandboxApp::initialize() {
	_gameContext->ambientMaterial = new AmbientLightningMaterial;
	_gameContext->instancedAmbientmaterial = new InstancedAmbientLightningMaterial;
	_gameContext->towers.init(_gameContext->ambientMaterial);
	RID textureID = loadImageFromFile("content\\TextureArray.png");
	_gameContext->particleContext = particles::create_context(textureID);
	_flowFieldScene = new MainGameScene(_gameContext);
	_towerTestScene = new TowerTestScene(_gameContext);
	_particlesTestScene = new ParticlesTestScene(_gameContext);
	//pushScene(_flowFieldScene);
	//pushScene(_towerTestScene);
	pushScene(_particlesTestScene);

	static const ds::vec4 vertexUVPos[4] =
	{
		ds::vec4( 0.0f, 1.0f, -1.0f, -1.0f ),
		ds::vec4( 0.0f, 0.0f, -1.0f, +1.0f ),
		ds::vec4( 1.0f, 1.0f, +1.0f, -1.0f ),
		ds::vec4( 1.0f, 0.0f, +1.0f, +1.0f ),
	};

	float size = 0.5f;

	ds::vec3 eyePos(0.0f, 2.0f, -1.0f);
	ds::vec3 pos(0.0f);

	ds::vec3 look = normalize(eyePos - pos);
	ds::vec3 right = normalize(cross(look, ds::vec3(0.0f, 1.0f, 0.0f)));
	ds::vec3 up = normalize(cross(look, right));

	ds::vec3 fp[4];
	ds::vec3 np[4];
	ds::vec3 tp[4];

	for (int i = 0; i < 4; ++i) {
		float hw = (i & 2) ? size : -size;
		float hh = (i % 2) ? size : -size;

		fp[i] = ds::vec3(pos + hw * right - hh * up);
		np[i] = ComputePosition(eyePos, pos , size, ds::vec2(vertexUVPos[i].z, vertexUVPos[i].w));

		float nw = (i & 2) ? 1.0f : -1.0f;
		float nh = (i % 2) ? 1.0f : -1.0f;

		tp[i] = ComputePosition(eyePos, pos, size, ds::vec2(nw,nh));
	}


}

// ---------------------------------------------------------------
// handle events
// ---------------------------------------------------------------
void SandboxApp::handleEvents(ds::EventStream* events) {
	if (events->num() > 0) {
		for (uint32_t i = 0; i < events->num(); ++i) {
			int type = events->getType(i);			
			if (type == 102) {
				stopGame();
			}
		}
	}
}


// ---------------------------------------------------------------
// update
// ---------------------------------------------------------------
void SandboxApp::update(float dt) {
	
}
