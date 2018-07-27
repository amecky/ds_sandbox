#include <diesel.h>
#include <memory>
#include <SpriteBatchBuffer.h>

class HUD {


public:
	void render(const std::unique_ptr<SpriteBatchBuffer>& sprites);
};