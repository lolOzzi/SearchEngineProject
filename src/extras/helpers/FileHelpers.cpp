#include <string>
#include "../../core/interfaces.h"
#include <fstream>
#include <regex>


static std::string extractFileSection(std::string filename, long long start, long long end) {
  std::string contents;
  std::ifstream file;
  file.open (filename);
  if (!file.good()) {
    printf("Error reading file");
    return contents;
  }
  if (!file.is_open()) {
    printf("No file is open");
    return contents;
  }
  if (!file) return {};

  file.seekg(start);
  long long n = end - start;
  std::string out;
  out.resize(n);
  file.read(&out[0], n);
  return out;
}


const std::vector<std::string> patterns = {
  /* common non-ASCII spaces */
  "\xC2\xA0",         // NO-BREAK SPACE U+00A0
  "\xE3\x80\x80",     // IDEOGRAPHIC SPACE U+3000
  //"\xE2\x80\x82",     // EN SPACE U+2002
  "\xE2\x80\x83",     // EM SPACE U+2003
  "\xE2\x80\x84",     // THREE-PER-EM SPACE U+2004
  "\xE2\x80\x85",     // FOUR-PER-EM SPACE U+2005
  "\xE2\x80\x86",     // SIX-PER-EM SPACE U+2006
  "\xE2\x80\x87",     // FIGURE SPACE U+2007
  "\xE2\x80\x88",     // PUNCTUATION SPACE U+2008
  "\xE2\x80\x89",     // THIN SPACE U+2009
  "\xE2\x80\x8A",     // HAIR SPACE U+200A
  "\xE2\x80\xAF",     // NARROW NO-BREAK SPACE U+202F
  "\xE2\x80\xA8",     // LINE SEPARATOR U+2028
  "\xE2\x80\xA9",     // PARAGRAPH SEPARATOR U+2029
  "\xE2\x80\x8B",     // ZERO WIDTH SPACE U+200B
  "\xE2\x80\x8C",     // ZERO WIDTH NON-JOINER U+200C
  "\xE2\x80\x8D",     // ZERO WIDTH JOINER U+200D
};

const std::vector<std::string> patterns_to_space = {
  /* dashes and hyphens */
  "\xE2\x80\x95",     // HORIZONTAL BAR U+2015
  "\xE2\x80\x94",     // EM DASH U+2014
  "\xE2\x80\x93",     // EN DASH U+2013
  "\xE2\x80\x92",     // FIGURE DASH U+2012
  "\xE2\x80\x90",     // HYPHEN U+2010
  "\xE2\x80\x91",     // NON-BREAKING HYPHEN U+2011
  "\x2D",             // HYPHEN-MINUS U+002D
  "\xE2\x88\x92",       // MINUS SIGN U+2212
  "\xCB\x97",           // MODIFIER LETTER MINUS SIGN U+02D7
  "\xE2\x81\xBB",       // SUPERSCRIPT MINUS U+207B
  "\xE2\x82\x8B",       // SUBSCRIPT MINUS U+208B
  // COLONS
  "\x3A",               // COLON U+003A
  "\xEF\xBC\x9A",       // FULLWIDTH COLON U+FF1A
  "\xEA\x9E\x89",       // MODIFIER LETTER COLON U+A789
  "\xD6\x89",           // ARMENIAN FULL STOP (looks like colon) U+0589
  // SEMICOLONS
  "\x3B",               // SEMICOLON U+003B
  "\xEF\xBC\x9B",       // FULLWIDTH SEMICOLON U+FF1B
  "\xCD\xBE",           // GREEK QUESTION MARK (semicolon form) U+037E
  // UNDERSCORES
  "\x5F",               // LOW LINE (underscore) U+005F
  "\xEF\xBC\xBF",       // FULLWIDTH LOW LINE U+FF3F
  "\xE2\x80\x97",       // DOUBLE LOW LINE U+2017
  "\xCC\xB2",           // COMBINING LOW LINE U+0332
  // SLASHES
  "\x2F",               // SOLIDUS (forward slash) U+002F
  "\x5C",               // REVERSE SOLIDUS (backslash) U+005C
  "\xE2\x81\x84",       // FRACTION SLASH U+2044
  "\xE2\x88\x95",       // DIVISION SLASH U+2215
  "\xEF\xBC\x8F",       // FULLWIDTH SOLIDUS U+FF0F
  "\xE2\xA7\xB8",        // BIG SOLIDUS U+29F8
  // QUESTION MARKS
  "\x3F",               // QUESTION MARK U+003F
  "\xEF\xBC\x9F",       // FULLWIDTH QUESTION MARK U+FF1F
  "\xE2\xB8\xAE",       // DOUBLE QUESTION MARK U+2E2E
  // EQUALS SIGNS
  "\x3D",               // EQUALS SIGN U+003D
  "\xEF\xBC\x9D",       // FULLWIDTH EQUALS SIGN U+FF1D
  "\xE2\x81\xBC",       // DOUBLE VERTICAL LINE EQUALS U+2A75
  // PERIODS (FULL STOPS)
  "\x2E",               // FULL STOP U+002E
  "\xEF\xBC\x8E",       // FULLWIDTH FULL STOP U+FF0E
  "\xE2\x80\xA4",       // ONE DOT LEADER U+2024
  "\xD6\x89",           // ARMENIAN FULL STOP U+0589
  // EXCLAMATION MARKS
  "\x21",               // EXCLAMATION MARK U+0021
  "\xEF\xBC\x81",       // FULLWIDTH EXCLAMATION MARK U+FF01
  "\xC7\x83",           // LATIN LETTER RETROFLEX CLICK (looks like !) U+01C3
  // TILDES
  "\x7E",               // TILDE U+007E
  "\xEF\xBD\x9E",       // FULLWIDTH TILDE U+FF5E
  "\xCB\x9C",           // SMALL TILDE U+02DC
  "\xE2\x88\xBC",       // TILDE OPERATOR U+223C
};


