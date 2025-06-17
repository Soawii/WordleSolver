# WordleSolver
A C++ program that solves the daily Wordle puzzle.
# Build
1. Clone the repository
2. Go into the repository folder and build using cmake
   
   ```bash
   cd WordleSolver
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run the resulting file
   
   ```bash
   ./WordleSolver
   ```
4. Follow the instructions in the terminal
   
   | Command | What to enter | Example | 
   | :---: | :---: | :---: |
   | Enter your guess word | Enter the chosen word without any spaces | apple |
   | Enter guess result | Enter the resulting combination of GREEN(2), YELLOW(1) and GRAY(0) tiles | 01020 - meaning GRAY,YELLOW,GRAY,GREEN,GRAY |


