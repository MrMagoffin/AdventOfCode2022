// Rocks spawned, in order (repeating):
// Origin shown as O
// 0:
// O###
// 1: (origin not part of rock)
// .#.
// ###
// O#.
// 2:
// ..#
// ..#
// O##
// 3:
// #
// #
// #
// O
// 4:
// ##
// O#
//
// Coordinates are +x=right, +y=up

#define PLAYING_FIELD_WIDTH 7
#define PLAYING_FIELD_HEIGHT_CHUNK 10000

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>

struct Rock {
  int x;
  long long y;
  int rockType;
  int filePos;
  bool operator!=(const Rock& rhs) const {
    return (filePos != rhs.filePos) || (x != rhs.x) || (rockType != rhs.rockType);
  }
};

int filePos = 0;
bool nextDir(FILE * fp) {
  char dir = fgetc(fp);
  if (feof(fp)) {
    fseek(fp, 0, SEEK_SET);
    filePos = 0;
    dir = fgetc(fp);
  }
  filePos++;
  return (dir == '>');
}

inline bool& playingFieldLookup(bool* playingField, int x, long long y) {
  return playingField[PLAYING_FIELD_WIDTH*y + x];
}

bool* resizePlayingField(bool* oldPlayingField, long long oldHeight, long long newHeight) {
  bool* newPlayingField = new bool[PLAYING_FIELD_WIDTH*newHeight];
  memcpy(newPlayingField, oldPlayingField, PLAYING_FIELD_WIDTH*oldHeight*sizeof(bool));
  for (int i = PLAYING_FIELD_WIDTH*oldHeight; i < PLAYING_FIELD_WIDTH*newHeight; i++) {
    newPlayingField[i] = false;
  }
  delete[] oldPlayingField;
  return newPlayingField;
}

inline int rockHeight(int rockType) {
  switch (rockType) {
    case 0:
      return 1;
    case 1:
      return 3;
    case 2:
      return 3;
    case 3:
      return 4;
    case 4:
      return 2;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
      return false;
  }
}

bool rockCanFall(bool* playingField, int x, long long y, int rockType) {
  // check to see if there's space for this rock in the space immediately below it
  // only check new locations, not space the rock is already taking up
  if (y == 0) return false; // hit floor
  switch (rockType) {
    case 0:
      if (playingFieldLookup(playingField, x, y-1)) return false;
      if (playingFieldLookup(playingField, x+1, y-1)) return false;
      if (playingFieldLookup(playingField, x+2, y-1)) return false;
      if (playingFieldLookup(playingField, x+3, y-1)) return false;
      return true;
    case 1:
      if (playingFieldLookup(playingField, x+1, y-1)) return false;
      if (playingFieldLookup(playingField, x, y)) return false;
      if (playingFieldLookup(playingField, x+2, y)) return false;
      return true;
    case 2:
      if (playingFieldLookup(playingField, x, y-1)) return false;
      if (playingFieldLookup(playingField, x+1, y-1)) return false;
      if (playingFieldLookup(playingField, x+2, y-1)) return false;
      return true;
    case 3:
      if (playingFieldLookup(playingField, x, y-1)) return false;
      return true;
    case 4:
      if (playingFieldLookup(playingField, x, y-1)) return false;
      if (playingFieldLookup(playingField, x+1, y-1)) return false;
      return true;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
      return false;
  }
}

bool rockCanMoveLeft(bool* playingField, int x, long long y, int rockType) {
  // check to see if there's space for this rock in the space immediately to its left
  // only check new locations, not space the rock is already taking up
  if (x == 0) return false; // hit wall
  switch (rockType) {
    case 0:
      if (playingFieldLookup(playingField, x-1, y)) return false;
      return true;
    case 1:
      if (playingFieldLookup(playingField, x, y)) return false;
      if (playingFieldLookup(playingField, x-1, y+1)) return false;
      if (playingFieldLookup(playingField, x, y+2)) return false;
      return true;
    case 2:
      if (playingFieldLookup(playingField, x-1, y)) return false;
      if (playingFieldLookup(playingField, x+1, y+1)) return false;
      if (playingFieldLookup(playingField, x+1, y+2)) return false;
      return true;
    case 3:
      if (playingFieldLookup(playingField, x-1, y)) return false;
      if (playingFieldLookup(playingField, x-1, y+1)) return false;
      if (playingFieldLookup(playingField, x-1, y+2)) return false;
      if (playingFieldLookup(playingField, x-1, y+3)) return false;
      return true;
    case 4:
      if (playingFieldLookup(playingField, x-1, y)) return false;
      if (playingFieldLookup(playingField, x-1, y+1)) return false;
      return true;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
      return false;
  }
}

