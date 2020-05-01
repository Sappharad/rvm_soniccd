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
#include "SDL_mixer.h"

extern int numGlobalSFX;
extern int numStageSFX;
extern bool sfxLoaded[256];
extern int channelSfxNum[8];
extern int nextChannelPos;
extern bool musicEnabled;
extern int musicVolume;
extern float musicVolumeSetting;
extern float sfxVolumeSetting;
extern int musicStatus;
extern int currentMusicTrack;
extern struct MusicTrackInfo musicTracks[16];

void InitAudioPlayback(void);
void AudioPlayback_ReleaseAudioPlayback(void);
void AudioPlayback_ReleaseGlobalSFX(void);
void AudioPlayback_ReleaseStageSFX(void);
void AudioPlayback_SetGameVolumes(int bgmVolume, int sfxVolume);
void AudioPlayback_StopAllSFX(void);
void AudioPlayback_PauseSound(void);
void AudioPlayback_ResumeSound(void);
void AudioPlayback_SetMusicTrack(char* fileName, int trackNo, uint8_t loopTrack, uint32_t loopPoint);
void AudioPlayback_SetMusicVolume(int volume);
void AudioPlayback_PlayMusic(int trackNo);
void AudioPlayback_StopMusic(void);
void AudioPlayback_LoadSfx(char* fileName, int sfxNum);
void AudioPlayback_PlaySfx(int sfxNum, uint8_t sLoop);
void AudioPlayback_StopSfx(int sfxNum);
void AudioPlayback_SetSfxAttributes(int sfxNum, int volume, int pan);

#endif /* AudioPlayback_h */
