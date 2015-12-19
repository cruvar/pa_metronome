#include "stdafx.h"
#include <iostream>
#include <portaudio.h>
#include <math.h>
#include <ctime>

#define SAMPLE_RATE (44100)

using namespace std;

struct myData
{
	int frequencyTick;
	int bpm;
	int buffer;
};

void HandleError(PaError &err)
{
	Pa_Terminate();
	std::cout << "��������� ������ �� ����� ������������� ������ Portaudio" << std::endl;
	std::cout << "����� ������: " << err << std::endl;
	std::cout << "��������� �� ������: " << Pa_GetErrorText(err) << std::endl;
	exit(err);
}

static int patestCallback(const void*                     inputBuffer,
	void*                           outputBuffer,
	unsigned long                   framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags           statusFlags,
	void*                           userData)

{
	static unsigned int timeTmp = 0;
	static unsigned int counter = 0;
	float sampleVal = 0;
	const float pi = 3.14159265358;
	myData *data = (myData*)userData;
	float *out = (float*)outputBuffer;

	float framesInMs = SAMPLE_RATE / 1000.0;	//���������� ������ � ����� ��. = 44.1
	float msCount = timeTmp / framesInMs;		//������� ��
	double tick = framesInMs * 100;				//������������ ������ ���� � ������
	double delayMs = (1000 * 60) / data->bpm;	//�������� � ��
	double interval = (framesInMs * delayMs);   //�������� ����� ������ � ������. =  
	unsigned int countCicle = 0;				//������� �������� �����. ���� ���� ��� 44100�� / 1000�� 
	
	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		sampleVal = (float)sin(2.0*pi*data->frequencyTick*(timeTmp) / SAMPLE_RATE);

		timeTmp++;
		countCicle = timeTmp / framesPerBuffer;

		if (counter < tick)
		{
			*out++ = sampleVal;
			*out++ = sampleVal;
			counter++;
		}
		else
			if (counter < interval)
			{
				*out++ = 0;
				*out++ = 0;
				counter++;
			}
			else
				counter = 0;
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

	std::cout << "������� ������� 1 � ������: ";
	std::cin >> data.frequencyTick;
	//std::cout << "������� ������ ������: ";
	//std::cin >> data.buffer;
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

	unsigned int start_time = clock();

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

	unsigned int end_time = clock();

	if (err != paNoError)
		HandleError(err);

	Pa_Terminate();



	std::cout << "������������� ��������� �������!" << std::endl;
	std::cout << "runtime = " << end_time - start_time << std::endl; // ����� ������ ���������  

	return err;
}