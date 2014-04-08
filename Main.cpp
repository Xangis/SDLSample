#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_keyboard.h"
#include "SDL_mouse.h"
#include "SDL_ttf.h"
#include <iostream>

int main(int argc, char* argv[])
{
    // A surface represents the drawing area of a window, or a surface containing
    // loaded image data.
    SDL_Window* mainWindow;
    SDL_Surface* mon1;
    SDL_Surface* bk1;
    SDL_Surface* bk2;
    SDL_Surface* bk3;
    SDL_Surface* life1;
    SDL_Surface* magic1;
    SDL_Texture* monster;
    SDL_Texture* background1;
    SDL_Texture* background2;
    SDL_Texture* background3;
    SDL_Texture* activeBackground;
    SDL_Texture* lifeMeter;
    SDL_Texture* magicMeter;
    int magics = 200;
    int lifes = 200;
    SDL_Rect lifeRect;
    lifeRect.x = 20;
    lifeRect.y = 50;
    lifeRect.w = lifes;
    lifeRect.h = 40;
    SDL_Rect magicRect;
    magicRect.x = 20;
    magicRect.y = 140;
    magicRect.w = magics;
    magicRect.h = 40;
    SDL_Rect monsterRect;
    monsterRect.x = 340;
    monsterRect.y = 120;
    monsterRect.w = 600;
    monsterRect.h = 456;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color color = { 224, 224, 192 };
    // Initialize everything. A sane developer would check the return value.
    SDL_Init(SDL_INIT_EVERYTHING);
    int initResult = IMG_Init(IMG_INIT_PNG);
    if( !initResult )
    {
        std::cout << "Failed to init PNG: " << IMG_GetError() << std::endl;
    }
    initResult = TTF_Init();
    if( initResult )
    {
        std::cout << "Failed to initialize TTF support: " << TTF_GetError() << std::endl;
    }
    initResult = Mix_Init(0);
    if( initResult )
    {
        std::cout << "Failed to initialize WAV file support: " << Mix_GetError() << std::endl;
    }
    // Set up the audio stream
    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to open audio: %s\n", SDL_GetError());
        exit(-1);
    }
    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        fprintf(stderr, "Unable to allocate mixing channels: %s\n", SDL_GetError());
        exit(-1);
    }

    // Create the main window. A sane developer would check the return value.
    mainWindow = SDL_CreateWindow("SDL Sample", 10, 10, 1280, 800, SDL_WINDOW_SHOWN);
    // Create the renderer as a hardware-accelerated flicker-free renderer.
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    std::cout << renderer << std::endl;
    // Load image resources.
    bk1 = IMG_Load("Background1.png");
    if( !bk1 )
    {
        std::cout << "Background1.png - IMG_Load: " << IMG_GetError() << std::endl;
    }
    background1 = SDL_CreateTextureFromSurface(renderer, bk1);
    SDL_FreeSurface(bk1);
    background2 = SDL_CreateTextureFromSurface(renderer, IMG_Load("Background2.png"));
    background3 = SDL_CreateTextureFromSurface(renderer, IMG_Load("Background3.png"));
    activeBackground = background1;
    mon1 = IMG_Load("Monster.png");
    if( !mon1 )
    {
        std::cout << "Monster.png - IMG_Load: " << IMG_GetError() << std::endl;
    }
    monster = SDL_CreateTextureFromSurface(renderer, mon1);
    std::cout << monster << std::endl;
    SDL_FreeSurface(mon1);
    life1 = IMG_Load("Life.png");
    lifeMeter = SDL_CreateTextureFromSurface(renderer, life1);
    SDL_FreeSurface(life1);
    magic1 = IMG_Load("Magic.png");
    magicMeter = SDL_CreateTextureFromSurface(renderer, magic1);
    SDL_FreeSurface(magic1);
    font = TTF_OpenFont("FreeSans.ttf", 28);
    if( !font )
    {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    SDL_Surface* healthText = TTF_RenderText_Blended(font, "Healths", color);
    SDL_Surface* magicText = TTF_RenderText_Blended(font, "Magicks", color);
    SDL_Surface* attackText = TTF_RenderText_Blended(font, "Whap the monster", color);
    SDL_Surface* makeMagicText = TTF_RenderText_Blended(font, "Make magicks", color);
    SDL_Surface* runAwayText = TTF_RenderText_Blended(font, "Run aways!", color);
    SDL_Texture* healthTxt = SDL_CreateTextureFromSurface(renderer, healthText);
    SDL_Texture* magicTxt = SDL_CreateTextureFromSurface(renderer, magicText);
    SDL_Texture* attackTxt = SDL_CreateTextureFromSurface(renderer, attackText);
    SDL_Texture* makeMagicTxt = SDL_CreateTextureFromSurface(renderer, makeMagicText);
    SDL_Texture* runAwayTxt = SDL_CreateTextureFromSurface(renderer, runAwayText);
    SDL_FreeSurface(healthText);
    SDL_FreeSurface(magicText);
    SDL_FreeSurface(attackText);
    SDL_FreeSurface(makeMagicText);
    SDL_FreeSurface(runAwayText);
    SDL_Rect healthRect;
    healthRect.x = 40;
    healthRect.y = 10;
    healthRect.w = 160;
    healthRect.h = 40;
    SDL_Rect magicTRect;
    magicTRect.x = 40;
    magicTRect.y = 90;
    magicTRect.w = 160;
    magicTRect.h = 50;
    SDL_Rect attackTRect;
    attackTRect.x = 300;
    attackTRect.y = 700;
    attackTRect.w = 180;
    attackTRect.h = 50;
    SDL_Rect makeMagicTRect;
    makeMagicTRect.x = 540;
    makeMagicTRect.y = 700;
    makeMagicTRect.w = 180;
    makeMagicTRect.h = 50;
    SDL_Rect runAwayTRect;
    runAwayTRect.x = 780;
    runAwayTRect.y = 700;
    runAwayTRect.w = 180;
    runAwayTRect.h = 50;
    Mix_Chunk* attackSound = Mix_LoadWAV("Attack.wav");
    Mix_Chunk* magicSound = Mix_LoadWAV("Magic.wav");

    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        while (SDL_PollEvent(&e))
        {
	        // Quit if user closes the window
	        if  (e.type == SDL_QUIT)
		        quit = true;
	        // Quit if user presses any key
	        else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym) 
                {
                    // Q quits the application.
                    case 'q':
                        quit = true;
                }
            }
            // Quit if user clicks the mouse
	        else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = ((SDL_MouseButtonEvent*)&e)->x;
                int y = ((SDL_MouseButtonEvent*)&e)->y;
                std::cout << "Mouse: X is " << x << ", Y is " << y << std::endl;
                if( x > attackTRect.x && x < (attackTRect.x + attackTRect.w) &&
                    y > attackTRect.y && y < (attackTRect.y + attackTRect.h))
                {
                    std::cout << "The attack button was clicked" << std::endl;
                    Mix_PlayChannel(-1, attackSound, 0);
                    if( lifes > 0 )
                    {
                        lifes -= 10;
                    }
                    if( lifes < 0 )
                    {
                        lifes = 0;
                    }
                    lifeRect.w = lifes;
                }
                if( x > makeMagicTRect.x && x < (makeMagicTRect.x + makeMagicTRect.w) &&
                    y > makeMagicTRect.y && y < (makeMagicTRect.y + makeMagicTRect.h))
                {
                    std::cout << "The make magic button was clicked" << std::endl;
                    Mix_PlayChannel(-1, magicSound, 0);
                    if( magics > 0 )
                    {
                        magics -= 10;
                    }
                    if( magics < 0 )
                    {
                        magics = 0;
                    }
                    if( lifes < 200 )
                    {
                        lifes += 10;
                    }
                    if( lifes > 200 )
                    {
                        lifes = 200;
                    }
                    magicRect.w = magics;
                    lifeRect.w = lifes;
                }
                if( x > runAwayTRect.x && x < (runAwayTRect.x + runAwayTRect.w) &&
                    y > runAwayTRect.y && y < (runAwayTRect.y + runAwayTRect.h))
                {
                    std::cout << "The run away button was clicked" << std::endl;
                    if( activeBackground == background3 ) activeBackground = background1;
                    else if( activeBackground == background2 ) activeBackground = background3;
                    else if( activeBackground == background1 ) activeBackground = background2;
                }
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, activeBackground, NULL, NULL);
        SDL_RenderCopy(renderer, lifeMeter, NULL, &lifeRect);
        SDL_RenderCopy(renderer, magicMeter, NULL, &magicRect);
        SDL_RenderCopy(renderer, monster, NULL, &monsterRect);
        SDL_RenderCopy(renderer, healthTxt, NULL, &healthRect);
        SDL_RenderCopy(renderer, magicTxt, NULL, &magicTRect);
        SDL_RenderCopy(renderer, attackTxt, NULL, &attackTRect);
        SDL_RenderCopy(renderer, makeMagicTxt, NULL, &makeMagicTRect);
        SDL_RenderCopy(renderer, runAwayTxt, NULL, &runAwayTRect);
        SDL_RenderPresent(renderer);
    }

    //SDL_BlitSurface(background, NULL, mainWindow, NULL);
    SDL_DestroyTexture(monster);
    SDL_DestroyTexture(background1);
    SDL_DestroyTexture(background2);
    SDL_DestroyTexture(background3);
    SDL_DestroyTexture(lifeMeter);
    SDL_DestroyTexture(magicMeter);
    SDL_DestroyTexture(healthTxt);
    SDL_DestroyTexture(magicTxt);
    SDL_DestroyTexture(attackTxt);
    SDL_DestroyTexture(makeMagicTxt);
    SDL_DestroyTexture(healthTxt);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
