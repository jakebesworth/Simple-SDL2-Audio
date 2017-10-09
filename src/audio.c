/*
 * Simple-SDL2-Audio
 *
 * Copyright 2016 Jake Besworth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "audio.h"

/*
 * Native WAVE format
 *
 * On some GNU/Linux you can identify a files properties using:
 *      mplayer -identify music.wav
 *
 * On some GNU/Linux to convert any music to this or another specified format use:
 *      ffmpeg -i in.mp3 -acodec pcm_s16le -ac 2 -ar 48000 out.wav
 */
/* SDL_AudioFormat of files, such as s16 little endian */
#define AUDIO_FORMAT AUDIO_S16LSB

/* Frequency of the file */
#define AUDIO_FREQUENCY 48000

/* 1 mono, 2 stereo, 4 quad, 6 (5.1) */
#define AUDIO_CHANNELS 2

/* Specifies a unit of audio data to be used at a time. Must be a power of 2 */
#define AUDIO_SAMPLES 4096

/* Max number of sounds that can be in the audio queue at anytime, stops too much mixing */
#define AUDIO_MAX_SOUNDS 25

/* Flags OR'd together, which specify how SDL should behave when a device cannot offer a specific feature
 * If flag is set, SDL will change the format in the actual audio file structure (as opposed to gDevice->want)
 *
 * Note: If you're having issues with Emscripten / EMCC play around with these flags
 *
 * 0                                    Allow no changes
 * SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
 * SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
 * SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
 * SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
 */
#define SDL_AUDIO_ALLOW_CHANGES SDL_AUDIO_ALLOW_ANY_CHANGE

/*
 * Definition for the game global sound device
 *
 */
typedef struct privateAudioDevice
{
    SDL_AudioDeviceID device;
    SDL_AudioSpec want;
    uint8_t audioEnabled;
} PrivateAudioDevice;

/* File scope variables to persist data */
static PrivateAudioDevice * gDevice;
static uint32_t gSoundCount;

/*
 * Add a music to the queue, addAudio wrapper for music due to fade
 *
 * @param new       New Audio to add
 *
 */
static void addMusic(Audio * root, Audio * new);

/*
 * Wrapper function for playMusic, playSound, playMusicFromMemory, playSoundFromMemory
 *
 * @param filename      Provide a filename to load WAV from, or NULL if using FromMemory
 * @param audio         Provide an Audio object if copying from memory, or NULL if using a filename
 * @param sound         1 if looping (music), 0 otherwise (sound)
 * @param volume        See playSound for explanation
 *
 */
static inline void playAudio(const char * filename, Audio * audio, uint8_t loop, int volume);

/*
 * Add a sound to the end of the queue
 *
 * @param root      Root of queue
 * @param new       New Audio to add
 *
 */
static void addAudio(Audio * root, Audio * new);

/*
 * Audio callback function for OpenAudioDevice
 *
 * @param userdata      Points to linked list of sounds to play, first being a placeholder
 * @param stream        Stream to mix sound into
 * @param len           Length of sound to play
 *
 */
static inline void audioCallback(void * userdata, uint8_t * stream, int len);

void playSound(const char * filename, int volume)
{
    playAudio(filename, NULL, 0, volume);
}

void playMusic(const char * filename, int volume)
{
    playAudio(filename, NULL, 1, volume);
}

void playSoundFromMemory(Audio * audio, int volume)
{
    playAudio(NULL, audio, 0, volume);
}

void playMusicFromMemory(Audio * audio, int volume)
{
    playAudio(NULL, audio, 1, volume);
}

void initAudio(void)
{
    Audio * global;
    gDevice = calloc(1, sizeof(PrivateAudioDevice));
    gSoundCount = 0;

    if(gDevice == NULL)
    {
        fprintf(stderr, "[%s: %d]Fatal Error: Memory c-allocation error\n", __FILE__, __LINE__);
        return;
    }

    gDevice->audioEnabled = 0;

    if(!(SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
    {
        fprintf(stderr, "[%s: %d]Error: SDL_INIT_AUDIO not initialized\n", __FILE__, __LINE__);
        return;
    }

    SDL_memset(&(gDevice->want), 0, sizeof(gDevice->want));

    (gDevice->want).freq = AUDIO_FREQUENCY;
    (gDevice->want).format = AUDIO_FORMAT;
    (gDevice->want).channels = AUDIO_CHANNELS;
    (gDevice->want).samples = AUDIO_SAMPLES;
    (gDevice->want).callback = audioCallback;
    (gDevice->want).userdata = calloc(1, sizeof(Audio));

    global = (gDevice->want).userdata;

    if(global == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return;
    }

    global->buffer = NULL;
    global->next = NULL;

    /* want.userdata = new; */
    if((gDevice->device = SDL_OpenAudioDevice(NULL, 0, &(gDevice->want), NULL, SDL_AUDIO_ALLOW_CHANGES)) == 0)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open audio device: %s\n", __FILE__, __LINE__, SDL_GetError());
    }
    else
    {
        /* Set audio device enabled global flag */
        gDevice->audioEnabled = 1;

        /* Unpause active audio stream */
        unpauseAudio();
    }
}

void endAudio(void)
{
    if(gDevice->audioEnabled)
    {
        pauseAudio();

        freeAudio((Audio *) (gDevice->want).userdata);

        /* Close down audio */
        SDL_CloseAudioDevice(gDevice->device);
    }

    free(gDevice);
}

void pauseAudio(void)
{
    if(gDevice->audioEnabled)
    {
        SDL_PauseAudioDevice(gDevice->device, 1);
    }
}

void unpauseAudio(void)
{
    if(gDevice->audioEnabled)
    {
        SDL_PauseAudioDevice(gDevice->device, 0);
    }
}

void freeAudio(Audio * audio)
{
    Audio * temp;

    while(audio != NULL)
    {
        if(audio->free == 1)
        {
            SDL_FreeWAV(audio->bufferTrue);
        }

        temp = audio;
        audio = audio->next;

        free(temp);
    }
}

Audio * createAudio(const char * filename, uint8_t loop, int volume)
{
    Audio * new = calloc(1, sizeof(Audio));

    if(new == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return NULL;
    }

    if(filename == NULL)
    {
        fprintf(stderr, "[%s: %d]Warning: filename NULL: %s\n", __FILE__, __LINE__, filename);
        return NULL;
    }

    new->next = NULL;
    new->loop = loop;
    new->fade = 0;
    new->free = 1;
    new->volume = volume;

    if(SDL_LoadWAV(filename, &(new->audio), &(new->bufferTrue), &(new->lengthTrue)) == NULL)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open wave file: %s error: %s\n", __FILE__, __LINE__, filename, SDL_GetError());
        free(new);
        return NULL;
    }

    new->buffer = new->bufferTrue;
    new->length = new->lengthTrue;
    (new->audio).callback = NULL;
    (new->audio).userdata = NULL;

    return new;
}

