#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Usage: crt input_file cycle_count" << std::endl;
    exit(1);
  }
  
  std::ifstream fs(argv[1]);
  int totalCycles = atoi(argv[2]);
  
  int cycle = 0;
  int cyclesRemaining = 0;
  int x = 1;
  int signalStrength = 0;
  enum {
    s_idle,
    s_addx,
    s_noop
  } crt_state = s_idle;
  while (!fs.eof() && cycle < totalCycles) {
    // start of cycle
    if (crt_state == s_idle) {
      // fetch instruction
      std::string inst;
      fs >> inst;
      if (fs.eof()) {
        break;
      }
      if (inst == "noop") {
        crt_state = s_noop;
        cyclesRemaining = 0;
      }
      else if (inst == "addx") {
        crt_state = s_addx;
        cyclesRemaining = 1;
      }
      else {
        std::cout << "Unknown instruction " << inst << std::endl;
        exit(1);
      }
    }
    // mid-cycle
    int col = cycle % 40;
    if (col == 19) {
      signalStrength += (cycle+1)*x;
    }
    int colDiff = x - col;
    if (colDiff < 2 && colDiff > -2) {
      std::cout << "#";
    }
    else {
      std::cout << ".";
    }
    if (col == 39) {
      std::cout << std::endl;
    }
    // end of cycle
    if (cyclesRemaining > 0) {
      cyclesRemaining--;
    }
    else {
      // instruction has finished executing
      switch (crt_state) {
        case s_addx:
          int addend;
          fs >> addend;
          x += addend;
          break;
        case s_noop:
          break;
        default:
          std::cout << "Execution error: executing null instruction" << std::endl;
          exit(1);
      }
      crt_state = s_idle;      
    }
    cycle++;
  }
  
  std::cout << "Total signal strength = " << signalStrength << std::endl;
  
  return 0;
}