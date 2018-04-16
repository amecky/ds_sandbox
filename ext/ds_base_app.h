#pragma once
#include <diesel.h>
#include <Windows.h>
#include <vector>
#include <stdint.h>

//#define BASE_APP_IMPLEMENTATION

class SpriteBatchBuffer;

namespace ds {

	// ----------------------------------------------------
	// Application settings
	// ----------------------------------------------------
	struct ApplicationSettings {
		bool useIMGUI;
		bool useGPUProfiling;
		int screenWidth;
		int screenHeight;
		const char* windowTitle;
		ds::Color clearColor;
		char guiToggleKey;
		char updateToggleKey;
		char singleStepKey;
	};

	// ----------------------------------------------------
	// event stream
	// ----------------------------------------------------
	class EventStream {

		struct EventHeader {
			uint32_t id;
			uint32_t type;
			size_t size;
		};

	public:
		EventStream();
		~EventStream();
		void reset();
		void add(uint32_t type);
		void add(uint32_t type, void* p, size_t size);
		bool get(uint32_t index, void* p, size_t size) const;
		int getType(uint32_t index) const;
		bool containsType(uint32_t type) const;
		const uint32_t num() const {
			return _num;
		}
	private:
		const static int MAX_ENTRIES = 1024;
		void addHeader(uint32_t type, size_t size);
		EventStream(const EventStream& orig) {}
		char* _data;
		uint32_t _mappings[MAX_ENTRIES];
		uint32_t _num;
		uint32_t _index;
	};

	struct RenderContext {

		RenderContext() {}
		~RenderContext() {}

	};

	// ----------------------------------------------------
	// Scene
	// ----------------------------------------------------
	class Scene {

	public:
		Scene() : _active(false), _initialized(false) {}
		virtual ~Scene() {}
		virtual void beforeRendering() {}
		virtual void render() {}
		virtual void afterRendering() {}
		virtual void update(float dt) {}
		virtual void prepare(ds::EventStream* events, RenderContext* renderContext) {
			if (!_initialized) {
				_events = events;
				_renderContext = renderContext;
				_initialized = true;
			}
		}
		virtual void initialize() {}
		void setActive(bool active) {
			if (active) {
				onActivation();
			}
			else {
				onDeactivation();
			}
			_active = active;
		}
		virtual void onActivation() {}
		virtual void onDeactivation() {}
		virtual void showGUI() {}
		bool isActive() const {
			return _active;
		}
		virtual void OnButtonClicked(int index) {}
	protected:
		RID loadImageFromFile(const char* name);
		ds::EventStream* _events;
		bool _active;
		bool _initialized;
		RenderContext* _renderContext;
	};

	// ----------------------------------------------------
	// SpriteScene
	// ----------------------------------------------------
	class SpriteScene : public Scene {

	public:
		SpriteScene(SpriteBatchBuffer* buffer) : Scene() , _buffer(buffer) {}
		virtual ~SpriteScene() {}
		void beforeRendering();
		void afterRendering();
	protected:
		SpriteBatchBuffer* _buffer;
	};

	// ----------------------------------------------------
	// BaseScene
	// ----------------------------------------------------
	class BaseScene : public Scene {

	public:
		BaseScene() : Scene() {}
		virtual ~BaseScene() {}
		virtual void prepare(ds::EventStream* events, RenderContext* renderContext) {
			if (!_initialized) {
				_events = events;
				_renderContext = renderContext;
				_initialized = true;
				_camera = ds::buildPerspectiveCamera(ds::vec3(0.0f, 3.0f, -6.0f));

				ds::ViewportInfo vpInfo = { ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f };
				_viewPort = ds::createViewport(vpInfo);

				ds::RenderPassInfo rpInfo = { &_camera, _viewPort, ds::DepthBufferState::ENABLED, 0, 0 };
				_basicPass = ds::createRenderPass(rpInfo);

				ds::RenderPassInfo noDepthInfo = { &_camera, _viewPort, ds::DepthBufferState::DISABLED, 0, 0 };
				_noDepthPass = ds::createRenderPass(noDepthInfo);

				ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
				_blendStateID = ds::createBlendState(blendInfo);
			}
		}
	protected:
		RID _viewPort;
		RID _blendStateID;
		RID _basicPass;
		RID _noDepthPass;
		ds::Camera _camera;
	};

	struct ButtonState {
		bool pressed;
		bool clicked;
	};

	// ----------------------------------------------------
	// BaseApp
	// ----------------------------------------------------
	class BaseApp {

		typedef std::vector<Scene*> Scenes;
		typedef std::vector<Scene*>::iterator ScenesIterator;