const std::vector<std::string> patterns_to_remove = {
// CURRENCY SYMBOLS
"\x24",                 // DOLLAR SIGN U+0024
"\xC2\xA2",             // CENT SIGN U+00A2
"\xC2\xA3",             // POUND SIGN U+00A3
"\xC2\xA4",             // CURRENCY SIGN U+00A4
"\xC2\xA5",             // YEN SIGN U+00A5
"\xE2\x82\xAC",         // EURO SIGN U+20AC
"\xE2\x82\xA9",         // WON SIGN U+20A9
"\xE2\x82\xB9",         // INDIAN RUPEE SIGN U+20B9
"\xE2\x82\xBD",         // RUBLE SIGN U+20BD
"\xE2\x82\xBA",         // TURKISH LIRA SIGN U+20BA
"\xE2\x82\xAB",         // DONG SIGN U+20AB
"\xE2\x82\xAA",         // NEW SHEQEL SIGN U+20AA
"\xE2\x82\xB1",         // PESO SIGN U+20B1
"\xE0\xB8\xBF",         // THAI BAHT SIGN U+0E3F
"\xE2\x82\xB4",         // HRYVNIA SIGN U+20B4
"\xE2\x82\xA6",         // NAIRA SIGN U+20A6
"\xE2\x82\xA1",         // COLON SIGN U+20A1
"\xE2\x82\xB5",         // CEDI SIGN U+20B5
"\xE2\x82\xB2",         // GUARANI SIGN U+20B2
"\xE2\x82\xB8",         // TENGE SIGN U+20B8
"\xE2\x82\xBC",         // MANAT SIGN U+20BC
"\xE2\x82\xBE",         // LARI SIGN U+20BE
// FULLWIDTH CURRENCY SYMBOLS
"\xEF\xBC\x84",         // FULLWIDTH DOLLAR SIGN U+FF04
"\xEF\xBF\xA0",         // FULLWIDTH CENT SIGN U+FFE0
"\xEF\xBF\xA1",         // FULLWIDTH POUND SIGN U+FFE1
"\xEF\xBF\xA4",         // FULLWIDTH EURO SIGN U+FFE4
"\xEF\xBF\xA5",         // FULLWIDTH YEN SIGN U+FFE5
"\xEF\xBF\xA6",         // FULLWIDTH WON SIGN U+FFE6
// AT SIGN
"\x40",                 // COMMERCIAL AT U+0040
"\xEF\xBC\xA0",         // FULLWIDTH COMMERCIAL AT U+FF20
// NUMBER SIGN
"\x23",                 // NUMBER SIGN U+0023
"\xEF\xBC\x83",         // FULLWIDTH NUMBER SIGN U+FF03
// PERCENT SIGNS
"\x25",                 // PERCENT SIGN U+0025
"\xEF\xBC\x85",         // FULLWIDTH PERCENT SIGN U+FF05
// AMPERSANDS
"\x26",                 // AMPERSAND U+0026
"\xEF\xBC\x86",         // FULLWIDTH AMPERSAND U+FF06
// SLASHES
"\x2F",                 // SOLIDUS (forward slash) U+002F
"\x5C",                 // REVERSE SOLIDUS (backslash) U+005C
"\xE2\x81\x84",         // FRACTION SLASH U+2044
"\xE2\x88\x95",         // DIVISION SLASH U+2215
"\xEF\xBC\x8F",         // FULLWIDTH SOLIDUS U+FF0F
// PARENTHESES
"\x28",                 // LEFT PARENTHESIS U+0028
"\x29",                 // RIGHT PARENTHESIS U+0029
"\xEF\xBC\x88",         // FULLWIDTH LEFT PARENTHESIS U+FF08
"\xEF\xBC\x89",         // FULLWIDTH RIGHT PARENTHESIS U+FF09
// SQUARE BRACKETS
"\x5B",                 // LEFT SQUARE BRACKET U+005B
"\x5D",                 // RIGHT SQUARE BRACKET U+005D
"\xEF\xBC\xBB",         // FULLWIDTH LEFT SQUARE BRACKET U+FF3B
"\xEF\xBC\xBD",         // FULLWIDTH RIGHT SQUARE BRACKET U+FF3D
// CURLY BRACES
"\x7B",                 // LEFT CURLY BRACKET U+007B
"\x7D",                 // RIGHT CURLY BRACKET U+007D
"\xEF\xBD\x9B",         // FULLWIDTH LEFT CURLY BRACKET U+FF5B
"\xEF\xBD\x9D",         // FULLWIDTH RIGHT CURLY BRACKET U+FF5D
// CARET
"\x5E",                 // CIRCUMFLEX ACCENT U+005E
"\xCB\x86",             // MODIFIER LETTER CIRCUMFLEX ACCENT U+02C6
"\xEF\xBC\xBE",         // FULLWIDTH CIRCUMFLEX ACCENT U+FF3E
// GRAVE ACCENT
"\x60",                 // GRAVE ACCENT U+0060
"\xCB\x8B",             // MODIFIER LETTER GRAVE ACCENT U+02CB
"\xEF\xBD\x80",         // FULLWIDTH GRAVE ACCENT U+FF40
// ACUTE ACCENT
"\xC2\xB4",             // ACUTE ACCENT U+00B4
// APOSTROPHES / QUOTES
"\x22",                 // QUOTATION MARK U+0022
"\x27",                 // APOSTROPHE U+0027
"\xE2\x80\x98",         // LEFT SINGLE QUOTATION MARK U+2018
"\xE2\x80\x99",         // RIGHT SINGLE QUOTATION MARK U+2019
"\xEF\xBC\x87"          // FULLWIDTH APOSTROPHE U+FF07
};

