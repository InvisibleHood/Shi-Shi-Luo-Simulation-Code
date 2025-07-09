#include <iostream>
#include <stdio.h>

#include "simbox.h"

int main() {
      // setup simulation box
  int coop = 0;
  int def = 0;
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; i++) {
    Simbox* simbox = new Simbox();
    std::cout << "Running simulation " << i << std::endl;
    int result = simbox->RunSimulation();
    if (result == 0) {
      def++;
    } else {
      coop++;
    }
    //clean up
    delete simbox;
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Simulation time: " << elapsed.count() << " seconds" << std::endl;
  std::cout << "Defector: " << def << std::endl;
  std::cout << "Cooperator: " << coop << std::endl;
    return 0;
}