	public:
		BaseApp();
		virtual ~BaseApp();
		const ApplicationSettings& getSettings() const {
			return _settings;
		}
		void init();
		virtual void initialize() = 0;
		virtual void handleEvents(ds::EventStream* events) {}
		void tick(float dt);
		virtual void render() {}
		virtual void update(float dt) {}
		void pushScene(Scene* scene) {
			scene->prepare(_events, _renderContext);
			scene->initialize();
			scene->onActivation();
			scene->setActive(true);
			_scenes.push_back(scene);
		}
		void popScene() {
			if (!_scenes.empty()) {
				Scene* scene = _scenes[_scenes.size() - 1];
				scene->onDeactivation();
				scene->setActive(false);
			}
			_scenes.pop_back();
		}
		void initializeSettings(const char* settingsFileName);
		void loadSettings();
		bool isRunning() const {
			return _running;
		}		
		SpriteBatchBuffer* createSpriteBatchBuffer(RID textureID, int maxSprites);
		RenderContext* getRenderContext() const {
			return _renderContext;
		}
	protected:
		void stopGame() {
			_running = false;
		}
		RID loadImageFromFile(const char* name);
		RID loadImageFromResource(LPCTSTR name, LPCTSTR type);		
		ApplicationSettings _settings;
		ds::EventStream* _events;
	private:
		void handleButtons();
		SpriteBatchBuffer* _sprites;
		Scenes _scenes;
		float _loadTimer;
		const char* _settingsFileName;
		bool _useTweakables;
		bool _guiKeyPressed;
		bool _guiActive;
		bool _running;
		bool _updateActive;
		bool _updateTogglePressed;
		bool _singleStepPressed;
		RenderContext* _renderContext;
		ButtonState _buttonStates[2];
	};


	
}

extern ds::BaseApp* app;

#ifdef BASE_APP_IMPLEMENTATION
//#include <SpriteBatchBuffer.h>
//#include <stb_image.h>
//#include <ds_tweakable.h>
//#include <ds_imgui.h>

namespace ds {

	void debug(const LogLevel& level, const char* message) {
#ifdef DEBUG
		OutputDebugString(message);
		OutputDebugString("\n");
#endif
	}

	// ---------------------------------------------------------------
	// load image using stb_image
	// ---------------------------------------------------------------
	RID BaseApp::loadImageFromResource(LPCTSTR name, LPCTSTR type) {
		int x, y, n;
		HRSRC resourceHandle = ::FindResource(NULL, name, type);
		if (resourceHandle == 0) {
			return NO_RID;
		}
		DWORD imageSize = ::SizeofResource(NULL, resourceHandle);
		if (imageSize == 0) {
			return NO_RID;
		}
		HGLOBAL myResourceData = ::LoadResource(NULL, resourceHandle);
		void* pMyBinaryData = ::LockResource(myResourceData);
		unsigned char *data = stbi_load_from_memory((const unsigned char*)pMyBinaryData, imageSize, &x, &y, &n, 4);
		ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
		RID textureID = ds::createTexture(info, "Texture");
		stbi_image_free(data);
		UnlockResource(myResourceData);
		FreeResource(myResourceData);
		return textureID;
	}

	RID BaseApp::loadImageFromFile(const char* name) {
		int x, y, n;
		unsigned char *data = stbi_load(name, &x, &y, &n, 4);
		ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
		RID textureID = ds::createTexture(info, name);
		stbi_image_free(data);
		return textureID;
	}

	// ---------------------------------------------------------------
	// load text file
	// ---------------------------------------------------------------
	const char* loadTextFile(LPCTSTR name) {

		HRSRC resourceHandle = ::FindResource(NULL, name, RT_RCDATA);
		if (resourceHandle == 0) {
			return 0;
		}
		DWORD imageSize = ::SizeofResource(NULL, resourceHandle);
		if (imageSize == 0) {
			return 0;
		}
		HGLOBAL myResourceData = ::LoadResource(NULL, resourceHandle);
		char* pMyBinaryData = (char*)::LockResource(myResourceData);
		UnlockResource(myResourceData);
		char* ret = new char[imageSize];
		memcpy(ret, pMyBinaryData, imageSize);
		FreeResource(myResourceData);
		return ret;
	}

	

	// -------------------------------------------------------
	// BaseApp
	// -------------------------------------------------------
	BaseApp::BaseApp() {
		_settings.useIMGUI = false;
		_settings.useGPUProfiling = false;
		_settings.screenWidth = 1280;
		_settings.screenHeight = 720;
		_settings.windowTitle = "BaseApp";
		_settings.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
		_settings.guiToggleKey = 'D';
		_settings.updateToggleKey = 'U';
		_settings.singleStepKey = 'P';
		_events = new ds::EventStream;
		_loadTimer = 0.0f;
		_useTweakables = false;
		_guiKeyPressed = false;
		_updateTogglePressed = false;
		_singleStepPressed = false;
		_guiActive = true;
		_running = true;
		_updateActive = true;
		_sprites = 0;
		_buttonStates[0] = { false, false };
		_buttonStates[1] = { false, false };
		_renderContext = new RenderContext;
	}

