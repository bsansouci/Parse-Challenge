#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <regex>

using namespace std;

// Read below to have comments on what those functions are doing and how
// they're doing it
char* decrypt(string input, char* dictionary);
char* stableMatching(vector<string> *todo, vector<string> *done, char sTod[256], char dTos[256]);
vector<string> split(const string &s, char delim);
vector<string> getSameLength(string word);

// Global dictionary for simplicity sake
vector<string> dictionary;

int main(int argc, char const *argv[])
{
  string buffer, secret = "//secret";
  vector<string> secrets;
  vector<string> results;

  ifstream file;
  if(argc > 1)
    file.open (argv[1], ifstream::in);
  else {
    cout << "Please call with the filename as an argument" << endl;
    exit(1);
  }

  getline(file, buffer);
  while(buffer.compare(secret) != 0) {
    dictionary.push_back(buffer);
    getline(file, buffer);
  }

  while(getline(file, buffer)) {
    secrets.push_back(buffer);
  }

  file.close();
  vector<string> decrypted;
  for (vector<string>::iterator it = secrets.begin() ; it != secrets.end(); ++it) {
    string line = *it;
    vector<string> todo = split(line, ' ');
    vector<string> done;
    char* sTod = (char *)malloc(256);
    memset(sTod, 0, 256);

    char* dTos = (char *)malloc(256);
    memset(dTos, 0, 256);

    char *result = stableMatching(&todo, &done, sTod, dTos);
    if(result == NULL) {
      cout << "Couldn't find a proper matching for " << line << endl;
      results.push_back(string("no matching found"));
    } else {
      // Now we simply go through the set of words and derypt them using the
      // mapping that we just got
      string decryptedLine;
      for (vector<string>::iterator it = done.begin() ; it != done.end(); ++it) {
        decryptedLine = decrypt(*it, result) + string(" ") + decryptedLine;
      }
      results.push_back(decryptedLine);
    }
  }

  int i = 0;
  for (vector<string>::iterator it = secrets.begin() ; it != secrets.end(); ++it) {
    cout << *it << " = " << results.at(i) << endl;
    i++;
  }


  return 0;
}


// Ok so this is where the magic happens
// Using the callstack, we're building a tree: each recursive call corresponds
// to going deeper within the tree and each time we loop inside the foor loop
// corresponds to looking at the neighbour of the current node

// So the idea is that we're going to perform depth first search on this tree
// but go back up if none of the neighbours give a correct matching using the
// current mapping (mapping from one char to another char, the first from the
// dictionary, the second one from the secrets).
// For example if dict: "bob" and secret: "121" then the mapping will be
// b -> 1 <==> dTos[(int)b] = 1
// 0 -> 2 <==> dTos[(int)o] = 2
// and
// 1 -> b <==> sTod[1] = b
// 2 -> o <==> sTod[2] = o
// Using this we can check for collisions and have a real 1 to 1 mapping
char* stableMatching(vector<string> *todo, vector<string> *done, char* dTos, char* sTod) {
  // First, always start by the recursive base case, here is when there are
  // no todos
  if(todo->size() == 0)
    return sTod;

  // Now we get the first (here last) word from the secret list (or the list
  // of encrypted words)
  string word = todo->back();
  todo->pop_back();
  done->push_back(word);
  // Here we find the words that we might be intereted in
  vector<string> curPossibleMatches = getSameLength(word);
  // Do some copying, in case we don't want to keep the changes that we made
  // Also we don't want out changes to be reflected on the previous call
  char* tmp1 = (char *)malloc(256);
  memset(tmp1, 0, 256);
  memcpy(tmp1, dTos, 256);

  char* tmp2 = (char *)malloc(256);
  memset(tmp2, 0, 256);
  memcpy(tmp2, sTod, 256);
  // Know loop through all the neighbours
  while(curPossibleMatches.size() > 0) {
    string match = curPossibleMatches.back();
    bool matched = true;
    // Now we simply compare this neighbour with the current word using
    // the current mapping
    uint len = word.length();
    for (uint i = 0; i < len; ++i) {
      int potentialChar = (int) match.at(i);
      int realChar = (int) word.at(i);
      // If one character is already mapped to another one, and this other
      // one is different from the current one, then this word won't work
      if((tmp1[potentialChar] != 0 && tmp1[potentialChar] != realChar) || (tmp2[realChar] != 0 && tmp2[realChar] != potentialChar)) {
        matched = false;
        break;
      }
      tmp1[potentialChar] = realChar;
      tmp2[realChar] = potentialChar;
    }

    if(matched) {
      // Now we check all the children of this node to see if maybe this
      // mapping will work. If yes then the function stable matchin will
      // return something not null
      char *result = stableMatching(todo, done, tmp1, tmp2);

      if(result != NULL) {
        return result;
      }
    }
    // We remove the changes previously made
    memcpy(tmp1, dTos, 256);
    memcpy(tmp2, sTod, 256);
    curPossibleMatches.pop_back();
  }

  // If we reached here in our tree that means that we tried neighbour, and
  // none work, so we need to go back up and try this nodes' parents'
  // siblings. We also have to remember to push the word back into the listS
  todo->push_back(word);
  done->pop_back();
  return NULL;
}

// Some helper function to slipt a string using a delim
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

// This just compares the words length with the length of the words in the
// gobal dictionary of words and returns a subsection of them that are the
// same length
vector<string> getSameLength(string word) {
  vector<string> result;
  uint len = word.length();
  for (vector<string>::iterator it = dictionary.begin() ; it != dictionary.end(); ++it) {
    if(it->length() == len) {
      result.push_back(*it);
    }
  }
  return result;
}

// Decrypt simply returns what the mapping gives, when applied onto an
// encrypted word
char* decrypt(string input, char* mapping) {
  if(mapping == NULL)
    return NULL;

  uint len = input.length(), i = 0;
  char* result = (char *)malloc(len + 1);
  memset(result, 0, len + 1);
  for (i = 0; i < len; ++i) {
    result[i] = mapping[((int)input.at(i))];
  }

  return result;
}
