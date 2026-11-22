# Cipher Cascade

A text-based cryptography game written in C++. Players decode messages encrypted with Caesar, Atbash, and Vigenere ciphers to progress through four levels and earn points.

Built for CS110 (Fundamentals of Computer Programming), Lab 08: Open Ended Lab, BESE-17A.

## Requirements

- A C++17 compiler (g++ or clang++)
- A terminal

## Build

```bash
g++ -std=c++17 -Wall -Wextra -o cipher_cascade main.cpp
```

## Run

```bash
./cipher_cascade
```

On Windows with MinGW, use `cipher_cascade.exe` instead.

## How to Play

1. Enter your name when prompted. Scores are saved under this name.
2. From the main menu, choose:
   - **1) Play game**: solve four cipher levels in order.
   - **2) Cipher tool**: encrypt or decrypt any text (free play).
   - **3) Quit**
3. In a level, read the description and ciphertext, then type your answer in uppercase or lowercase.

### Commands during a level

| Input  | Effect                                              |
|--------|-----------------------------------------------------|
| `hint` | Reveals a hint. Costs 25 points. Can be used once per level. |
| `quit` | Leaves the game. Points from levels already solved are kept. |

### Scoring

- Each level starts at **100 points**.
- Each wrong answer costs **15 points**.
- Using the hint costs **25 points**.
- You get **3 attempts** per level. Running out of attempts ends the game.
- Your total score is the sum of points from solved levels. It is saved to `scores.txt`.

### Levels

1. **Caesar Cipher**: letters are shifted forward by a stated amount. Shift them back.
2. **Atbash Cipher**: A becomes Z, B becomes Y, and so on. Decrypting works the same way as encrypting.
3. **Vigenere Cipher**: each letter is shifted by the matching letter of a keyword. The keyword repeats. Spaces and punctuation do not advance the keyword.
4. **Brute Force**: a Caesar cipher with an unknown shift. The hint reveals the first letter of the word.

Words and keywords are chosen at random, so each game is different.

### Cipher Tool

The cipher tool lets you try the ciphers on any text:

- **Caesar**: enter text and any whole-number shift. Choose encrypt or decrypt.
- **Atbash**: enter text. Encryption and decryption are the same operation.
- **Vigenere**: enter text and a keyword (letters only). Choose encrypt or decrypt.

Non-letter characters are never changed by the ciphers.

## Project Structure

```
.
├── main.cpp      # All game logic, cipher functions, and menus
├── README.md     # This file
└── Cipher_Cascade_Lab08_Report.pdf  # Game concept, design decisions, and concepts used
```

`scores.txt` is created in the working directory the first time a game ends.

## Known Limitations

- Input is read line by line. Typing `hint` in a level does not count as an answer attempt.
- The Caesar shift in Level 4 is not shown, so the player must try shifts in the cipher tool to find the word.
- Scores are stored in plain text and are not checked for tampering.

## Team

| Member | CMS ID | Contribution |
|--------|--------|--------------|
| Syed Muhammad Aliyan Ali | 585981 | Game idea and concept |
| Affan Akbar | 592135 | Scoring mechanism |
| Muhammad Yahya Shahzad | 604750 | Overall functionality |
