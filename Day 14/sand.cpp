#include <iostream>
#include <fstream>
#include <vector>

struct Line {
  int startX;
  int startY;
  int endX;
  int endY;
};

struct Point {
  int x;
  int y;
};

// class wrapping around a dynamic array of booleans for quick lookup of a true-false value at a point
class PointLookup {
private:
  bool* points;
  int minX;
  int maxX;
  // minY assumed to be 0
  int maxY;
  int pointCount;
  int preCalcYMult; // store value used for indexing into array for speed
public:
  PointLookup(int _minX, int _maxX, int _maxY) {
    minX = _minX;
    maxX = _maxX;
    maxY = _maxY;
    pointCount = 0;
    preCalcYMult = maxX-minX+1;
    points = new bool[preCalcYMult*(maxY+1)];
    for (int i = 0; i < preCalcYMult*(maxY+1); i++) {
      points[i] = false;
    }
  }
  ~PointLookup() {
    delete[] points;
  }
  bool checkPoint(int x, int y) {
    return points[x-minX + y*preCalcYMult];
  }
  void addPoint(int x, int y) {
    points[x-minX + y*preCalcYMult] = true;
    pointCount++;
  }
  int getPointCount() {
    return pointCount;
  }
};

bool operator==(const Point& a, const Point& b) {
  return (a.x == b.x) && (a.y == b.y);
}

// function for determining whether a line and a point intersect
bool intersect(Line line, Point point) {
  if (line.startX == line.endX) {
    // vertical line
    if (point.x == line.startX) {
      if (line.startY > line.endY) {
        return (point.y >= line.endY && point.y <= line.startY);
      }
      else {
        return (point.y <= line.endY && point.y >= line.startY);
      }
    }
    else {
      return false;
    }
  }
  else {
    // horizontal line
    if (point.y == line.startY) {
      if (line.startX > line.endX) {
        return (point.x >= line.endX && point.x <= line.startX);
      }
      else {
        return (point.x <= line.endX && point.x >= line.startX);
      }
    }
    else {
      return false;
    }
  }
}

