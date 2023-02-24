#include "JNIMp3.h"

//写pcm文件
void pcmWrite_int16(char* filename, int16_t* buffer, uint32_t totalSampleCount) {
	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) {
		printf("文件打开失败.\n");
		return;
	}
	//修正写入的buffer长度
	totalSampleCount *= sizeof(int16_t);
	fwrite(buffer, totalSampleCount, 1, fp);
	fclose(fp);
}
#define COMBINE(l,r) (((int32_t)(l) + (r)) >> 1)
void stereo_2_mono(const int16_t* src_audio, int sapmples_per_channel, int16_t* dst_audio) {
	for (int i = 0; i < sapmples_per_channel; i++)
	{
		//注意 >>1 只有在有符号整形的情况下才代表除以2
		dst_audio[i] = COMBINE(src_audio[2 * i], src_audio[2 * i + 1]);
	}
}


void resampleData(const int16_t* sourceData, int32_t sampleRate, uint32_t srcSize, int16_t* destinationData,
	int32_t newSampleRate) {
	if (sampleRate == newSampleRate) {
		memcpy(destinationData, sourceData, srcSize * sizeof(int16_t));
		return;
	}
	uint32_t last_pos = srcSize - 1;
	uint32_t dstSize = (uint32_t)(srcSize * ((float)newSampleRate / sampleRate));
	for (uint32_t idx = 0; idx < dstSize; idx++) {
		float index = ((float)idx * sampleRate) / (newSampleRate);
		uint32_t p1 = (uint32_t)index;
		float coef = index - p1;
		uint32_t p2 = (p1 == last_pos) ? last_pos : p1 + 1;
		destinationData[idx] = (int16_t)((1.0f - coef) * sourceData[p1] + coef * sourceData[p2]);
	}
}


//写wav文件
void wavWrite_int16(char* filename, int16_t* buffer, int sampleRate, uint32_t totalSampleCount, int channels) {
	if (!channels) {
		channels = 1;
	}
	FILE* fp = fopen(filename, "wb");
	if (fp == NULL) {
		printf("文件打开失败.\n");
		return;
	}
	//修正写入的buffer长度
	totalSampleCount *= sizeof(int16_t) * channels;
	int nbit = 16;
	int FORMAT_PCM = 1;
	int nbyte = nbit / 8;
	char text[4] = { 'R', 'I', 'F', 'F' };
	uint32_t long_number = 36 + totalSampleCount;
	fwrite(text, 1, 4, fp);
	fwrite(&long_number, 4, 1, fp);
	text[0] = 'W';
	text[1] = 'A';
	text[2] = 'V';
	text[3] = 'E';
	fwrite(text, 1, 4, fp);
	text[0] = 'f';
	text[1] = 'm';
	text[2] = 't';
	text[3] = ' ';
	fwrite(text, 1, 4, fp);

	long_number = 16;
	fwrite(&long_number, 4, 1, fp);
	int16_t short_number = FORMAT_PCM;//默认音频格式
	fwrite(&short_number, 2, 1, fp);
	short_number = channels; // 音频通道数
	fwrite(&short_number, 2, 1, fp);
	long_number = sampleRate; // 采样率
	fwrite(&long_number, 4, 1, fp);
	long_number = sampleRate * nbyte; // 比特率
	fwrite(&long_number, 4, 1, fp);
	short_number = nbyte; // 块对齐
	fwrite(&short_number, 2, 1, fp);
	short_number = nbit; // 采样精度
	fwrite(&short_number, 2, 1, fp);
	char data[4] = { 'd', 'a', 't', 'a' };
	fwrite(data, 1, 4, fp);
	long_number = totalSampleCount;
	fwrite(&long_number, 4, 1, fp);
	fwrite(buffer, totalSampleCount, 1, fp);
	fclose(fp);
}
//读取文件buffer
char* getFileBuffer(const char* fname, int* size)
{
	FILE* fd = fopen(fname, "rb");
	if (fd == 0)
		return 0;
	struct stat st;
	char* file_buf = 0;
	if (fstat(fileno(fd), &st) < 0)
		goto doexit;
	file_buf = (char*)malloc(st.st_size + 1);
	if (file_buf != NULL)
	{
		if (fread(file_buf, st.st_size, 1, fd) < 1)
		{
			fclose(fd);
			return 0;
		}
		file_buf[st.st_size] = 0;
	}

	if (size)
		*size = st.st_size;
doexit:
	fclose(fd);
	return file_buf;
}
//mp3解码
int16_t* DecodeMp3ToBuffer(char* filename, uint32_t* sampleRate, uint32_t* totalSampleCount, unsigned int* channels)
{
	int music_size = 0;
	int alloc_samples = 1024 * 1024, num_samples = 0;
	int16_t* music_buf = (int16_t*)malloc(alloc_samples * 2 * 2);
	unsigned char* file_buf = (unsigned char*)getFileBuffer(filename, &music_size);
	if (file_buf != NULL)
	{
		unsigned char* buf = file_buf;
		mp3dec_frame_info_t info;
		mp3dec_t dec;

		mp3dec_init(&dec);
		for (;;)
		{
			int16_t frame_buf[2 * 1152];
			int samples = mp3dec_decode_frame(&dec, buf, music_size, frame_buf, &info);
			if (alloc_samples < (num_samples + samples))
			{
				alloc_samples *= 2;
				int16_t* tmp = (int16_t*)realloc(music_buf, alloc_samples * 2 * info.channels);
				if (tmp)
					music_buf = tmp;
			}
			if (music_buf)
				memcpy(music_buf + num_samples * info.channels, frame_buf, samples * info.channels * 2);
			num_samples += samples;
			if (info.frame_bytes <= 0 || music_size <= (info.frame_bytes + 4))
				break;
			buf += info.frame_bytes;
			music_size -= info.frame_bytes;
		}
		if (alloc_samples > num_samples)
		{
			int16_t* tmp = (int16_t*)realloc(music_buf, num_samples * 2 * info.channels);
			if (tmp)
				music_buf = tmp;
		}

		if (sampleRate)
			*sampleRate = info.hz;
		if (channels)
			*channels = info.channels;
		if (num_samples)
			*totalSampleCount = num_samples;

		free(file_buf);
		return music_buf;
	}
	if (music_buf)
		free(music_buf);
	return 0;
}




