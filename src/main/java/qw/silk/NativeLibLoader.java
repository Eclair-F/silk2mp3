package qw.silk;

import java.io.*;
import java.util.List;
import java.util.stream.Collectors;


public class NativeLibLoader {

    public static void load() {
        InputStream fileListInputStream = NativeLibLoader.class.getResourceAsStream("src/main/resources/silk_libs/filelist.txt");
        InputStreamReader inputStreamReader = new InputStreamReader(fileListInputStream);
        BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
        List<String> fileList = bufferedReader.lines().collect(Collectors.toList());
        int successCount = 0;
        for (String libFile :
                fileList) {
            if (successCount >= 2) {
                return;
            }
            try {
               System.load(NativeLibLoader.class.getResource(libFile).getPath());
                successCount++;
            } catch (UnsatisfiedLinkError ignored) {
            }
        }
        if (successCount < 2) {
            throw new UnsatisfiedLinkError("未找到适用于当前操作系统的动态链接库，请联系作者");
        }
    }
}
