# Simple SDL2 Audio

## About

* A simple native SDL2 Audio library that has 2 files, and an easy to use interface.
* This library works without SDL2 Mixer, and plays a single music file at a time, and unlimited sounds (Mixes audio natively without Mixer)

## How to use

* Include audio.c and audio.h into your project
* test.c shows how to simply load wave files as music or sound, and a simple pause and unpause function

## API Functions:

```c
void initAudio(void);
void endAudio(void);
void playSound(const char * filename, int volume);
void playMusic(const char * filename, int volume);
void pauseAudio(void);
void unpauseAudio(void);
```

## Music vs Sound

* Only one music can play at a time, and it loops (to close music you can just run `endAudio()`, or use `pauseAudio()` and `unpauseAudio()`).
   * If you add another music when one is playing, the first one fades out before ending, and then playing the second.
   * If you play more than 2 music at once, the first fades as expected, only the last music queued before the first fade out is used

* Any amount of sounds can be played at once, but obviously the more, can become distorted.

## Limitations

* This implementation uses SDL_MixAudioFormat for mixing for simplicity. It's noted "Do not use this function for mixing together more than two streams of sample data". While only playing 1 music removes a lot of these issues, if you need something more powerful you should write your own mixing function.
* Multiple Music cannot be mixed together, only music and sound
* This implementation ONLY plays WAV files, and they should all be the same format, but can have differing formats if you play around with `SDL_AUDIO_ALLOW_CHANGES` in `audio.c`, see the top of audio.c to set the format, stereo vs mono etc... No conversion

## Features to add

- WAV data caching: Currently a read occurs everytime you play a sound and music. An addition would be storing the data as RWops in memory and just playing from that. Limitations include how much memory you want to use, garbage collection rotation, some sort of storage, probably hash map (filepath => data). May want to only cache frequently used sounds and not music.
- Pause / unpause only music, only sound or ~~both~~
- Current implementation uses callback method, however in SDL 2.0.4 there exists `SDL_QueueAudio()` (no callback)

## Emscripten Compatibility

* [Github Issue - Solution](https://github.com/jakebesworth/Simple-SDL2-Audio/issues/2)

## Resources

* I made this project as a more modern version of https://gist.github.com/armornick/3447121
* https://davidgow.net/handmadepenguin/ch7.html
* http://rerwarwar.weebly.com/sdl2-audio.html

## Contributors

* [Jake Besworth](https://github.com/jakebesworth)
* [Lorenzo Mancini](https://github.com/lmancini)
* [Ted](https://github.com/claimred)