bool rockCanMoveRight(bool* playingField, int x, long long y, int rockType) {
  // check to see if there's space for this rock in the space immediately to its right
  // only check new locations, not space the rock is already taking up
  switch (rockType) {
    case 0:
      if (x == PLAYING_FIELD_WIDTH - 4) return false; // hit wall
      if (playingFieldLookup(playingField, x+4, y)) return false;
      return true;
    case 1:
      if (x == PLAYING_FIELD_WIDTH - 3) return false; // hit wall
      if (playingFieldLookup(playingField, x+2, y)) return false;
      if (playingFieldLookup(playingField, x+3, y+1)) return false;
      if (playingFieldLookup(playingField, x+2, y+2)) return false;
      return true;
    case 2:
      if (x == PLAYING_FIELD_WIDTH - 3) return false; // hit wall
      if (playingFieldLookup(playingField, x+3, y)) return false;
      if (playingFieldLookup(playingField, x+3, y+1)) return false;
      if (playingFieldLookup(playingField, x+3, y+2)) return false;
      return true;
    case 3:
      if (x == PLAYING_FIELD_WIDTH - 1) return false; // hit wall
      if (playingFieldLookup(playingField, x+1, y)) return false;
      if (playingFieldLookup(playingField, x+1, y+1)) return false;
      if (playingFieldLookup(playingField, x+1, y+2)) return false;
      if (playingFieldLookup(playingField, x+1, y+3)) return false;
      return true;
    case 4:
      if (x == PLAYING_FIELD_WIDTH - 2) return false; // hit wall
      if (playingFieldLookup(playingField, x+2, y)) return false;
      if (playingFieldLookup(playingField, x+2, y+1)) return false;
      return true;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
      return false;
  }
}

void commitRock(bool* playingField, int x, long long y, int rockType) {
  switch (rockType) {
    case 0:
      playingFieldLookup(playingField, x, y) = true;
      playingFieldLookup(playingField, x+1, y) = true;
      playingFieldLookup(playingField, x+2, y) = true;
      playingFieldLookup(playingField, x+3, y) = true;
      break;
    case 1:
      playingFieldLookup(playingField, x+1, y) = true;
      playingFieldLookup(playingField, x, y+1) = true;
      playingFieldLookup(playingField, x+1, y+1) = true;
      playingFieldLookup(playingField, x+2, y+1) = true;
      playingFieldLookup(playingField, x+1, y+2) = true;
      break;
    case 2:
      playingFieldLookup(playingField, x, y) = true;
      playingFieldLookup(playingField, x+1, y) = true;
      playingFieldLookup(playingField, x+2, y) = true;
      playingFieldLookup(playingField, x+2, y+1) = true;
      playingFieldLookup(playingField, x+2, y+2) = true;
      break;
    case 3:
      playingFieldLookup(playingField, x, y) = true;
      playingFieldLookup(playingField, x, y+1) = true;
      playingFieldLookup(playingField, x, y+2) = true;
      playingFieldLookup(playingField, x, y+3) = true;
      break;
    case 4:
      playingFieldLookup(playingField, x, y) = true;
      playingFieldLookup(playingField, x+1, y) = true;
      playingFieldLookup(playingField, x, y+1) = true;
      playingFieldLookup(playingField, x+1, y+1) = true;
      break;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
  }
}

bool isPartOfRock(int x, long long y, int rockType, int checkX, long long checkY) {
  switch (rockType) {
    case 0:
      if (checkY == y && checkX >= x && checkX <= x+3) return true;
      return false;
    case 1:
      if (checkX == x+1 && checkY >= y && checkY <= y+2) return true;
      if (checkX == x && checkY == y+1) return true;
      if (checkX == x+2 && checkY == y+1) return true;
      return false;
    case 2:
      if (checkY == y && checkX >= x && checkX <= x+2) return true;
      if (checkX == x+2 && checkY >= y && checkY <= y+2) return true;
      return false;
    case 3:
      if (checkX == x && checkY >= y && checkY <= y+3) return true;
      return false;
    case 4:
      if (checkY == y && (checkX == x || checkX == x+1)) return true;
      if (checkY == y+1 && (checkX == x || checkX == x+1)) return true;
      return false;
    default:
      printf("Invalid rock type %d\n", rockType);
      exit(1);
      return false;
  }
}

void displayPlayingFieldWithRock(bool* playingField, long long maxHeight, int rockX, long long rockY, int rockType) {
  //printf("\x1B[2J\x1B[H");
  
  for (long long y = maxHeight+6; y >= 0; y--) {
    printf("|");
    for (int x = 0; x < PLAYING_FIELD_WIDTH; x++) {
      if (playingFieldLookup(playingField, x, y)) {
        printf("#");
      }
      else if (isPartOfRock(rockX, rockY, rockType, x, y)) {
        printf("@");
      }        
      else {
        printf(".");
      }
    }
    printf("|\n");
  }
  printf("+-------+\n");
}

