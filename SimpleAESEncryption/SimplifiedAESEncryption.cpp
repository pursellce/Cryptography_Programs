/*
Author: Connor Pursell
Date: 12/8/22
Description: Program to encrypt a user provided plaintext file with a 16 character user provided key using a 
simplified version of AES. While encrypting the plain text, the program writes the output following each step 
of encryption to both the screen and a user specified output file including the final ciphertext produced by 
the encryption process.*/

#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <ctype.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

//Function to perform substituion using the Vigenere Cipher
string &vigCipher(string &text, string &key, string &subResult, int MAX_KEY_LENGTH){
    char s;
    int j = 0;
    for(int i = 0; i < text.length(); i++){
        if(j == MAX_KEY_LENGTH){
            j = 0;
        }
        s = (text[i]+key[j])%26;
        s +='A';
        subResult += s;
        j++;
    }
    return subResult;
}
//Function to to reverse characters in a string -- helper function for shiftRow function
void reverse(string &text, int start, int end){
    while(start < end){
        char temp = text[start];
        text[start] = text[end];
        text[end] = temp;
        start++;
        end--;
    }
}
//Function to perform a circular left shift of a given shift value on a string by first reversing the whole string,
//then reversing the shift value # of charaters at then end of the string, and finally reversing the remaining characters.
void shiftRow(string &row, int shiftVal){
    reverse(row, 0, shiftVal-1);
    reverse(row, shiftVal, row.length()-1);
    reverse(row, 0, row.length()-1);

}
//Function to determine parity of a character's numerical value, and if parity is odd change most significant bit to make it even.
bool checkParity(char c){
    int parity = 0;
    while(c){
        parity += (c&1);
        c = c>>1;
    }
    if(parity%2 != 0){
        return false;
    }
    else{
        return true;
    }
}
//Function to perform multiplication on a number x with y = 1, 2, or 3 in Rijndael's Galois field.
int rgfMul(int x, int y){
    int result;
    if(y == 1){
        result = x;
        return result;
    }
    else{
        result = x << 1;
        if(y == 3){
            result = result^x;
        }
        if(x >= 128){
            result = result^27;
        }
        if(result >=256){
            result -= 256;
        }
        return result;
    }
    return -1;
}

int main(){
    int MAX_FILE_NAME = 100;           // max possible length of file name
    int MAX_KEY_LENGTH = 16;           // max # of characters in key
    int BLOCK_SIZE = 16;               // size of blocks used in encryption steps d)-f)
    int BLOCK_ROW = 4;                 // # of rows/columns in block
    char inputFile[MAX_FILE_NAME];     // char array to hold user inputted plaintext file name
    char keyFile[MAX_FILE_NAME];       // char array to hold user inputted key file name
    char outputFile[MAX_FILE_NAME];    // char array to hold user inputted ciphertext output file name
    char c;                            // char used to read in plaintext file and in preprocessing step
    string key;                        // string to hold the key read from user inputted file
    string plaintext;                  // string to hold the plaintext read from upser inputted file
    string preproctext;                // string to hold the result of preprocssing step
    string subbedtext;                 //string to hold result of Vigenere cipher substitution step

    //Get input and output file names from user
    cout<<"Enter the name of the input plaintext file: ";
    cin>>inputFile;
    cout<<"Enter the name of the input key file: ";
    cin>>keyFile;
    cout<<"Enter the name of the output ciphertext file: ";
    cin>>outputFile;

    //open key input file and output file
    ifstream fin (keyFile);
    ofstream fout (outputFile);
    //If unable to open input key file, print error message
    if(!fin.is_open()){
        cout<<"Coule not open file: "<<keyFile<<endl;
        return 0;
    }
    //Read in and store Key
    getline(fin, key);
    fin.close();
    //switch to reading inputFile, print error message if unable to open file
    fin.open(inputFile);
    if(!fin.is_open()){
        cout<<"Coule not open file: "<<inputFile<<endl; 
        return 0;
    }
    //Read in and store each char of input plaintext file, excluding newline characters
    while(fin.get(c)){
        if(c != '\n'){
            plaintext+=c;
        }
    }
    //close ifstream
    fin.close();

//////////////////////BEGIN ENCRYPTION//////////////////////////
////////a) Preprocessing: Remove any punctuation and whitespace characters from input;
    //For each char of plaintext string, copy to preproctext only if uppercase letter A-Z (ASCII value between 65 and 90, inclusively)   
    for(int i = 0; i < plaintext.length(); i++){
        c = plaintext[i];
        if((c >= 65) && (c <= 90)){
            preproctext += c;
        }
    }
    //Print results of Preprocessing to screen and output file
    cout<<endl<<"Preprocessing:";
    fout<<endl<<"Preprocessing:";
    for(int i = 0; i < preproctext.length(); i++){
        if (i%80 == 0){
            cout<<endl;
            fout<<endl;
        }
        cout<<preproctext[i];
        fout<<preproctext[i];
    }
    cout<<endl;
    fout<<endl;

///////b) Substitution: Perform polyalphabetic substitution on input using the Vigenere cipher with the key provided
    //Call vigCipher function to perform substitution and store results in subbedText string
    subbedtext = vigCipher(preproctext, key, subbedtext, MAX_KEY_LENGTH);
    //Print results of Substitution to screen and output file.
    cout<<endl<<"Substitution:";
    fout<<endl<<"Substitution:";
    for(int i = 0; i < subbedtext.length(); i++){
        if(i%80 == 0){
            cout<<endl;
            fout<<endl;
        }
        cout<<subbedtext[i];
        fout<<subbedtext[i];
    }
    cout<<endl;
    fout<<endl;

///////c) Padding: Place input into 4x4 blocks for following encryption steps, padding input if necessary
    //Check if subbedText is divisible by BLOCK_SIZE(4x4), if not then pad input
    int remainder = subbedtext.length()%BLOCK_SIZE;
    if(remainder != 0){
        int padding = BLOCK_SIZE - remainder;
        for(int i = 0; i < padding; i++){
            subbedtext +='A';
        }
    }
    //Determine # of 4x4 blocks needed and put padded input into block array.
    int blockNum = subbedtext.length()/BLOCK_SIZE;
    string blocks[blockNum][BLOCK_ROW];
    for(int i = 0; i < blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            blocks[i][j] = subbedtext.substr((i*BLOCK_SIZE)+(j*BLOCK_ROW), BLOCK_ROW);
        }
    }   
    //Print results of Padding
    cout<<endl<<"Padding:"<<endl;
    fout<<endl<<"Padding:"<<endl;
    for(int i = 0; i < blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            cout<<blocks[i][j]<<endl;
            fout<<blocks[i][j]<<endl;
        }
        cout<<endl;
        fout<<endl;
    }
  
