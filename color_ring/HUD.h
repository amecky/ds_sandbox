#include <diesel.h>
#include <memory>
#include <SpriteBatchBuffer.h>

class HUD {


public:
	HUD();
	void render(const std::unique_ptr<SpriteBatchBuffer>& sprites);
private: 
	void drawNumber(const std::unique_ptr<SpriteBatchBuffer>& sprites,const ds::vec2& pos, int value, int digits);
	int _score;
};