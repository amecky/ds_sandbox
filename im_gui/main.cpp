#define DS_IMPLEMENTATION
#include <diesel.h>
#define DS_IMGUI_IMPLEMENTATION
#include <ds_imgui.h>

struct TestSettings {
	int iv;
	float fv;
	bool bv;
	int stepValue;
	ds::vec2 v2;
	ds::vec3 v3;
	int state;
	ds::Color color;
	float sinTable[36];
	float hTable[16];
	int menu;
	int menuItem;
	int diagramState;
	int listIndex;
	int listOffset;
	int valueState;
	int dropState;
	int dopIndex;
	int dropOffset;
	float angle;
	int selectedTab;
};
// ---------------------------------------------------------------
// initialize rendering system
// ---------------------------------------------------------------
void initialize() {
	ds::RenderSettings rs;
	rs.width = 1280;
	rs.height = 720;
	rs.title = "IMGUI Demo";
	rs.clearColor = ds::Color(0.0f, 0.0f, 0.0f, 1.0f);
	rs.multisampling = 4;
	ds::init(rs);
}

char* message = "Hello World";
const char* ITEMS[] = { "First","Second","Third","Fourth" };
const char* MENU_ONE_ITEMS[] = { "Open","Close","Three","Four" };
// ---------------------------------------------------------------
// show menu
// ---------------------------------------------------------------
int showDialog(TestSettings* settings) {
	int ret = 0;
	p2i p(20, 720);
	gui::start(&p, 500);
	
	gui::BeginMenu(ITEMS, 4, &settings->menu);
	if (settings->menu == 1) {
		if (gui::MenuItems(MENU_ONE_ITEMS, 4, &settings->menuItem)) {
			if (settings->menuItem != -1) {
				settings->menu = -1;
				settings->menuItem = -1;
			}
		}
	}
	/*
	if (gui::MenuBar("First")) {
		settings->menu = 1;
	}
	if (gui::MenuBar("Second")) {
		settings->menu = 2;
	}
	if (gui::MenuBar("Third")) {
		settings->menu = 3;
	}
	if (gui::MenuBar("Fourth")) {
		settings->menu = 4;
	}
	*/
	gui::EndMenu();

	const char* TABS[] = { "Basic","Values","Diagram"};

	
	gui::begin("Basic elements",&settings->state);

	gui::Tabs(TABS, 3, &settings->selectedTab);

	if (settings->selectedTab == 0) {
		ds::vec2 mp = ds::getMousePosition();
		gui::Value("Mouse Position", mp);
		gui::Value("Menu", settings->menu);
		gui::Text("Simple text example");
		gui::Label("Pos", "100.0 200.0");
		gui::Label("Message", "Hello");
		gui::Input("Value", &settings->iv);
		gui::Input("Float Value", &settings->fv);
		gui::Checkbox("Check me", &settings->bv);
		gui::Separator();
		gui::StepInput("Step input", &settings->stepValue, 0, 100, 5);
		gui::Input("Vec2 value", &settings->v2);
		gui::Input("Vec3 value", &settings->v3);
		gui::Input("Color value", &settings->color);
		gui::ListBox("Listbox", ITEMS, 4, &settings->listIndex, &settings->listOffset, 3);
		gui::Slider("Slider", &settings->iv, 0, 200,100.0f);
		gui::SliderAngle("Angle Slider", &settings->angle);
		gui::DropDownBox("Dropdown", ITEMS, 4, &settings->dropState, &settings->dopIndex, &settings->dropOffset, 3, true);
		gui::Button("Button");
	}
	if ( settings->selectedTab == 1) {
		gui::Value("Int", 200);
		gui::Value("Float", 123.0f);
		gui::Value("Vec2", ds::vec2(100.0f, 200.0f));
		gui::Value("Vec3", ds::vec3(12.0f, 34.0f, 56.0f));
		gui::Value("Vec4", ds::vec4(10, 20, 50, 60));
		gui::Value("Color", ds::Color(192, 32, 64, 128));
		gui::Message("This is a %d kind of %d", 100, 200);
	}
	if (settings->selectedTab == 2) {
		gui::Histogram(settings->hTable, 16, 0.0f, 20.0f, 5.0f, 300.0f,200.0f);
		gui::Diagram(settings->sinTable, 36, -1.0f, 1.0f, 0.5f);
		gui::beginGroup();
		if (gui::Button("OK")) {
			ret = 1;
		}
		if (gui::Button("Cancel")) {
			ret = 2;
		}
		gui::endGroup();
	}
	gui::end();
	return ret;
}

int showSimpleDialog(TestSettings* settings) {
	int ret = 0;
	p2i p(20, 720);
	gui::start(&p, 500);
	gui::begin("Basic elements", &settings->state);
	gui::Button("Button");
	gui::end();
	return ret;
}

// ---------------------------------------------------------------
// main method
// ---------------------------------------------------------------
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow) {
int main(int argc, char *argv[]) {
	
	initialize();

	gui::init();

	TestSettings settings;
	settings.bv = true;
	settings.stepValue = 20;
	settings.iv = 120;
	settings.fv = 4.0f;
	settings.v2 = ds::vec2(100, 200);
	settings.v3 = ds::vec3(100, 200, 300);
	settings.color = ds::Color(192, 32, 64, 255);
	settings.state = 1;
	settings.selectedTab = 0;
	for (int i = 0; i < 36; ++i) {
		settings.sinTable[i] = sin(static_cast<float>(i) / 36.0f * ds::PI * 2.0f);
	}
	for (int i = 0; i < 16; ++i) {
		settings.hTable[i] = ds::random(5.0f, 15.0f);
	}
	settings.menu = -1;
	settings.menuItem = -1;
	settings.diagramState = 0;
	settings.listIndex = -1;
	settings.listOffset = 0;
	settings.valueState = 0;
	settings.dropState = 0;
	settings.dopIndex = -1;
	settings.dropOffset = 0;
	settings.angle = ds::PI;
	while (ds::isRunning()) {

		ds::begin();

		int ret = showDialog(&settings);

		//showSimpleDialog(&settings);

		ds::end();
	}
	ds::shutdown();
}