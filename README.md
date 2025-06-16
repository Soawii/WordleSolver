# WordleSolver
Solves the daily wordle puzzle
## Functionality
1. Clone the repository
2. Go into the repository folder and build using cmake
   ```
   cd WordleSolver
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run the resulting file
   ```
   ./WordleSolver
   ```
4. Follow the instructions in the terminal
   "Enter your guess word" -> enter the chosen from the list (or not) word without any spaces.
   "Enter guess result" -> enter the resulting combination of GREEN(2), YELLOW(1) and GRAY(0) tiles.
     Example: you guess the word "tares" while the answer is "apple", resulting in GRAY,YELLOW,GRAY,YELLOW,GRAY -> you have to enter 01010