static inline void playAudio(const char * filename, Audio * audio, uint8_t loop, int volume)
{
    Audio * new;

    /* Check if audio is enabled */
    if(!gDevice->audioEnabled)
    {
        return;
    }

    /* If sound, check if under max number of sounds allowed, else don't play */
    if(loop == 0)
    {
        if(gSoundCount >= AUDIO_MAX_SOUNDS)
        {
            return;
        }
        else
        {
            gSoundCount++;
        }
    }

    /* Load from filename or from Memory */
    if(filename != NULL)
    {
        /* Create new music sound with loop */
        new = createAudio(filename, loop, volume);
    }
    else if(audio != NULL)
    {
        new = malloc(sizeof(Audio));

        if(new == NULL)
        {
            fprintf(stderr, "[%s: %d]Fatal Error: Memory allocation error\n", __FILE__, __LINE__);
            return;
        }

        memcpy(new, audio, sizeof(Audio));

        new->volume = volume;
        new->loop = loop;
        new->free = 0;
    }
    else
    {
        fprintf(stderr, "[%s: %d]Warning: filename and Audio parameters NULL\n", __FILE__, __LINE__);
        return;
    }

    /* Lock callback function */
    SDL_LockAudioDevice(gDevice->device);

    if(loop == 1)
    {
        addMusic((Audio *) (gDevice->want).userdata, new);
    }
    else
    {
        addAudio((Audio *) (gDevice->want).userdata, new);
    }

    SDL_UnlockAudioDevice(gDevice->device);

}

static void addMusic(Audio * root, Audio * new)
{
    uint8_t musicFound = 0;
    Audio * rootNext = root->next;

    /* Find any existing musics, 0, 1 or 2 and fade them out */
    while(rootNext != NULL)
    {
        /* Phase out any current music */
        if(rootNext->loop == 1 && rootNext->fade == 0)
        {
            if(musicFound)
            {
                rootNext->length = 0;
                rootNext->volume = 0;
            }

            rootNext->fade = 1;
        }
        /* Set flag to remove any queued up music in favour of new music */
        else if(rootNext->loop == 1 && rootNext->fade == 1)
        {
            musicFound = 1;
        }

        rootNext = rootNext->next;
    }

    addAudio(root, new);
}

static inline void audioCallback(void * userdata, uint8_t * stream, int len)
{
    Audio * audio = (Audio *) userdata;
    Audio * previous = audio;
    int tempLength;
    uint8_t music = 0;

    /* Silence the main buffer */
    SDL_memset(stream, 0, len);

    /* First one is place holder */
    audio = audio->next;

    while(audio != NULL)
    {
        if(audio->length > 0)
        {
            if(audio->fade == 1 && audio->loop == 1)
            {
                music = 1;

                if(audio->volume > 0)
                {
                    audio->volume--;
                }
                else
                {
                    audio->length = 0;
                }
            }

            if(music && audio->loop == 1 && audio->fade == 0)
            {
                tempLength = 0;
            }
            else
            {
                tempLength = ((uint32_t) len > audio->length) ? audio->length : (uint32_t) len;
            }

            SDL_MixAudioFormat(stream, audio->buffer, AUDIO_FORMAT, tempLength, audio->volume);

            audio->buffer += tempLength;
            audio->length -= tempLength;

            previous = audio;
            audio = audio->next;
        }
        else if(audio->loop == 1 && audio->fade == 0)
        {
            audio->buffer = audio->bufferTrue;
            audio->length = audio->lengthTrue;
        }
        else
        {
            previous->next = audio->next;

            if(audio->loop == 0)
            {
                gSoundCount--;
            }

            audio->next = NULL;
            freeAudio(audio);

            audio = previous->next;
        }
    }
}

static void addAudio(Audio * root, Audio * new)
{
    if(root == NULL)
    {
        return;
    }

    while(root->next != NULL)
    {
        root = root->next;
    }

    root->next = new;
}
