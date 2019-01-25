#include "sprite_editor.h"
#include <ds_imgui.h>

namespace editor {

	// ---------------------------------------------------------------------------
	// handle dragging
	// ---------------------------------------------------------------------------
	static void check_dragging(SpriteEditorContext* ctx) {
		if (ds::isMouseButtonPressed(0)) {
			ds::vec2 mp = ds::getMousePosition();
			mp[0] -= 300.0f;
			if (mp[0] >= ctx->rect.x && mp[0] <= (ctx->rect.x + ctx->rect.z)) {
				if (mp[1] <= (768.0f - ctx->rect.y) && mp[1] >= (768.0f - ctx->rect.y - ctx->rect.w)) {
					if (!ctx->dragging) {
						ds::vec2 center = ds::vec2(ctx->rect.x, 768.0f - ctx->rect.y);
						ctx->draggingPos = mp - center;
					}
					ctx->dragging = true;
				}
			}
			else {
				ctx->dragging = false;
			}
		}
		else {
			ctx->dragging = false;
		}

		if (ctx->dragging) {
			ds::vec2 mp = ds::getMousePosition();
			mp[0] -= 300.0f;
			ds::vec2 delta = mp - ctx->draggingPos;
			ctx->rect.x = delta.x();
			ctx->rect.y = 768.0f - delta.y();
		}
	}

	// ---------------------------------------------------------------------------
	// render selection box
	// ---------------------------------------------------------------------------
	static void render_selection_box(SpriteEditorContext* ctx) {
		float xd = ctx->rect.z * ctx->scale;
		float yd = ctx->rect.w * ctx->scale;
		float sx = xd / 100.0f;
		float sy = yd / 100.0f;

		float rxp = 300.0f + xd * 0.5f + ctx->rect.x;
		float ryp = 768.0f - yd * 0.5f - ctx->rect.y;

		sprites::draw(ctx->textureId, ds::vec2(rxp, ryp + yd * 0.5f), ds::vec4(200, 480, 100, 4), ds::vec2(sx, 1.0f));
		sprites::draw(ctx->textureId, ds::vec2(rxp, ryp - yd * 0.5f), ds::vec4(200, 480, 100, 4), ds::vec2(sx, 1.0f));
		sprites::draw(ctx->textureId, ds::vec2(rxp + xd * 0.5f, ryp), ds::vec4(350, 350, 4, 100), ds::vec2(1.0f, sy));
		sprites::draw(ctx->textureId, ds::vec2(rxp - xd * 0.5f, ryp), ds::vec4(350, 350, 4, 100), ds::vec2(1.0f, sy));

		sprites::draw(ctx->textureId, ds::vec2(rxp - xd * 0.5f, ryp + yd * 0.5f), ds::vec4(360, 350, 8, 8));
		sprites::draw(ctx->textureId, ds::vec2(rxp + xd * 0.5f, ryp + yd * 0.5f), ds::vec4(360, 350, 8, 8));
		sprites::draw(ctx->textureId, ds::vec2(rxp - xd * 0.5f, ryp - yd * 0.5f), ds::vec4(360, 350, 8, 8));
		sprites::draw(ctx->textureId, ds::vec2(rxp + xd * 0.5f, ryp - yd * 0.5f), ds::vec4(360, 350, 8, 8));
	}

	// ---------------------------------------------------------------------------
	// render sprite preview
	// ---------------------------------------------------------------------------
	static void render_sprite_preview(SpriteEditorContext* ctx) {
		float bsx = ctx->rect.z / 40.0f;
		float bsy = ctx->rect.w / 40.0f;
		sprites::draw(ctx->textureId, ds::vec2(150.0f, 150.0f), ds::vec4(150, 0, 40, 40), ds::vec2(bsx, bsy));

		sprites::draw(ctx->textureId, ds::vec2(150.0f, 150.0f), ctx->rect);
	}

	// ---------------------------------------------------------------------------
	// render
	// ---------------------------------------------------------------------------
	void render(SpriteEditorContext* ctx) {
		float step = 128.0f * ctx->scale;
		float dd = step * 0.5f;
		for (int x = 0; x < 6; ++x) {
			for (int y = 0; y < 6; ++y) {
				sprites::draw(ctx->textureId, ds::vec2(364 + x * 128, 64 + y * 128), ds::vec4(200, 350, 128, 128));
			}
		}
		
		sprites::draw(ctx->textureId, ds::vec2(512 + 300, 384), ds::vec4(0.0f, 0.0f, 1024.0f, 768.0f), ds::vec2(ctx->scale));


		render_selection_box(ctx);

		render_sprite_preview(ctx);
		
		check_dragging(ctx);
	}

