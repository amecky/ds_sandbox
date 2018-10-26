#define DS_IMPLEMENTATION
#include <diesel.h>
#define SPRITE_IMPLEMENTATION
#include <ds_sprites.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>
#include "Boids.h"
#include "Obstacles.h"
#include "math.h"

const int kNUM_SPRITES = 256;

RID loadImageFromFile(const char* name) {
	int x, y, n;
	unsigned char *data = stbi_load(name, &x, &y, &n, 4);
	if (data != 0) {
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
	return NO_RID;
}

struct Blocks {
	ds::vec2* positions;
	ds::vec2* velocities;
	float* rotations;
};

void move(ds::vec2* positions, ds::vec2* velocities, float* rotations, int num, const ds::vec4& boundingRect) {
	for (int i = 0; i < num; ++i) {
		positions[i] += velocities[i] * ds::getElapsedSeconds();
		bool bounced = false;
		if (positions[i].x < boundingRect.x || positions[i].x > boundingRect.z) {
			velocities[i].x *= -1.0f;
			bounced = true;
		}
		if (positions[i].y < boundingRect.y || positions[i].y > boundingRect.w) {
			velocities[i].y *= -1.0f;
			bounced = true;
		}
		if (bounced) {
			rotations[i] = calculate_rotation(velocities[i]);
			positions[i] += velocities[i] * ds::getElapsedSeconds();
		}
	}
}

struct Obstacle {
	ds::vec2 position;
};
// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
	//
	// prepare application
	//
	ds::RenderSettings rs;
	rs.width = 1024;
	rs.height = 768;
	rs.title = "Triangle demo";
	rs.clearColor = ds::Color(0.1f, 0.1f, 0.1f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	ds::init(rs);

	gui::init();

	//gui::setAlphaLevel(0.2f);

	sprites::initialize(SpriteBatchDesc().MaxSprites(2048));

	RID textureId = loadImageFromFile("Textures.png");
	if (textureId == NO_RID) {
		exit(-1);
	}

	BoidSettings settings;
	settings.minDistance = 20.0f;
	settings.relaxation = 1.0f;
	settings.separate = true;
	settings.seek = true;
	settings.seekVelocity = 20.0f;
	Boids boids(textureId, &settings);

	RID nextTextureId = loadImageFromFile("test.png");

	ds::vec2 target(512, 384);

	int addCount = 20;

	Obstacles obstacles(textureId);

	while (ds::isRunning()) {

		ds::begin();

		if (ds::isMouseButtonClicked(0)) {
			target = ds::getMousePosition();
		}

		if (ds::isMouseButtonClicked(1)) {
			obstacles.add(ds::getMousePosition());
		}

		sprites::begin();

		sprites::draw(nextTextureId, ds::vec2(100, 200), ds::vec4(0, 0, 200, 100));

		sprites::draw(textureId, target,ds::vec4(40,75,40,40));

		obstacles.render();

		boids.move(target, ds::getElapsedSeconds());
		boids.render();

		sprites::flush();

		p2i p(10, 758);
		gui::start(&p, 250);
		gui::begin("Sprites", 0);
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("DrawCalls", ds::numDrawCalls());
		gui::Input("Min Dist", &settings.minDistance);
		gui::Input("Relaxation", &settings.relaxation);
		gui::Input("Seek Vel", &settings.seekVelocity);
		gui::Checkbox("Separate", &settings.separate);
		gui::Checkbox("Seek", &settings.seek);
		gui::Input("Add count", &addCount);
		if (gui::Button("Add boids")) {
			boids.add(addCount);
		}
		gui::end();
		
		ds::end();
	}
	gui::shutdown();
	sprites::shutdown();
	ds::shutdown();
	return 0;
}