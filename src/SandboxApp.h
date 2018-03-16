#pragma once
#include <ds_base_app.h>
#define DS_PERF_PANEL
#include "PerfPanel.h"
#define DS_PROFILER_IMPLEMENTATION
#include <ds_profiler.h>
#define DS_VM_IMPLEMENTATION
#include <ds_vm.h>
#include "flow\FlowFieldApp.h"
#include "binary\BinaryClock.h"

class SandboxApp: public ds::BaseApp {

public:
	SandboxApp();
	~SandboxApp();
	void initialize();
	void handleEvents(ds::EventStream* events);
	void update(float dt);
private:
	FlowFieldApp* _flowFieldScene;
	BinaryClock* _clock;
};