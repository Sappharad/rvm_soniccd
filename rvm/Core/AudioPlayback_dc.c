//
//  AudioPlayback.c
//  rvm
//

#include "AudioPlayback_dc.h"

const int MUSIC_STOPPED = 0;
const int MUSIC_PLAYING = 1;
const int MUSIC_PAUSED = 2;
const int MUSIC_LOADING = 3;
const int MUSIC_READY = 4;
const int NUM_CHANNELS = 8;
int numGlobalSFX;
int numStageSFX;
//Mix_Chunk* sfxSamples[256];
bool sfxLoaded[256];
int channelSfxNum[8];
int nextChannelPos;
bool musicEnabled;
int musicVolume;
float musicVolumeSetting;
float sfxVolumeSetting;
int musicStatus;
int currentMusicTrack;
struct MusicTrackInfo musicTracks[16];

void InitAudioPlayback()
{
    numGlobalSFX = 0;
    numStageSFX = 0;
    musicEnabled = true;
    musicVolume = 100;
    musicVolumeSetting = 1.0f;
    sfxVolumeSetting = 1.0f;
    musicStatus = 0;
    /*
    //Init SDL_Mixer and Audio - Must be done before audio can be loaded.
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
        printf("Failed to open audio mixer!");
    }
    else{
        Mix_AllocateChannels(MIX_CHANNELS);
    }*/
    
    struct FileData fileData;
    char array[32];
    for (int i = 0; i < 8; i++)
    {
        channelSfxNum[i] = -1;
    }
    if (FileIO_LoadFile("Data/Game/GameConfig.bin", &fileData))
    {
        uint8_t b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        uint8_t b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            int i;
            uint8_t b2;
            for (i = 0; i < (int)b; i++)
            {
                b2 = FileIO_ReadByte();
                array[i] = (char)b2;
            }
            array[i] = '\0';
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
        }
        b3 = FileIO_ReadByte();
        numGlobalSFX = (int)b3;
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                array[i] = (char)b2;
            }
            array[i] = '\0';
            FileIO_GetFileInfo(&fileData);
            FileIO_CloseFile();
            AudioPlayback_LoadSfx(array, j);
            FileIO_SetFileInfo(&fileData);
        }
    }
}
void AudioPlayback_ReleaseAudioPlayback()
{
    //Mix_CloseAudio();
}
void AudioPlayback_ReleaseGlobalSFX()
{
}
void AudioPlayback_ReleaseStageSFX()
{
}
void AudioPlayback_SetGameVolumes(int bgmVolume, int sfxVolume)
{
    musicVolumeSetting = (float)bgmVolume * 0.01f;
    AudioPlayback_SetMusicVolume(musicVolume);
    sfxVolumeSetting = (float)sfxVolume * 0.01f;
}
void AudioPlayback_StopAllSFX()
{
    /*for (int i = 0; i < NUM_CHANNELS; i++)
    {
        Mix_HaltChannel(i);
    }*/
}
void AudioPlayback_PauseSound()
{
    /*Mix_PauseMusic();
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        Mix_Pause(i);
    }*/
}
void AudioPlayback_ResumeSound()
{
    /*Mix_ResumeMusic();
    for (int i = 0; i < NUM_CHANNELS; i++)
    {
        Mix_Resume(i);
    }*/
}
void AudioPlayback_SetMusicTrack(char* fileName, int trackNo, uint8_t loopTrack, uint loopPoint)
{
    /*struct FileData fData;
    char array[] = "Data/Music/";
    int num = (int)strlen(fileName);
    if (num < 0)
    {
        num = 0;
    }
    if (strlen(fileName) > 0)
    {
        fileName[num] = '\0';
    }
    FileIO_StrCopy(musicTracks[trackNo].trackName, sizeof(musicTracks[trackNo].trackName), array, sizeof(array));
    FileIO_StrAdd(musicTracks[trackNo].trackName, sizeof(musicTracks[trackNo].trackName), fileName, (int)strlen(fileName));
    if (loopTrack == 1)
    {
        musicTracks[trackNo].loop = true;
    }
    else
    {
        musicTracks[trackNo].loop = false;
    }
    musicTracks[trackNo].loopPoint = loopPoint;
    if(musicTracks[trackNo].mixerAudio){
        Mix_FreeMusic(musicTracks[trackNo].mixerAudio);
        musicTracks[trackNo].mixerAudio = NULL;
    }
    if(musicTracks[trackNo].musicData){
        free(musicTracks[trackNo].musicData);
        musicTracks[trackNo].musicData = NULL;
    }
    if(num == 0){
        return; //We just wanted to clear the audio track entry by replacing it with a blank one.
    }
    else if (FileIO_LoadFile(musicTracks[trackNo].trackName, &fData))
    {
        //Todo: Use the loop point
        musicTracks[trackNo].musicData = malloc(fData.fileSize);
        FileIO_ReadByteArray(musicTracks[trackNo].musicData, fData.fileSize);
        musicTracks[trackNo].mixerAudio = Mix_LoadMUSType_RW(SDL_RWFromMem(musicTracks[trackNo].musicData, fData.fileSize), MUS_OGG, SDL_TRUE);
        FileIO_CloseFile();
    }*/
}
void AudioPlayback_SetMusicVolume(int volume)
{
    /*if (volume < 0)
    {
        volume = 0;
    }
    if (volume > 100)
    {
        volume = 100;
    }
    musicVolume = volume;
    double sdl_volume = 128.0 * (musicVolume / 100.0);
    Mix_VolumeMusic((int)sdl_volume);*/
}
void AudioPlayback_PlayMusic(int trackNo)
{
    /*if (musicTracks[trackNo].trackName[0] != '\0' && musicTracks[trackNo].mixerAudio)
    {
        currentMusicTrack = trackNo;
        musicVolume = 100;
        musicStatus = 1;
        double sdl_volume = 128.0 * (musicVolume / 100.0);
        Mix_VolumeMusic((int)sdl_volume);
        Mix_PlayMusic(musicTracks[trackNo].mixerAudio, (musicTracks[trackNo].loop > 0) ? -1 : 0);
    }*/
}
void AudioPlayback_StopMusic()
{
    /*if (musicTracks[currentMusicTrack].mixerAudio)
    {
        Mix_HaltMusic();
    }*/
    musicStatus = 0;
}
void AudioPlayback_LoadSfx(char* fileName, int sfxNum)
{
    /*struct FileData fData;
    char filePath[64];
    char array[] = "Data/SoundFX/";
    if (sfxNum > -1 && sfxNum < 256)
    {
        FileIO_StrCopy(filePath, sizeof(filePath), array, sizeof(array));
        FileIO_StrAdd(filePath, sizeof(filePath), fileName, (int)strlen(fileName));
        if (FileIO_LoadFile(filePath, &fData))
        {
            uint8_t* sampleData = malloc(fData.fileSize);
            FileIO_ReadByteArray(sampleData, fData.fileSize);
            if (sfxLoaded[sfxNum])
            {
                Mix_FreeChunk(sfxSamples[sfxNum]);
                sfxLoaded[sfxNum] = false;
            }
            sfxSamples[sfxNum] = Mix_LoadWAV_RW(SDL_RWFromMem(sampleData, fData.fileSize), SDL_TRUE);
#if DEBUG
            if(!sfxSamples[sfxNum]) {
                printf("Mix_LoadWAV_RW: %s\n", Mix_GetError());
            }
#endif
            sfxLoaded[sfxNum] = true;
            FileIO_CloseFile();
        }
    }*/
}
void AudioPlayback_PlaySfx(int sfxNum, uint8_t sLoop)
{
    /*for (int i = 0; i < NUM_CHANNELS; i++)
    {
        if (channelSfxNum[i] == sfxNum)
        {
            Mix_HaltChannel(i);
            nextChannelPos = i;
            i = 8;
        }
    }
    Mix_SetPanning(nextChannelPos, 255, 255);
    Mix_PlayChannel(nextChannelPos, sfxSamples[sfxNum], (sLoop==1)?-1:0);
    channelSfxNum[nextChannelPos] = sfxNum;
    nextChannelPos++;
    if (nextChannelPos == 8)
    {
        nextChannelPos = 0;
    }*/
}
void AudioPlayback_StopSfx(int sfxNum)
{
    /*for (int i = 0; i < NUM_CHANNELS; i++)
    {
        if (channelSfxNum[i] == sfxNum)
        {
            channelSfxNum[i] = -1;
            Mix_HaltChannel(i);
        }
    }*/
}
void AudioPlayback_SetSfxAttributes(int sfxNum, int volume, int pan)
{
    /*for (int i = 0; i < NUM_CHANNELS; i++)
    {
        if (channelSfxNum[i] == sfxNum)
        {
            Mix_HaltChannel(i);
            nextChannelPos = i;
            i = 8;
        }
    }
    uint8_t sdlPan = (pan*255)/100;
    Mix_SetPanning(nextChannelPos, sdlPan, 255-sdlPan);
    Mix_PlayChannel(nextChannelPos, sfxSamples[sfxNum], 0);
    channelSfxNum[nextChannelPos] = sfxNum;
    nextChannelPos++;
    if (nextChannelPos == 8)
    {
        nextChannelPos = 0;
    }*/
}
