#include <SDL2/SDL.h>
#include "audio.h"

int main(int argc, char * argv[])
{
    /* Initialize only SDL Audio on default device */
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1; 
    }

    /* Init Simple-SDL2-Audio */
    initAudio();

    /* Play music and a sound */
    playMusic("music/highlands.wav", SDL_MIX_MAXVOLUME);
    playSound("sounds/door1.wav", SDL_MIX_MAXVOLUME / 2);

    /* While using delay for showcase, don't actually do this in your project */
    SDL_Delay(1000);

    /* Override music, play another sound */
    playMusic("music/road.wav", SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);

    playSound("sounds/door2.wav", SDL_MIX_MAXVOLUME / 2);
    SDL_Delay(7000);

    /* End Simple-SDL2-Audio */
    endAudio();

    SDL_Quit();

    return 0;
}
