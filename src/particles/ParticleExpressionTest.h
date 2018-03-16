#pragma once
#include "..\TestSpriteApp.h"
#include "Particles.h"
#include <ds_profiler.h>
#include <ds_imgui.h>


class ParticleExpressionTest : public TestSpriteApp {

public:	
	ParticleExpressionTest();
	~ParticleExpressionTest();
	ds::RenderSettings getRenderSettings();
	bool usesGUI() {
		return true;
	}
	bool init();
	void tick(float dt);
	void render();
	void renderGUI();
private:
	void loadExpressionsFile(Particlesystem* system);
	void renderExpressions(Particlesystem* system);
	void convertFloat(float v,int precision);
	int _dialogState;
	p2i _dialogPos;
	int _numParticles;
	float _emissionTime;
	bool _showParticleInfo;
	bool _showExpressions;
	int _selectedType;
	int _selectedExpression;
	char _tmpBuffer[64];
	
	//log_panel_plugin* _logPanelPlugin;
	Particlesystem* _system;
};

