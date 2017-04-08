#include <stdio.h>
#include <stdlib.h>
#include <al.h>
#include <alc.h>

int menu(void);
int loadWavFile(char fileName[], size_t* size, short* formatType, short* channel,
	size_t* sampleRate, size_t* byteRate, short* bytesPerSample, short* bitsPerSample,
	size_t* dataSize, unsigned char** wavData);

int main(int argc, char** argv) {
	menu();

	return 0;
}

int menu() {
	int option;
	int option2;
	char fileName[_MAX_PATH];

	size_t size;
	short formatType;
	short channel;
	size_t sampleRate;
	size_t byteRate;
	short bytesPerSample;
	short bitsPerSample;
	size_t dataSize;
	unsigned char* wavData = NULL;

	ALCdevice* device;
	ALCcontext* context;
	ALuint buffer;
	ALuint source;
	ALsizei frequency;
	ALenum format;
	ALfloat sourcePos[] = {0.0, 0.0, 0.0};
	ALfloat sourceVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerPos[] = {0.0, 0.0, 0.0};
	ALfloat listenerVel[] = {0.0, 0.0, 0.0};
	ALfloat listenerOri[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};
	ALfloat volume;
	ALint loop;

	device = alcOpenDevice(NULL);
	if (!device) {
		printf("ERROR: NO device\n");
		return -1;
	}
	context = alcCreateContext(device, NULL);
	if (!alcMakeContextCurrent(context)) {
		printf("ERROR: NO context\n");
		return -1;
	}

	printf("+-----------------------------+\n"
		"|   Easy WAVE Player v0.01    |\n"
		"|        By WingHearn         |\n"
		"+-----------------------------+\n\n"
		"** WARNING **\n"
		"This program ONLY supports pure WAVE format.\n"
		"This program ONLY supports file names without spaces.\n"
	);

	do {
		printf("\n\n\n1. Enter WAV file path\n"
			"2. Quit\n"
			"Choose an option (1 or 2): ");

		scanf("%d", &option);

		switch (option) {
		case 1:
			printf("\nPath: ");
			scanf("%s", fileName);
			if (loadWavFile(fileName, &size, &formatType, &channel,
				&sampleRate, &byteRate, &bytesPerSample, &bitsPerSample,
				&dataSize, &wavData)) {

				alGenBuffers(1, &buffer);
				alGenSources(1, &source);

				if (bitsPerSample == 16) {
					if (channel == 2)
						format = AL_FORMAT_STEREO16;
					else if (channel == 1)
						format = AL_FORMAT_MONO16;
					else {
						printf("ERROR: WEIRD NUMBER\n");
						break;
					}
				}
				else if (bitsPerSample == 8) {
					if (channel == 2)
						format = AL_FORMAT_STEREO8;
					else if (channel == 1)
						format = AL_FORMAT_MONO8;
					else {
						printf("ERROR: WEIRD NUMBER\n");
						break;
					}
				}
				else {
					printf("ERROR: WEIRD NUMBER\n");
					break;
				}

				frequency = sampleRate;
				volume = (float)0.2;
				loop = AL_FALSE;
				alBufferData(buffer, format, wavData, dataSize, frequency);
				alListenerfv(AL_POSITION, listenerPos);
				alListenerfv(AL_VELOCITY, listenerVel);
				alListenerfv(AL_ORIENTATION, listenerOri);
				alSourcei(source, AL_BUFFER, buffer);
				alSourcef(source, AL_PITCH, 1.0);
				alSourcef(source, AL_GAIN, volume);
				alSourcefv(source, AL_POSITION, sourcePos);
				alSourcefv(source, AL_VELOCITY, sourceVel);
				alSourcei(source, AL_LOOPING, loop);

				do {
					printf("\n\n1. Play\n"
						"2. Pause\n"
						"3. Stop\n"
						"4. Volume up\n"
						"5. Volume down\n"
						"6. Repeat ON/OFF\n"
						"7. Quit\n"
						"Choose an option (1 - 7): ");

					scanf("%d", &option2);

					switch (option2) {
					case 1:
						alSourcePlay(source);
						option2 = 0;
						break;
					case 2:
						alSourcePause(source);
						option2 = 0;
						break;
					case 3:
						alSourceStop(source);
						option = 0;
						break;
					case 4:
						alGetSourcef(source, AL_GAIN, &volume);
						alSourcef(source, AL_GAIN, volume + (float)0.1);
						option2 = 0;
						break;
					case 5:
						alGetSourcef(source, AL_GAIN, &volume);
						alSourcef(source, AL_GAIN, volume - (float)0.1);
						option2 = 0;
						break;
					case 6:
						alGetSourcei(source, AL_LOOPING, &loop);
						if (loop) {
							alSourcei(source, AL_LOOPING, AL_FALSE);
							printf("\nRepeat mode is switched to OFF\n");
						}
						else {
							alSourcei(source, AL_LOOPING, AL_TRUE);
							printf("\nRepeat mode is switched to ON\n");
						}
						option2 = 0;
						break;
					case 7:
						break;
					default:
						printf("\nWrong option!\n\n");
						while (getchar() != '\n');
						option2 = 0;
					}

				} while (option2 != 7);

				if (wavData)
					free(wavData);
				alDeleteBuffers(1, &buffer);
				alDeleteSources(1, &source);
			}

			option = 0;
			break;
		case 2:
			printf("\nSee ya!\n\n");
			break;
		default:
			printf("\nWrong option!\n\n");
			while (getchar() != '\n');
			option = 0;
		}

	} while (option != 2);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}

int loadWavFile(char fileName[], size_t* size, short* formatType, short* channel,
	size_t* sampleRate, size_t* byteRate, short* bytesPerSample, short* bitsPerSample,
	size_t* dataSize, unsigned char** wavData) {

	FILE* wavFile = NULL;
	char type[4];
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
					fread(size, 4, 1, wavFile);
					fread(formatType, 2, 1, wavFile);
					fread(channel, 2, 1, wavFile);
					fread(sampleRate, 4, 1, wavFile);
					fread(byteRate, 4, 1, wavFile);
					fread(bytesPerSample, 2, 1, wavFile);
					fread(bitsPerSample, 2, 1, wavFile);
					fread(type, 1, 4, wavFile);
					if (type[0] == 'd' && type[1] == 'a' && type[2] == 't' && type[3] == 'a') {
						fread(dataSize, 4, 1, wavFile);
						*wavData = (unsigned char*)malloc((*dataSize) * sizeof(unsigned char));
						fread(*wavData, 1, *dataSize, wavFile);
					}
					else {
						printf("ERROR: CANNOT FIND data HEAD\n");
						fclose(wavFile);
						return 0;
					}
				}
				else {
					printf("ERROR: NOT fmt\n");
					fclose(wavFile);
					return 0;
				}
			}
			else {
				printf("ERROR: NOT WAVE\n");
				fclose(wavFile);
				return 0;
			}
		}
		else {
			printf("ERROR: NOT RIFF\n");
			fclose(wavFile);
			return 0;
		}
	}
	else {
		printf("ERROR: CANNOT OPEN FILE\n");
		//fclose(wavFile);
		return 0;
	}
	printf("File loaded\n");
	fclose(wavFile);
	return 1;
}