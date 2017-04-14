#ifndef ALUTIL_H
#define ALUTIL_H
#include <al.h>
#include <alc.h>

int alInit(void);
void alExit(void);
ALuint bufferWavData(char[]);
ALuint createNormalSource(ALuint);

#endif
