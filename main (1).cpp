/*
 * Cipher Cascade - a text-based cryptography game
 * CS110 Lab 08 (Open Ended Lab), BESE-17A
 *
 * Team:
 *   Syed Muhammad Aliyan Ali  (CMS 585981) - game idea and concept
 *   Affan Akbar               (CMS 592135) - scoring mechanism
 *   Muhammad Yahya Shahzad    (CMS 604750) - overall functionality
 *
 * Build:  g++ -std=c++17 -Wall -Wextra -o cipher_cascade main.cpp
 * Run:    ./cipher_cascade
 */

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// ============================================================
//  Cipher functions
// ============================================================

// Caesar cipher: shifts each letter forward by `shift` positions.
// Case is preserved; non-letters are left unchanged.
string caesarEncrypt(const string& text, int shift) {
    shift = ((shift % 26) + 26) % 26;  // normalize to 0..25
    string result = text;
    for (char& ch : result) {
        if (isupper(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>('A' + (ch - 'A' + shift) % 26);
        } else if (islower(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>('a' + (ch - 'a' + shift) % 26);
        }
    }
    return result;
}

string caesarDecrypt(const string& text, int shift) {
    return caesarEncrypt(text, -shift);
}

// Atbash cipher: A<->Z, B<->Y, C<->X, ... Applying it twice returns the original.
string atbash(const string& text) {
    string result = text;
    for (char& ch : result) {
        if (isupper(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>('Z' - (ch - 'A'));
        } else if (islower(static_cast<unsigned char>(ch))) {
            ch = static_cast<char>('z' - (ch - 'a'));
        }
    }
    return result;
}

// Vigenere cipher: each letter is shifted by the matching letter of the keyword.
// The keyword only advances on letters, so spaces and punctuation are preserved.
// The keyword must contain letters only.
string vigenere(const string& text, const string& key, bool decrypt) {
    string result = text;
    if (key.empty()) return result;

    size_t k = 0;
    for (char& ch : result) {
        if (!isalpha(static_cast<unsigned char>(ch))) continue;

        int shift = toupper(static_cast<unsigned char>(key[k % key.size()])) - 'A';
        if (decrypt) shift = -shift;

        char base = isupper(static_cast<unsigned char>(ch)) ? 'A' : 'a';
        ch = static_cast<char>(base + ((ch - base + shift) % 26 + 26) % 26);
        ++k;
    }
    return result;
}

// ============================================================
//  Input and string helpers
// ============================================================

string toUpper(string s) {
    for (char& ch : s) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    return s;
}

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool isAlphaOnly(const string& s) {
    return !s.empty() &&
           all_of(s.begin(), s.end(), [](unsigned char c) { return isalpha(c) != 0; });
}

// Reads one trimmed line. If input ends (e.g. Ctrl+D), exit cleanly.
string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {
        cout << "\nGoodbye!\n";
        exit(0);
    }
    return trim(line);
}

// Keeps asking until the user types a whole number.
int readInt(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos = 0;
            int value = stoi(s, &pos);
            if (pos == s.size()) return value;
        } catch (const std::exception&) {
            // not a valid integer; ask again below
        }
        cout << "Please enter a whole number.\n";
    }
}

bool askDecrypt() {
    while (true) {
        string s = toUpper(readLine("Encrypt or decrypt? (E/D): "));
        if (s == "E") return false;
        if (s == "D") return true;
        cout << "Please type E or D.\n";
    }
}

// ============================================================
//  Game challenges
// ============================================================

struct Challenge {
    string title;
    string description;  // what the player is told
    string ciphertext;   // what the player must decode
    string hint;         // costs points to reveal
    string answer;       // expected plaintext, uppercase
};

// Builds four levels with random words (and random keys/shifts where relevant),
// so every game is different.
vector<Challenge> buildChallenges(mt19937& rng) {
    const vector<string> words = {"PROGRAM",  "COMPUTER", "SECURITY", "ALGORITHM", "FUNCTION",
                                  "VARIABLE", "KEYBOARD", "NETWORK",  "BINARY",    "PROCESSOR"};
    const vector<string> keywords = {"LEMON", "CODE", "SHIELD", "KEY"};

    auto randomIndex = [&rng](size_t n) {
        return uniform_int_distribution<size_t>(0, n - 1)(rng);
    };

    vector<Challenge> challenges;

    // Level 1: Caesar cipher with a stated shift.
    {
        int shift = 1 + static_cast<int>(randomIndex(25));  // 1..25
        string plain = words[randomIndex(words.size())];
        challenges.push_back({
            "Caesar Cipher",
            "Each letter was shifted FORWARD by " + to_string(shift) +
                " positions. Shift it back to decode.",
            caesarEncrypt(plain, shift),
            "Move each letter BACKWARD by " + to_string(shift) +
                " positions in the alphabet (wrapping from A to Z).",
            plain});
    }

    // Level 2: Atbash cipher.
    {
        string plain = words[randomIndex(words.size())];
        challenges.push_back({
            "Atbash Cipher",
            "This message was encrypted with the Atbash cipher.",
            atbash(plain),
            "Mirror the alphabet: A<->Z, B<->Y, C<->X, and so on. "
            "Decrypting works exactly like encrypting.",
            plain});
    }

    // Level 3: Vigenere cipher with a keyword.
    {
        string key = keywords[randomIndex(keywords.size())];
        string plain = words[randomIndex(words.size())];
        challenges.push_back({
            "Vigenere Cipher",
            "This message uses a Vigenere cipher with a secret keyword. Decrypt it.",
            vigenere(plain, key, false),
            "The keyword is " + key +
                ". Shift each letter back by the value of the matching keyword letter "
                "(A=0, B=1, ...). The keyword repeats.",
            plain});
    }

    // Level 4: Caesar cipher with an unknown shift (brute force).
    {
        int shift = 1 + static_cast<int>(randomIndex(25));
        string plain = words[randomIndex(words.size())];
        challenges.push_back({
            "Brute Force",
            "This word was shifted by an unknown Caesar amount. Try shifts until "
            "you find the real word.",
            caesarEncrypt(plain, shift),
            "The first letter of the word is " + string(1, plain[0]) + ".",
            plain});
    }

    return challenges;
}

