//
//  AudioPlayback.h
//  rvm
//

#ifndef AudioPlayback_h
#define AudioPlayback_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "MusicTrackInfo.h"
#include "FileData.h"
#include "FileIO.h"
//#include "SDL_mixer.h"

//extern Game gameRef;
extern int numGlobalSFX;
extern int numStageSFX;
//extern SoundEffect sfxSamples[256];
extern bool sfxLoaded[256];
//extern SoundEffectInstance channelInstance[8];
extern int channelSfxNum[8];
extern int nextChannelPos;
extern bool musicEnabled;
extern int musicVolume;
extern float musicVolumeSetting;
extern float sfxVolumeSetting;
extern int musicStatus;
extern int currentMusicTrack;
extern struct MusicTrackInfo musicTracks[16];

void InitAudioPlayback();
void AudioPlayback_ReleaseAudioPlayback();
void AudioPlayback_ReleaseGlobalSFX();
void AudioPlayback_ReleaseStageSFX();
void AudioPlayback_SetGameVolumes(int bgmVolume, int sfxVolume);
void AudioPlayback_StopAllSFX();
void AudioPlayback_PauseSound();
void AudioPlayback_ResumeSound();
void AudioPlayback_SetMusicTrack(char* fileName, int trackNo, uint8_t loopTrack, uint loopPoint);
void AudioPlayback_SetMusicVolume(int volume);
void AudioPlayback_PlayMusic(int trackNo);
void AudioPlayback_StopMusic();
void AudioPlayback_LoadSfx(char* fileName, int sfxNum);
void AudioPlayback_PlaySfx(int sfxNum, uint8_t sLoop);
void AudioPlayback_StopSfx(int sfxNum);
void AudioPlayback_SetSfxAttributes(int sfxNum, int volume, int pan);

#endif /* AudioPlayback_h */
