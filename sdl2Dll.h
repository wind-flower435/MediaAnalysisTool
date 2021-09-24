#ifndef SDL2DLL_H
#define SDL2DLL_H

typedef int (*P_SDL_Init)(Uint32 flags);
typedef  int  (*P_SDL_OpenAudio)(SDL_AudioSpec * desired,SDL_AudioSpec * obtained);
typedef const char * (*P_SDL_GetError)(void);
typedef void  (*P_SDL_PauseAudio)(int pause_on);
typedef  void  (*P_SDL_Delay)(Uint32 ms);
typedef void  (*P_SDL_PauseAudio)(int pause_on);
typedef void (*P_SDL_CloseAudio)(void);
typedef void (*P_SDL_Quit)(void);
typedef int (*P_SDL_GetNumAudioDevices)(int iscapture);
typedef const char * (*P_SDL_GetAudioDeviceName)(int index,int iscapture);
typedef  void*  (*P_SDL_memset)(SDL_OUT_BYTECAP(len) void *dst, int c, size_t len);
typedef  void  (*P_SDL_MixAudio)(Uint8 * dst, const Uint8 * src, Uint32 len, int volume);
#endif // SDL2DLL_H
