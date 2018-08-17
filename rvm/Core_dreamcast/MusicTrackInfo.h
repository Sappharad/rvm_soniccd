//
//  MusicTrackInfo.h
//  rvm
//

#ifndef MusicTrackInfo_h
#define MusicTrackInfo_h

#ifdef SDL
#include "SDL_mixer.h"
#endif

struct MusicTrackInfo {
    char trackName[64];
    bool loop;
    unsigned int loopPoint;
    unsigned char* musicData;
#ifdef SDL
    Mix_Music* mixerAudio;
#endif
};

#endif /* MusicTrackInfo_h */
