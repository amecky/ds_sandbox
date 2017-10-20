#pragma once
#include <diesel.h>

struct RTVertex {
	ds::vec3 p;
	ds::vec2 t;

	RTVertex() : p(0.0f), t(0.0f) {}
};

struct RTVConstantBuffer {
	ds::matrix viewprojectionMatrix;
	ds::matrix worldMatrix;
};

class RenderTextureViewer {

public:
	RenderTextureViewer(RID rtID, int size);
	~RenderTextureViewer();
	void render(RID renderPass);
private:
	RTVertex* _vertices;
	ds::matrix _worldMatrix;
	ds::matrix _vpMatrix;
	RID _drawItem;
	RID renderTarget;
	RTVConstantBuffer _constantBuffer;
	RID _renderPass;
};

//RID create_rt_view_draw_item(RID constantBuffer, RID renderPass, RID rtID);
