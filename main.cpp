#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_starter.h"
#include "sdl_assets_loader.h"

const int SPEED = 300*scale;

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController* controller = nullptr;

bool isGamePaused;

Sprite alienSprite;

Mix_Chunk *sound = nullptr;
Mix_Music *music = nullptr;

SDL_Texture *pauseTexture = nullptr;
SDL_Rect pauseBounds;

TTF_Font *fontSquare = nullptr;

void quitGame()
{
    Mix_FreeChunk(sound);
    SDL_DestroyTexture(alienSprite.texture);
    SDL_DestroyTexture(pauseTexture);
    SDL_GameControllerClose(controller);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        if (event.type == SDL_QUIT) 
        {
            quitGame();
            exit(0);
        }

//when I need a more precise input i should use this method of input reading
        if (event.type == SDL_CONTROLLERBUTTONDOWN && event.cbutton.button == SDL_CONTROLLER_BUTTON_START)
        {
            isGamePaused = !isGamePaused;
            Mix_PlayChannel(-1, sound, 0);
        }
    }
}

void update(float deltaTime)
{
    if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_UP) && alienSprite.textureBounds.y > 0) 
    {
        alienSprite.textureBounds.y -= SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) && alienSprite.textureBounds.y < SCREEN_HEIGHT - alienSprite.textureBounds.h) 
    {
        alienSprite.textureBounds.y += SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) && alienSprite.textureBounds.x > 0) 
    {
        alienSprite.textureBounds.x -= SPEED * deltaTime;
    }

    else if (SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && alienSprite.textureBounds.x < SCREEN_WIDTH - alienSprite.textureBounds.w) 
    {
        alienSprite.textureBounds.x += SPEED * deltaTime;
    }
}

void renderSprite(Sprite sprite)
{
    SDL_RenderCopy(renderer, sprite.texture, NULL, &sprite.textureBounds);
}

void render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if(isGamePaused) 
    {
        SDL_RenderCopy(renderer, pauseTexture, NULL, &pauseBounds);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    renderSprite(alienSprite);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *args[])
{
    window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
   
   // Using SDL_RENDERER_PRESENTVSYNC effectively synchronizes your rendering with the display's refresh rate, which should cap the framerate to the refresh rate
    //  of the display (commonly 60 Hz, resulting in 60 FPS). This means that, in most cases, you don't need to manually cap the framerate since VSync already
    //  limits the maximum framerate to match the display's refresh rate.
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
 
    if(startSDL(window, renderer) > 0) 
    {
        return 1;
    }

    if (SDL_NumJoysticks() < 1) 
    {
        printf("No game controllers connected!\n");
        return -1;
    } 
    else 
    {
        controller = SDL_GameControllerOpen(0);
        if (controller == NULL) 
        {
            printf("Unable to open game controller! SDL Error: %s\n", SDL_GetError());
            return -1;
        }
    }

    fontSquare = TTF_OpenFont("square_sans_serif_7.ttf", 32*scale);

    // load title
    updateTextureText(pauseTexture, "Game Paused", fontSquare, renderer);

    SDL_QueryTexture(pauseTexture, NULL, NULL, &pauseBounds.w, &pauseBounds.h);
    pauseBounds.x = SCREEN_WIDTH / 2 - pauseBounds.w / 2;
    pauseBounds.y = 100;
    // After I use the &titleRect.w, &titleRect.h in the SDL_QueryTexture.
    //  I get the width and height of the actual texture
    
    //The path of the file references the build folder
    alienSprite = loadSprite(renderer, "alien_1.png", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    sound = loadSound("laser.wav");

    // method to reduce the volume of the sound in half.
    Mix_VolumeChunk(sound, MIX_MAX_VOLUME / 2);

    // Load music file (only one data piece, intended for streaming)
    music = loadMusic("music.wav");

    // reduce music volume in half
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);

    // Start playing streamed music, put -1 to loop indifinitely
    Mix_PlayMusic(music, -1);

    Uint32 previousFrameTime = SDL_GetTicks();
    Uint32 currentFrameTime = previousFrameTime;
    float deltaTime = 0.0f;

    while (true)
    {
        currentFrameTime = SDL_GetTicks();
        deltaTime = (currentFrameTime - previousFrameTime) / 1000.0f;
        previousFrameTime = currentFrameTime;

        SDL_GameControllerUpdate();

        handleEvents();

        if(!isGamePaused)
        {
            update(deltaTime);
        }

        render();
    }

    quitGame();
}