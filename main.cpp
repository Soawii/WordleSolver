#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <math.h>
#include <algorithm>
#include <queue>
#include <set>
#include <iomanip>
#include <unordered_map>

enum Letter {
    GRAY = 0,
    YELLOW = 1,
    GREEN = 2
};

struct Permutation 
{
    Letter letter[5];
};

struct Guess 
{
    double entropy;
    double probability;
    int word_index;
};

struct GuessCompare 
{
    bool operator()(const Guess& left, const Guess& right) 
    {
        if (left.entropy != right.entropy) 
            return left.entropy > right.entropy;
        return left.probability > right.probability;
    }
};

std::vector<std::string> original_words;
std::vector<std::pair<std::string, int>> words_copy, words;
std::vector<std::array<int8_t, 26>> word_to_letter;
std::array<Permutation, 243> all_permutations;
std::vector<double> word_frequency;
int permutation_index = 0;

void trimStart(std::string& s) {
    int front_spaces = 0;
    for (int i = 0; i < s.size() && s[i] == ' '; i--)
        front_spaces++;
    s = s.substr(front_spaces, 5);
}

void loadWords(std::string filename, std::vector<std::string>& where) {
    std::fstream file;
    {
        std::string word;
        file.open(filename, std::ios::in);
        while (file >> word) {
            where.push_back(word);
        }
        file.close();
    }
}

void loadFrequency(int CENTER, double X_COEFF) {
    std::vector<std::string> words_by_frequency;
    std::fstream file;
    {
        file.open("word_frequency.txt", std::ios::in);
        std::string word, frequency;
        while (file >> word >> frequency) {
            words_by_frequency.push_back(word);
        }
        file.close();
    }

    std::unordered_map<std::string, size_t> word_to_index;
    for (int i = 0; i < original_words.size(); i++) {
        word_to_index[original_words[i]] = i;
    }
    const double MIN_VALUE = 0.00001;
    word_frequency.resize(original_words.size(), 0);

    for (int i = 0; i < word_frequency.size(); i++) {
        double X = CENTER - i;
        double sigmoid = std::max(MIN_VALUE, double(1) / (double(1) + double(exp(-X_COEFF * X))));
        word_frequency[word_to_index[words_by_frequency[i]]] = sigmoid; 
    }
}

void fillPermutationsHelper(int depth, Permutation& permutation) {
    if (depth == 5) {
        all_permutations[permutation_index++] = permutation;
        return;
    }
    permutation.letter[depth] = Letter::GRAY;
    fillPermutationsHelper(depth + 1, permutation);
    permutation.letter[depth] = Letter::YELLOW;
    fillPermutationsHelper(depth + 1, permutation);
    permutation.letter[depth] = Letter::GREEN;
    fillPermutationsHelper(depth + 1, permutation);
    return;
}

void fillPermutations() {
    Permutation permutation;
    fillPermutationsHelper(0, permutation);
    return;
}

void printPermutation(const Permutation& p) {
    for (int i =0 ; i < 5; i++) {
        std::cout << p.letter[i];
    }
    std::cout << std::endl;
}

void resetWords() {
    words = words_copy;
}

void fillWordToLetter() {
    word_to_letter.resize(original_words.size());
    for (int i = 0; i < original_words.size(); i++) {
        const std::string& word = original_words[i];
        for (int j = 0; j < 26; j++)
            word_to_letter[i][j] = 0;
        for (char c : word) {
            word_to_letter[i][c - 'a']++;
        }
    }
    words_copy.resize(original_words.size());
    for (int i = 0; i < words_copy.size(); i++) {
        words_copy[i] = std::make_pair(original_words[i], i);
    }
}

