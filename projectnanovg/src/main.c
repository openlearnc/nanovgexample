//#include <stdio.h>
//#include <assert.h>

//#include <SDL.h>

//#include "gl.h"
//#include "nanovg.h"
#include "stb_sprintf.h"

#include <stdio.h>
#include <stdlib.h>
//#include <android/log.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
//#define GLFW_INCLUDE_ES2
#define NANOVG_GLES2_IMPLEMENTATION
#include "nanovg.h"
#include "nanovg_gl.h"
#include "nanovg_gl_utils.h"
SDL_Window* window;

NVGcontext* nanovg_create_context()
{
	const int flags = NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG;
	
		return nvgCreateGLES2(flags);
	
}
static void window_size(int* width, int* height, float* pixel_ratio)
{
	int prev_width = *width;
	int prev_height = *height;
	SDL_GL_GetDrawableSize(window, width, height);
	if ((*width != prev_width || *height != prev_height)) {
		printf("%d×%d -> %d×%d\n", prev_width, prev_height, *width, *height);
	}

	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	*pixel_ratio = *width / w;
}

static void star(NVGcontext* vg, int n_teeth, float r1, float r2)
{
	const int n_steps = n_teeth*2;
	nvgBeginPath(vg);
	for (int i = 0; i < n_steps; i++) {
		const float a = ((float)i / (float)n_steps) * NVG_PI * 2.0f;
		const float dx = cosf(a);
		const float dy = sinf(a);
		const float x1 = dx * r1;
		const float y1 = dy * r1;
		const float x2 = dx * r2;
		const float y2 = dy * r2;

		if (i&1) {
			nvgLineTo(vg, x1, y1);
			nvgLineTo(vg, x2, y2);
		} else if (i == 0) {
			nvgMoveTo(vg, x2, y2);
			nvgLineTo(vg, x1, y1);
		} else {
			nvgLineTo(vg, x2, y2);
			nvgLineTo(vg, x1, y1);
		}
	}
	nvgClosePath(vg);
}


