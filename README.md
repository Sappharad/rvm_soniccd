# RVM
Incomplete / Work in Progress.

An attempt to recreate the engine behind the 2011 mobile versions of Sonic CD. Requires original game data (not included) to function.

Using mobile Data.rsdk from version 2.0 or newer is encouraged. The original mobile versions, PC and Console ports use an older file format that is not compatible with the newer one. RVM will automatically detect this older file format and switch into a compatibility mode to support it, but there are some differences that have not been accounted for yet such as collision flags and color palettes.

TO DO (Windows + Mac)
- Use music loop points
- Video Playback
- PC/Console water palettes for Tidal Tempest
- Other "old RSDK version" compatibility fixes
- Fix bugs?
- Release Web port

TO DO (Dreamcast)
- Start over with the newer OpenGL port that supports paletted textures
- Music and Sound playback natively on AICA, may require conversion at image creation time
- Is there enough memory for video playback and a library fast/small enough to use? (Will also require conversion)

If you want to do another port or other changes open an issue so we can track changes and make sure they get integrated back to here.

## Advantages over the PC version
- Supports Data.rsdk from the latest mobile versions of the game, with fixes that were never released to other platforms.
- Native support for loading raw data files outside of Data.rsdk
- Sharper video output

## Disadvantages
- Unfinished (see to do list)

# How to build
## Windows
- Open the Visual Studio project and compile it

## macOS
- Open the XCode project and compile it.

## Dreamcast
- Don't. Code for Dreamcast will live in the Dreamcast branch, but the current branch is outdated. It required a working linux KOS setup. The build will be migrated to a docker container at some point.

# Background
This project began in 2013, when I learned that most Windows Phone 7 developers did not properly strip and obfuscate their builds. In a matter of hours I had the WP7 code decompiled and running on macOS with MonoGame. Other games took much longer to perform the same process back then, but the code for Sonic CD was very clean and easy to fix. I wanted to release an unofficial Mac port at that time, but porting the WP7 version at that time would bring undeserved attention to the obfuscation problem which affected a large number of games. That's because even if I properly obfuscated it, the fact that .NET was used would increase the chances of someone else looking into the original WP7 release.

I decided to translate the entire game in C, since the original engine was written in C and this would allow the game to be ported to even more platforms. My initial goal was to complete the macOS port then release that along with a Sega Dreamcast port at the same time. This plan changed when I found out that in March 2020 someone had posted decompiled WP7 versions of both Sonic CD and Sonic 4 Episode 1 to GitHub. Both of those codebases lacked macOS ports along with some fixes and improvements that I had made to private copies back in 2013, but now that working code was floating around I felt it would be wise to make this project public since it contains a number of improvements over the base game. 

A video of the Dreamcast port was first posted on YouTube in July of 2017. At that point most of the game was already playable but there were some bugs with gameplay and the special stage rendering was broken. Additionally, there were Dreamcast-specific problems due to hardware constraints that I had not yet accounted for yet. I made the decision to stop working on the Dreamcast port until all of the base code that could be shared with Dreamcast was fixed. The last major problem was fixed in April 2020, and since sound and video code won't be shared between PC and Dreamcast it's when I started migrating the newer codebase over to Dreamcast.

There will be no official releases until the TO DO list above is completed. Unsupported beta builds can be found in the releases tab.