	BaseApp::~BaseApp() {
		if (_useTweakables) {
			twk_shutdown();
		}
		if (_settings.useIMGUI) {
			gui::shutdown();
		}
		if (_sprites != 0) {
			delete _sprites;
		}
		delete _renderContext;
		delete _events;
	}

	// ---------------------------------------------------------------
	// handle buttons
	// ---------------------------------------------------------------
	void BaseApp::handleButtons() {
		for (int i = 0; i < 2; ++i) {
			if (ds::isMouseButtonPressed(i)) {
				_buttonStates[i].pressed = true;
			}
			else if (_buttonStates[i].pressed) {
				_buttonStates[i].pressed = false;
				if (!_buttonStates[i].clicked) {
					_buttonStates[i].clicked = true;
				}
				else {
					_buttonStates[i].clicked = false;
				}
			}
		}
	}

	// -------------------------------------------------------
	// init
	// -------------------------------------------------------
	void BaseApp::init() {
		SetThreadAffinityMask(GetCurrentThread(), 1);
		//
		// prepare application
		//
		ds::RenderSettings rs;
		rs.width = _settings.screenWidth;
		rs.height = _settings.screenHeight;
		rs.title = _settings.windowTitle;
		rs.clearColor = _settings.clearColor;
		rs.multisampling = 4;
		rs.useGPUProfiling = _settings.useGPUProfiling;
#ifdef DEBUG
		rs.logHandler = debug;
		rs.supportDebug = true;
#endif
		ds::init(rs);

		if (_settings.useIMGUI) {
			ds::log(LogLevel::LL_DEBUG, "=> IMGUI is enabled");
			gui::init();
		}

		ds::log(LogLevel::LL_DEBUG, "=> Press '%c' to toggle GUI", _settings.guiToggleKey);
		ds::log(LogLevel::LL_DEBUG, "=> Press '%c' to toggle UPDATE", _settings.updateToggleKey);
		ds::log(LogLevel::LL_DEBUG, "=> Press '%c' to for single step", _settings.singleStepKey);
		initialize();
	}

	SpriteBatchBuffer* BaseApp::createSpriteBatchBuffer(RID textureID, int maxSprites) {
		SpriteBatchBufferInfo sbbInfo = { 2048, textureID , ds::TextureFilters::LINEAR };
		_sprites = new SpriteBatchBuffer(sbbInfo);
		return _sprites;
	}

	
	// -------------------------------------------------------
	// intialize settings
	// -------------------------------------------------------
	void BaseApp::initializeSettings(const char* settingsFileName) {
		_settingsFileName = settingsFileName;
		_useTweakables = true;
		ds::log(LogLevel::LL_DEBUG, "=> Tweakables are enabled");
#ifdef DEBUG
		twk_init(_settingsFileName);
#else
		twk_init();
#endif
	}

