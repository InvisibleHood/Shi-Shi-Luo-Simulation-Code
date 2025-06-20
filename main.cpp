#include <iostream>
#include <stdio.h>

#include "simbox.h"

int main() {
      // setup simulation box
  int coop = 0;
  int def = 0;
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
  std::cout << "Defector: " << def << std::endl;
  std::cout << "Cooperator: " << coop << std::endl;
    return 0;
}
