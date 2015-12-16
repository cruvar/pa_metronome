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
	std::cout << "Произошла ошибка во время использования потока Portaudio" << std::endl;
	std::cout << "Номер ошибки: " << err << std::endl;
	std::cout << "Сообщение об ошибке: " << Pa_GetErrorText(err) << std::endl;
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
	float sampleVal = 0;
	const float pi = 3.14159265358;
	myData *data = (myData*)userData;
	float *out = (float*)outputBuffer;

	float framesInMs = SAMPLE_RATE / 1000.0;	//количество кадров в одной мс. = 44.1
	float msCount = timeTmp / framesInMs;		//счетчик мс
	double tick = 100;							//длительность одного тика
	double delayMs = (1000 * 60) / data->bpm;	//интервал в мс
	double interval = framesInMs * delayMs;		//интервал между тиками в кадрах. =  
	unsigned int counter = 0;					//счетчик проходов цикла. один цикл это 44100Гц / 1000мс 
	
	//bool sw = true;

	for (unsigned int i = 0; i < framesPerBuffer; i++)
	{
		sampleVal = (float)sin(2.0*pi*data->frequencyTick*(timeTmp) / SAMPLE_RATE);
		
		timeTmp++;
		counter = timeTmp / framesPerBuffer;
		
		if (msCount < tick)
		{
			*out++ = sampleVal;
			*out++ = sampleVal;
		}
		else
		{
			*out++ = 0;
			*out++ = 0;
		}

	}

	cout << endl;
	cout << "timeTmp = \t\t" << timeTmp << endl;
	cout << "counter = \t\t" << counter << endl;
	//cout << "counter % 100 = \t" << counter % 100 << endl;
	cout << "ms = \t\t\t" << msCount << endl;
	//cout << "ms % 100 = \t\t" << ms % 100 << endl;
	//cout << "ms % 1000 = \t\t" << ms % 1000 << endl;
	//cout << "delay = \t\t" << delayMs << endl;
	//cout << "sec = \t\t" << counter / SAMPLE_RATE << endl;
	//cout << "framesPerBuffer = \t" << framesPerBuffer << endl;
	//printf("%.5f\n", framesInMs);
	//cout << "framesInMs = \t\t" << framesInMs << endl;
	//cout << "interval = \t" << interval << endl;

	return 0;
}


int main(void)
{
	setlocale(LC_ALL, "rus");
	myData data;
	PaError err;
	PaStream *stream;
	PaStreamParameters outputParameters;

	std::cout << "Тестируем Portaudio: метроном" << std::endl;

	std::cout << "Введите частоту 1 в герцах: ";
	std::cin >> data.frequencyTick;
	std::cout << "Введите размер буфера: ";
	std::cin >> data.buffer;
	std::cout << "Введите BPM: ";
	std::cin >> data.bpm;
	std::cout << "Нажмите ENTER для запуска программы\n";
	getchar();

	err = Pa_Initialize();

	if (err != paNoError)
		HandleError(err);

	outputParameters.device = Pa_GetDefaultOutputDevice();
	if (outputParameters.device == paNoDevice)
	{
		std::cout << "Ошибка: Стандартное устройство вывода не найдено" << std::endl;
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
		data.buffer,               /* Frames per buffer. */
		paClipOff,         /* No out of range samples expected. */
		patestCallback,
		&data);

	

	if (err != paNoError)
		HandleError(err);

	err = Pa_StartStream(stream);

	if (err != paNoError)
		HandleError(err);

	std::cout << "Нажмите ENTER для остановки программы";
	getchar();

	err = Pa_CloseStream(stream);

	unsigned int end_time = clock();

	if (err != paNoError)
		HandleError(err);

	Pa_Terminate();

	

	std::cout << "Генерирование завершено успешно!" << std::endl;
	std::cout << "runtime = " << end_time - start_time << std::endl; // время работы программы  

	return err;
}