///////d) ShiftRows: Circularly shift the rows of each block as follows: First row remains the same, second
    //row shifts one position to the left, third row shifts two to the left, fourth row shifts three to the left.
    //For each block, call shiftRow function
    for(int i = 0; i < blockNum; i++){
        for(int j = 1; j < BLOCK_ROW; j++){
            shiftRow(blocks[i][j], j);
        }
    }
   //Print results of ShiftRows to screen and output file
   cout<<"Shift Rows: "<<endl;
   fout<<"Shift Rows: "<<endl;
   for(int i = 0; i < blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            cout<<blocks[i][j]<<endl;
            fout<<blocks[i][j]<<endl;
        }
        cout<<endl;
        fout<<endl;
    }
   
///////e) Parity Bit: Convert each character to its numerical value and use the most significant bit as a parity bit
    //Check parity of each char of input and store results in parityResults array
    int parityResults[blockNum][BLOCK_ROW][BLOCK_ROW];
    for(int i = 0; i <blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            for(int k = 0; k < BLOCK_ROW; k++){
               if(checkParity(blocks[i][j][k])){
                    parityResults[i][j][k] = blocks[i][j][k];
               }
               else{
                    parityResults[i][j][k] = (blocks[i][j][k]|0x80);
               }
            }
        }
    }

    //Print results of Parity Bit to screen and output file
    cout<<"Parity Bit:\n";
    fout<<"Parity Bit:\n";
    for(int i = 0; i <blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            for(int k = 0; k < BLOCK_ROW; k++){
                cout<<hex<<parityResults[i][j][k]<<" ";
                fout<<hex<<parityResults[i][j][k]<<" ";
            }
            cout<<endl;
            fout<<endl;
        }
    }
    cout<<endl;
    fout<<endl;

//////f) Mix Columns: Diffuse the data by multiplying(in the Rijndael's Galois field) the circulant MDS matrix with each column.
    int mixColumnsResults[blockNum][BLOCK_ROW][BLOCK_ROW];
    int MDSMatrix[BLOCK_ROW][BLOCK_ROW] = {
        {2, 3, 1, 1},
        {1, 2, 3, 1},
        {1, 1, 2, 3},
        {3, 1, 1, 2}
    };
    int temp[BLOCK_ROW];
    for(int i = 0; i < blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            for(int k = 0; k < BLOCK_ROW; k++){
                for(int l = 0; l < BLOCK_ROW; l++){
                    temp[l] = rgfMul(parityResults[i][l][j], MDSMatrix[k][l]);
                }
                mixColumnsResults[i][k][j] = temp[0]^temp[1];
                mixColumnsResults[i][k][j] = mixColumnsResults[i][k][j]^temp[2];
                mixColumnsResults[i][k][j] = mixColumnsResults[i][k][j]^temp[3];
            }
        }
    }
    //Print results of Mix Columns to screen and output file
    cout<<"Mix Columns:\n";
    fout<<"Mix Columns:\n";
    for(int i = 0; i < blockNum; i++){
        for(int j = 0; j < BLOCK_ROW; j++){
            for(int k = 0; k < BLOCK_ROW; k++){
                cout<<hex<<mixColumnsResults[i][j][k]<<" ";
                fout<<hex<<mixColumnsResults[i][j][k]<<" ";
            }
            cout<<endl;
            fout<<endl;
        }
    }
    cout<<endl;
    fout<<endl;
    //Encryption complete, close ofstream and end program
    fout.close();
    return 0;
}