#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "Headers/InputManager.h"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_CreateWindowAndRenderer("Game of Life", 800, 600, 0, &window, &renderer) == false) {
        fprintf(stderr, "Window/Renderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    InputManager inputManager = {0};

    bool quit = false;

    while (!quit) {
        input_manager_begin_frame(&inputManager);

        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_ESCAPE)) quit = true;

        SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
        SDL_RenderClear(renderer);

        SDL_FRect rect = { 350.0f, 250.0f, 100.0f, 100.0f };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}