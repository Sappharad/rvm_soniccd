//
//  MusicTrackInfo.h
//  rvm
//

#ifndef MusicTrackInfo_h
#define MusicTrackInfo_h

#include "SDL_mixer.h"

struct MusicTrackInfo {
    char trackName[64];
    bool loop;
    unsigned int loopPoint;
    unsigned char* musicData;
    Mix_Music* mixerAudio;
};

#endif /* MusicTrackInfo_h */
