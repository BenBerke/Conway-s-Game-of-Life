#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "config.h"
#include "Headers/InputManager.h"
#include "Headers/Vector2.h"

typedef struct History {
    bool (*items)[ROW][COL];
    int count;
    int size;
    int chunkSize;
} History;

void create_history(History* list, int chunkSize) {
    list->count = 0;
    list->size = chunkSize;
    list->chunkSize = chunkSize;
    list->items = malloc((size_t)list->size * sizeof(*list->items));
}

void push_history(History* list, bool grid[ROW][COL]) {
    if (list->count >= list->size) {
        list->size += list->chunkSize;
        list->items = realloc(list->items, (size_t)list->size * sizeof(*list->items));
    }

    memcpy(list->items[list->count], grid, sizeof(bool) * ROW * COL);
    list->count++;
}

bool pop_history(History* list, bool outGrid[ROW][COL]) {
    if (list->count <= 0) {
        return false;
    }

    list->count--;
    memcpy(outGrid, list->items[list->count], sizeof(bool) * ROW * COL);

    if (list->size - list->count >= list->chunkSize && list->size > list->chunkSize) {
        list->size -= list->chunkSize;
        list->items = realloc(list->items, (size_t)list->size * sizeof(*list->items));
    }

    return true;
}

void free_history(History* list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->size = 0;
    list->chunkSize = 0;
}

void copy_grid(bool dst[ROW][COL], bool src[ROW][COL]) {
    for (int y = 0; y < ROW; y++) {
        for (int x = 0; x < COL; x++) {
            dst[y][x] = src[y][x];
        }
    }
}

void load_history_state(bool grid[ROW][COL], const History* history, int index) {
    if (index < 0 || index >= history->count) return;
    copy_grid(grid, history->items[index]);
}

void discard_future_history(History* history, int currentView) {
    history->count = currentView + 1;
}

void initialize_grid(bool grid[ROW][COL]) {
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            grid[i][j] = 0;
        }
    }
}

void update_grid(bool grid[ROW][COL], SDL_Renderer* renderer) {
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
}

void tick(bool grid[ROW][COL]) {
    bool snapshot[ROW][COL];

    copy_grid(snapshot, grid);

    for (int y = 0; y < ROW; y++) {
        for (int x = 0; x < COL; x++) {
            int liveNeighbours = 0;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;

                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx < 0 || nx >= COL || ny < 0 || ny >= ROW) continue;
                    if (snapshot[ny][nx]) liveNeighbours++;
                }
            }

            if (snapshot[y][x]) {
                grid[y][x] = !(liveNeighbours < 2 || liveNeighbours > 3);
            } else {
                grid[y][x] = (liveNeighbours == 3);
            }
        }
    }
}

void set_grid(bool grid[ROW][COL], bool (*snapshot)[ROW][COL]) {
    for (int i = 0; i < COL; i++) {
        for (int j = 0; j < ROW; j++) {
            grid[j][i] = snapshot[j][i];
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
    TTF_Font* font = TTF_OpenFont("../assets/ariel.ttf", FONT_SIZE);
    if (!font) {
        fprintf(stderr, "TTF_OpenFont failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    InputManager inputManager = {0};
    History history;
    create_history(&history, 32);

    TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
    TTF_Text* controls = TTF_CreateText(textEngine, font, "Left / Right arrow to step backwards / forwards", 0);

    bool grid[ROW][COL];
    initialize_grid(grid);

    push_history(&history, grid);
    int currentView = 0;

    bool running = true;
    while (running) {
        input_manager_begin_frame(&inputManager);
        Vector2 mousePosition = input_manager_get_mouse_position(&inputManager);

        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_ESCAPE)) running = false;

        SDL_SetRenderDrawColor(renderer,69, 61, 69, 255);
        SDL_RenderClear(renderer);

        if (input_manager_get_mouse_down(&inputManager, SDL_BUTTON_LEFT)) {
            int x = (int)mousePosition.x / CELL_SIZE;
            int y = (int)mousePosition.y / CELL_SIZE;

            if (x >= 0 && x < COL && y >= 0 && y < ROW) {
                grid[x][y] = !grid[x][y];

                discard_future_history(&history, currentView);
                push_history(&history, grid);
                currentView = history.count - 1;
            }
        }
        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_LEFT)) {
            if (currentView > 0) {
                currentView--;
                load_history_state(grid, &history, currentView);
            }
        }

        if (input_manager_get_key_down(&inputManager, SDL_SCANCODE_RIGHT)) {
            if (currentView < history.count - 1) {
                currentView++;
                load_history_state(grid, &history, currentView);
            } else {
                tick(grid);
                push_history(&history, grid);
                currentView = history.count - 1;
            }
        }

        SDL_SetRenderDrawColor(renderer, 71, 115, 145, 255);
        SDL_FRect rect = { 0,  SCREEN_HEIGHT, SCREEN_WIDTH, CELL_SIZE * UI_HEIGHT};
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        TTF_DrawRendererText(controls, 15.0f, TEXT_Y);

        update_grid(grid, renderer);
        SDL_RenderPresent(renderer);
    }

    free_history(&history);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}