import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;


class Index2 {

    WikiItem start;
    private static String END = "---END.OF.DOCUMENT---";

    private class WikiItem {
        String str;
        WikiItem next;

        WikiItem(String s, WikiItem n) {
            str = s;
            next = n;
        }
    }

    public Index2(String filename) {
        String word;
        WikiItem current, tmp;
        try {
            Scanner input = new Scanner(new File(filename), "UTF-8");
            word = input.next();
            start = new WikiItem(word, null);
            current = start;
            while (input.hasNext()) {   // Read all words in input
                word = input.next();
                //System.out.println(word);
                tmp = new WikiItem(word, null);
                if (word.toString().equals('\n')){
                    System.out.println(word);
                }
                current.next = tmp;
                current = tmp;
            }
            input.close();
        } catch (FileNotFoundException e) {
            System.out.println("Error reading file " + filename);
        }
    }

    public List<String> search(String searchstr) {
        WikiItem current = start;
        String doc_header = start.str;
        List<String> headers = new ArrayList<>();
        while (current != null) {
            if (current.str.equals(searchstr)) {
                if (!headers.contains(doc_header)){
                    headers.add(doc_header);
                }
            }
            if (current.str.equals(END)){
                if (current.next == null) break;
                doc_header = current.next.str;
            }
            current = current.next;
        }
        return headers;
    }
}