import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;


class Index4 {
    private static String END = "---END.OF.DOCUMENT---";
    WikiItemMap map;

    public class WikiDoc {
        String header;
        WikiDoc next;

        WikiDoc(String header, WikiDoc next){
            this.header = header;
            this.next = next;
        }
    }

    private class WikiItemMap {
        private WikiItem[] Buckets;
        private int numBuckets;

        public WikiItemMap(int n){
            numBuckets = n;
            Buckets = new WikiItem[numBuckets];
            for (var bucket : Buckets){
                bucket = null;
            }
        }

        public WikiItem Get(String s){
            WikiItem item = Buckets[Hash(s)];

            while (item != null){
                if (item.str.equals(s)) {
                    return item;
                }
                item = item.next;
            }
            return item;
        }

        public void Add(WikiItem new_item) {

            //if (Get(new_item.str) != null) return;


            int index = Hash(new_item.str);

            new_item.next = Buckets[index];
            Buckets[index] = new_item;
        }

        public int Hash(String s){
            return Math.abs(s.hashCode()) % numBuckets;
        }
    }

    private class WikiItem {
        String str;
        WikiDoc docsIn;
        WikiItem next;


        WikiItem(String s, WikiItem n) {
            str = s;
            docsIn = null;
            next = n;
        }

        public void AddDoc(WikiDoc doc) {

            WikiDoc docco = new WikiDoc(doc.header, null);
            WikiDoc curr = docsIn;
            while (curr != null){
                if (curr.header.equals(docco.header)) {
                    return;
                }
                curr = curr.next;
            }
            docco.next = docsIn;
            docsIn = docco;
        }
    }

    public Index4(String filename) {
        String word;
        WikiItem current, tmp;
        map = new WikiItemMap(10_000_000);
        try {
            Scanner input = new Scanner(new File(filename), "UTF-8");
            word = input.next();
            current = new WikiItem(word, null);;
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
                tmp = map.Get(word);
                if (tmp == null){
                    tmp = new WikiItem(word, null);
                    current.next = tmp;
                    current = tmp;
                    map.Add(current);
                }
                else {
                    int i = 1;
                }
                tmp.AddDoc(doc);

            }
            input.close();
        } catch (FileNotFoundException e) {
            System.out.println("Error reading file " + filename);
        }
    }

    public WikiDoc search(String searchstr) {
        WikiItem item = map.Get(searchstr);
        if (item != null) {
            return item.docsIn;
        }
        return null;
    }

    public static void main(String[] args) {
        String filename = "src/data/WestburyLab.wikicorp.201004_100KB.txt";
        System.out.println("Preprocessing " + filename);
        Index4 i = new Index4(filename);
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
