#include <SDL2/SDL.h>
#include <cstdio>

int main(int argc, char* argv[])
{

	SDL_Window* screen = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		screen = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, SDL_WINDOW_SHOWN);

		if (!screen)
		{
			printf("SDL_Window error: %s\n", SDL_GetError());
		}
		else
		{
			SDL_Surface* screenSurface = SDL_GetWindowSurface(screen);

			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			SDL_UpdateWindowSurface(screen);

			SDL_Event e; bool quit = false; while (quit == false) { while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) quit = true; } }
		}


	}
	SDL_DestroyWindow(screen);

	SDL_Quit();

	return 0;
}


