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

/*  
 * audio.h
 *
 * All audio related functions go here
 *
 */
#ifndef SOUND_
#define SOUND_

#ifdef __cplusplus
extern "C"
{
#endif

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
#define AUDIO_FREQUENCY 48000;

/* 1 mono, 2 stereo, 4 quad, 6 (5.1) */
#define AUDIO_CHANNELS 2;

/* Specifies a unit of audio data to be used at a time. Must be a power of 2 */
#define AUDIO_SAMPLES 4096;

/*  
 * Queue structure for all loaded sounds
 *
 */
typedef struct sound
{
    uint32_t length;
    uint32_t lengthTrue;
    uint8_t * bufferTrue;
    uint8_t * buffer;
    uint8_t loop;
    uint8_t fade;
    uint8_t volume;

    SDL_AudioSpec audio;

    struct sound * next;
} Sound;

/*
 * Definition for the game global sound device
 *
 */
typedef struct globalAudioDevice
{
    SDL_AudioDeviceID device;
    SDL_AudioSpec want;
} GlobalAudioDevice;

/*  
 * Add a sound to the end of the queue
 *
 * @param root      Root of queue
 * @param new       New Sound to add
 *
 */
void addSound(Sound * root, Sound * new);

/*  
 * Plays a new music, only 1 at a time plays
 *
 * @param filename      Filename of the WAVE file to load
 * @param volum         Volume read playSound for moree
 *
 */
void playMusic(char * filename, int volume);

/*  
 * Frees as many chained Sounds as given
 *
 * @param sound     Chain of sounds to free
 *
 */
void freeSound(Sound * sound);

/*  
 * Create a Sound object
 *
 * @param filename      Filename for the WAVE file to load
 * @param loop          Loop 0, ends after playing, 1 refreshes
 * @param volume        Volume, read playSound()
 *
 * @return returns a new Sound or NULL on failure
 *
 */
Sound * createSound(char * filename, uint8_t loop, int volume);

/*  
 * Free all audio related variables
 *
 */
void endAudio();

/*  
 * Initialize Audio Variable
 *
 */
void initAudio();

/*  
 * Play a wave file currently must be S16LE format 2 channel stereo
 *
 * @param filename      Filename to open, use getAbsolutePath
 * @param volume        Volume 0 - 128. SDL_MIX_MAXVOLUME constant for max volume
 *
 */
void playSound(char * filename, int volume);

/*  
 * Audio callback function for OpenAudioDevice
 *
 * @param userdata      Points to linked list of sounds to play, first being a placeholder
 * @param stream        Stream to mix sound into
 * @param len           Length of sound to play
 *
 */
void audioCallback(void * userdata, uint8_t * stream, int len);

#ifdef __cplusplus
}
#endif

#endif
