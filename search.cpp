#include "include/search.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

using namespace std;

string cleanToken(const string& token) {
  string cleaned = "";
  bool hasLetter = false;

  for (char c : token) {
    if (isalpha(static_cast<unsigned char>(c))) {
      hasLetter = true;
    }
    cleaned += static_cast<char>(tolower(static_cast<unsigned char>(c)));
  }

  if (!hasLetter) {
    return "";
  }

  while (!cleaned.empty() && ispunct(static_cast<unsigned char>(cleaned.front()))) {
    cleaned.erase(0, 1);
  }

  while (!cleaned.empty() && ispunct(static_cast<unsigned char>(cleaned.back()))) {
    cleaned.pop_back();
  }

  return cleaned;
}

set<string> gatherTokens(const string& text) {
  set<string> uniqueTokens;
  stringstream ss(text);
  string rawToken;

  while (ss >> rawToken) {
    string cleaned = cleanToken(rawToken);

    if (!cleaned.empty()) {
      uniqueTokens.insert(cleaned);
    }
  }

  return uniqueTokens;
}

int buildIndex(const string& filename, map<string, set<string>>& index) {
  ifstream infile(filename);

  if (!infile.is_open()) {
    return 0;
  }

  string url;
  string content;
  int pagesProcessed = 0;

  while (getline(infile, url)) {
    if (getline(infile, content)) {
      set<string> tokens = gatherTokens(content);

      for (const string& token : tokens) {
        index[token].insert(url);
      }

      pagesProcessed++;
    }
  }

  return pagesProcessed;
}

set<string> findQueryMatches(const map<string, set<string>>& index,
                             const string& sentence) {
  set<string> result;
  stringstream ss(sentence);
  string term;
  bool firstTerm = true;

  while (ss >> term) {
    char modifier = '\0';
    string word = term;

    if (!firstTerm && (term[0] == '+' || term[0] == '-')) {
      modifier = term[0];
      word = term.substr(1);
    }

    string cleanedWord = cleanToken(word);
    set<string> currentMatches;
    if (index.count(cleanedWord)) {
      currentMatches = index.at(cleanedWord);
    }

    if (firstTerm) {
      result = currentMatches;
      firstTerm = false;
    } else {
      set<string> temp;
      if (modifier == '+') {
        set_intersection(result.begin(), result.end(), currentMatches.begin(), currentMatches.end(), inserter(temp, temp.begin()));
      } else if (modifier == '-') {
        set_difference(result.begin(), result.end(), currentMatches.begin(), currentMatches.end(), inserter(temp, temp.begin()));
      } else {
        set_union(result.begin(), result.end(), currentMatches.begin(), currentMatches.end(), inserter(temp, temp.begin()));
      }
      result = temp;
    }
  }
  return result;
}

void searchEngine(const string& filename) {
  ifstream infile(filename);
  if (!infile.is_open()) {
    cout << "Invalid filename." << endl;
  }
  map<string, set<string>> index;
  cout << "Stand by while building index..." << endl;
  int pagesProcessed = buildIndex(filename, index);
  cout << "Indexed " << pagesProcessed << " pages containing " << index.size() << " unique terms" << endl << endl; 

  string query;
  while (true) {
    cout << "Enter query sentence (press enter to quit): ";
    getline(cin, query);
    
    if (query.empty()) {
      break;
    }
    
    set<string> matches = findQueryMatches(index, query); 
    cout << "Found " << matches.size() << " matching pages" << endl;
    
    for (const string& url : matches) {
      cout << url << endl;
    }
    cout << endl;
  }
  cout << "Thank you for searching!" << endl;
}
