#pragma once
#include <diesel.h>
#include <Windows.h>
#include <stb_image.h>
#include "..\resource.h"

class TestApp {

public:
	TestApp() {}

	virtual ~TestApp() {}

	virtual ds::RenderSettings getRenderSettings() = 0;

	virtual bool init() = 0;

	virtual void tick(float dt) = 0;

	virtual void render() = 0;

	virtual bool usesGUI() = 0;

	virtual void renderGUI() {}

	virtual void OnReload() {}

	virtual void OnButtonClicked(int index) {}

	virtual bool useFixedTimestep() const {
		return false;
	}

	virtual const char* getSettingsFileName() const {
		return 0;
	}

	void prepareDefaults() {
		_camera = ds::buildPerspectiveCamera(ds::vec3(0.0f, 3.0f, -6.0f));

		ds::ViewportInfo vpInfo = { ds::getScreenWidth(), ds::getScreenHeight(), 0.0f, 1.0f };
		RID vp = ds::createViewport(vpInfo);

		ds::RenderPassInfo rpInfo = { &_camera, vp, ds::DepthBufferState::ENABLED, 0, 0 };
		_basicPass = ds::createRenderPass(rpInfo);

		ds::BlendStateInfo blendInfo = { ds::BlendStates::SRC_ALPHA, ds::BlendStates::SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, ds::BlendStates::INV_SRC_ALPHA, true };
		_blendStateID = ds::createBlendState(blendInfo);
	}

	virtual void shutdown() {}

protected:
	RID _blendStateID;
	RID _basicPass;
	ds::Camera _camera;

	RID loadImage(const char* name) {
		int x, y, n;
		HRSRC resourceHandle = ::FindResource(NULL, MAKEINTRESOURCE(IDB_PNG2), "PNG");
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
		RID textureID = ds::createTexture(info, name);
		stbi_image_free(data);
		UnlockResource(myResourceData);
		FreeResource(myResourceData);
		return textureID;
	}

	RID loadImageFromFile(const char* name) {
		int x, y, n;
		unsigned char *data = stbi_load(name, &x, &y, &n, 4);
		ds::TextureInfo info = { x,y,n,data,ds::TextureFormat::R8G8B8A8_UNORM , ds::BindFlag::BF_SHADER_RESOURCE };
		RID textureID = ds::createTexture(info, name);
		stbi_image_free(data);
		return textureID;
	}

};
