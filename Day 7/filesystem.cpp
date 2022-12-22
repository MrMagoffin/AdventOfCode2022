#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

const int MAX_SMALL = 100000;
const int FREE_NEEDED = 30000000;
const int FULL_SIZE = 70000000;

class FilesystemEntry {
private:
  FilesystemEntry * parent;
  bool isDir;
  int size;
  std::string name;
  
public:
  std::unordered_map<std::string, FilesystemEntry *> children;

  FilesystemEntry(std::string _name, FilesystemEntry * _parent = nullptr, bool _isDir = true, int _size = 0) {
    name = _name;
    parent = _parent;
    isDir = _isDir;
    size = _size;
  }
  
  ~FilesystemEntry() {
    for (auto it = children.begin(); it != children.end(); it++) {
      delete it->second;
    }
  }
  
  bool isDirectory() {
    return isDir;
  }
  
  int getSize() {
    if (isDir && size == 0) {
      for (auto it = children.begin(); it != children.end(); it++) {
        size += it->second->getSize();
      }
    }
    return size;
  }
  
  std::string getName() {
    return name;
  }
  
  FilesystemEntry * getChild(std::string childName) {
    try {
      return children.at(childName);
    }
    catch (std::out_of_range e) {
      return nullptr;
    }
  }
  
  FilesystemEntry * addFile(std::string childName, int _size) {
    FilesystemEntry * child = new FilesystemEntry(childName, this, false, _size);
    children[childName] = child;
    return child;
  }
  
  FilesystemEntry * addDirectory(std::string childName) {
    FilesystemEntry * child = new FilesystemEntry(childName, this, true);
    children[childName] = child;
    return child;
  }
  
  FilesystemEntry * getParent() {
    return parent;
  }
  
  int recursiveSum() {
    int sum = 0;
    if (getSize() < MAX_SMALL) {
      sum += size;
    }
    for (auto it = children.begin(); it != children.end(); it++) {
      if (it->second->isDirectory()) {
        sum += it->second->recursiveSum();
      }
    }
    return sum;
  }
  
  std::pair<int, FilesystemEntry *> recursiveBest(int needed) {
    if (size < needed) {
      return std::make_pair<int, FilesystemEntry *>(-1, nullptr);
    }
    std::pair<int, FilesystemEntry *> best(size, this);
    for (auto it = children.begin(); it != children.end(); it++) {
      if (it->second->isDirectory()) {
        std::pair<int, FilesystemEntry *> compareVal = it->second->recursiveBest(needed);
        if (compareVal.first != -1 && compareVal.first < best.first) {
          best = compareVal;
        }          
      }
    }
    return best;
  }
};



int main(int argc, char* argv[]) {
  std::fstream fs("filesystem.txt", std::fstream::in);
  std::string parseString;
  
  FilesystemEntry * root = new FilesystemEntry("");
  FilesystemEntry * cur = root;
  
  fs >> parseString;
  if (parseString != "$") {
    std::cout << "File does not start with \"$\"!" << std::endl;
    exit(1);
  }
  
  while (!fs.eof()) {
    fs >> parseString;
    if (parseString == "ls") {
      // directory listing
      while (!fs.eof()) {
        fs >> parseString;
        if (fs.eof()) {
          break;
        }
        if (parseString == "dir") {
          // directory
          fs >> parseString;
          FilesystemEntry * child = cur->getChild(parseString);
          if (child == nullptr) {
            cur->addDirectory(parseString);
          }
        }
        else if (parseString == "$") {
          // done
          break;
        }
        else {
          // should be a number indicating a size
          int size;
          try {
            size = std::stoi(parseString);
          }
          catch (std::logic_error e) {
            std::cout << "Unexpected " << parseString << " in directory listing" << std::endl;
            exit(1);
          }
          // followed by name of file
          fs >> parseString;
          FilesystemEntry * child = cur->getChild(parseString);
          if (child == nullptr) {
            cur->addFile(parseString, size);
          }
        }
      }
    }
    else if (parseString == "cd") {
      // change directory
      fs >> parseString;
      if (parseString == "/") {
        // changing to root directory
        cur = root;
      }
      else if (parseString == ".") {
        // unchanged directory
      }
      else if (parseString == "..") {
        // parent directory, if possible
        FilesystemEntry * parent = cur->getParent();
        if (parent != nullptr) {
          cur = parent;
        }
      }
      else {
        // changing to named directory
        FilesystemEntry * child = cur->getChild(parseString);
        if (child == nullptr) {
          // create new directory
          cur = cur->addDirectory(parseString);
        }
        else {
          cur = child;
        }
      }
      fs >> parseString;
      if (parseString != "$") {
        std::cout << "Expected \"$\", got " << parseString << std::endl;
      }
    }
    else {
      std::cout << "Unrecognized command " << parseString << std::endl;
      exit(1);
    }
  }
  
  // step through completed tree and check sizes
  std::cout << "Sum of big directories is " << root->recursiveSum() << std::endl;
  std::pair<int, FilesystemEntry *> best = root->recursiveBest(root->getSize()+FREE_NEEDED-FULL_SIZE);
  std::string bestDir;
  cur = best.second;
  while (cur != nullptr) {
    bestDir = cur->getName() + "/" + bestDir;
    cur = cur->getParent();
  }
  std::cout << "Best directory " << bestDir << " has size " << best.first << std::endl;
  
  // clean up
  delete root;
  
  return 0;
}