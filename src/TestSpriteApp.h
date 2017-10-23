#pragma once
#include "TestApp.h"
#include <SpriteBatchBuffer.h>

class TestSpriteApp : public TestApp {

public:
	TestSpriteApp();

	virtual ~TestSpriteApp();

	virtual ds::RenderSettings getRenderSettings() = 0;

	bool init();

	virtual void tick(float dt) = 0;

	virtual void render() = 0;

protected:

	SpriteBatchBuffer* _spriteBuffer;
};
