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
#define DS_TWEENING_IMPLEMENTATION
#include <ds_tweening.h>
#define DS_PROFILER
#include <ds_profiler.h>
#include "WarpingGrid.h"
#include "ColorRing.h"
#include "math.h"
#include "Board.h"
#include "common\common.h"
#include "utils\PerfPanel.h"

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
	rs.clearColor = ds::Color(10,10,10,255);
	rs.multisampling = 4;
	rs.logHandler = my_debug;
#ifdef DEBUG
	rs.supportDebug = true;
#endif
	ds::init(rs);

	gui::init();

	gui::setAlphaLevel(0.2f);

	perf::init();
	
	float reloadTimer = 0.0f;

	

	RID vertexShader = ds::createShader(ds::ShaderDesc()
		.CSOName("Solid_vs.cso")
		.ShaderType(ds::ShaderType::ST_VERTEX_SHADER)
		);

	RID pixelShader = ds::createShader(ds::ShaderDesc()
		.CSOName("Solid_ps.cso")
		.ShaderType(ds::ShaderType::ST_PIXEL_SHADER)
		);

	RenderEnvironment env;
	env.textureId = loadImageFromFile("Textures.png");
	prepareRenderEnvironment(&env, vertexShader, pixelShader, 144 * 4);

	Board board(&env);

	bool rightPressed = false;

	board.reset();

	PerfPanel perfPanel;

	int perfPanelState = 1;

	bool running = true;

	int spriteIndex = -1;

	while (ds::isRunning()) {

		perf::reset();

		ds::begin();

		{
			perf::ZoneTracker("Tick");
			if (running) {
				board.tick(ds::getElapsedSeconds());
			}
		}

		if (ds::isKeyPressed('A')) {
			board.debug();
		}

		if (ds::isMouseButtonPressed(1)) {
			rightPressed = true;
		}
		else if ( rightPressed) {
			rightPressed = false;
			board.selectNextColor();
		}
		
		{
			perf::ZoneTracker("Render");
			board.render();
		}
		/*
		{
			perf::ZoneTracker("Debug");
			ds::dbgPrint(0, 36, "     FPS: %d", ds::getFramesPerSecond());
			int p = 35;
			for (int i = 0; i < perf::num_events(); ++i) {
				ds::dbgPrint(0, p, "%8s: %2.6f ms", perf::get_name(i), perf::avg(i));
				--p;
			}
		}
		*/

		p2i p(10, 758);
		gui::start(&p, 250);

		perfPanel.tick(ds::getElapsedSeconds());

		perfPanel.render();

		gui::begin("Game", 0);
		gui::Checkbox("Running", &running);
		gui::Value("Sprites", env.spriteArray->numObjects);
		gui::Slider("Index", &spriteIndex,0,env.spriteArray->numObjects-1);
		if (spriteIndex != -1) {
			Sprite& sp = env.spriteArray->objects[spriteIndex];
			gui::Value("ID", sp.id);
			gui::Input("Pos", &sp.position);
			gui::Input("Scale", &sp.scaling);
			gui::SliderAngle("Rotation", &sp.rotation);
			gui::Input("Texture", &sp.textureRect);
			gui::Input("Color", &sp.color);
		}

		gui::end();

		ds::end();

		perf::finalize();

		reloadTimer += ds::getElapsedSeconds();
		if (reloadTimer >= 0.3f) {
			reloadTimer -= 0.3f;
			ReloadChangedTweakableValues();
		}
	}

	delete env.sprites;
	delete env.particles;
	sprites::shutdown(*env.spriteArray);
	gui::shutdown();

	ds::shutdown();
}