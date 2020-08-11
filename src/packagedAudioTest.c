#include <SDL2/SDL.h>
#include "audio.h"

/*
The addresses of the packaged files on the memory.
One can check these values with the "objdump -x <file>.o" command.
*/

extern char _binary_music_highlands_wav_start[];
extern char _binary_music_highlands_wav_end[];

extern char _binary_sounds_door1_wav_start[];
extern char _binary_sounds_door1_wav_end[];

extern char _binary_sounds_door2_wav_start[];
extern char _binary_sounds_door2_wav_end[];

int main(void)
{
    /* Initialize only SDL Audio on default device */
    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        return 1; 
    }

    /* Init Simple-SDL2-Audio */
    initAudio();

    /* Creates the audio buffers from the packaged audio files */
    Audio * door1 = createAudioFromMemory(_binary_sounds_door1_wav_start,_binary_sounds_door1_wav_end, 0, SDL_MIX_MAXVOLUME / 2);
    Audio * door2 = createAudioFromMemory(_binary_sounds_door2_wav_start,_binary_sounds_door2_wav_end, 0, SDL_MIX_MAXVOLUME / 2);
    Audio * music = createAudioFromMemory(_binary_music_highlands_wav_start,_binary_music_highlands_wav_end, 1, SDL_MIX_MAXVOLUME);

    /* Tests them by themselves, one by one with music and combined+music */
    playSoundFromMemory(door1, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);
    playSoundFromMemory(door2, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);
    playMusicFromMemory(music, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);
    playSoundFromMemory(door1, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);
    playSoundFromMemory(door2, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);
    playSoundFromMemory(door1, SDL_MIX_MAXVOLUME);
    playSoundFromMemory(door2, SDL_MIX_MAXVOLUME);
    SDL_Delay(2000);

    stopMusic();
    playSoundFromMemory(door2, SDL_MIX_MAXVOLUME);

    SDL_Delay(2000);
    playSoundFromMemory(door2, SDL_MIX_MAXVOLUME);
    SDL_Delay(8000);

    /* End Simple-SDL2-Audio */
    endAudio();

    /* Important to free audio after ending Simple-SDL2-Audio because they might be referenced still */
    freeAudio(door1);
    freeAudio(door2);
    freeAudio(music);

    SDL_Quit();

    return 0;
}