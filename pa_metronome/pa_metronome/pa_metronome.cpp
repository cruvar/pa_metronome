#include "stdafx.h"
#include <iostream>
#include <portaudio.h>
#include <math.h>

#define SAMPLE_RATE (44100)

struct myData
{
	int frequency;
	int bpm;
};

void HandleError(PaError &err)
{
	Pa_Terminate();
	std::cout << "��������� ������ �� ����� ������������� ������ Portaudio" << std::endl;
	std::cout << "����� ������: " << err << std::endl;
	std::cout << "��������� �� ������: " << Pa_GetErrorText(err) << std::endl;
	exit(err);
}

static int patestCallback(	const void*                     inputBuffer,
							void*                           outputBuffer,
							unsigned long                   framesPerBuffer,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags           statusFlags,
							void*                           userData)

{
	static unsigned int timeTmp = 0;
	float sampleValTmp = 0;
	const float pi = 3.14159265358;
	myData *data = (myData*)userData;
	float *out = (float*)outputBuffer;


	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		sampleValTmp = (float)sin(2.0*pi*data->frequency*(timeTmp++) / SAMPLE_RATE);
		*out++ = sampleValTmp;
		*out++ = sampleValTmp;
	}
	return 0;
}

int main(void)
{
	setlocale(LC_ALL, "rus");
	myData data;
	PaError err;
	PaStream *stream;
	PaStreamParameters outputParameters;

	std::cout << "��������� Portaudio: ��������" << std::endl;

	std::cout << "������� ������� ����� � ������: ";
	std::cin >> data.frequency;
	std::cout << "������� BPM: ";
	std::cin >> data.bpm;
	std::cout << "������� ENTER ��� ������� ���������\n";
	getchar();

	err = Pa_Initialize();

	if (err != paNoError)
		HandleError(err);

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice)
	{
		std::cout << "������: ����������� ���������� ������ �� �������" << std::endl;
		HandleError(err);
	}

	outputParameters.channelCount = 2;
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&stream,
		NULL,              /* No input. */
		&outputParameters, /* As above. */
		SAMPLE_RATE,
		256,               /* Frames per buffer. */
		paClipOff,         /* No out of range samples expected. */
		patestCallback,
		&data);

	if (err != paNoError)
		HandleError(err);

	err = Pa_StartStream(stream);

	if (err != paNoError)
		HandleError(err);

	std::cout << "������� ENTER ��� ��������� ���������";
	getchar();

	err = Pa_CloseStream(stream);

	if (err != paNoError)
		HandleError(err);

	Pa_Terminate();

	std::cout << "������������� ��������� �������!" << std::endl;

	return err;
}

