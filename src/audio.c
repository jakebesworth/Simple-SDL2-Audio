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
#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "audio.h"

static GlobalSound * gSound;

void playSound(char * filename, int volume)
{
    Sound * new;

    new = createSound(filename, 0, volume);

    SDL_LockAudioDevice(gSound->device);
    addSound((Sound *) (gSound->want).userdata, new);

    SDL_UnlockAudioDevice(gSound->device);
}

void playMusic(char * filename, int volume)
{
    Sound * global;
    Sound * new;
    uint8_t music = 0;

    /* Create new music sound with loop */
    new = createSound(filename, 1, volume);

    /* Lock callback function */
    SDL_LockAudioDevice(gSound->device);
    global = ((Sound *) (gSound->want).userdata)->next;

    /* Find any existing musics, 0, 1 or 2 */
    while(global != NULL)
    {
        /* Phase out any current music */
        if(global->loop == 1 && global->fade == 0)
        {
            if(music)
            {
                global->length = 0;
                global->volume = 0;
            }

            global->fade = 1;
        }
        /* Set flag to remove any queued up music in favour of new music */
        else if(global->loop == 1 && global->fade == 1)
        {
            music = 1;
        }

        global = global->next;
    }

    addSound((Sound *) (gSound->want).userdata, new);

    SDL_UnlockAudioDevice(gSound->device);
}

void initAudio()
{
    Sound * global;
    gSound = calloc(1, sizeof(GlobalSound));

    if(gSound == NULL)
    {
        fprintf(stderr, "[%s: %d]Fatal Error: Memory c-allocation error\n", __FILE__, __LINE__);
        return;
    }

    SDL_memset(&(gSound->want), 0, sizeof(gSound->want));

    (gSound->want).freq = AUDIO_FREQUENCY;
    (gSound->want).format = AUDIO_FORMAT;
    (gSound->want).channels = AUDIO_CHANNELS;
    (gSound->want).samples = AUDIO_SAMPLES;
    (gSound->want).callback = audioCallback;
    (gSound->want).userdata = calloc(1, sizeof(Sound));

    global = (gSound->want).userdata;
    global->buffer = NULL;

    if(global == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return;
    }

    global->next = NULL;

    /* want.userdata = new; */
    if((gSound->device = SDL_OpenAudioDevice(NULL, 0, &(gSound->want), NULL, SDL_AUDIO_ALLOW_ANY_CHANGE)) == 0)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open audio device: %s\n", __FILE__, __LINE__, SDL_GetError());
    }
    else
    {
        /* Unpause active audio stream */
        SDL_PauseAudioDevice(gSound->device, 0);
    }
}

void endAudio()
{
    SDL_PauseAudioDevice(gSound->device, 1);

    freeSound((Sound *) (gSound->want).userdata);

    /* Close down audio */
    SDL_CloseAudioDevice(gSound->device);

    free(gSound);
}

Sound * createSound(char * filename, uint8_t loop, int volume)
{
    Sound * new = calloc(1, sizeof(Sound));

    if(new == NULL)
    {
        fprintf(stderr, "[%s: %d]Error: Memory allocation error\n", __FILE__, __LINE__);
        return NULL;
    }

    new->next = NULL;
    new->loop = loop;
    new->fade = 0;
    new->volume = volume;

    if(SDL_LoadWAV(filename, &(new->audio), &(new->bufferTrue), &(new->lengthTrue)) == NULL)
    {
        fprintf(stderr, "[%s: %d]Warning: failed to open wave file: %s err: %s\n", __FILE__, __LINE__, filename, SDL_GetError());
        free(new);
        return NULL;
    }

    new->buffer = new->bufferTrue;
    new->length = new->lengthTrue;
    (new->audio).callback = NULL;
    (new->audio).userdata = NULL;

    return new;
}

void audioCallback(void * userdata, uint8_t * stream, int len)
{
    Sound * sound = (Sound *) userdata;
    Sound * previous = sound;
    int tempLength;
    uint8_t music = 0;

    /* Silence the main buffer */
    SDL_memset(stream, 0, len);

    /* First one is place holder */
    sound = sound->next;

    while(sound != NULL)
    {
        if(sound->length > 0)
        {
            if(sound->fade == 1 && sound->loop == 1)
            {
                music = 1;
                sound->volume--;

                if(sound->volume == 0)
                {
                    sound->length = 0;
                }
            }

            if(music && sound->loop == 1 && sound->fade == 0)
            {
                tempLength = 0;
            }
            else
            {
                tempLength = (len > sound->length) ? sound->length : len;
            }

            SDL_MixAudioFormat(stream, sound->buffer, AUDIO_FORMAT, tempLength, sound->volume);

            sound->buffer += tempLength;
            sound->length -= tempLength;

            previous = sound;
            sound = sound->next;
        }
        else if(sound->loop == 1 && sound->fade == 0)
        {
            sound->buffer = sound->bufferTrue;
            sound->length = sound->lengthTrue;
        }
        else
        {
            previous->next = sound->next;
            SDL_FreeWAV(sound->bufferTrue);
            free(sound);

            sound = previous->next;
        }
    }
}

void addSound(Sound * root, Sound * new)
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

void freeSound(Sound * sound)
{
    Sound * temp;

    while(sound != NULL)
    {
        SDL_FreeWAV(sound->bufferTrue);

        temp = sound;
        sound = sound->next;

        free(temp);
    }
}
