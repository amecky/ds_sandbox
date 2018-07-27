#define DS_IMPLEMENTATION
#include <diesel.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#define DS_TWEAKABLE_IMPLEMENTATION
#include <ds_tweakable.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define SPRITE_IMPLEMENTATION
#include <SpriteBatchBuffer.h>
#include "WarpingGrid.h"
#include "ColorRing.h"
#include "math.h"
#include "Board.h"

void my_debug(const LogLevel& level, const char* message) {
	OutputDebugString(message);
	OutputDebugString("\n");
}


RID loadImageFromFile(const char* name) {
	int x, y, n;
	unsigned char *data = stbi_load(name, &x, &y, &n, 4);
	RID textureID = ds::createTexture(ds::TextureDesc()
		.BindFlags(ds::BindFlag::BF_SHADER_RESOURCE)
		.Channels(n)
		.Data(data)
		.Format(ds::TextureFormat::R8G8B8A8_UNORM)
		.Width(x)
		.Height(y)
		, name
		);
	stbi_image_free(data);
	return textureID;
}




// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	// prepare application
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Color Ring";
	rs.clearColor = ds::Color(20,0,0,255);
	rs.multisampling = 4;
	rs.logHandler = my_debug;
#ifdef DEBUG
	rs.supportDebug = true;
#endif
	ds::init(rs);
	
	float reloadTimer = 0.0f;

	RID tid = loadImageFromFile("Textures.png");

	Board board(tid);

	bool rightPressed = false;

	board.reset();

	while (ds::isRunning()) {

		ds::begin();

		board.tick(ds::getElapsedSeconds());

		if (ds::isKeyPressed('A')) {
			board.debug();
		}
		/*
		ds::vec2 vel = ds::vec2(0.0f);
		if (ds::isKeyPressed('A')) {
			vel.x -= 100.0f;
		}
		if (ds::isKeyPressed('D')) {
			vel.x += 100.0f;
		}
		if (ds::isKeyPressed('W')) {
			vel.y += 100.0f;
		}
		if (ds::isKeyPressed('S')) {
			vel.y -= 100.0f;
		}

		*/
		if (ds::isMouseButtonPressed(1)) {
			rightPressed = true;
		}
		else if ( rightPressed) {
			rightPressed = false;
			board.selectNextColor();
		}
		
		board.render();

		ds::end();

		reloadTimer += ds::getElapsedSeconds();
		if (reloadTimer >= 0.3f) {
			reloadTimer -= 0.3f;
			ReloadChangedTweakableValues();
		}
	}

	ds::shutdown();
}