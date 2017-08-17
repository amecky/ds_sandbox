#define DS_IMPLEMENTATION
#include <diesel.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#include "..\resource.h"
#include "utils\tweening.h"
#include "TweeningTest.h"
#include "AnimationTest.h"
// ---------------------------------------------------------------
// load image from the resources
// ---------------------------------------------------------------
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

// ---------------------------------------------------------------
// initialize rendering system
// ---------------------------------------------------------------
void initialize() {
	ds::RenderSettings rs;
	rs.width = 1280;
	rs.height = 960;
	rs.title = "ds_sandbox";
	rs.clearColor = ds::Color(0.0f, 0.0f, 0.0f, 1.0f);
	rs.multisampling = 4;
	ds::init(rs);
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	
	//_CrtSetBreakAlloc(237);

	initialize();
	
	gui::init();

	// load image using stb_image
	RID textureID = loadImage("content\\TextureArray.png");

	// create the sprite batch buffer
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::POINT };
	SpriteBatchBuffer spriteBuffer(sbbInfo);
	
	bool running = true;

	TweeningTest tweeningTest(&spriteBuffer);

	AnimationTest animationTest(&spriteBuffer);

	
	while (ds::isRunning() && running) {

		ds::begin();

		spriteBuffer.begin();		
		
		//tweeningTest.render();
		animationTest.render();

		//font::renderText(ds::vec2(20, 550), "Hello World", &spriteBuffer);
		//font::renderText(ds::vec2(20, 90), "RSTUVWXYZ", &spriteBuffer);
		
		spriteBuffer.flush();

		ds::Event event;
		while (ds::get_event(&event)) {
			if (event.type == ds::EventType::ET_MOUSEBUTTON_PRESSED) {				
				animationTest.onButtonClicked(event.mouse.button);
			}
		}

		//tweeningTest.renderGUI();
		animationTest.renderGUI();

		ds::end();

		
	}
	gui::shutdown();
	ds::shutdown();
}