enum class Outcome { Solved, Failed, Quit };

struct ChallengeResult {
    Outcome outcome;
    int points;
};

// Runs one challenge. Scoring:
//   start at 100 points, -15 per wrong answer, -25 for using the hint.
ChallengeResult playChallenge(const Challenge& c, int levelNumber) {
    const int maxAttempts = 3;
    const int wrongPenalty = 15;
    const int hintCost = 25;

    int points = 100;
    int attemptsLeft = maxAttempts;
    bool hintShown = false;

    cout << "\n==============================\n";
    cout << " Level " << levelNumber << ": " << c.title << "\n";
    cout << "==============================\n";
    cout << c.description << "\n\n";
    cout << "Ciphertext: " << c.ciphertext << "\n";

    while (attemptsLeft > 0) {
        cout << "\n[Attempts left: " << attemptsLeft << " | Points available: " << points << "]\n";
        string input = readLine("Your answer ('hint' costs " + to_string(hintCost) +
                                " pts, 'quit' to leave): ");
        string upper = toUpper(input);

        if (upper == "QUIT") {
            return {Outcome::Quit, 0};
        }

        if (upper == "HINT") {
            if (hintShown) {
                cout << "You already used the hint.\n";
            } else {
                hintShown = true;
                points = max(0, points - hintCost);
                cout << "Hint: " << c.hint << "\n";
            }
            continue;
        }

        if (input.empty()) {
            cout << "Please type an answer.\n";
            continue;
        }

        if (upper == c.answer) {
            cout << "Correct! You earned " << points << " points.\n";
            return {Outcome::Solved, points};
        }

        --attemptsLeft;
        points = max(0, points - wrongPenalty);
        cout << "Wrong answer.\n";
    }

    cout << "Out of attempts! The answer was " << c.answer << ".\n";
    return {Outcome::Failed, 0};
}

// Plays through all levels. A failed level ends the game.
int playGame(mt19937& rng) {
    vector<Challenge> challenges = buildChallenges(rng);
    int totalScore = 0;
    size_t solved = 0;

    for (size_t i = 0; i < challenges.size(); ++i) {
        ChallengeResult result = playChallenge(challenges[i], static_cast<int>(i) + 1);

        if (result.outcome == Outcome::Quit) {
            cout << "\nYou left the game early.\n";
            break;
        }
        if (result.outcome == Outcome::Failed) {
            cout << "\nGAME OVER.\n";
            break;
        }

        ++solved;
        totalScore += result.points;
        cout << "Total score so far: " << totalScore << "\n";
    }

    if (solved == challenges.size()) {
        cout << "\nYou cracked every cipher!\n";
    }
    return totalScore;
}

// Appends "name,score" to scores.txt so results persist between runs.
void saveScore(const string& name, int score) {
    ofstream out("scores.txt", ios::app);
    if (!out) {
        cout << "Warning: could not save score to scores.txt\n";
        return;
    }
    out << name << "," << score << "\n";
}

// Free-play tool for trying the ciphers on any text.
void cipherTool() {
    while (true) {
        cout << "\n--- Cipher Tool ---\n"
             << "1) Caesar   2) Atbash   3) Vigenere   4) Back\n";
        string choice = readLine("Choose: ");

        if (choice == "1") {
            string text = readLine("Text: ");
            int shift = readInt("Shift (any whole number): ");
            bool decrypt = askDecrypt();
            string result = decrypt ? caesarDecrypt(text, shift) : caesarEncrypt(text, shift);
            cout << "Result: " << result << "\n";
        } else if (choice == "2") {
            string text = readLine("Text: ");
            cout << "Result: " << atbash(text) << "\n";
        } else if (choice == "3") {
            string text = readLine("Text: ");
            string key = readLine("Keyword (letters only): ");
            if (!isAlphaOnly(key)) {
                cout << "The keyword must contain letters only.\n";
                continue;
            }
            bool decrypt = askDecrypt();
            cout << "Result: " << vigenere(text, key, decrypt) << "\n";
        } else if (choice == "4") {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

// ============================================================
//  Main menu
// ============================================================

int main() {
    cout << "*******************************\n"
         << "         CIPHER CASCADE\n"
         << "*******************************\n";

    string name = readLine("Enter your name: ");
    if (name.empty()) name = "Player";

    random_device rd;
    mt19937 rng(rd());

    while (true) {
        cout << "\n1) Play game   2) Cipher tool   3) Quit\n";
        string choice = readLine("Choose: ");

        if (choice == "1") {
            int score = playGame(rng);
            cout << "\n" << name << ", your final score is " << score << ".\n";
            saveScore(name, score);
        } else if (choice == "2") {
            cipherTool();
        } else if (choice == "3") {
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    cout << "Thanks for playing!\n";
    return 0;
}