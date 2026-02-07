import javax.swing.plaf.InsetsUIResource;
import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;


class Main {

    private static boolean PrintDocs = true;

    public static void SearchIndex1(String str, Index1 index1) {
        var res = index1.search(str);
        System.out.println("Index1 Result: " + res);
    }
    public static void PreProcessAndSearchIndex1(String filepath, String str) {
        Index1 i1 = new Index1(filepath);
        SearchIndex1(str, i1);
    }

    public static void SearchIndex2(String str, Index2 index2) {
        var res = index2.search(str);
        System.out.println("Index2 Result: " + res);

        if (PrintDocs){
            for (String s : res){
                System.out.println(s);
            }
        }
    }
    public static void PreProcessAndSearchIndex2(String filepath, String str) {
        Index2 i2 = new Index2(filepath);
        SearchIndex2(str, i2);
    }

    public static void SearchIndex3(String str, Index3 index3) {
        var res = index3.search(str);
        System.out.println("Index3 Result: " + res);

        if (PrintDocs){
            while (res != null){
                System.out.println(res.header);
                res = res.next;
            }
        }
    }
    public static void PreProcessAndSearchIndex3(String filepath, String str) {
        Index3 i3 = new Index3(filepath);
        SearchIndex3(str, i3);
    }

    public static void SearchIndex4(String str, Index4 index4) {
        var res = index4.search(str);
        System.out.println("Index4 Result: " + res);
        if (PrintDocs){
            while (res != null){
                System.out.println(res.header);
                res = res.next;
            }
        }
    }
    public static void PreProcessAndSearchIndex4(String filepath, String str) {
        Index4 i4 = new Index4(filepath);
        SearchIndex4(str, i4);
    }

    private static void Test(String file_name, Scanner console){
        System.out.println("Started preprocessing Index1");
        Index1 i1 = new Index1(file_name);
        System.out.println("Started preprocessing Index2");
        Index2 i2 = new Index2(file_name);
        System.out.println("Started preprocessing Index4");
        Index5 i5 = new Index5(file_name);
        System.out.println("Finished preprocessing");
        for (;;) {
            System.out.println("Input search string or type exit to stop");
            String searchstr = console.nextLine();
            if (searchstr.equals("exit")) {
                break;
            }

            long startTime = System.nanoTime();
            SearchIndex1(searchstr, i1);
            long endTime = System.nanoTime();
            long duration1 = (endTime - startTime); // i milis

            startTime = System.nanoTime();
            SearchIndex2(searchstr, i2);
            endTime = System.nanoTime();
            long duration2 = (endTime - startTime); // i milis

            startTime = System.nanoTime();
            i5.search(searchstr);
            endTime = System.nanoTime();
            long duration4 = (endTime - startTime); // i milis

            System.out.println("Index 1 Search Time: " + duration1);
            System.out.println("Index 2 Search Time: " + duration2);
            System.out.println("Index 4 Search Time: " + duration4);

            if (!OnlyPreprocessing) {
                startTime = System.nanoTime();
                PreProcessAndSearchIndex1(file_name, searchstr);
                endTime = System.nanoTime();
                duration1 = (endTime - startTime)/1000000; // i milis

                startTime = System.nanoTime();
                PreProcessAndSearchIndex2(file_name, searchstr);
                endTime = System.nanoTime();
                duration2 = (endTime - startTime)/1000000; // i milis

                startTime = System.nanoTime();
                PreProcessAndSearchIndex4(file_name, searchstr);
                endTime = System.nanoTime();
                duration4 = (endTime - startTime)/1000000; // i milis

                System.out.println("Index 1 Preprocess and Search time: " + duration1);
                System.out.println("Index 2 Preprocess and Search time: " + duration2);
                System.out.println("Index 4 Preprocess and Search time: " + duration4);
            }
        }
    }

    private static boolean OnlyPreprocessing = true;
    public static void main(String[] args) {
        Scanner console = new Scanner(System.in);
        String file_name = args[0];
        /*System.out.println("Started preprocessing Index4");
        Index4 i4 = new Index4(file_name);
        System.out.println("Started preprocessing Index4");

         */

        Test(file_name, console);
        for (;;) {
            System.out.println("Input search string or type exit to stop");
            String searchstr = console.nextLine();
            if (searchstr.equals("exit")) {
                break;
            }
            //SearchIndex4(searchstr, i4);
        }

        console.close();
    }
}