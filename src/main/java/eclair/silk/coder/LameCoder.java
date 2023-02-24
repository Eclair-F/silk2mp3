package eclair.silk.coder;

import java.io.*;

public class LameCoder {


    public LameCoder() {
        initializeDefault();
    }

    public LameCoder(LameBuilder builder) {
        initialize(builder);
    }


    private void initialize(LameBuilder builder) {
        initialize(builder.inSampleRate, builder.outChannel, builder.outSampleRate,
                builder.outBitrate, builder.scaleInput, getIntForMode(builder.mode),
                getIntForVbrMode(builder.vbrMode), builder.quality, builder.vbrQuality, builder.abrMeanBitrate,
                builder.lowpassFreq, builder.highpassFreq, builder.id3tagTitle, builder.id3tagArtist,
                builder.id3tagAlbum, builder.id3tagYear, builder.id3tagComment);
    }

    public void close() {
        lameClose();
    }

    public static void encode(String source, String dest, int bitrate) {

        LameCoder coder = new LameBuilder()
                .setInSampleRate(bitrate)
                .setOutChannels(1)
                .setOutBitrate(48)
                .setOutSampleRate(24000)
                .setQuality(8)
                .build();
        encodeFile(source, dest);
        coder.close();
    }



    private static native void initializeDefault();

    private static native void initialize(int inSamplerate, int outChannel,
                                          int outSamplerate, int outBitrate, float scaleInput, int mode, int vbrMode,
                                          int quality, int vbrQuality, int abrMeanBitrate, int lowpassFreq,
                                          int highpassFreq, String id3tagTitle,
                                          String id3tagArtist, String id3tagAlbum, String id3tagYear,
                                          String id3tagComment);


    private native static void lameClose();

    private native static void encodeFile(String source, String dest);




    private static int getIntForMode(LameBuilder.Mode mode) {
        switch (mode) {
            case STEREO:
                return 0;
            case JSTEREO:
                return 1;
            case MONO:
                return 3;
            case DEFAULT:
                return 4;
        }
        return -1;
    }

    private static int getIntForVbrMode(LameBuilder.VbrMode mode) {
        switch (mode) {
            case VBR_OFF:
                return 0;
            case VBR_RH:
                return 2;
            case VBR_ABR:
                return 3;
            case VBR_MTRH:
                return 4;
            case VBR_DEFAUT:
                return 6;
        }
        return -1;
    }

}
