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
#include "sprite_editor.h"
#include "simd_vec.h"
#include "border.h"
#define QUAD_BATCH_IMPLEMENTATION
#include <ds_squares.h>
#include <ds_particles_2D.h>

const int kNUM_SPRITES = 256;

enum SceneType {
	SC_GAME,
	SC_OBSTACLES_EDITOR,
	SC_SPRITE_EDITOR
};

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
	for (int j = 0; j < bullets->array->size; ++j) {
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

void load_grid(ObstaclesContainer* obstacles) {
	char buffer[255];
	FILE* fp = fopen("grid.txt", "r");
	obstacles::clear(obstacles);
	int x = 0;
	int y = 0;
	int r = 0;
	if (fp) {
		while (fgets(buffer, 255, (FILE*)fp)) {
			printf("%s\n", buffer);
			sscanf(buffer, "%d %d %d", &x, &y, &r);
			obstacles::add(obstacles, x, y, r);
		}
		fclose(fp);
	}

}

void save_grid(ObstaclesContainer* obstacles) {
	char buffer[255];
	FILE* fp = fopen("grid.txt", "w");
	if (fp) {
		for ( int x = 0; x < obstacles->dimX; ++x) {
			for (int y = 0; y < obstacles->dimY; ++y) {
				if (obstacles::contains(obstacles, x, y)) {
					fprintf(fp, "%d %d %d\n", x, y, obstacles->grid[x + y * obstacles->dimX]);
				}
			}
		}
		fclose(fp);
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
	rs.clearColor = ds::Color(0.05f, 0.05f, 0.05f, 1.0f);
	rs.multisampling = 4;
	rs.useGPUProfiling = false;
	rs.supportDebug = false;
	ds::init(rs);

	gui::init();

	//gui::setAlphaLevel(0.2f);

	sprites::initialize(SpriteBatchDesc().MaxSprites(4096));

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
	//load_grid(&obstacles);

	bool update = true;

	bool dragging = false;

	bool pressed = false;

	float rtimer = 0.0f;

	SceneType sceneType = SceneType::SC_GAME;

	ObstabcleEditorContext obstacleEditorCtx;
	obstacleEditorCtx.rightButtonPressed = false;
	obstacleEditorCtx.leftButtonPressed = false;
	obstacleEditorCtx.gx = 0;
	obstacleEditorCtx.gy = 0;
	obstacleEditorCtx.rectIndex = 0;

	editor::SpriteEditorContext spriteEditorCtx;
	spriteEditorCtx.textureId = textureId;
	spriteEditorCtx.rect = ds::vec4(0, 0, 256, 256);
	spriteEditorCtx.scale = 1.0f;
	spriteEditorCtx.selection = -1;
	sprintf(spriteEditorCtx.name, "Sprite1");
	spriteEditorCtx.sprites.add("Test", ds::vec4(75, 0, 26, 26));
	spriteEditorCtx.dragging = false;
	spriteEditorCtx.textureOffset = ds::vec2(0.0f);
	spriteEditorCtx.center = ds::vec2(512, 384);

	TrailPieces trailPieces;
	trailPieces.settings.ttl = 0.3f;
	trails::initialize(&trailPieces, 2048);
	trailPieces.textureId = textureId;
	
	float addTimer = 0.0f;
	bool addBoids = false;

	float bulletTime = 0.2f;

	float trailTimer = 0.0f;
	float trailTTL = 0.1f;

	quads::initialize(QuadBatchDesc().MaxSprites(1024));

	Border border;
	initialize_border(&border);
	
	float bt = 0.0f;

	float fixedTimeStep = 1.0f / 60.0f;
	float accumulator = 0.0f;
	float gameTimer = 0.0f;

	ParticleArray particles;

	while (ds::isRunning()) {

		ds::begin();

		

		sprites::begin();

		if (sceneType == SceneType::SC_OBSTACLES_EDITOR) {
			obstacles::edit(&obstacles, &obstacleEditorCtx);
			obstacles::render(&obstacles);
			
		}
		else if (sceneType == SceneType::SC_SPRITE_EDITOR) {			
			editor::render(&spriteEditorCtx);
		}
		else {
			if (ds::isMouseButtonPressed(0)) {
				rtimer += ds::getElapsedSeconds();
				if (rtimer >= bulletTime) {
					rtimer -= bulletTime;
					bullets::add(&bullets, &player);
				}
			}

			if (addBoids && sceneType == SceneType::SC_GAME) {
				addTimer += ds::getElapsedSeconds();
				if (addTimer >= 0.1f) {
					addTimer -= 0.1f;
					boid::add(&boidContainer, ds::getMousePosition(), player.pos);
				}
			}

			player::move(&player, ds::getElapsedSeconds());
			if (update) {

				float delta = ds::getElapsedSeconds();
				if (delta > 0.5f) {
					delta = 0.5f;
				}
				gameTimer += delta;
				while (gameTimer >= fixedTimeStep) {

					bullets::move(&bullets, &obstacles, fixedTimeStep);

					boid::reset_forces(&boidContainer);
					boid::move(&boidContainer, player.pos, &obstacles, fixedTimeStep);
					boid::avoid(&boidContainer, &obstacles);
					if (update) {
						boid::apply_forces(&boidContainer, fixedTimeStep);
					}
					boid::kill_boids(&boidContainer, player.pos, 15.0f);

					check_collisions(&boidContainer, &bullets);

					for (int i = 0; i < bullets.array->size; ++i) {
						ds::vec2 d = bullets.positions[i] - bullets.prevPositions[i];
						if (sqr_length(d) > 16.0f) {
							trails::add(&trailPieces, bullets.prevPositions[i], bullets.rotations[i]);
							bullets.prevPositions[i] = bullets.positions[i];
						}
					}
					trails::tick(&trailPieces, fixedTimeStep);

					check_collisions(&border, &bullets);

					update_border(&border, fixedTimeStep);

					gameTimer -= fixedTimeStep;
				}

			}
			
			sprites::begin();

			trails::render(&trailPieces);

			obstacles::render(&obstacles);

			bullets::render(&bullets);
			
			boid::render(&boidContainer);
			
			player::render(&player);
			
		}

		sprites::flush();

		
		if (sceneType == SceneType::SC_GAME) {
			quads::begin();

			render_border(&border, textureId);

			quads::flush();
		}
		
		//
		// GUI
		//
		/*
		p2i p(10, 758);
		gui::start(&p, 280);
		gui::begin("Grid", 0);
		gui::beginGroup();
		if (gui::Button("GM",20)) {
			sceneType = SceneType::SC_GAME;
		}
		if (gui::Button("OE",20)) {
			sceneType = SceneType::SC_OBSTACLES_EDITOR;
		}
		if (gui::Button("SE",20)) {
			sceneType = SceneType::SC_SPRITE_EDITOR;
		}
		gui::endGroup();
		

		if (sceneType == SceneType::SC_OBSTACLES_EDITOR) {
			
			gui::Value("Grid Pos", p2i(obstacleEditorCtx.gx, obstacleEditorCtx.gy));
			gui::Value("Rect Idx", obstacleEditorCtx.rectIndex);
			gui::beginGroup();
			if (gui::Button("Load")) {
				load_grid(&obstacles);
			}
			if (gui::Button("Save")) {
				save_grid(&obstacles);
			}
			if (gui::Button("Clear")) {
				obstacles::clear(&obstacles);
			}
			gui::endGroup();
			
		}
		else if (sceneType == SceneType::SC_SPRITE_EDITOR) {
			editor::renderGUI(&spriteEditorCtx);
		}
		else {
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
			gui::Value("Bullets", bullets.array->size);
			gui::Value("Trails", trailPieces.array->size);
			gui::Input("Bullet time", &bulletTime);
			gui::Checkbox("Update", &update);
			gui::Checkbox("Add boids", &addBoids);
			gui::Input("Add count", &addCount);
			if (gui::Button("Add boids")) {
				boid::add(&boidContainer, addCount, player.pos);
			}
		}
		gui::end();
		*/
		ds::end();
	}
	gui::shutdown();
	sprites::shutdown();
	bullets::shutdown(&bullets);
	ds::shutdown();
	return 0;
}