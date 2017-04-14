#include <stdio.h>
#include <stdlib.h>
#include "alutil.h"

int menu(void);

int main(int argc, char** argv) {
	menu();

	return 0;
}

int menu() {
	int option;
	int option2;
	char fileName[_MAX_PATH]; // _MAX_PATH = 260
	ALuint buffer;
	ALuint source;
	ALfloat volume;
	ALuint loop;

	if (!alInit()) {
		fprintf(stderr, "ERROR: CANNOT INITIALIZE\n");
		return -1;
	}

	printf("+-----------------------------+\n"
		"|   Easy WAVE Player v0.03    |\n"
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
			buffer = bufferWavData(fileName);
			source = createNormalSource(buffer);
			if (buffer) {
				printf("FILE LOADED SUCCESSFULLY\n");
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

				alSourceStop(source);
				alDeleteSources(1, &source);
				alDeleteBuffers(1, &buffer);
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

	alExit();

	return 0;
}