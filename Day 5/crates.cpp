#include <cstdlib>
#include <cstdio>
#include <deque>
#include <string>
#include <stdexcept>

int parseNumber(FILE * fp) {
  std::string numString;
  while (!feof(fp)) {
    char c = fgetc(fp);
    if (c >= '0' && c <= '9') {
      numString += c;
    }
    else {
      break;
    }
  }
  try {
    return std::stoi(numString);
  }
  catch (std::invalid_argument e) {
    return -1;
  }
}

void parseHeader(FILE * fp, std::deque<char>* stacks) {
  // return to start of file
  fseek(fp, 0, SEEK_SET);
  // parse header
  bool headerDone = false;
  int col = 0;
  while (!feof(fp) && !headerDone) {
    char c = fgetc(fp);
    if (c == '\n') {
      col = 0;
      if (headerDone) {
        break;
      }
    }
    else {
      col++;
      if (c >= 'A' && c <= 'Z') {
        printf("Putting %c in stack %d\n", c, col/4);
        stacks[col/4].push_back(c);
      }
      else if (c >= '0' && c <= '9') {
        headerDone = true;
      }
    }
  }
  
  // skip empty lines
  int skippedLines = 0;
  while (!feof(fp)) {
    char c = fgetc(fp);
    if (c == '\n') {
      skippedLines++;
      if (skippedLines >= 2) {
        break;
      }
    }
  }
}

void parseStacks(FILE * fp, std::deque<char>* stacks, bool partTwo = false) {
  // parse the meat
  while (!feof(fp)) {
    int count, origin, dest;
    fseek(fp, 5, SEEK_CUR);
    count = parseNumber(fp);
    if (feof(fp)) {
      break;
    }
    if (count == -1) {
      char c;
      printf("Next characters:\n");
      for (int i = 0; i < 10; i++) {
        c = fgetc(fp);
        putchar(c);
      }
      putchar('\n');
      exit(1);
    }
    fseek(fp, 5, SEEK_CUR);
    origin = parseNumber(fp);
    fseek(fp, 3, SEEK_CUR);
    dest = parseNumber(fp);
    printf("Moving %d from %d to %d\n", count, origin, dest);
    if (partTwo) {
      for (int i = 0; i < count; i++) {
        stacks[dest-1].push_front(stacks[origin-1][count-i-1]);
        stacks[origin-1].erase(stacks[origin-1].begin()+(count-i-1));
      }
    }
    else {
      for (int i = 0; i < count; i++) {
        stacks[dest-1].push_front(stacks[origin-1].front());
        stacks[origin-1].pop_front();
      }
    }
    fseek(fp, 1, SEEK_CUR);
  }
}

void outputResults(std::deque<char>* stacks, int stackCount) {
  // output results
  for (int i = 0; i < stackCount; i++) {
    if (stacks[i].size() > 0) {
      putchar(stacks[i].front());
    }
    else {
      putchar(' ');
    }
  }
  putchar('\n');
}

int main(int argc, char * argv[]) {
  FILE * fp;
  fp = fopen("crates.txt", "r");
  
  int stackCount = 0;
  // determine how many stacks there are
  while (!feof(fp)) {
    char c = fgetc(fp);
    if (c == '\n' || c == '\r') {
      break;
    }
    else {
      stackCount++;
    }
  }
  
  // initialize number of stacks
  stackCount = (stackCount+1)/4;
  printf("Found %d stacks\n", stackCount);
  std::deque<char>* stacks = new std::deque<char>[stackCount];
  
  parseHeader(fp, stacks);
  parseStacks(fp, stacks);
  printf("Part 1 output:");
  outputResults(stacks, stackCount);
  
  delete[] stacks;
  stacks = new std::deque<char>[stackCount];
  
  parseHeader(fp, stacks);
  parseStacks(fp, stacks, true);
  printf("Part 2 output:");
  outputResults(stacks, stackCount);
  
  delete[] stacks;
  return 0;
}