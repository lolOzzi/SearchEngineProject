import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;


class Index3 {

    WikiItem start;
    private static String END = "---END.OF.DOCUMENT---";

    public class WikiDoc {
        String header;
        WikiDoc next;

        WikiDoc(String header, WikiDoc next){
            this.header = header;
            this.next = next;
        }
    }

    private class WikiItem {
        String str;
        WikiItem next;
        WikiDoc docsIn;

        WikiItem(String s, WikiItem n) {
            str = s;
            next = n;
            docsIn = null;
        }

        public void AddDoc(WikiDoc doc) {
            doc.next = docsIn;
            docsIn = doc;
        }
    }

    public Index3(String filename) {
        String word;
        WikiItem current, tmp;
        try {
            Scanner input = new Scanner(new File(filename), "UTF-8");
            word = input.next();
            start = new WikiItem(word, null);
            current = start;
            String doc_name = word;
            WikiDoc doc = new WikiDoc(doc_name, null);
            boolean take_next = false;
            while (input.hasNext()) {   // Read all words in input

                take_next = word.equals(END);
                word = input.next();
                if (take_next){
                    doc_name = word;
                    doc = new WikiDoc(doc_name, null);
                }

                //System.out.println(word);
                tmp = GetWikiItem(word);
                if (tmp == null){
                    tmp = new WikiItem(word, null);
                    current.next = tmp;
                    current = tmp;
                }
                tmp.AddDoc(doc);

            }
            input.close();
        } catch (FileNotFoundException e) {
            System.out.println("Error reading file " + filename);
        }
    }

    private WikiItem GetWikiItem(String word){
        WikiItem item = start;
        while (item != null){
            if (item.str.equals(word)) return item;

            item = item.next;
        }
        return item;
    }

    public WikiDoc search(String searchstr) {
        WikiItem current = start;
        while (current != null) {
            if (current.str.equals(searchstr)) {
                return current.docsIn;
            }
            current = current.next;
        }
        return null;
    }

    public static void main(String[] args) {
        System.out.println("Preprocessing " + args[0]);
        Index3 i = new Index3(args[0]);
        Scanner console = new Scanner(System.in);
        for (;;) {
            System.out.println("Input search string or type exit to stop");
            String searchstr = console.nextLine();
            if (searchstr.equals("exit")) {
                break;
            }
            WikiDoc res = i.search(searchstr);
            if (res != null) {
                System.out.println(searchstr + " exists");
                while (res != null){
                    System.out.println(res.header);
                    res = res.next;
                }
            } else {
                System.out.println(searchstr + " does not exist");
            }
        }
        console.close();
    }
}