// function determining whether a given point interects with either of two PointLookups
// optionally also checks against an infinite horizontal line
bool checkIntersections(Point point, PointLookup* lines, PointLookup* points, bool infiniteFloor = false, int infiniteFloorY = 0) {
  if (infiniteFloor) {
    if (point.y == infiniteFloorY) {
      return true;
    }
  }
  if (points->checkPoint(point.x, point.y)) {
    return true;
  }
  if (lines->checkPoint(point.x, point.y)) {
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: sand input_file" << std::endl;
    exit(1);
  }
  std::vector<Line> lines;
  char c;
  std::string str;
  Line line;
  std::ifstream fs(argv[1]);
  int minX, maxX, maxY;
  // process input file, collecting min and max x and y coordinates for lines along the way
  fs >> line.startX;
  minX = line.startX;
  maxX = line.startX;
  fs >> c;
  fs >> line.startY;
  maxY = line.startY;
  fs >> str;
  while (!fs.eof() && !fs.fail()) {
    fs >> line.endX;
    if (line.endX > maxX) {
      maxX = line.endX;
    }
    else if (line.endX < minX) {
      minX = line.endX;
    }
    fs >> c;
    fs >> line.endY;
    if (line.startY > maxY) {
      maxY = line.endY;
    }
    lines.push_back(line);
    line.startX = line.endX;
    line.startY = line.endY;
    fs >> c;
    if (c >= '0' && c <= '9') {
      fs.seekg(-1, std::ios_base::cur);
      fs >> line.startX;
      if (line.startX > maxX) {
        maxX = line.startX;
      }
      else if (line.startX < minX) {
        minX = line.startX;
      }
      fs >> c;
      fs >> line.startY;
      if (line.startY > maxY) {
        maxY = line.startY;
      }
      fs >> str;
    }
    else {
      fs >> str;
    }
  }
  // precalculate line intersections, i.e. create a lookup table for where lines exist
  PointLookup* lineLookup = new PointLookup(std::min(minX, 500-(maxY+2)), std::max(maxX, 500+(maxY+2)), maxY+2);
  for (auto it = lines.begin(); it != lines.end(); it++) {
    if (it->startX == it->endX) {
      // vertical line
      if (it->startY > it->endY) {
        for (int y = it->endY; y <= it->startY; y++) {
          lineLookup->addPoint(it->startX, y);
        }
      }
      else {
        for (int y = it->startY; y <= it->endY; y++) {
          lineLookup->addPoint(it->startX, y);
        }
      }
    }
    else {
      // horizontal line
      if (it->startX > it->endX) {
        for (int x = it->endX; x <= it->startX; x++) {
          lineLookup->addPoint(x, it->startY);
        }
      }
      else {
        for (int x = it->startX; x <= it->endX; x++) {
          lineLookup->addPoint(x, it->startY);
        }
      }
    }
  }
  
  // part 1
  // allocate an array for grains at rest
  PointLookup* pointLookup = new PointLookup(minX, maxX, maxY);
  while (1) {
    Point grain;
    grain.x = 500;
    grain.y = 0;
    while (grain.y <= maxY) {
      // check below
      Point checkPoint;
      checkPoint.x = grain.x;
      checkPoint.y = grain.y + 1;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup)) {
        grain = checkPoint;
        continue;
      }
      // check left
      checkPoint.x--;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup)) {
        grain = checkPoint;
        continue;
      }
      // check right
      checkPoint.x += 2;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup)) {
        grain = checkPoint;
        continue;
      }
      break;
    }
    if (grain.y > maxY) {
      break;
    }
    else {
      pointLookup->addPoint(grain.x, grain.y);
    }
  }
  for (int y = 0; y <= maxY; y++) {
    for (int x = minX; x <= maxX; x++) {
      Point point;
      point.x = x;
      point.y = y;
      if (pointLookup->checkPoint(x, y)) {
        std::cout << "o";
      }
      else if (lineLookup->checkPoint(x, y)) {
        std::cout << "#";
      }
      else {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
  }
  std::cout << "Total grains at rest: " << pointLookup->getPointCount() << std::endl;
  
  // part 2
  // delete old array and allocate new grains-at-rest lookup table
  delete pointLookup;
  pointLookup = new PointLookup(std::min(minX, 500-(maxY+2)), std::max(maxX, 500+(maxY+2)), maxY+2);
  while (1) {
    Point grain;
    grain.x = 500;
    grain.y = 0;
    while (1) {
      // check below
      Point checkPoint;
      checkPoint.x = grain.x;
      checkPoint.y = grain.y + 1;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup, true, maxY+2)) {
        grain = checkPoint;
        continue;
      }
      // check left
      checkPoint.x--;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup, true, maxY+2)) {
        grain = checkPoint;
        continue;
      }
      // check right
      checkPoint.x += 2;
      if (!checkIntersections(checkPoint, lineLookup, pointLookup, true, maxY+2)) {
        grain = checkPoint;
        continue;
      }
      break;
    }
    if (grain.x == 500 && grain.y == 0) {
      pointLookup->addPoint(grain.x, grain.y);
      break;
    }
    else {
      pointLookup->addPoint(grain.x, grain.y);
    }
  }
  for (int y = 0; y <= maxY+2; y++) {
    for (int x = std::min(minX, 500-(maxY+2)); x <= std::max(maxX, 500+(maxY+2)); x++) {
      Point point;
      point.x = x;
      point.y = y;
      if (pointLookup->checkPoint(x, y)) {
        std::cout << "o";
      }
      else if (lineLookup->checkPoint(x, y)) {
        std::cout << "#";
      }
      else {
        std::cout << ".";
      }
    }
    std::cout << std::endl;
  }
  std::cout << "Total grains at rest with infinite floor: " << pointLookup->getPointCount() << std::endl;
  
}