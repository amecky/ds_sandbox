#include <diesel.h>

struct RenderEnvironment;

void showFilled(RenderEnvironment* env, int segment, int numSegments, const ds::Color& clr);

void rebound(RenderEnvironment* env, const ds::vec2& p, float angle, const ds::Color& clr);