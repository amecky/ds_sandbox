#define DS_IMPLEMENTATION
#include <diesel.h>
#define SPRITE_IMPLEMENTATION
#include <ds_sprites.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>

const int kNUM_SPRITES = 256;

float clamp(float d, float minValue, float maxValue) {
	if (d < minValue) {
		return minValue;
	}
	if (d > maxValue) {
		return maxValue;
	}
	return d;
}

float calculate_rotation(const ds::vec2& v) {
	ds::vec2 vn = normalize(v);
	ds::vec2 right(1.0f, 0.0f);
	if (vn != right) {
		float dt = clamp(dot(vn, right), -1.0f, 1.0f);
		float tmp = acos(dt);
		float cross = -1.0f * vn.y;
		if (cross > 0.0f) {
			tmp = 2.0f * ds::PI - tmp;
		}
		return tmp;
	}
	else {
		return 0.0f;
	}
}

float get_rotation(const ds::vec2& v) {
	ds::vec2 xa = ds::vec2(1.0f, 0.0f);
	if (v != xa) {
		ds::vec2 vn1 = normalize(xa);
		ds::vec2 vn2 = normalize(v);
		float dt = dot(vn1, vn2);
		if (dt < -1.0f) {
			dt = -1.0f;
		}
		if (dt > 1.0f) {
			dt = 1.0f;
		}
		float tmp = acos(dt);
		/*
		float crs = cross(vn1, vn2);
		//LOG << "cross " << crs;
		if (crs < 0.0f) {
			tmp = ds::TWO_PI - tmp;
		}
		*/
		return tmp;
	}
	else {
		return 0.0f;
	}
}

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
	RID nextTextureId = loadImageFromFile("test.png");

	Blocks blocks;
	blocks.positions = new ds::vec2[kNUM_SPRITES];
	blocks.velocities = new ds::vec2[kNUM_SPRITES];
	blocks.rotations = new float[kNUM_SPRITES];

	for (int i = 0; i < kNUM_SPRITES; ++i) {
		blocks.positions[i] = ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 600.0f));
		float angle = ds::random(0.0f, ds::TWO_PI);
		blocks.rotations[i] = angle;
		blocks.velocities[i] = ds::vec2(cos(angle), sin(angle)) * ds::random(100.0f, 250.0f);
	}

	Sprite sprites[kNUM_SPRITES];
	for (int i = 0; i < kNUM_SPRITES; ++i) {
		Sprite& sp = sprites[i];
		sp.position = ds::vec2(ds::random(100.0f, 900.0f), ds::random(100.0f, 600.0f));
		sp.textureID = textureId;
		sp.scaling = ds::vec2(1.0f);
		sp.rotation = ds::random(0.0f, ds::TWO_PI);
		sp.color = ds::Color(255, 255, 255, 255);
		sp.textureRect = ds::vec4(40, 75, 40, 40);
	}
	
	int type = 0;

	while (ds::isRunning()) {

		ds::begin();

		sprites::begin();

		sprites::draw(nextTextureId, ds::vec2(100, 200), ds::vec4(0, 0, 200, 100));

		sprites::draw(textureId, ds::vec2(512,384),ds::vec4(40,75,40,40));

		if (type == 0) {
			for (int i = 0; i < kNUM_SPRITES; ++i) {
				sprites::draw(textureId, blocks.positions[i], ds::vec4(40, 75, 40, 40), ds::vec2(1.0f), blocks.rotations[i]);
			}
		}
		else {
			sprites::draw(sprites, kNUM_SPRITES);
		}

		sprites::flush();

		if (type == 0) {
			move(blocks.positions, blocks.velocities, blocks.rotations, kNUM_SPRITES, ds::vec4(50.0f, 50.0f, 900.0f, 650.0f));
		}
		if (type == 1) {
			for (int i = 0; i < kNUM_SPRITES; ++i) {
				ds::vec2 v = ds::vec2(cos(sprites[i].rotation), sin(sprites[i].rotation));
				sprites[i].position += v * 20.0f * ds::getElapsedSeconds();
				bool bounced = false;
				if (sprites[i].position.x < 50.0f || sprites[i].position.x > 900.0f) {
					v.x *= -1.0f;
					bounced = true;
				}
				if (sprites[i].position.y < 50.0f || sprites[i].position.y > 600.0f) {
					v.y *= -1.0f;
					bounced = true;
				}
				if (bounced) {
					sprites[i].rotation = calculate_rotation(v);
					sprites[i].position += v * 20.0f * ds::getElapsedSeconds();
				}
			}
		}
		p2i p(10, 758);
		gui::start(&p, 250);
		gui::begin("Sprites", 0);
		gui::Value("FPS", ds::getFramesPerSecond());
		gui::Value("DrawCalls", ds::numDrawCalls());
		gui::StepInput("Type", &type,0,1,1);
		gui::end();
		
		ds::end();
	}
	gui::shutdown();
	sprites::shutdown();
	ds::shutdown();
	return 0;
}