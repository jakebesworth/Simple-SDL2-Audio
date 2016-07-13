# Simple SDL2 Audio

## About

* A simple native SDL2 Audio library that has 2 files, and an easy to use interface.
* This library works without SDL2 Mixer, and plays a single music file at a time, and unlimited sounds (Mixes audio natively without Mixer)

## How to use

* Include audio.c and audio.h into your project
* test.c shows how to simply load wave files as music or sound

## Music vs Sound

* Only one music can play at a time, and it loops (to close music you can just run endAudio() lazily, or write your own pause function which is not difficult).
   * If you add another music when one is playing, the first one fades out before ending, and then playing the second.
   * If you play more than 2 music at once, the first fades as expected, only the last music queued before the first fade out is used

* Any amount of sounds can be played at once, but obviously the more, can become distorted.

## Limitations

* This implementation uses SDL_MixAudioFormat for mixing for simplicity. It's noted "Do not use this function for mixing together more than two streams of sample data". While only playing 1 music removes a lot of these issues, if you need something more powerful you should write your own mixing function.
* Multiple Music cannot be mixed together, only music and sound
* This implementation ONLY plays WAV files, and they must all be of the same format, see the top of audio.c to set the format, stereo vs mono etc... No conversion

## Resources

* I made this project as a more modern version of https://gist.github.com/armornick/3447121
* https://davidgow.net/handmadepenguin/ch7.html
* http://rerwarwar.weebly.com/sdl2-audio.html

## Contributors

* [Jake Besworth](https://github.com/jakebesworth)
* [Lorenzo Mancini](https://github.com/lmancini)