int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	atexit(SDL_Quit);

	SDL_GLContext glctx;
	{
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
		

		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);

		window = SDL_CreateWindow(
				"SDL2/NanoVG/GLES2",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				1920, 1080,
				SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
		if (window == NULL) {
			fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
			abort();
		}
		glctx = SDL_GL_CreateContext(window);
		if (!glctx) {
			fprintf(stderr, "SDL_GL_CreateContextfailed: %s\n", SDL_GetError());
			abort();
		}
	}

	NVGcontext* vg = nanovg_create_context();
	

	int font = nvgCreateFont(vg, "sans", "fonts/DroidSans.ttf");
	

	NVGpaint rpaint = nvgRadialGradient(vg, 0, 0, 0, 100, nvgRGBA(255,255,255,200), nvgRGBA(255,100,0,50));

	int screen_width = 0;
	int screen_height = 0;
	float pixel_ratio = 0.0f;
	window_size(&screen_width, &screen_height, &pixel_ratio);

	int swap_interval = 1;
	SDL_GL_SetSwapInterval(1);

	float phi = 0.0f;
	int exiting = 0;
	float fps = 0.0f;
	int fps_counter = 0;
	int fullscreen = 0;
	Uint32 last_ticks = 0;
	while (!exiting) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				exiting = 1;
			} else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					exiting = 1;
				} else if (e.key.keysym.sym == SDLK_SPACE) {
					swap_interval ^= 1;
					SDL_GL_SetSwapInterval(swap_interval);
				} else if (e.key.keysym.sym == SDLK_f) {
					fullscreen = !fullscreen;
					//SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
					SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
				}
			} else if (e.type == SDL_WINDOWEVENT) {
				if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
					window_size(&screen_width, &screen_height, &pixel_ratio);
				}
			}
		}

		glViewport(0, 0, screen_width, screen_height);
		glClearColor(0, 0.1, 0.4, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		nvgBeginFrame(vg, screen_width / pixel_ratio, screen_height / pixel_ratio, pixel_ratio);

		phi += 0.1f;

		{
			nvgSave(vg);

			nvgRotate(vg, sinf(phi) * 0.02f);

			nvgFontSize(vg, 100.0f);
			nvgTextAlign(vg, NVG_ALIGN_LEFT|NVG_ALIGN_MIDDLE);
			nvgFillColor(vg, nvgRGBA(0,128,255,255));
			nvgText(vg, 300, 300, "hello world", NULL);

			nvgLineCap(vg, NVG_ROUND);
			nvgLineJoin(vg, NVG_ROUND);
			nvgStrokeWidth(vg, 5.0f);
			nvgStrokeColor(vg, nvgRGBA(255,255,255,100));
			nvgBeginPath(vg);
			nvgMoveTo(vg, 30, 30);
			nvgLineTo(vg, 1000, 500);
			nvgLineTo(vg, 700, 1000);
			nvgClosePath(vg);

			nvgFillColor(vg, nvgRGBA(10,20,200,150));
			nvgFill(vg);
			nvgStroke(vg);

			nvgRestore(vg);
		}

		#if 0
		{
			nvgSave(vg);
			nvgTranslate(vg, 600, 600);
			nvgRotate(vg, phi*0.05f);
			star(vg, 200, 200, 600);
			nvgFillColor(vg, nvgRGBA(255,50,50,50));
			nvgStrokeWidth(vg, 2.0f);
			nvgStrokeColor(vg, nvgRGBA(255,255,0,100));
			nvgFill(vg);
			nvgStroke(vg);
			nvgRestore(vg);
		}
		#endif

		{
			nvgSave(vg);
			nvgTranslate(vg, 500, 400);
			nvgRotate(vg, phi*0.01f);
			star(vg, 50, 100, 200);
			nvgFillColor(vg, nvgRGBA(0,50,255,100));
			nvgFill(vg);
			nvgStrokeWidth(vg, 1.5f);
			nvgStrokeColor(vg, nvgRGBA(0,0,0,100));
			nvgStroke(vg);
			nvgRestore(vg);
		}

		{
			nvgSave(vg);
			nvgTranslate(vg, 800, 700);
			nvgRotate(vg, phi*-0.03f);
			star(vg, 30, 46, 50);
			nvgFillColor(vg, nvgRGBA(0,255,0,100));
			nvgFill(vg);
			nvgStrokeWidth(vg, 1.5f);
			nvgStrokeColor(vg, nvgRGBA(255,255,255,100));
			nvgStroke(vg);
			nvgRestore(vg);
		}

		for (int i = 0; i < 15; i++) {
			nvgSave(vg);
			nvgTranslate(vg, 100 + i*100, 100 + i*50);
			nvgRotate(vg, phi*0.05f*(i&1 ? 1.0f : -1.0f));
			const float r1 = 100 - i*5.0f;
			const float r2 = r1 + 10.0f;
			const int n_teeth = 40 - i;
			star(vg, n_teeth, r1, r2);
			//nvgFillColor(vg, nvgRGBA(0,255,0,100));
			nvgFillPaint(vg, rpaint);
			nvgFill(vg);
			nvgStrokeWidth(vg, 1.5f);
			nvgStrokeColor(vg, nvgRGBA(0,0,0,100));
			nvgStroke(vg);
			nvgRestore(vg);
		}

		nvgSave(vg);
		nvgBeginPath(vg);
		const float spacing = 10.0f;
		for (float y = -50; y < screen_height; y+=spacing) {
			float dy = fmodf(phi*8.0f, spacing);
			nvgMoveTo(vg, 1000, y + dy);
			nvgLineTo(vg, 1200, y+20 + dy);
			nvgLineTo(vg, 1400, y + dy);
		}
		nvgStrokeWidth(vg, 2.5f);
		nvgStrokeColor(vg, nvgRGBA(255,255,0,255));
		nvgStroke(vg);
		nvgRestore(vg);

		{
			char buf[100];
			stbsp_snprintf(buf, sizeof buf, "%.1f fps", fps);

			nvgSave(vg);
			nvgTranslate(vg, 10, 30);
			nvgFontSize(vg, 30.0f);
			nvgTextAlign(vg, NVG_ALIGN_LEFT);
			nvgFillColor(vg, nvgRGBA(0,0,0,255));
			nvgText(vg, 2, 2, buf, NULL);
			nvgFillColor(vg, nvgRGBA(255,255,255,255));
			nvgText(vg, 0, 0, buf, NULL);
			nvgRestore(vg);
		}

		{
			nvgSave(vg);
			nvgTranslate(vg, 500, 500);
			nvgScale(vg, 10, 10);
			nvgRotate(vg, phi*0.1f);
			nvgTranslate(vg, -15, -30);
			
			nvgFillColor(vg, nvgRGBA(0,0,0,100));
			nvgFill(vg);
			nvgStrokeWidth(vg, .25f);
			nvgStrokeColor(vg, nvgRGBA(255,255,255,255));
			nvgStroke(vg);
			nvgRestore(vg);
		}

		nvgEndFrame(vg);

		SDL_GL_SwapWindow(window);

		fps_counter++;
		Uint32 ticks = SDL_GetTicks();
		if (ticks > last_ticks + 1000) {
			fps = ((float)ticks - (float)last_ticks)*0.001f * (float)fps_counter;
			last_ticks = ticks;
			fps_counter = 0;
		}
	}

	SDL_GL_DeleteContext(glctx);
	SDL_DestroyWindow(window);

	return EXIT_SUCCESS;
}