static void replace_any_simple(std::string &s, const std::vector<std::string> &given_patterns, const std::string& new_val) {
  for (const auto &pat : given_patterns) {
    if (pat.empty()) continue;
    std::size_t pos = 0;
    while ((pos = s.find(pat, pos)) != std::string::npos) {
      s.replace(pos, pat.size(), new_val);
      pos += 1;
    }
  }
}

static int isSpace(char32_t c) {
  return c == U' ' || c == U'\u3000' || c == U'\n' || c == U'\r' || c == U'\t';;
}

const std::string rep_white_val = " ";
static void getWordsFromLine(std::string &line, const std::vector<std::string> &given_patterns, std::vector<std::string> &words) {
  std::string tmp_word;

  replace_any_simple(line, given_patterns, rep_white_val);

  for (int i = 0; i < line.length(); ++i) {
    if (!isSpace(line[i]) ) {
      tmp_word += line[i];
    } else if (!tmp_word.empty()) {
      words.push_back(tmp_word);
      tmp_word = "";
    }
    if (!tmp_word.empty() && i == line.length()-1 ){
      words.push_back(tmp_word);
      tmp_word = "";
    }
  }
}


static void cleanString(const std::string &word, std::vector<std::string> &cleaned_words) {
  std::string lower_case_word = word;
  for (char &c : lower_case_word) {
    c = std::tolower(static_cast<unsigned char>(c));
  }
  getWordsFromLine(lower_case_word, patterns_to_space, cleaned_words);
}

static int getDocumentCountFromFile(const std::string &filename) {
  const std::string END = "---END.OF.DOCUMENT---";
  int counter = 0;
  std::ifstream file;
  file.open (filename);
  if (!file.good() || !file.is_open()) {
    printf("Error reading file");
    return counter;
  }
  std::string line;
  while (std::getline(file, line)) {
    if (line == END) counter++;
  }
  return counter;
}

static std::string trim_string(const std::string& word) {
  int skip_start = 0;
  for (int i = 0; i < word.size(); i++) {
    if (word[i] != ' ') {
      skip_start = i;
      break;
    }
  }
  int skip_end = 0;
  for (int i = word.size()-1; i > 0 ; i--) {
    if (word[i] != ' ') {
      skip_end = i+1;
      break;
    }
  }
  auto temp = word.substr(skip_start, skip_end);
  return temp;
}

void replaceAll(std::string* str, const std::string& from, const std::string& to) {
  auto&& pos = str->find(from, size_t{});
  while (pos != std::string::npos)
  {
    str->replace(pos, from.length(), to);
    pos = str->find(from, pos + to.length());
  }
}

const std::string empt = "";
static std::string remove_unwanted_and_trim(const std::string* word_pointer) {
  const std::string& word = *word_pointer;
  std::string clean_word = word;
  for (const std::string& pat : patterns_to_remove) {
    replaceAll(&clean_word, pat, empt);
  }
  return trim_string(clean_word);
}
