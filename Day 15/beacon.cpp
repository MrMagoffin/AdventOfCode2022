#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

class Sensor {
private:
  // fundamental values provided by AoC
  int x;
  int y;
  int beaconX;
  int beaconY;
  // pre-calculated values
  int dist;
  int minX;
  int maxX;
public:
  Sensor(int x, int y, int beaconX, int beaconY) : x(x), y(y), beaconX(beaconX), beaconY(beaconY) {
    dist = abs(beaconX - x) + abs(beaconY - y);
    minX = x - dist;
    maxX = x + dist;
  }
  bool isAt(int compX, int compY) {
    return (beaconX == compX && beaconY == compY);
  }
  bool inRange(int compX, int compY) {
    int calcDist = abs(compX - x) + abs(compY - y);
    return (calcDist <= dist);
  }
  int getMinX() {
    return minX;
  }
  int getMaxX() {
    return maxX;
  }
  int getDist() {
    return dist;
  }
  int getX() {
    return x;
  }
  int getY() {
    return y;
  }
};

bool isLocationPossible(std::vector<Sensor *>& sensors, int x, int y) {
  for (auto it = sensors.begin(); it != sensors.end(); it++) {
    if ((*it)->inRange(x, y)) {
      return false;
    }
  }
  return true;
}

int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: beacon input_file y_value" << std::endl;
    exit(1);
  }
  FILE* fp = fopen(argv[1], "r");
  int yValue = atoi(argv[2]);
  
  std::vector<Sensor *> sensors;
  int minX;
  int maxX;
  while (!feof(fp)) {
    // loop through sensors
    std::string strX;
    std::string strY;
    std::string strBeaconX;
    std::string strBeaconY;
    char c;
    // skip to x value
    do {
      c = fgetc(fp);
    } while (!feof(fp) && c != '=');
    if (feof(fp)) {
      break;
    }
    // x value
    c = fgetc(fp);
    do {
      strX.push_back(c);
      c = fgetc(fp);
    } while (c >= '-' && c <= '9');
    // skip to y value
    do {
      c = fgetc(fp);
    } while (!feof(fp) && c != '=');
    // y value
    c = fgetc(fp);
    do {
      strY.push_back(c);
      c = fgetc(fp);
    } while (c >= '-' && c <= '9');
    // skip to x value
    do {
      c = fgetc(fp);
    } while (!feof(fp) && c != '=');
    if (feof(fp)) {
      break;
    }
    // beacon x value
    c = fgetc(fp);
    do {
      strBeaconX.push_back(c);
      c = fgetc(fp);
    } while (c >= '-' && c <= '9');
    // skip to y value
    do {
      c = fgetc(fp);
    } while (!feof(fp) && c != '=');
    // beacon y value
    c = fgetc(fp);
    do {
      strBeaconY.push_back(c);
      c = fgetc(fp);
    } while (c >= '-' && c <= '9');
    Sensor * sensor = new Sensor(std::stoi(strX), std::stoi(strY), std::stoi(strBeaconX), std::stoi(strBeaconY));
    sensors.push_back(sensor);
    if (sensors.size() == 1) {
      minX = sensor->getMinX();
      maxX = sensor->getMaxX();
    }
    else {
      if (minX > sensor->getMinX()) {
        minX = sensor->getMinX();
      }
      if (maxX < sensor->getMaxX()) {
        maxX = sensor->getMaxX();
      }
    }
  }
  
  std::cout << "Loaded " << sensors.size() << " sensors." << std::endl;
  std::cout << "minX: " << minX << std::endl;
  std::cout << "maxX: " << maxX << std::endl;
  
  // part 1
  int noBeaconCount = 0;
  bool foundBeacon;
  for (int i = minX; i <= maxX; i++) {
    foundBeacon = false;
    for (auto it = sensors.begin(); it != sensors.end(); it++) {
      if ((*it)->isAt(i, yValue)) {
        foundBeacon = true;
        break;
      }
    }
    if (foundBeacon) {
      continue;
    }
    for (auto it = sensors.begin(); it != sensors.end(); it++) {
      if ((*it)->inRange(i, yValue)) {
        noBeaconCount++;
        break;
      }
    }
  }
  
  std::cout << "There are " << noBeaconCount << " positions on y=" << yValue << " where a beacon cannot be present." << std::endl;
  
  // part 2
  // search around each sensor's range
  foundBeacon = false;
  int x;
  int y;
  for (auto jt = sensors.begin(); jt != sensors.end(); jt++) {
    int dist = (*jt)->getDist();
    x = (*jt)->getX();
    y = (*jt)->getY() + dist + 1;
    // go up and to the right
    for (int i = 0; i < dist + 1; i++) {
      if (isLocationPossible(sensors, x, y)) {
        std::cout << "Beacon can be at (" << x << "," << y << ")!" << std::endl;
        foundBeacon = true;
        break;
      }
      x++;
      y--;
    }
    if (foundBeacon) {
      break;
    }
    // go up and to the left
    for (int i = 0; i < dist + 1; i++) {
      if (isLocationPossible(sensors, x, y)) {
        std::cout << "Beacon can be at (" << x << "," << y << ")!" << std::endl;
        foundBeacon = true;
        break;
      }
      x--;
      y--;
    }
    if (foundBeacon) {
      break;
    }
    // go down and to the left
    for (int i = 0; i < dist + 1; i++) {
      if (isLocationPossible(sensors, x, y)) {
        std::cout << "Beacon can be at (" << x << "," << y << ")!" << std::endl;
        foundBeacon = true;
        break;
      }
      x--;
      y++;
    }
    if (foundBeacon) {
      break;
    }
    // go down and to the right
    for (int i = 0; i < dist + 1; i++) {
      if (isLocationPossible(sensors, x, y)) {
        std::cout << "Beacon can be at (" << x << "," << y << ")!" << std::endl;
        foundBeacon = true;
        break;
      }
      x++;
      y++;
    }
    if (foundBeacon) {
      break;
    }
  }
  unsigned long long tuningFreq;
  tuningFreq = 4000000;
  tuningFreq *= x;
  tuningFreq += y;
  std::cout << "Tuning frequency: " << tuningFreq << std::endl;
  return 0;
}