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

	virtual bool useFixedTimestep() const {
		return false;
	}

	virtual void shutdown() {}

protected:

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

};