	// -------------------------------------------------------
	// load settings
	// -------------------------------------------------------
	void BaseApp::loadSettings() {
		if (_useTweakables) {
#ifdef DEBUG
			twk_load();
#else
			const char* txt = loadTextFile(_settingsFileName);
			if (txt != 0) {
				twk_parse(txt);
				delete[] txt;
			}
#endif
		}
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void BaseApp::tick(float dt) {
		// reload tweakables if necessary
		if (_useTweakables) {
#ifdef DEBUG
			_loadTimer += ds::getElapsedSeconds();
			if (_loadTimer >= 1.0f) {
				_loadTimer -= 1.0f;
				twk_load();
			}
#endif
		}
		// toggle show gui
		if (ds::isKeyPressed(_settings.guiToggleKey)) {
			if (!_guiKeyPressed) {
				_guiActive = !_guiActive;
				_guiKeyPressed = true;
			}
		}
		else {
			_guiKeyPressed = false;
		}
		// toggle update loop
		if (ds::isKeyPressed(_settings.updateToggleKey)) {
			if (!_updateTogglePressed) {
				_updateActive = !_updateActive;
				_updateTogglePressed = true;
			}
		}
		else {
			_updateTogglePressed = false;
		}
		// handle single step key
		if (!_updateActive) {
			if (ds::isKeyPressed(_settings.singleStepKey)) {
				if (!_singleStepPressed) {
					_singleStepPressed = true;
				}
			}
			else {
				_singleStepPressed = false;
			}
		}

		handleButtons();
		// send button events to scenes
		for (int i = 0; i < 2; ++i) {
			if (_buttonStates[i].clicked) {
				ScenesIterator it = _scenes.begin();
				while (it != _scenes.end()) {
					if ((*it)->isActive()) {
						(*it)->OnButtonClicked(i);
					}
					++it;
				}
				_buttonStates[i].clicked = false;
			}
		}
		// check if we should update
		bool doUpdate = true;
		if (!_updateActive) {
			if (_singleStepPressed) {
				dt = 1.0f / 60.0f;
			}
			else {
				doUpdate = false;
			}
		}
		// update app
		if (doUpdate) {
			_events->reset();
			update(dt);
		}
		// render app
		render();
		// update scenes
		if (doUpdate) {
			ScenesIterator it = _scenes.begin();
			while (it != _scenes.end()) {
				(*it)->update(dt);
				++it;
			}
		}
		// render scenes
		ScenesIterator it = _scenes.begin();
		while (it != _scenes.end()) {
			(*it)->beforeRendering();
			(*it)->render();
			(*it)->afterRendering();
			++it;
		}
		// render GUI if active
		if (_settings.useIMGUI && _guiActive) {
			it = _scenes.begin();
			while (it != _scenes.end()) {
				(*it)->showGUI();
				++it;
			}
		}
		// handle events if active
		if (doUpdate) {
			handleEvents(_events);
		}
	}

	RID Scene::loadImageFromFile(const char* name) {
		int x, y, n;
		unsigned char *data = stbi_load(name, &x, &y, &n, 4);
		ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
		RID textureID = ds::createTexture(info, name);
		stbi_image_free(data);
		return textureID;
	}

	void SpriteScene::beforeRendering() {
		_buffer->begin();
	}

	void SpriteScene::afterRendering() {
		_buffer->flush();
	}

	// -------------------------------------------------------
	// EventStream
	// -------------------------------------------------------
	EventStream::EventStream() {
		_data = new char[4096];
		reset();
	}

	EventStream::~EventStream() {
		delete[] _data;
	}

	// -------------------------------------------------------
	// reset
	// -------------------------------------------------------
	void EventStream::reset() {
		_num = 0;
		_index = 0;
	}

	// -------------------------------------------------------
	// add event
	// -------------------------------------------------------
	void EventStream::add(uint32_t type, void* p, size_t size) {
		int capacity = sizeof(EventHeader) + size;
		if ((_index + capacity) < 4096 && (_num + 1) < MAX_ENTRIES) {
			addHeader(type, size);
			char* data = _data + _index + sizeof(EventHeader);
			memcpy(data, p, size);						
			_mappings[_num++] = _index;
			_index += capacity;
		}
	}

	// -------------------------------------------------------
	// add event
	// -------------------------------------------------------
	void EventStream::add(uint32_t type) {
		int capacity = sizeof(EventHeader) + sizeof(type);
		if ((_index + capacity) < 4096 && (_num + 1) < MAX_ENTRIES) {
			addHeader(type, 0);
			char* data = _data + _index;			
			_mappings[_num++] = _index;	
			_index += capacity;
		}
	}

	// -------------------------------------------------------
	// add header
	// -------------------------------------------------------
	void EventStream::addHeader(uint32_t type, size_t size) {
		//DBG_LOG("creating event - type: %d size: %d header: %d",type,size,sizeof(EventHeader));
		EventHeader header;
		header.id = _num;
		header.size = size;
		header.type = type;
		char* data = _data + _index;
		memcpy(data, &header, sizeof(EventHeader));
	}

	// -------------------------------------------------------
	// get
	// -------------------------------------------------------
	bool EventStream::get(uint32_t index, void* p, size_t size) const {
		//XASSERT(index < _mappings.size(), "Index out of range");
		int lookup = _mappings[index];
		char* data = _data + lookup;
		EventHeader* header = (EventHeader*)data;
		if (header->size == size) {
			data += sizeof(EventHeader);
			memcpy(p, data, header->size);
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// get type
	// -------------------------------------------------------
	int EventStream::getType(uint32_t index) const {
		//XASSERT(index < _mappings.size(), "Index out of range");
		int lookup = _mappings[index];
		char* data = _data + lookup;
		EventHeader* header = (EventHeader*)data;
		return header->type;
	}

	// -------------------------------------------------------
	// contains type
	// -------------------------------------------------------
	bool EventStream::containsType(uint32_t type) const {
		for (int i = 0; i < _num; ++i) {
			if (getType(i) == type) {
				return true;
			}
		}
		return false;
	}


}

#endif
