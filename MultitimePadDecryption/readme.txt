README
Author: Connor Pursell
Date: 11/15/22
Description: Program to decrypt at least 90% of the possible characters that can be decrypted in a set of 
60-character long ciphertexts that have been created using by reusing a one-time pad on multiple 60-character 
plaintexts. Program prompts user to enter the name of the file containing the ciphertexts and then prints the decrypted
message with '_' representing still encrypted characters.
Included are two python programs to generate cipher texts for testing.

To compile:
	g++ MultiTimePadExploitDecryption.cpp
To run:
	./a.out