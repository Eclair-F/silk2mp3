package eclair.silk.coder;

public class MP3Coder {


    public static int decode(String source, String dest, int channels, int sampleRate) {

        return decodeMP3(source, dest, channels, sampleRate);
    }


    private native static int decodeMP3(String source, String dest, int channels, int sampleRate);


}
