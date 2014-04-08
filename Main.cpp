// Always necessary.
#include "SDL.h"
// Gives us image loading support
#include "SDL_image.h"
// Gives us audio playback support. We're only using .WAV files, but we could play
// Ogg, MP3, FLAC, MOD, or even MIDI files.
#include "SDL_mixer.h"
// Adds keyboard event processing.
#include "SDL_keyboard.h"
// Adds mouse event handling.
#include "SDL_mouse.h"
// Adds true-type font rendering.
#include "SDL_ttf.h"
// So we can use cout for debugging.
#include <iostream>

using namespace std;

bool InitializeSDL()
{
    // Initialize SDL
    int initResult = SDL_Init(SDL_INIT_EVERYTHING);
    if( initResult )
    {
        cout << "Failed to initialize SDL: " << SDL_GetError() << endl;
        return false;
    }
    // Initialize image support.
    initResult = IMG_Init(IMG_INIT_PNG);
    if( !initResult )
    {
        cout << "Failed to initialize PNG support: " << IMG_GetError() << endl;
        return false;
    }
    // Initialize TTF font support.
    initResult = TTF_Init();
    if( initResult )
    {
        cout << "Failed to initialize TTF support: " << TTF_GetError() << endl;
        return false;
    }
    // Initialize mixer support.
    initResult = Mix_Init(0);
    if( initResult )
    {
        cout << "Failed to initialize audio mixer: " << Mix_GetError() << endl;
        return false;
    }
    // Set up the audio stream as 44.1 KHz, 2-channel, 1024-byte buffer.
    int result = Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024);
    if( result < 0 )
    {
        cout << "Unable to open audio: " << Mix_GetError() << endl;
        return false;
    }
    // Create 4 audio channels so we can have up to 4 sounds playing at once.
    result = Mix_AllocateChannels(4);
    if( result < 0 )
    {
        cout << "Unable to allocate mixing channels: " << Mix_GetError() << endl;
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    // Variables to keep track of our magic and life points.
    int magics = 200;
    int lifes = 200;
    // Declare a TON of variables for our textures and surfaces.
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
    // Drawing location for life meter, magic meter, and monster image.
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
    // Font variables.
    TTF_Font* font;
    SDL_Color color = { 255, 192, 128 };

    // Activate our libraries.
    if( !InitializeSDL() )
    {
        cout << "I give up." << endl;
        exit(-1);
    }

    // Create the main window. A sane developer would check the return value.
    mainWindow = SDL_CreateWindow("SDL Sample", 10, 10, 1280, 800, SDL_WINDOW_SHOWN);
    if( mainWindow == NULL )
    {
        cout << "Failed to create main window." << endl;
        exit(-1);
    }
    // Create the renderer as a hardware-accelerated flicker-free renderer.
    renderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if( renderer == NULL )
    {
        cout << "failed to create the renderer" << endl;
        exit(-1);
    }

    // Load image resources.
    bk1 = IMG_Load("Background1.png");
    if( !bk1 )
    {
        cout << "Background1.png - IMG_Load: " << IMG_GetError() << endl;
    }
    background1 = SDL_CreateTextureFromSurface(renderer, bk1);
    SDL_FreeSurface(bk1);
    bk2 = IMG_Load("Background2.png");
    background2 = SDL_CreateTextureFromSurface(renderer, bk2);
    bk3 = IMG_Load("Background3.png");
    background3 = SDL_CreateTextureFromSurface(renderer, bk3);
    activeBackground = background1;
    mon1 = IMG_Load("Monster.png");
    if( !mon1 )
    {
        cout << "Monster.png - IMG_Load: " << IMG_GetError() << endl;
    }
    monster = SDL_CreateTextureFromSurface(renderer, mon1);
    SDL_FreeSurface(mon1);
    life1 = IMG_Load("Life.png");
    lifeMeter = SDL_CreateTextureFromSurface(renderer, life1);
    SDL_FreeSurface(life1);
    magic1 = IMG_Load("Magic.png");
    magicMeter = SDL_CreateTextureFromSurface(renderer, magic1);
    SDL_FreeSurface(magic1);
    // Load font and create text resources.
    font = TTF_OpenFont("FreeSans.ttf", 28);
    if( !font )
    {
        cout << "Failed to load font: " << TTF_GetError() << endl;
    }
    SDL_Surface* healthText = TTF_RenderText_Blended(font, "Healths", color);
    SDL_Surface* magicText = TTF_RenderText_Blended(font, "Magicks", color);
    SDL_Surface* attackText = TTF_RenderText_Blended(font, "Whap the monster", color);
    SDL_Surface* makeMagicText = TTF_RenderText_Blended(font, "Make magicks", color);
    SDL_Surface* runAwayText = TTF_RenderText_Blended(font, "Run aways!", color);
    SDL_Surface* attackResultText = TTF_RenderText_Blended(font, "You try to whap the monster but it bited you. You are ouched for 10 healths.", color);
    SDL_Surface* magicResultText = TTF_RenderText_Blended(font, "You use some magicks and make your healths feel a little better.", color);
    SDL_Surface* nomagicResultText = TTF_RenderText_Blended(font, "You is out of magicks and cannot make your healths feel better.", color);
    SDL_Surface* fleeResultText = TTF_RenderText_Blended(font, "You try to run away, but the monster followed you to the new place.", color);
    SDL_Texture* healthTxt = SDL_CreateTextureFromSurface(renderer, healthText);
    SDL_Texture* magicTxt = SDL_CreateTextureFromSurface(renderer, magicText);
    SDL_Texture* attackTxt = SDL_CreateTextureFromSurface(renderer, attackText);
    SDL_Texture* makeMagicTxt = SDL_CreateTextureFromSurface(renderer, makeMagicText);
    SDL_Texture* runAwayTxt = SDL_CreateTextureFromSurface(renderer, runAwayText);
    SDL_Texture* attackResultTxt = SDL_CreateTextureFromSurface(renderer, attackResultText);
    SDL_Texture* magicResultTxt = SDL_CreateTextureFromSurface(renderer, magicResultText);
    SDL_Texture* nomagicResultTxt = SDL_CreateTextureFromSurface(renderer, magicResultText);
    SDL_Texture* fleeResultTxt = SDL_CreateTextureFromSurface(renderer, fleeResultText);
    SDL_Texture* currentNotificationText = NULL;
    // Create locations for text drawing.
    SDL_Rect healthRect;
    healthRect.x = 20;
    healthRect.y = 10;
    healthRect.w = healthText->w;
    healthRect.h = healthText->h;
    SDL_Rect magicTRect;
    magicTRect.x = 20;
    magicTRect.y = 90;
    magicTRect.w = magicText->w;
    magicTRect.h = magicText->h;
    SDL_Rect attackTRect;
    attackTRect.x = 300;
    attackTRect.y = 700;
    attackTRect.w = attackText->w;
    attackTRect.h = attackText->h;
    SDL_Rect makeMagicTRect;
    makeMagicTRect.x = 600;
    makeMagicTRect.y = 700;
    makeMagicTRect.w = makeMagicText->w;
    makeMagicTRect.h = makeMagicText->h;
    SDL_Rect runAwayTRect;
    runAwayTRect.x = 860;
    runAwayTRect.y = 700;
    runAwayTRect.w = runAwayText->w;
    runAwayTRect.h = runAwayText->h;
    SDL_Rect notificationRect;
    notificationRect.x = 200;
    notificationRect.y = 744;
    notificationRect.w = 900;
    notificationRect.h = 44;
    SDL_FreeSurface(healthText);
    SDL_FreeSurface(magicText);
    SDL_FreeSurface(attackText);
    SDL_FreeSurface(makeMagicText);
    SDL_FreeSurface(runAwayText);
    SDL_FreeSurface(attackResultText);
    SDL_FreeSurface(magicResultText);
    SDL_FreeSurface(nomagicResultText);
    SDL_FreeSurface(fleeResultText);
    // Load our sound effects.
    Mix_Chunk* attackSound = Mix_LoadWAV("Attack.wav");
    Mix_Chunk* magicSound = Mix_LoadWAV("Magic.wav");

    // Main run loop. Handles keyboard, mouse, and quit events.
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
	        else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                int x = ((SDL_MouseButtonEvent*)&e)->x;
                int y = ((SDL_MouseButtonEvent*)&e)->y;
                cout << "Mouse: X is " << x << ", Y is " << y << endl;
                if( x > attackTRect.x && x < (attackTRect.x + attackTRect.w) &&
                    y > attackTRect.y && y < (attackTRect.y + attackTRect.h))
                {
                    // Attacking just means the monster bites you and you lose health.
                    cout << "The attack button was clicked" << endl;
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
                    currentNotificationText = attackResultTxt;
                }
                if( x > makeMagicTRect.x && x < (makeMagicTRect.x + makeMagicTRect.w) &&
                    y > makeMagicTRect.y && y < (makeMagicTRect.y + makeMagicTRect.h))
                {
                    // Using magic heals you, increasing your life meter.
                    cout << "The make magic button was clicked" << endl;
                    Mix_PlayChannel(-1, magicSound, 0);
                    if( magics > 0 )
                    {
                        magics -= 10;
                        if( lifes < 200 )
                        {
                            lifes += 10;
                        }
                        if( lifes > 200 )
                        {
                            lifes = 200;
                        }
                        currentNotificationText = magicResultTxt;
                    }
                    else
                    {
                        currentNotificationText = nomagicResultTxt;
                        if( magics < 0 )
                        {
                            magics = 0;
                        }
                    }
                    magicRect.w = magics;
                    lifeRect.w = lifes;
                }
                if( x > runAwayTRect.x && x < (runAwayTRect.x + runAwayTRect.w) &&
                    y > runAwayTRect.y && y < (runAwayTRect.y + runAwayTRect.h))
                {
                    // The monster is faster than you, so running away just changes the scenery.
                    cout << "The run away button was clicked" << endl;
                    if( activeBackground == background3 ) activeBackground = background1;
                    else if( activeBackground == background2 ) activeBackground = background3;
                    else if( activeBackground == background1 ) activeBackground = background2;
                    currentNotificationText = fleeResultTxt;
                }
            }
        }
        // Clear the scene.
        SDL_RenderClear(renderer);
        // Draw the background.
        SDL_RenderCopy(renderer, activeBackground, NULL, NULL);
        // Draw our sprites and text over the top of the background.
        SDL_RenderCopy(renderer, lifeMeter, NULL, &lifeRect);
        SDL_RenderCopy(renderer, magicMeter, NULL, &magicRect);
        SDL_RenderCopy(renderer, monster, NULL, &monsterRect);
        SDL_RenderCopy(renderer, healthTxt, NULL, &healthRect);
        SDL_RenderCopy(renderer, magicTxt, NULL, &magicTRect);
        SDL_RenderCopy(renderer, attackTxt, NULL, &attackTRect);
        SDL_RenderCopy(renderer, makeMagicTxt, NULL, &makeMagicTRect);
        SDL_RenderCopy(renderer, runAwayTxt, NULL, &runAwayTRect);
        // If there are any notifications, draw them too.
        if( currentNotificationText != NULL )
        {
            SDL_RenderCopy(renderer, currentNotificationText, NULL, &notificationRect);
        }
        // Show the finished scene.
        SDL_RenderPresent(renderer);
    }
    // Texture cleanup.
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
    SDL_DestroyTexture(attackResultTxt);
    SDL_DestroyTexture(magicResultTxt);
    SDL_DestroyTexture(nomagicResultTxt);
    SDL_DestroyTexture(fleeResultTxt);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(mainWindow);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
