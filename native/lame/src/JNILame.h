//
// Created by mzdlu on 2021-02-05.
//

#ifndef NATIVE_JNILAME_H
#define NATIVE_JNILAME_H
#include "eclair_silk_coder_LameCoder.h"
#include "lame.h"
#include <jni.h>


lame_global_flags *initializeDefault(
        JNIEnv *env);

lame_global_flags *initialize(
        JNIEnv *env,

        jint inSamplerate, jint outChannel,
        jint outSamplerate, jint outBitrate, jfloat scaleInput, jint mode, jint vbrMode,
        jint quality, jint vbrQuality, jint abrMeanBitrate, jint lowpassFreq, jint highpassFreq,
        jstring id3tagTitle, jstring id3tagArtist, jstring id3tagAlbum,
        jstring id3tagYear, jstring id3tagComment);



void close(
        lame_global_flags *glf);


#endif //NATIVE_JNILAME_H
