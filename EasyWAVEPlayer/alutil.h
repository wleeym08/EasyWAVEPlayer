#ifndef ALUTIL_H
#define ALUTIL_H
#include <al.h>
#include <alc.h>
	
int alInit(ALCdevice**, ALCcontext**);
void alExit(ALCdevice**, ALCcontext**);
ALuint bufferWavData(char[]);
ALuint createNormalSource(ALuint);

#endif
