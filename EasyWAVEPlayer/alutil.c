#include <stdio.h>
#include "alutil.h"

int alInit() {
	ALCdevice* device;
	ALCcontext* context;

	device = alcOpenDevice(NULL);
	if (device == NULL) {
		fprintf(stderr, "ERROR: NO DEVICE\n");
		return 0;
	}
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		fprintf(stderr, "ERROR: NO CONTEXT\n");
		alcCloseDevice(device);
		return 0;
	}

	return 1;
}

void alExit() {
	ALCdevice* device;
	ALCcontext* context;

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

ALuint bufferWavData(char fileName[]) {
	char type[4];
	ALuint size;
	ALshort formatType;
	ALshort channel;
	ALuint sampleRate;
	ALuint byteRate;
	ALshort bytesPerSample;
	ALshort bitsPerSample;
	ALuint dataSize;
	unsigned char* data;
	FILE* wavFile;
	ALuint buffer;
	ALenum format;

	int fileNameSize = strlen(fileName);

	for (int i = 0; i < fileNameSize; i++) {
		if (fileName[i] == '\\')
			fileName[i] = '/';
	}

	wavFile = fopen(fileName, "rb");
	if (wavFile) {
		fread(type, 1, 4, wavFile);
		if (type[0] == 'R' && type[1] == 'I' && type[2] == 'F' && type[3] == 'F') {
			fseek(wavFile, 8, SEEK_SET);
			fread(type, 1, 4, wavFile);
			if (type[0] == 'W' && type[1] == 'A' && type[2] == 'V' && type[3] == 'E') {
				fread(type, 1, 4, wavFile);
				if (type[0] == 'f' && type[1] == 'm' && type[2] == 't' && type[3] == ' ') {
					fread(&size, 4, 1, wavFile);
					fread(&formatType, 2, 1, wavFile);
					fread(&channel, 2, 1, wavFile);
					fread(&sampleRate, 4, 1, wavFile);
					fread(&byteRate, 4, 1, wavFile);
					fread(&bytesPerSample, 2, 1, wavFile);
					fread(&bitsPerSample, 2, 1, wavFile);
					fread(type, 1, 4, wavFile);
					if (type[0] == 'd' && type[1] == 'a' && type[2] == 't' && type[3] == 'a') {
						fread(&dataSize, 4, 1, wavFile);
						data = (unsigned char*)malloc(dataSize * sizeof(unsigned char));
						fread(data, 1, dataSize, wavFile);
						fclose(wavFile);
					}
					else {
						fprintf(stderr, "ERROR: NO DATA SIGN\n");
						fclose(wavFile);
						return 0;
					}
				}
				else {
					fprintf(stderr, "ERROR: NOT fmt\n");
					fclose(wavFile);
					return 0;
				}
			}
			else {
				fprintf(stderr, "ERROR: NOT WAVE\n");
				fclose(wavFile);
				return 0;
			}
		}
		else {
			fprintf(stderr, "ERROR: NOT RIFF\n");
			fclose(wavFile);
			return 0;
		}
	}
	else {
		fprintf(stderr, "ERROR: CANNOT OPEN FILE\n");
		return 0;
	}

	if (bitsPerSample == 16) {
		if (channel == 2)
			format = AL_FORMAT_STEREO16;
		else if (channel == 1)
			format = AL_FORMAT_MONO16;
		else {
			fprintf(stderr, "ERROR: WEIRD NUMBER\n");
			return 0;
		}
	}
	else if (bitsPerSample == 8) {
		if (channel == 2)
			format = AL_FORMAT_STEREO8;
		else if (channel == 1)
			format = AL_FORMAT_MONO8;
		else {
			fprintf(stderr, "ERROR: WEIRD NUMBER\n");
			return 0;
		}
	}
	else {
		fprintf(stderr, "ERROR: WEIRD NUMBER\n");
		return 0;
	}

	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, data, dataSize, sampleRate);
	if (data)
		free(data);

	return buffer;
}

ALuint createNormalSource(ALuint buffer) {
	ALfloat sourcePos[] = {0.0, 0.0, 0.0};
	ALfloat sourceVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerPos[] = {0.0, 0.0, 0.0};
	ALfloat listenerVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	ALuint source = 0;

	if (buffer != 0) {
		alGenSources(1, &source);
		alListenerfv(AL_POSITION, listenerPos);
		alListenerfv(AL_VELOCITY, listenerVel);
		alListenerfv(AL_ORIENTATION, listenerOri);
		alSourcei(source, AL_BUFFER, buffer);
		alSourcef(source, AL_PITCH, 1.0f);
		alSourcef(source, AL_GAIN, 0.2f);
		alSourcefv(source, AL_POSITION, sourcePos);
		alSourcefv(source, AL_VELOCITY, sourceVel);
		alSourcei(source, AL_LOOPING, AL_FALSE);
	}

	return source;
}