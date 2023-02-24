package eclair.silk.utils;

import eclair.silk.coder.LameCoder;
import eclair.silk.coder.MP3Coder;
import eclair.silk.coder.SilkCoder;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;


public class AudioUtils {
    private static File tempDir;

    /**
     * 初始化
     */
    public static void init() throws IOException {
        init(new File(System.getProperty("java.io.tmpdir")));
    }

    public static void init(File tmpDir) throws IOException {
        AudioUtils.tempDir = tmpDir;
        if (!tempDir.canWrite()) {
            throw new IOException("缓存目录无写入权限，请重试");
        }
        NativeLibLoader.load();
    }

    public static File mp3ToSilk(File mp3File) throws IOException {
        return mp3ToSilk(mp3File, 24000);
    }

    public static File mp3ToSilk(File mp3File, int bitRate) throws IOException {
        if (!mp3File.exists() || mp3File.length() == 0) {
            throw new IOException("文件不存在或为空");
        }
        File pcmFile = getTempFile("pcm");
        File silkFile = getTempFile("silk");
        int sampleRate = MP3Coder.decode(mp3File.getAbsolutePath(), pcmFile.getAbsolutePath(),1,24000);
              SilkCoder.encode(pcmFile.getAbsolutePath(), silkFile.getAbsolutePath(), sampleRate, bitRate);
        pcmFile.delete();
        return silkFile;
    }

    public static File mp3ToPcm(File mp3File) throws IOException {
        return mp3ToPcm(mp3File, 24000);
    }

    public static File mp3ToPcm(File mp3File, int bitRate) throws IOException {
        if (!mp3File.exists() || mp3File.length() == 0) {
            throw new IOException("文件不存在或为空");
        }
        File pcmFile = getTempFile("pcm");
        MP3Coder.decode(mp3File.getAbsolutePath(), pcmFile.getAbsolutePath(),1,24000);
        return pcmFile;
    }

    public static File mp3ToSilk(InputStream mp3FileStream) throws IOException {
        return mp3ToSilk(mp3FileStream, 24000);
    }

    public static File mp3ToSilk(InputStream mp3FileStream, int bitRate) throws IOException {
        File mp3File = getTempFile("mp3");
        streamToTempFile(mp3FileStream, mp3File);
        return mp3ToSilk(mp3File, bitRate);
    }

    public static File silkToMp3(File silkFile) throws IOException {
        return silkToMp3(silkFile, 24000);
    }

    public static File silkToMp3(File silkFile, int bitrate) throws IOException {
        if (!silkFile.exists() || silkFile.length() == 0) {
            throw new IOException("文件不存在或为空");
        }
        File pcmFile = getTempFile("pcm");
        File mp3File = getTempFile("mp3");
        SilkCoder.decode(silkFile.getAbsolutePath(), pcmFile.getAbsolutePath());
        LameCoder.encode(pcmFile.getAbsolutePath(), mp3File.getAbsolutePath(), bitrate);
//        pcmFile.delete();
        return mp3File;
    }

    public static File silkToPcm(File silkFile) throws IOException {
        return silkToPcm(silkFile, 24000);
    }

    public static File silkToPcm(File silkFile, int bitrate) throws IOException {
        if (!silkFile.exists() || silkFile.length() == 0) {
            throw new IOException("文件不存在或为空");
        }
        File pcmFile = getTempFile("pcm");
        SilkCoder.decode(silkFile.getAbsolutePath(), pcmFile.getAbsolutePath());

        return pcmFile;
    }

    public static File silkToMp3(InputStream silkFileStream) throws IOException {
        return silkToMp3(silkFileStream, 24000);
    }

    public static File silkToMp3(InputStream silkFileStream, int bitrate) throws IOException {
        File mp3File = getTempFile("silk");
        streamToTempFile(silkFileStream, mp3File);
        return silkToMp3(mp3File, bitrate);
    }


    static void streamToTempFile(InputStream inputStream, File tmpFile) throws IOException {
        FileOutputStream fileOutputStream = new FileOutputStream(tmpFile);
        byte[] buf = new byte[1024];
        int bytesRead;
        while ((bytesRead = inputStream.read(buf)) > 0) {
            fileOutputStream.write(buf, 0, bytesRead);
        }
        inputStream.close();
        fileOutputStream.close();
    }

    static File getTempFile(String type) {
        String fileName = "mirai_audio_" +
                type +
                "_" +
                System.currentTimeMillis() +
                "." +
                type;
        return new File(tempDir, fileName);
    }
}
