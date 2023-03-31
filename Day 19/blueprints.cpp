#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <utility>
#include <queue>

struct Blueprint {
  // given
  int id;
  int oreRobotOreCost;
  int clayRobotOreCost;
  int obsidianRobotOreCost;
  int obsidianRobotClayCost;
  int geodeRobotOreCost;
  int geodeRobotObsidianCost;
  // calculated
  int maxOreRobots;
};

struct State {
  int rounds;
  int ore;
  int clay;
  int obsidian;
  int geodes;
  int oreRobots;
  int clayRobots;
  int obsidianRobots;
  int geodeRobots;
};

bool operator<(const State& lhs, const State& rhs) {
  if (lhs.rounds < rhs.rounds) return true;
  if (lhs.rounds > rhs.rounds) return false;
  
  if (lhs.ore < rhs.ore) return true;
  if (lhs.ore > rhs.ore) return false;
  
  if (lhs.clay < rhs.clay) return true;
  if (lhs.clay > rhs.clay) return false;
  
  if (lhs.obsidian < rhs.obsidian) return true;
  if (lhs.obsidian > rhs.obsidian) return false;
  
  if (lhs.geodes < rhs.geodes) return true;
  if (lhs.geodes > rhs.geodes) return false;
  
  if (lhs.oreRobots < rhs.oreRobots) return true;
  if (lhs.oreRobots > rhs.oreRobots) return false;
  
  if (lhs.clayRobots < rhs.clayRobots) return true;
  if (lhs.clayRobots > rhs.clayRobots) return false;
  
  if (lhs.obsidianRobots < rhs.obsidianRobots) return true;
  if (lhs.obsidianRobots > rhs.obsidianRobots) return false;
  
  if (lhs.geodeRobots < rhs.geodeRobots) return true;
  return false;
}

int ceilDivide(int x, int y) {
  if (y == 1) return x;
  return (x + y - 1) / y;
}

