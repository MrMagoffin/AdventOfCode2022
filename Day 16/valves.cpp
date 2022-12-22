#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <cstring>

const char* START_ROOM = "AA";

class Room {
private:
  std::string name;
public:
  int flowRate;
  bool visited;
  std::vector<Room *> linkedNodes;
  std::vector<std::pair<int, Room *>> weightedEdges;
  Room(std::string name) : name(name) { }
  // default destructor, not responsible for managing other nodes' memory
  std::string getName() {
    return name;
  }
};

int recursiveMinDistance(Room* start, Room* end, int depth, int& maxDepth) {
  if (start == end) {
    if (depth < maxDepth) {
      maxDepth = depth;
    }
    return 0;
  }
  else if (depth > maxDepth) {
    return depth;
  }
  else {
    int minDist = maxDepth;
    for (auto it = start->linkedNodes.begin(); it != start->linkedNodes.end(); it++) {
      if ((*it)->visited == false) {
        (*it)->visited = true;
        int dist = recursiveMinDistance(*it, end, depth+1, maxDepth);
        (*it)->visited = false;
        if (dist < minDist) {
          minDist = dist;
        }
      }
    }
    return minDist+1;
  }
}

std::pair<int, std::string> bestPathFlow(Room* room, int remainingRounds) {
  int best = 0;
  std::string bestName = "";
  for (auto it = room->weightedEdges.begin(); it != room->weightedEdges.end(); it++) {
    if (it->second->visited == false && it->first < remainingRounds) {
      // can get to this before we're out of rounds
      it->second->visited = true;
      std::pair<int, std::string> compBest = bestPathFlow(it->second, remainingRounds-1-it->first);
      it->second->visited = false;
      if (compBest.first > best) {
        best = compBest.first;
        bestName = it->second->getName() + std::to_string(best) + compBest.second;
      }
    }
  }
  std::pair<int, std::string> returnVal;
  returnVal.first = remainingRounds*room->flowRate + best;
  returnVal.second = bestName;
  return returnVal;
}

