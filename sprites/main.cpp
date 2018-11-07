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
#include "player.h"

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

bool check_collision(const ds::vec2& p, float radius, Bullets* bullets) {
	for (int j = 0; j < bullets->num; ++j) {
		if (math::collides(p, radius, bullets->positions[j], 8.0f)) {
			bullets::remove(bullets, j);
			return true;
		}
	}
	return false;
}

void check_collisions(BoidContainer* boids, Bullets* bullets) {
	for (int i = 0; i < boids->num; ++i) {
		ds::vec2 bp = boids->positions[i];
		if (check_collision(bp, 10.0f, bullets)) {
			boid::kill_boid(boids, i);
		}
	}
}
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
	rs.clearColor = ds::Color(0.0f, 0.0f, 0.0f, 1.0f);
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
	//ds::vec2 target(512, 384);
	RID nextTextureId = loadImageFromFile("test.png");

	Player player;
	player.rotation = 0.0f;
	player.textureId = textureId;
	player.pos = ds::vec2(700, 384);

	Bullets bullets;
	bullets::initialize(&bullets, 256);
	bullets.textureId = textureId;

	BoidContainer boidContainer;
	boid::initialize(&boidContainer, 512);
	boidContainer.settings.minDistance = 20.0f;
	boidContainer.settings.relaxation = 100.0f;
	boidContainer.settings.separate = true;
	boidContainer.settings.seek = true;
	boidContainer.settings.seekVelocity = 150.0f;
	boidContainer.settings.textureId = textureId;
	boidContainer.settings.avoidVelocity = 400.0f;
	boid::add(&boidContainer, ds::vec2(100,100), player.pos);

	int addCount = 4;

	ObstaclesContainer obstacles;
	obstacles::intialize(&obstacles, textureId, 40, 20);
	obstacles::add(&obstacles,  9, 10);
	obstacles::add(&obstacles, 10, 10);
	obstacles::add(&obstacles, 11, 10);
	obstacles::add(&obstacles, 12, 10);
	obstacles::add(&obstacles, 12, 11);
	obstacles::add(&obstacles, 12, 9);
	obstacles::add(&obstacles, 13, 10);
	obstacles::add(&obstacles, 14, 10);
	obstacles::add(&obstacles, 15, 10);

	bool update = true;

	bool dragging = false;

	bool pressed = false;

	float rtimer = 0.0f;

	while (ds::isRunning()) {

		ds::begin();

		if (ds::isMouseButtonPressed(0)) {
			rtimer += ds::getElapsedSeconds();
			if (rtimer >= 0.1f) {
				rtimer -= 0.1f;
				boid::add(&boidContainer, ds::getMousePosition(), player.pos);
				bullets::add(&bullets, &player);
			}
		}
		
		player::move(&player, ds::getElapsedSeconds());

		bullets::move(&bullets, &obstacles, ds::getElapsedSeconds());

		sprites::begin();

		obstacles::render(&obstacles);	

		bullets::render(&bullets);

		player::render(&player);

		boid::reset_forces(&boidContainer);
		boid::move(&boidContainer, player.pos, &obstacles, ds::getElapsedSeconds());
		boid::avoid(&boidContainer, &obstacles);
		if (update) {
			boid::apply_forces(&boidContainer, ds::getElapsedSeconds());
		}
		boid::kill_boids(&boidContainer, player.pos, 15.0f);
		boid::render(&boidContainer);

		check_collisions(&boidContainer, &bullets);

		sprites::flush();

		p2i p(10, 758);
		gui::start(&p, 250);
		gui::begin("Sprites", 0);
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("DrawCalls", ds::numDrawCalls());
		//gui::Value("Target", target);
		gui::Input("Min Dist", &boidContainer.settings.minDistance);
		gui::Input("Relaxation", &boidContainer.settings.relaxation);
		gui::Input("Seek Vel", &boidContainer.settings.seekVelocity);
		gui::Checkbox("Separate", &boidContainer.settings.separate);
		gui::Checkbox("Seek", &boidContainer.settings.seek);
		gui::Input("Avoid Vel", &boidContainer.settings.avoidVelocity);
		gui::Value("Boids", boidContainer.num);
		gui::Value("Bullets", bullets.num);
		gui::Checkbox("Update", &update);
		gui::Input("Add count", &addCount);
		if (gui::Button("Add boids")) {
			boid::add(&boidContainer,addCount, player.pos);
		}
		
		gui::end();
		
		ds::end();
	}
	gui::shutdown();
	sprites::shutdown();
	bullets::shutdown(&bullets);
	ds::shutdown();
	return 0;
}