int evaluateBestStrategy(const Blueprint &bp, const State &state) {
  if (state.rounds == 0) {
    // done
    return state.geodes;
  }
  if (state.rounds == 1) {
    // can't build anything meaningful
    return state.geodes + state.geodeRobots;
  }
  // default case, nothing else is built
  int bestGeodes = 0;
  if (state.geodeRobots > 0) {
    bestGeodes = state.geodes + state.rounds*state.geodeRobots;
  }
  if (state.obsidianRobots > 0) {
    // will be able to build a geode robot at some point, skip to that point if within time range
    int obsidianRounds; 
    if (state.obsidian >= bp.geodeRobotObsidianCost) {
      obsidianRounds = 0;
    }
    else {
      obsidianRounds = ceilDivide(bp.geodeRobotObsidianCost - state.obsidian, state.obsidianRobots);
    }
    int oreRounds; 
    if (state.ore >= bp.geodeRobotOreCost) {
      oreRounds = 0;
    }
    else {
      oreRounds = ceilDivide(bp.geodeRobotOreCost - state.ore, state.oreRobots);
    }
    int roundsNeeded = std::max(oreRounds, obsidianRounds) + 1;
    if (roundsNeeded < state.rounds) {
      // skip to having built the geode robot
      State newState = state;
      newState.rounds -= roundsNeeded;
      newState.ore += state.oreRobots*roundsNeeded - bp.geodeRobotOreCost;
      newState.clay += state.clayRobots*roundsNeeded;
      newState.obsidian += state.obsidianRobots*roundsNeeded - bp.geodeRobotObsidianCost;
      if (state.geodeRobots > 0) {
        newState.geodes += state.geodeRobots*roundsNeeded;
      }
      newState.geodeRobots++;
      bestGeodes = evaluateBestStrategy(bp, newState);
    }
  }
  // only try to build something other than a geode robot if our rates can't maintain geode robot production
  if (state.oreRobots < bp.geodeRobotOreCost || state.obsidianRobots < bp.geodeRobotObsidianCost) {
    if (state.clayRobots > 0 && state.obsidianRobots < bp.geodeRobotObsidianCost) {
      // will be able to build an obsidian robot at some point, skip to that point if within time range
      int clayRounds; 
      if (state.clay >= bp.obsidianRobotClayCost) {
        clayRounds = 0;
      }
      else {
        clayRounds = ceilDivide(bp.obsidianRobotClayCost - state.clay, state.clayRobots);
      }
      int oreRounds;
      if (state.ore >= bp.obsidianRobotOreCost) {
        oreRounds = 0;
      }
      else {
        oreRounds = ceilDivide(bp.obsidianRobotOreCost - state.ore, state.oreRobots);
      }
      int roundsNeeded = std::max(oreRounds, clayRounds) + 1;
      if (roundsNeeded < state.rounds) {
        // skip to having built the obsidian robot
        State newState = state;
        newState.rounds -= roundsNeeded;
        newState.ore += state.oreRobots*roundsNeeded - bp.obsidianRobotOreCost;
        newState.clay += state.clayRobots*roundsNeeded - bp.obsidianRobotClayCost;
        if (state.obsidianRobots > 0) {
          newState.obsidian += state.obsidianRobots*roundsNeeded;
          if (state.geodeRobots > 0) {
            newState.geodes += state.geodeRobots*roundsNeeded;
          }
        }
        newState.obsidianRobots++;
        int geodes = evaluateBestStrategy(bp, newState);
        if (geodes > bestGeodes) {
          bestGeodes = geodes;
        }
      }
    }
    if (state.oreRobots > 0 && state.clayRobots < bp.obsidianRobotClayCost) {
      // will be able to build a clay robot at some point, skip to that point if within time range
      int roundsNeeded;
      if (state.ore >= bp.clayRobotOreCost) {
        roundsNeeded = 1;
      }
      else {
        roundsNeeded = ceilDivide(bp.clayRobotOreCost - state.ore, state.oreRobots) + 1;
      }
      if (roundsNeeded < state.rounds) {
        // skip to having built the clay robot
        State newState = state;
        newState.rounds -= roundsNeeded;
        newState.ore += state.oreRobots*roundsNeeded - bp.clayRobotOreCost;
        if (state.clayRobots > 0) {
          newState.clay += state.clayRobots*roundsNeeded;
          if (state.obsidianRobots > 0) {
            newState.obsidian += state.obsidianRobots*roundsNeeded;
            if (state.geodeRobots > 0) {
              newState.geodes += state.geodeRobots*roundsNeeded;
            }
          }
        }
        newState.clayRobots++;
        int geodes = evaluateBestStrategy(bp, newState);
        if (geodes > bestGeodes) {
          bestGeodes = geodes;
        }
      }
    }
    if (state.oreRobots < bp.maxOreRobots) {
      // will be able to build an ore robot at some point, skip to that point if within time range
      int roundsNeeded;
      if (state.ore >= bp.oreRobotOreCost) {
        roundsNeeded = 1;
      }
      else {
        roundsNeeded = ceilDivide(bp.oreRobotOreCost - state.ore, state.oreRobots) + 1;
      }
      if (roundsNeeded < state.rounds) {
        // skip to having built the ore robot
        State newState = state;
        newState.rounds -= roundsNeeded;
        newState.ore += state.oreRobots*roundsNeeded - bp.oreRobotOreCost;
        if (state.clayRobots > 0) {
          newState.clay += state.clayRobots*roundsNeeded;
          if (state.obsidianRobots > 0) {
            newState.obsidian += state.obsidianRobots*roundsNeeded;
            if (state.geodeRobots > 0) {
              newState.geodes += state.geodeRobots*roundsNeeded;
            }
          }
        }
        newState.oreRobots++;
        int geodes = evaluateBestStrategy(bp, newState);
        if (geodes > bestGeodes) {
          bestGeodes = geodes;
        }
      }
    }
  }
  return bestGeodes;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: blueprints input_file rounds" << std::endl;
    exit(1);
  }
  std::ifstream fs(argv[1]);
  int roundCount = atoi(argv[2]);
  std::vector<Blueprint> blueprints;
  while (!fs.eof() && !fs.fail()) {
    std::string str;
    Blueprint bp;
    fs.seekg(10, std::ios_base::cur);
    fs >> bp.id;
    if (fs.eof()) break;
    fs.seekg(23, std::ios_base::cur);
    fs >> bp.oreRobotOreCost;
    fs.seekg(28, std::ios_base::cur);
    fs >> bp.clayRobotOreCost;
    fs.seekg(32, std::ios_base::cur);
    fs >> bp.obsidianRobotOreCost;
    fs.seekg(9, std::ios_base::cur);
    fs >> bp.obsidianRobotClayCost;
    fs.seekg(30, std::ios_base::cur);
    fs >> bp.geodeRobotOreCost;
    fs.seekg(9, std::ios_base::cur);
    fs >> bp.geodeRobotObsidianCost;
    fs.seekg(11, std::ios_base::cur);
    bp.maxOreRobots = std::max(bp.oreRobotOreCost, bp.clayRobotOreCost);
    bp.maxOreRobots = std::max(bp.maxOreRobots, bp.obsidianRobotOreCost);
    bp.maxOreRobots = std::max(bp.maxOreRobots, bp.geodeRobotOreCost);
    blueprints.push_back(bp);
  }
  std::cout << "Loaded " << blueprints.size() << " blueprints." << std::endl;
  State startState;
  startState.rounds = roundCount;
  startState.ore = 0;
  startState.clay = 0;
  startState.obsidian = 0;
  startState.geodes = 0;
  startState.oreRobots = 1;
  startState.clayRobots = 0;
  startState.obsidianRobots = 0;
  startState.geodeRobots = 0;
  int totalQuality = 0;
  for (auto it = blueprints.begin(); it != blueprints.end(); it++) {
    int best = evaluateBestStrategy(*it, startState);
    std::cout << "Best for blueprint " << it->id << " is " << best << std::endl;
    totalQuality += best*it->id;
  }
  std::cout << "Total quality " << totalQuality << std::endl;
  return 0;
}