bool doesMatch(const std::string& word1, const std::string& word2, const Permutation& p) {
    std::array<int8_t, 26> word2_contains;
    word2_contains.fill(0);
    for (char c : word2) {
        word2_contains[c - 'a']++;
    }

    for (int i = 0; i < 5; i++) {
        if (p.letter[i] == Letter::GREEN) {
            if (word1[i] != word2[i])
                return false;
            word2_contains[word1[i] - 'a']--;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (p.letter[i] == Letter::YELLOW) {
            if (!word2_contains[word1[i] - 'a'] || word1[i] == word2[i])
                return false;
            word2_contains[word1[i] - 'a']--;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (p.letter[i] == Letter::GRAY) {
            if (word2_contains[word1[i] - 'a'])
                return false;
        }
    }

    return true;
}

int permutationToIndex(const Permutation& p) {
    int index = 0;
    for (int i = 0; i < 5; i++) {
        index *= 3;
        index += p.letter[i];
    }
    return index;
}

Permutation getPermutation(const std::string& word1, const std::string& word2) {
    std::array<int8_t, 26> word2_to_letter;
    word2_to_letter.fill(0);

    for (char c : word2)
        word2_to_letter[c - 'a']++;

    Permutation p;

    for (int i = 0; i < 5; i++) {
        p.letter[i] = Letter::GRAY;
    }

    for (int i = 0; i < 5; i++) {
        if (word1[i] == word2[i]) {
            word2_to_letter[word1[i] - 'a']--;
            p.letter[i] = Letter::GREEN;
        }
    }

    for (int i = 0; i < 5; i++) {
        if (word2_to_letter[word1[i] - 'a'] && word1[i] != word2[i]) {
            word2_to_letter[word1[i] - 'a']--;
            p.letter[i] = Letter::YELLOW;
        }
    }
    
    return p;
}

int getPermutationIdx(int word1, int word2) {
    std::array<int8_t, 26> word2_to_letter = word_to_letter[word2];
    int permutation_idx = 0;
    const std::string& word1_str = original_words[word1];
    const std::string& word2_str = original_words[word2];

    int multiplier = 1;
    for (int i = 4; i >= 0; i--) {
        if (word1_str[i] == word2_str[i]) {
            word2_to_letter[word1_str[i] - 'a']--;
            permutation_idx += 2 * (multiplier);
        }
        multiplier *= 3;
    }

    multiplier = 81;
    for (int i = 0; i < 5; i++) {
        if (word2_to_letter[word1_str[i] - 'a'] && word1_str[i] != word2_str[i]) {
            word2_to_letter[word1_str[i] - 'a']--;
            permutation_idx += multiplier;
        }
        multiplier /= 3;
    }
    
    return permutation_idx;
}

double getEntropyForWord(int word) {
    std::array<double, 243> permutation_count;
    permutation_count.fill(0);

    for (int i = 0; i < words.size(); i++) {
        int p_idx = getPermutationIdx(word, words[i].second);
        permutation_count[p_idx] += word_frequency[words[i].second];
    }

    double total_sum = 0;
    for (int i = 0; i < 243; i++) 
        total_sum += permutation_count[i];

    double entropy = 0;
    for (int i = 0; i < 243; i++) {
        if (permutation_count[i] == 0)
            continue;
        double match_percent = double(permutation_count[i]) / double(total_sum);
        entropy -= match_percent * log2(match_percent);
    }

    return entropy;
}

int simulateGame(std::string guessed_word) {
    const int N = original_words.size();

    resetWords();
    std::vector<double> entropies(original_words.size(), 0);
    std::vector<bool> stillIn(original_words.size(), true);

    for (int g = 0; g < 6; g++)
    {
        double sum_probabilities = 0;
        for (int i = 0; i < words.size(); i++) {
            sum_probabilities += word_frequency[words[i].second];
        }

        std::string best_guess = "";
        if (g != 0) {
            #pragma omp parallel for
            for (int i = 0; i < N; i++) {
                entropies[i] = getEntropyForWord(i);
            }
            double best_entropy = 0;
            for (int i = 0; i < entropies.size(); i++) {
                double probability = (stillIn[i] ? (word_frequency[i] / sum_probabilities) : 0.0f);
                double new_entropy = (stillIn[i] ? (entropies[i] + 1.5 * pow(probability, 2)) : entropies[i]);
                if (new_entropy > best_entropy) {
                    best_entropy = new_entropy;
                    best_guess = original_words[i];
                }
            }
        }
        else 
            best_guess = "tares";

        Permutation p = getPermutation(best_guess, guessed_word);

        std::for_each(words.begin(), words.end(), [&best_guess, &p, &stillIn](const std::pair<std::string, int>& pair){
            if (!doesMatch(best_guess, pair.first, p)) {
                stillIn[pair.second] = false;
            }
        });
        auto new_end = std::remove_if(words.begin(), words.end(), [&best_guess, &p](const std::pair<std::string, int>& pair) {
            return !doesMatch(best_guess, pair.first, p);
        });
        words.erase(new_end, words.end());

        if (words.size() == 1) {
            if (words[0].first != best_guess) {
                return g + 2;
            }
            return g + 1;
        }
    }
    return -1;
}

void simulateGames(std::string filenameWithWords) {
    std::vector<std::string> answers;
    loadWords(filenameWithWords, answers);

    std::cout << "Starting the simulation\n";
    int total_guesses_taken = 0;
    int correct_guesses = 0;

    for (int i = 0; i < answers.size(); i++) {
        if (i % 100 == 0)
            std::cout << i << "/" << answers.size() << " games processed...\n";
        int guesses_taken = simulateGame(answers[i]);
        if (guesses_taken != -1) {
            total_guesses_taken += guesses_taken;
            correct_guesses++;
        }
        else {
            total_guesses_taken += 6;
        }
    }

    std::cout << "Words guessed correctly       - " << correct_guesses << "/" << answers.size() << std::endl;
    std::cout << "Correct word percentage       - " << double(correct_guesses) / double(answers.size()) << std::endl;
    std::cout << "Average guess count per word  - " << double(total_guesses_taken) / double(correct_guesses) << std::endl;
}

void playGame() {
    const int BEST_GUESS_PRINT_AMOUNT = 5;

    resetWords();

    std::cout << "How you should enter guess results: 01201 [0 - GRAY, 1 - YELLOW, 2 - GREEN]" << std::endl;

    std::vector<bool> stillIn(original_words.size(), true);
    std::vector<double> entropies(original_words.size(), 0);

    for (int g = 0; g < 6; g++)
    {
        if (words.size() == 1) {
            std::cout << "FINAL GUESS - " << words[0].first << std::endl;
            break;
        }

        double sum_probabilities = 0;
        for (int i = 0; i < words.size(); i++) {
            sum_probabilities += word_frequency[words[i].second];
        }

        std::cout << "Processing " << words.size() << " words..." << std::endl;
        std::vector<Guess> best_overall_guesses, best_possible_guesses; 

        if (g != 0) {
            #pragma omp parallel for
            for (int i = 0; i < original_words.size(); i++) {
                entropies[i] = getEntropyForWord(i);
            }

            std::priority_queue<Guess, std::vector<Guess>, GuessCompare> overall_pq, possible_pq;
            for (int i = 0 ; i < entropies.size(); i++) {
                double probability = (stillIn[i] ? (word_frequency[i] / sum_probabilities) : 0.0f);
                double new_entropy = (stillIn[i] ? (entropies[i] + 1.5 * pow(probability, 1.5)) : entropies[i]);
                overall_pq.push({new_entropy, probability, i});
                if (stillIn[i])
                    possible_pq.push({new_entropy, probability, i});
                if (overall_pq.size() > BEST_GUESS_PRINT_AMOUNT)
                    overall_pq.pop();
                if (possible_pq.size() > BEST_GUESS_PRINT_AMOUNT)
                    possible_pq.pop();
            }
            while (!overall_pq.empty()) {
                best_overall_guesses.push_back(overall_pq.top());
                overall_pq.pop();
            }
            reverse(best_overall_guesses.begin(), best_overall_guesses.end());

            while (!possible_pq.empty()) {
                best_possible_guesses.push_back(possible_pq.top());
                possible_pq.pop();
            }
            reverse(best_possible_guesses.begin(), best_possible_guesses.end());
        }
        else {
            best_overall_guesses.push_back({6.32, 0.0001, 12810});
            best_overall_guesses.push_back({6.25, 0.0001, 6691});
            best_overall_guesses.push_back({6.22, 0.0003, 10374});
            best_overall_guesses.push_back({6.20, 0.0002, 10303});
            best_overall_guesses.push_back({6.20, 0.0003, 12886});
            best_possible_guesses = best_overall_guesses;
        }

        std::cout << std::setiosflags(std::ios::left | std::ios::fixed);
        std::cout << std::setw(45) << "\nBest Overall Guesses" << std::setw(45) << "Best Possible Guesses" << std::endl;
        std::cout << std::setw(15) << "[WORD]" << std::setw(15) << "[SCORE]" << std::setw(15) << "[%]" << std::setw(15) << "[WORD]" << std::setw(15) << "[SCORE]" << std::setw(15) << "[%]" << std::endl;
        for (int i = 0; i < best_overall_guesses.size(); i++) {
            std::cout << std::setprecision(2) << std::setw(15) << original_words[best_overall_guesses[i].word_index] << std::setw(15) << best_overall_guesses[i].entropy << std::setw(15) << 100 * best_overall_guesses[i].probability;
            if (best_possible_guesses.size() > i)
                std::cout << std::setw(15) << original_words[best_possible_guesses[i].word_index] << std::setw(15) << best_possible_guesses[i].entropy << std::setw(15) << 100 * best_possible_guesses[i].probability;
            std::cout << std::endl;
        }

        std::cout << std::setw(25) << "Enter your guess word -";
        std::string guess_word;
        std::cin >> guess_word;
        trimStart(guess_word);

        std::cout << std::setw(25) << "Enter guess result -";
        std::string guess_result;
        std::cin >> guess_result;
        trimStart(guess_result);
        
        if (guess_result == "22222") {
            break;
        }

        Permutation p;
        for (int i = 0; i < 5; i++) {
            p.letter[i] = Letter(guess_result[i] - '0');
        }

        int le_temp = doesMatch(guess_word, "about", p);
        std::cout << "Excluding incorrect words..." << std::endl;

        std::for_each(words.begin(), words.end(), [&guess_word, &p, &stillIn](const std::pair<std::string, int>& pair){
            if (!doesMatch(guess_word, pair.first, p)) {
                stillIn[pair.second] = false;
            }
        });
        auto new_end = std::remove_if(words.begin(), words.end(), [&guess_word, &p](const std::pair<std::string, int>& pair) {
            return !doesMatch(guess_word, pair.first, p);
        });
        words.erase(new_end, words.end());
    }
}

struct Result {
    double avg_guess;
    int center;
    double coeff;
};

int main(int argc, char **argv) {
    bool simulation = false;
    if (argc == 2) {
        if (std::string(argv[1]) == "-sim") {
            simulation = true;
        }
    }

    loadWords("valid_words.txt", original_words);
    loadFrequency(3400, 0.0029);
    fillPermutations();
    fillWordToLetter();
    
    if (simulation) {
        simulateGames("answer_words.txt");
    }
    else {
        while (true) {
            playGame();
            std::cout << "Play again? y/n - ";
            char answer;
            while(true)
            {
                std::cin >> answer;
                if (answer == 'y' || answer == 'n')
                    break;
                else
                    std::cout << "Unknown command, try again - ";
            }
            if (answer == 'y')
                continue;
            else {
                std::cout << "Thanks for playing!\n";
                break;
            }
        }
    }
    

    return 0;
}