void displayPlayingField(bool* playingField, long long maxHeight) {
  //printf("\x1B[2J\x1B[H");
  
  for (long long y = maxHeight; y >= 0; y--) {
    printf("|");
    for (int x = 0; x < PLAYING_FIELD_WIDTH; x++) {
      if (playingFieldLookup(playingField, x, y)) {
        printf("#");
      }
      else {
        printf(".");
      }
    }
    printf("|\n");
  }
  printf("+-------+\n");
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: rocks input_file rounds\n");
    exit(1);
  }
  FILE * fp = fopen(argv[1], "r");
  long long rounds = atoll(argv[2]);
  
  long long maxY = 0;
  long long playingFieldHeight = PLAYING_FIELD_HEIGHT_CHUNK;
  bool* playingField = new bool[PLAYING_FIELD_WIDTH*playingFieldHeight];
  for (int i = 0; i < PLAYING_FIELD_WIDTH*playingFieldHeight; i++) {
    playingField[i] = false;
  }
  std::vector<Rock> rockHistory;
  for (long long i = 0; i < rounds; i++) {
    // spawn rock
    Rock rock;
    rock.x = 2;
    rock.y = maxY + 4;
    rock.rockType = i % 5;
    if (rock.y >= playingFieldHeight-10) {
      // resize playing field
      long long newHeight = playingFieldHeight+PLAYING_FIELD_HEIGHT_CHUNK;
      playingField = resizePlayingField(playingField, playingFieldHeight, newHeight);
      playingFieldHeight = newHeight;
    }
    bool canFall;
    do {
      rock.y--; // move rock down
      //displayPlayingFieldWithRock(playingField, maxY, x, y, rockType);
      if (nextDir(fp)) {
        // try to move right
        if (rockCanMoveRight(playingField, rock.x, rock.y, rock.rockType)) {
          rock.x++;
        }
      }
      else {
        // try to move left
        if (rockCanMoveLeft(playingField, rock.x, rock.y, rock.rockType)) {
          rock.x--;
        }
      }
      //displayPlayingFieldWithRock(playingField, maxY, rock.x, rock.y, rock.rockType);
      // test for falling
      canFall = rockCanFall(playingField, rock.x, rock.y, rock.rockType);
    } while (canFall);
    // commit rock to playing field
    long long compMax = rock.y + rockHeight(rock.rockType);
    if (compMax > maxY) {
      maxY = compMax;
    }
    commitRock(playingField, rock.x, rock.y, rock.rockType);
    rock.filePos = filePos;
    rockHistory.push_back(rock);
    
    if (i % 100000 == 99999) {
      printf("Max Y after %d steps: %d\n", i+1, maxY);
      // scan for repeating pattern
      bool foundPattern = false;
      for (int j = 1; j < rockHistory.size()/2; j++) {
        auto it = rockHistory.end() - 1;
        auto jt = rockHistory.end() - (j+1);
        foundPattern = true;
        for (int k = 0; k < j; k++) {
          if ((*it) != (*jt)) {
            foundPattern = false;
            break;
          }
          it--;
          jt--;
        }
        if (foundPattern) {
          it = rockHistory.end() - 1;
          jt = rockHistory.end() - (j+1);
          printf("Found pattern with period %lld\n", j);
          long long yDel = it->y - jt->y;
          printf("Y delta %lld\n", yDel);
          printf("Rounds %lld\n", rounds);
          long long remainingRocks = rounds-i-1;
          printf("Remaining rocks %lld\n", remainingRocks);
          long long skipPeriods = remainingRocks/j;
          printf("Skip periods %lld\n", skipPeriods);
          long long extraRocks = remainingRocks%j;
          printf("Extra rocks %lld\n", extraRocks);
          it = jt + extraRocks;
          long long extraYDel = it->y - jt->y;
          printf("Extra y delta %lld\n", extraYDel);
          printf("Max y %lld\n", maxY);
          printf("Skip y %lld\n", yDel*skipPeriods);
          long long totalHeight = maxY + yDel*skipPeriods + extraYDel;
          printf("Calculated total height %lld\n", totalHeight);
          exit(0);
        }
      }
      printf("No pattern found.\n");
    }
      
    //displayPlayingField(playingField, maxY);
  }
  //displayPlayingField(playingField, maxY);
  printf("Max Y after %d steps: %lld\n", rounds, maxY);
  delete[] playingField;
  return 0;
}