std::pair<int, std::string> bestElephantPathFlow(Room* humanRoom, int remainingHumanRounds, Room* elephantRoom, int remainingElephantRounds) {
  int best = 0;
  std::string bestName = "";
  bool foundValidPair = false;
  // choose new human room and new elephant room
  for (auto it = humanRoom->weightedEdges.begin(); it != humanRoom->weightedEdges.end(); it++) {
    if (it->second->visited == false && it->first < remainingHumanRounds) {
      // can get to this before we're out of rounds
      it->second->visited = true;
      for (auto jt = elephantRoom->weightedEdges.begin(); jt != elephantRoom->weightedEdges.end(); jt++) {
        if (jt->second->visited == false && jt->first < remainingElephantRounds) {
          // can get to this before the elephant is out of rounds
          foundValidPair = true;
          jt->second->visited = true;
          std::pair<int, std::string> compBest = bestElephantPathFlow(it->second, remainingHumanRounds-1-it->first, jt->second, remainingElephantRounds-1-jt->first);
          compBest.first += (remainingHumanRounds-1-it->first)*it->second->flowRate;
          compBest.first += (remainingElephantRounds-1-jt->first)*jt->second->flowRate;
          jt->second->visited = false;
          if (compBest.first > best) {
            best = compBest.first;
            bestName = it->second->getName() + "H" + jt->second->getName() + "E" + std::to_string(best) + " " + compBest.second;
          }
        }
      }
      it->second->visited = false;
    }
  }
      
      
  // Couldn't find both a room for human and elephant at once
  if (!foundValidPair) {
    for (auto it = humanRoom->weightedEdges.begin(); it != humanRoom->weightedEdges.end(); it++) {
      if (it->second->visited == false && it->first < remainingHumanRounds) {
        // can get to this before we're out of rounds
        it->second->visited = true;
        std::pair<int, std::string> compBest = bestElephantPathFlow(it->second, remainingHumanRounds-1-it->first, elephantRoom, remainingElephantRounds);
        compBest.first += (remainingHumanRounds-1-it->first)*it->second->flowRate;
        it->second->visited = false;
        if (compBest.first > best) {
          best = compBest.first;
          bestName = it->second->getName() + "H" + std::to_string(best) + " " + compBest.second;
        }
      }
    }
    for (auto it = elephantRoom->weightedEdges.begin(); it != elephantRoom->weightedEdges.end(); it++) {
      if (it->second->visited == false && it->first < remainingElephantRounds) {
        // can get to this before the elephant is out of rounds
        it->second->visited = true;
        std::pair<int, std::string> compBest = bestElephantPathFlow(humanRoom, remainingHumanRounds, it->second, remainingElephantRounds-1-it->first);
        compBest.first += (remainingElephantRounds-1-it->first)*it->second->flowRate;
        it->second->visited = false;
        if (compBest.first > best) {
          best = compBest.first;
          bestName = it->second->getName() + "E" + std::to_string(best) + " " + compBest.second;
        }
      }
    }
  }
  std::pair<int, std::string> returnVal;
  returnVal.first = best;
  returnVal.second = bestName;
  return returnVal;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: valves input_file rounds" << std::endl;
    exit(1);
  }
  FILE * fp = fopen(argv[1], "r");
  int roundCount = atoi(argv[2]);
  int totalValves = 0;
  std::unordered_map<std::string, Room *> rooms;
  std::vector<Room *> valves;
  while (!feof(fp)) {
    char roomName[3];
    char c;
    Room * room;
    Room * linkRoom;
    // skip to room name
    fseek(fp, 6, SEEK_CUR);
    if (feof(fp)) {
      break;
    }
    fgets(roomName, sizeof roomName, fp);
    if (rooms.find(roomName) != rooms.end()) {
      room = rooms[roomName];
    }
    else {
      room = new Room(roomName);
      rooms[roomName] = room;
    }
    // skip to flow rate
    fseek(fp, 15, SEEK_CUR);
    fscanf(fp, "%d", &(room->flowRate));
    if (room->flowRate != 0 || strncmp(roomName, "AA", 2) == 0) {
      valves.push_back(room);
    }
    // skip to connections list
    fseek(fp, 23, SEEK_CUR);
    do {
      do {
        c = fgetc(fp);
      } while (!feof(fp) && c != ' ');
      fgets(roomName, sizeof roomName, fp);
      if (rooms.find(roomName) != rooms.end()) {
        linkRoom = rooms[roomName];
      }
      else {
        linkRoom = new Room(roomName);
        rooms[roomName] = linkRoom;
      }
      room->linkedNodes.push_back(linkRoom);
      c = fgetc(fp);
    } while (c == ',');
    while (!feof(fp) && c != '\n') {
      c = fgetc(fp);
    }
  }
  
  std::cout << valves.size() << " valves (including start) in " << rooms.size() << " rooms" << std::endl;
  // now simplify room graph into valve graph with weights between valves
  for (auto it = valves.begin(); it != valves.end(); it++) {
    // generate weighted edges
    for (auto jt = valves.begin(); jt != valves.end(); jt++) {
      if (*it == *jt) continue; // skip self
      // find minimum distance between *it and *jt
      std::pair<int, Room*> weightedEdge;
      int maxDepth = rooms.size();
      int minDist = recursiveMinDistance(*it, *jt, 0, maxDepth);
      weightedEdge.first = minDist;
      weightedEdge.second = *jt;
      (*it)->weightedEdges.push_back(weightedEdge);
    }
  }
  
  std::cout << "Generated simplified graph." << std::endl;
  
  // finally search for best path to visit all valves
  Room * curRoom = rooms[START_ROOM];
  std::pair<int, std::string> bestPath;
  curRoom->visited = true;
  bestPath = bestPathFlow(curRoom, roundCount);
  std::cout << "Best possible is " << bestPath.first << ", " << bestPath.second << std::endl;
  bestPath = bestElephantPathFlow(curRoom, roundCount-4, curRoom, roundCount-4);
  std::cout << "Best possible with elephant is " << bestPath.first << ", " << bestPath.second << std::endl;
  curRoom->visited = false;
  
  for (auto it = rooms.begin(); it != rooms.end(); it++) {
    delete it->second;
  }
  
  return 0;
}