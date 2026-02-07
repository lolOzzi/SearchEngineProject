import java.io.*;
import java.util.*;

// Kan blive kørt måske nemmere ved

// java src/extras/java/Test.java

class Test {

    public static void main(String[] args) {

        String filename = "data/WestburyLab.wikicorp.201004_100MB.txt";
        String test_filepath = "src/components/test/test_100MB.txt";
        System.out.println("Preprocessing " + filename);
        Test test = new Test(filename, test_filepath);
    }

    HashMap<String, List<String>> holder;
    String end = "---END.OF.DOCUMENT---";

    public Test(String filename, String test_filepath) {
        String word = "";
        String document = "";
        boolean next_is_doc = true;
        holder = new HashMap<>();
        try {
            Scanner input = new Scanner(new File(filename), "UTF-8");
            while (input.hasNext()) {   // Read all words in input

                if (next_is_doc){
                    word = input.nextLine();
                    while (input.hasNext() && word.trim().equals("")){
                        word = input.nextLine();
                    }
                    char last = word.charAt(word.length() - 1);
                    if (last == ',' || last == '.' || last == '?'){
                        word = word.substring(0, word.length() - 1);
                    }
                    document = word;
                } else {
                    word = input.next();
                    char last = word.charAt(word.length() - 1);
                    if (last == ',' || last == '.' || last == '?'){
                        word = word.substring(0, word.length() - 1);
                    }
                }



                if (holder.containsKey(word)) {
                    var list = holder.get(word);
                    if (!list.get(list.size() - 1).equals(document)){
                        holder.get(word).add(document);
                    }
                } else {
                    holder.put(word, new ArrayList<String>(Arrays.asList(document)));
                }
                next_is_doc = word.equals(end);
            }
            input.close();

            PrintWriter writer = new PrintWriter(test_filepath, "UTF-8");
            String end_of_word = "---END.OF.WORD---";

            for (String key : holder.keySet()){
                writer.println(key);
                for (String val : holder.get(key)){
                    writer.println(val);
                }
                writer.println(end_of_word);
            }
        } catch (FileNotFoundException e) {
            System.out.println("Error reading file " + filename);
        } catch (UnsupportedEncodingException e) {
            throw new RuntimeException(e);
        }
    }
}