JNIEXPORT jint JNICALL Java_eclair_silk_coder_MP3Coder_decodeMP3
(JNIEnv* env, jclass jclass1, jstring source, jstring dest, jint channel, jint out_sampleRate) {
	const char* source_path, * target_path;
	source_path = (*env)->GetStringUTFChars(env, source, NULL);
	target_path = (*env)->GetStringUTFChars(env, dest, NULL);

	//总音频采样数
	uint32_t totalSampleCount = 0;
	//音频采样率
	uint32_t sampleRate = 0;
	//通道数
	unsigned int channels = 0;
	int16_t* wavBuffer = NULL;
	int16_t* pcmBuffer = NULL;
	int16_t* resample = NULL;

	wavBuffer = DecodeMp3ToBuffer(source_path, &sampleRate, &totalSampleCount, &channels);
	
	if (channels != channel)
	{
		pcmBuffer = (int16_t*)malloc(totalSampleCount * sizeof(int16_t*));
		stereo_2_mono(wavBuffer, totalSampleCount, pcmBuffer);
	}
	else
	{
		pcmBuffer = wavBuffer;

	}
	if (out_sampleRate != sampleRate)
	{
		totalSampleCount = (uint32_t)(totalSampleCount * ((float)out_sampleRate / sampleRate));
		resample = (int16_t*)malloc(totalSampleCount * sizeof(int16_t));
		resampleData(pcmBuffer, sampleRate, totalSampleCount, resample, out_sampleRate);
	}
	else
	{
		resample = pcmBuffer;
	}
	pcmWrite_int16(target_path, resample, totalSampleCount);
	if (wavBuffer)
	{
		free(wavBuffer);
	}
	if (pcmBuffer)
	{
		free(pcmBuffer);
	}if (resample)
	{
		free(resample);
	}
	return out_sampleRate;

}
