# WordleSolver
A C++ program that solves the daily Wordle puzzle.
# Build
### 1. Clone the repository
### 2. Go into the repository folder and build using cmake
   ```bash
   cd WordleSolver
   mkdir build
   cd build
   cmake ..
   ```
### 3. Build the solution
   #### Windows:
   Open and build the generated .sln file  
   #### Linux:
   ```bash
   make
   ```
### 4. Run the resulting file
   #### Windows:
   Find the folder the .exe was generated in (Debug or Release) and run it.
   #### Linux:
   ```bash
   ./WordleSolver
   ```
### 6. Follow the instructions in the terminal
   
   | Command | What to enter | Example | 
   | :---: | :---: | :---: |
   | Enter your guess word | Enter the chosen word without any spaces | apple |
   | Enter guess result | Enter the resulting combination of GREEN(2), YELLOW(1) and GRAY(0) tiles | 01020 - meaning GRAY,YELLOW,GRAY,GREEN,GRAY |

### 7. Ways to run the program  
   | Command | What is does | 
   | :---: | :---: |
   | `./WordleSolver` | Default way that plays wordle |
   | `./WordleSolver -sim` | Runs the wordle simulation on all the possible answers, outputs the statistics |
   

