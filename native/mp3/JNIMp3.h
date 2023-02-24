//
// Created by mzdlu on 2021-02-05.
//

#ifndef NATIVE_JNIFFMPEG_H
#define NATIVE_JNIFFMPEG_H
#include "eclair_silk_coder_MP3Coder.h"
#include <jni.h>

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE 1 
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include <stdio.h>
#include <stdlib.h>    
#include <stdint.h>    

#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"
#include <sys/stat.h>

void pcmWrite_int16(char*, int16_t*, int, uint32_t, int);
void wavWrite_int16(char*, int16_t*, int, uint32_t, int);
void stereo_2_mono(const int16_t* , int , int16_t* );
void resampleData(const int16_t* , int32_t , uint32_t , int16_t* ,int32_t );
char* getFileBuffer(const char*, int*);

int16_t* DecodeMp3ToBuffer(char*, uint32_t*, uint32_t*, unsigned int*);

JNIEXPORT jint JNICALL Java_eclair_silk_coder_MP3Coder_decodeMP3
  (JNIEnv *, jclass, jstring, jstring, jint, jint);

#endif //NATIVE_JNIFFMPEG_H