	// ---------------------------------------------------------------------------
	// save binary and text file
	// ---------------------------------------------------------------------------
	static void save(SpriteEditorContext* ctx) {
		FILE* fp = fopen("sprites.bin","w");
		char name[20];
		if (fp) {
			for (int i = 0; i < ctx->sprites.size(); ++i) {
				for (int j = 0; j < 20; ++j) {
					name[j] = '\0';
				}
				sprintf(name, "%s", ctx->sprites.getName(i));
				ds::vec4 r = ctx->sprites.getRect(i);
				int rx = r.x;
				int ry = r.y;
				int w = r.z;
				int h = r.w;
				fprintf(fp, "%s %d %d %d %d\n", name, rx, ry, w, h);
			}
			fclose(fp);
		}
		fp = fopen("sprites_definitions.h", "w");
		if (fp) {
			fprintf(fp, "#pragma once\n#include <diesel.h>\n\nenum SpriteIndex {\n");
			for (int i = 0; i < ctx->sprites.size(); ++i) {
				// FIXME: convert chars to uppercase
				fprintf(fp, "\t%s,\n", ctx->sprites.getName(i));
			}
			fprintf(fp, "\n};\n\nconst ds::vec4 SPRITES[] = {\n");
			for (int i = 0; i < ctx->sprites.size(); ++i) {
				ds::vec4 r = ctx->sprites.getRect(i);
				int rx = r.x;
				int ry = r.y;
				int w = r.z;
				int h = r.w;
				fprintf(fp, "\tds::vec4(%d,%d,%d,%d),\n", rx, ry, w, h);
			}
			fprintf(fp, "\n};\n");
			fclose(fp);
		}
	}

	// ---------------------------------------------------------------------------
	// load binary file
	// ---------------------------------------------------------------------------
	static void load(SpriteEditorContext* ctx) {
		FILE* fp = fopen("sprites.bin", "r");
		ctx->sprites.clear();
		char buffer[256];
		char name[20];
		if (fp) {
			while (fgets(buffer, 255, (FILE*)fp)) {
				printf("%s\n", buffer);
				int rx = 0;
				int ry = 0;
				int w = 0;
				int h = 0;
				sscanf(buffer, "%s %d %d %d %d", name, &rx, &ry, &w, &h);
				ctx->sprites.add(name, ds::vec4(rx, ry, w, h));
			}
			fclose(fp);
		}
		
	}

	// ---------------------------------------------------------------------------
	// render GUI
	// ---------------------------------------------------------------------------
	void renderGUI(SpriteEditorContext* ctx) {
		gui::Value("FPS", ds::getFramesPerSecond());
		if (gui::Input("Center", &ctx->center)) {
			sprites::set_screen_center(ctx->center);
		}
		gui::Value("Dragging", ctx->dragging);
		gui::Value("Dragging Pos", ctx->draggingPos);
		int rx = ctx->rect.x;
		gui::Slider("RX", &rx, 0, 1024, true);
		int ry = ctx->rect.y;
		gui::Slider("RY", &ry, 0, 1024);
		int w = ctx->rect.z;
		gui::Slider("W", &w, 0, 1024);
		int h = ctx->rect.w;
		gui::Slider("H", &h, 0, 1024);
		ctx->rect.x = rx;
		ctx->rect.y = ry;
		ctx->rect.z = w;
		ctx->rect.w = h;
		gui::Slider("Scale", &ctx->scale,1.0f,4.0f,1);
		gui::Input("Name", ctx->name, 20);
		if (gui::Button("Add")) {
			ctx->sprites.add(ctx->name, ctx->rect);
		}
		if (gui::ListBox("Sprites", ctx->sprites, 5)) {
			int s = ctx->sprites.getSelection();
			ctx->selection = s;
			ctx->rect = ctx->sprites.getRect(s);
			sprintf(ctx->name, "%s", ctx->sprites.getName(s));
		}
		if (gui::Button("Load")) {
			load(ctx);
		}
		if (gui::Button("Save")) {
			save(ctx);
		}
	}
}