#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "config.h"
#include "Headers/InputManager.h"
#include "Headers/Vector2.h"

void initialize_grid(bool grid[ROW][COL]) {
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            grid[i][j] = 0;
        }
    }
}

void update_grid(bool grid[ROW][COL], SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 53, 59, 61, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            SDL_RenderLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
            SDL_RenderLine(renderer, 0, j * CELL_SIZE, SCREEN_WIDTH, j * CELL_SIZE);

            if (grid[i][j] == false) continue;
            SDL_FRect rect = { (float)i * CELL_SIZE, (float)j * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_SetRenderDrawColor(renderer, 122, 72, 122, 255);
    SDL_FRect uiBg = { 0, TOTAL_HEIGHT - UI_HEIGHT * CELL_SIZE, SCREEN_WIDTH, UI_HEIGHT * CELL_SIZE };
    SDL_RenderFillRect(renderer, &uiBg);
    SDL_RenderPresent(renderer);
}

void tick(bool grid[ROW][COL]) {
    bool snapshot[ROW][COL];
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            snapshot[j][i] = grid[j][i];
        }
    }

    for (int y = 0; y < COL; y++) {
        for (int x = 0; x < ROW; x++) {
            int liveNeighbours = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0 || nx >= COL || ny < 0 || ny >= ROW) continue;
                    if (snapshot[nx][ny]) liveNeighbours++;
                }
            }

            if (snapshot[y][x]) grid[y][x] = !(liveNeighbours < 2 || liveNeighbours > 3);
            else grid[y][x] = liveNeighbours == 3;
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (SDL_CreateWindowAndRenderer("Game of Life", SCREEN_WIDTH, TOTAL_HEIGHT, 0, &window, &renderer) == false) {
        fprintf(stderr, "Window/Renderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    if (!TTF_Init()) {
        fprintf(stderr, "TTF_INIT failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    TTF_Font* font = TTF_OpenFont("../assets/ariel.ttf", 24);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    InputManager inputManager = {0};

    TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
    TTF_Text* hello = TTF_CreateText(textEngine, font, "Hello world", 0);

    bool grid[ROW][COL];
    initialize_grid(grid);
    update_grid(grid, renderer);

    bool running = true;
    bool editMode = true;
    while (running) {
        input_manager_begin_frame(&inputManager);
        Vector2 mousePosition = input_manager_get_mouse_position(&inputManager);

        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_ESCAPE)) running = false;
        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_E)) editMode = !editMode;
        if (editMode) {
            if (input_manager_get_mouse_down(&inputManager, SDL_BUTTON_LEFT)) {
                int x = (int)mousePosition.x / CELL_SIZE;
                int y = (int)mousePosition.y / CELL_SIZE;

                if (x >= COL | y >= ROW){

                    continue;
                }

                grid[x][y] = !grid[x][y];
                update_grid(grid, renderer);
            }
            continue;
        }
        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_X)) {
            tick(grid);
            update_grid(grid, renderer);
        }

    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}