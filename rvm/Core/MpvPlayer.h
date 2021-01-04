//
//  MpvPlayer.h
//  rvm
//

#ifndef MpvPlayer_h
#define MpvPlayer_h

#ifdef ENABLE_MPV
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <stdio.h>
#include "GlobalAppDefinitions.h"

void MpvPlayer(char* fileName);

#endif
#endif /* MpvPlayer_h */
