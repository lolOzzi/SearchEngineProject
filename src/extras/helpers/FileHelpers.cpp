#include "../../core/interfaces.h"
#include <fstream>

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


std::vector<std::string> patterns = {
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

  /* dashes and hyphens */
  "\xE2\x80\x94",     // EM DASH U+2014
  //"\xE2\x80\x93",     // EN DASH U+2013
  //"\xE2\x80\x92",     // FIGURE DASH U+2012
  "\xE2\x80\x90",     // HYPHEN U+2010
  "\xE2\x80\x91"      // NON-BREAKING HYPHEN U+2011
};


static void replace_any_simple(std::string &s, const std::vector<std::string> &patterns) {
  for (const auto &pat : patterns) {
    if (pat.empty()) continue;
    std::size_t pos = 0;
    while ((pos = s.find(pat, pos)) != std::string::npos) {
      s.replace(pos, pat.size(), " ");
      pos += 1;
    }
  }
}
static int isSpace(char32_t c) {
  return c == U' ' || c == U'\u3000' || c == U'\n' || c == U'\r' || c == U'\t';;
}

static void getWordsFromLine(std::string &line, std::vector<std::string> &words) {
  std::string tmp_word;
  replace_any_simple(line, patterns);

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