#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

struct Point3D {
  int x;
  int y;
  int z;
};

class Space3D {
private:
  int minX;
  int maxX;
  int minY;
  int maxY;
  int minZ;
  int maxZ;
  int yMult;
  int zMult;
  int spaceSize;
  bool* space;
  bool defaultVal;
public:
  Space3D(int minX, int maxX, int minY, int maxY, int minZ, int maxZ, bool defaultVal = false) :
    minX(minX), maxX(maxX), minY(minY), maxY(maxY), minZ(minZ), maxZ(maxZ), defaultVal(defaultVal) {
    yMult = (maxX-minX+1);
    zMult = yMult*(maxY-minY+1);
    spaceSize = zMult*(maxZ-minZ+1);
    space = new bool[spaceSize];
    for (int i = 0; i < spaceSize; i++) {
      space[i] = false;
    }
  }
  ~Space3D() {
    delete[] space;
  }
  bool setValueAt(int x, int y, int z, bool value) {
    if (x < minX || x > maxX || y < minY || y > maxY || z < minZ || z > maxZ) {
      return false;
    }
    space[x-minX + yMult*(y-minY) + zMult*(z-minZ)] = value;
    return true;
  }
  bool getValueAt(int x, int y, int z) {
    if (x < minX || x > maxX || y < minY || y > maxY || z < minZ || z > maxZ) {
      return defaultVal;
    }
    return space[x-minX + yMult*(y-minY) + zMult*(z-minZ)];
  }
};

int sidesTouching(Point3D point, Space3D &space) {
  int touchedSides = 0;
  if (space.getValueAt(point.x-1, point.y, point.z)) {
    touchedSides++;
  }
  if (space.getValueAt(point.x+1, point.y, point.z)) {
    touchedSides++;
  }
  if (space.getValueAt(point.x, point.y-1, point.z)) {
    touchedSides++;
  }
  if (space.getValueAt(point.x, point.y+1, point.z)) {
    touchedSides++;
  }
  if (space.getValueAt(point.x, point.y, point.z-1)) {
    touchedSides++;
  }
  if (space.getValueAt(point.x, point.y, point.z+1)) {
    touchedSides++;
  }
  return touchedSides;
}

void checkAndAddPoint(int x, int y, int z, Space3D &visitedSpace, Space3D &cubeSpace, std::queue<Point3D> &pointQueue) {
  if (!visitedSpace.getValueAt(x, y, z) && !cubeSpace.getValueAt(x, y, z)) {
    Point3D point;
    point.x = x;
    point.y = y;
    point.z = z;
    pointQueue.push(point);
    visitedSpace.setValueAt(x, y, z, true);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: cubes input_file");
    exit(1);
  }
  std::ifstream fs(argv[1]);
  std::vector<Point3D> cubes;
  int minX, maxX, minY, maxY, minZ, maxZ;
  bool extremesSet = false;
  while (!fs.eof() && !fs.fail()) {
    Point3D cube;
    char c;
    fs >> cube.x;
    if (!extremesSet || cube.x < minX) {
      minX = cube.x;
    }
    if (!extremesSet || cube.x > maxX) {
      maxX = cube.x;
    }
    if (fs.eof()) {
      break;
    }
    fs >> c;
    fs >> cube.y;
    if (!extremesSet || cube.y < minY) {
      minY = cube.y;
    }
    if (!extremesSet || cube.y > maxY) {
      maxY = cube.y;
    }
    fs >> c;
    fs >> cube.z;
    if (!extremesSet || cube.z < minZ) {
      minZ = cube.z;
    }
    if (!extremesSet || cube.z > maxZ) {
      maxZ = cube.z;
    }
    cubes.push_back(cube);
    extremesSet = true;
  }
  std::cout << "Loaded " << cubes.size() << " cubes." << std::endl;
  Space3D space(minX, maxX, minY, maxY, minZ, maxZ);
  for (auto it = cubes.begin(); it != cubes.end(); it++) {
    space.setValueAt(it->x, it->y, it->z, true);
  }
  // part 1
  int sharedSides = 0;
  for (auto it = cubes.begin(); it != cubes.end(); it++) {
    sharedSides += sidesTouching(*it, space);
  }
  std::cout << sharedSides << " shared sides." << std::endl;
  std::cout << cubes.size()*6 - sharedSides << " surface area." << std::endl;
  // part 2
  // starting point on the outside
  Point3D cur;
  Space3D visited(minX-1, maxX+1, minY-1, maxY+1, minZ-1, maxZ+1, true);
  int touchedSides = 0;
  cur.x = minX-1;
  cur.y = minY-1;
  cur.z = minZ-1;
  std::queue<Point3D> pointQueue;
  pointQueue.push(cur);
  visited.setValueAt(cur.x, cur.y, cur.z, true);
  while (!pointQueue.empty()) {
    cur = pointQueue.front();
    // count any sides of the droplet being touched
    touchedSides += sidesTouching(cur, space);
    // add points not yet visited bordering this point
    checkAndAddPoint(cur.x-1, cur.y, cur.z, visited, space, pointQueue);
    checkAndAddPoint(cur.x+1, cur.y, cur.z, visited, space, pointQueue);
    checkAndAddPoint(cur.x, cur.y-1, cur.z, visited, space, pointQueue);
    checkAndAddPoint(cur.x, cur.y+1, cur.z, visited, space, pointQueue);
    checkAndAddPoint(cur.x, cur.y, cur.z-1, visited, space, pointQueue);
    checkAndAddPoint(cur.x, cur.y, cur.z+1, visited, space, pointQueue);
    pointQueue.pop();
  }
  std::cout << touchedSides << " touched sides." << std::endl;
  return 0;
}