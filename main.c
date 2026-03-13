#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define MAP_SIZE 20
#define TILE_SIZE 64
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 960

int map[MAP_SIZE][MAP_SIZE] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,0,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1},
    {1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,0,1,1,0,1},
    {1,0,0,0,0,1,0,1,0,1,1,1,1,0,1,0,0,0,0,1},
    {1,1,1,0,1,1,0,1,0,1,0,0,1,0,1,1,1,0,1,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,0,1,0,0,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,1,1,0,1,1,0,0,1,1,0,1,1,0,1,0,1},
    {1,0,0,0,1,1,0,1,1,0,0,1,1,0,1,1,0,0,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,1},
    {1,0,0,0,0,1,0,1,0,0,0,0,1,0,1,0,0,0,0,1},
    {1,0,1,1,0,0,0,1,0,1,1,0,1,0,0,0,1,1,0,1},
    {1,0,1,1,0,1,1,1,0,1,1,0,1,1,1,0,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

typedef struct {
    float x, y, dirX, dirY, planeX, planeY, w, h, speed, rotSpeed;
} Player;

Player player = {96.0f, 96.0f, 1.0f, 0.0f, 0.0f, 0.66f, 8.0f, 8.0f, 300.0f, 3.0f};

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Raycast Engine DDA", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    bool quit = false;
    SDL_Event e;
    Uint64 lastTime = SDL_GetTicks();

    while (!quit) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&e)) if (e.type == SDL_EVENT_QUIT) quit = true;

        const bool* keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_D]) {
            float rotDir = keys[SDL_SCANCODE_A] ? -1.0f : 1.0f;
            float rot = rotDir * player.rotSpeed * deltaTime;
            float oldDirX = player.dirX;
            player.dirX = player.dirX * cosf(rot) - player.dirY * sinf(rot);
            player.dirY = oldDirX * sinf(rot) + player.dirY * cosf(rot);
            float oldPlaneX = player.planeX;
            player.planeX = player.planeX * cosf(rot) - player.planeY * sinf(rot);
            player.planeY = oldPlaneX * sinf(rot) + player.planeY * cosf(rot);
        }

        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_S]) {
            float moveDir = keys[SDL_SCANCODE_W] ? 1.0f : -1.0f;
            float stepX = player.dirX * player.speed * moveDir * deltaTime;
            float stepY = player.dirY * player.speed * moveDir * deltaTime;
            if (map[(int)((player.y+4)/TILE_SIZE)][(int)((player.x+4+stepX)/TILE_SIZE)] == 0) player.x += stepX;
            if (map[(int)((player.y+4+stepY)/TILE_SIZE)][(int)((player.x+4)/TILE_SIZE)] == 0) player.y += stepY;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_FRect ceiling = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT/2};
        SDL_RenderFillRect(renderer, &ceiling);

        float pCX = player.x + 4;
        float pCY = player.y + 4;

        
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            float cameraX = 2.0f * i / (float)SCREEN_WIDTH - 1.0f;
            float rayDX = player.dirX + player.planeX * cameraX;
            float rayDY = player.dirY + player.planeY * cameraX;

            int mX = (int)(pCX / TILE_SIZE), mY = (int)(pCY / TILE_SIZE);
            float dDX = (rayDX == 0) ? 1e30 : fabsf(1 / rayDX);
            float dDY = (rayDY == 0) ? 1e30 : fabsf(1 / rayDY);

            float sDX, sDY;
            int stepX, stepY;

            if (rayDX < 0) { stepX = -1; sDX = (pCX / TILE_SIZE - mX) * dDX; }
            else { stepX = 1; sDX = (mX + 1.0f - pCX / TILE_SIZE) * dDX; }
            if (rayDY < 0) { stepY = -1; sDY = (pCY / TILE_SIZE - mY) * dDY; }
            else { stepY = 1; sDY = (mY + 1.0f - pCY / TILE_SIZE) * dDY; }

            int hit = 0, side;
            while (hit == 0) {
                if (sDX < sDY) { sDX += dDX; mX += stepX; side = 0; }
                else { sDY += dDY; mY += stepY; side = 1; }
                if (mX < 0 || mX >= MAP_SIZE || mY < 0 || mY >= MAP_SIZE) break;
                if (map[mY][mX] > 0) hit = 1;
            }

            float pWD = (side == 0) ? (sDX - dDX) : (sDY - dDY);
            if (pWD < 0.1f) pWD = 0.1f;
            float h = SCREEN_HEIGHT / pWD;

            float intensity = 1.0f / (1.0f + pWD * pWD * 0.03f);
            Uint8 r = (Uint8)(255 * intensity);
            if (side == 1) r *= 0.7; // Side shading

            SDL_SetRenderDrawColor(renderer, r, 0, 0, 255);
            SDL_FRect slice = {(float)i, (SCREEN_HEIGHT - h) / 2, 1, h};
            SDL_RenderFillRect(renderer, &slice);
        }

        float ms = 0.12f, mt = TILE_SIZE * ms, mar = 20.0f;
        for (int y = 0; y < MAP_SIZE; y++) {
            for (int x = 0; x < MAP_SIZE; x++) {
                if (map[y][x] > 0) {
                    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
                    SDL_FRect w = {mar + x * mt, mar + y * mt, mt, mt};
                    SDL_RenderFillRect(renderer, &w);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_FRect mp = {mar + player.x * ms - 4, mar + player.y * ms - 4, 7, 7};
        SDL_RenderFillRect(renderer, &mp);

        SDL_RenderPresent(renderer);
    }
    SDL_Quit();
    return 0;
}
