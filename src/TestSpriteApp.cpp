#include "TestSpriteApp.h"
#include <ds_imgui.h>

TestSpriteApp::TestSpriteApp() : TestApp() {

}

TestSpriteApp::~TestSpriteApp() {
	delete _spriteBuffer;
}

bool TestSpriteApp::init() {
	// load image using stb_image
	RID textureID = loadImage("content\\TextureArray.png");
	// create the sprite batch buffer
	SpriteBatchBufferInfo sbbInfo = { 2048, textureID, ds::TextureFilters::LINEAR };
	_spriteBuffer = new SpriteBatchBuffer(sbbInfo);
	return true;
}
