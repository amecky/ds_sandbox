#pragma once

struct ds_api_registry {

	void (*add)(const char* name, void* interf);

	void* (*get)(const char* name);
};

ds_api_registry create_registry();

void shutdown_registry();