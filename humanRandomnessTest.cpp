/*
Tell the user they are going to try to make a random sequence of 24 0/1s (or coin flips, left right mixups, ect). 
Note: a single 0/1 in a sequence will be called a flip.
Tell the user that they are then going to be given part of a sequence from someone else that was made in the same way they made their sequence.
They will be shown 8 of the flips in order and will be asked to guess the 9th flip, after they guess the next flip is revealed.
They repeat this process on the 10th flip, 11th flip, and so on until they have guessed 8 flips
(8 numbers guessed, 16 flips shown including the last one they guessed).
Tell them they will do this twice (or some multiple of two), then they will be given some score based on how well they guessed.

What the program will actually do is randomly choose half of the sequences from a database of user generated sequences.
The other half of the sequences will be randomly generated without telling the user that it is random.

The program requires two tables of data. The program needs read and write access for both tables. 
Each flip should be stored as one bit, so a sequence of 8 should be one byte.
1st table contains user made sequences. It should be something that allows fast random access.
2nd table contains the sequence that the user was shown, the user's guess, and if the sequence was randomly generated or not.
My guess for the best way to do this would be to have the 8 revealed flips seperate from the first 8 shown flips.
If you xor the sequence of user guesses with the revealed flips then that gets you correctness for each guess.

Right now this is a simple proof of concept program. 
It works, but it is not efficient or user friendly.

Contact me if you do something with this, quickest way is through twitter @nodthenbow
*/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <random>
#include <string>
#include <ctime>
#include <cmath>
#include <cstdio>

const unsigned int amountOfSequences = 4;
const unsigned int userGeneratedSequenceLength = 24;
const unsigned int userGuessLength = 8;
const unsigned int sequenceHeaderLength = 8;
const unsigned int sequenceRevealLength = 8;

class sequenceHolder
{
private:
	int currentIndex;
	std::string seqHeader;
	std::string seqReveal;
	std::string seqUserGet;
	bool isUserMade;
public:
	sequenceHolder() {
		currentIndex = 0;
		seqHeader = "";
		seqReveal = "";
		seqUserGet = "";
		isUserMade = 0;
	};
	int getCurrentIndex(){
		return currentIndex;
	}
	std::string getSeqHeader(){
		return seqHeader;
	}
	std::string getSeqReveal(){
		return seqReveal;
	}
	std::string getSeqUserGet(){
		return seqUserGet;
	}
	bool getIsUserMade(){
		return isUserMade;
	}
	std::string getCurrentlyRevealed(){
		return seqHeader + seqReveal.substr(0,currentIndex);
	}
	
	//adds the param to the guess string then returns the next char to be revealed or e when there are no more reveals
	char saveGuessAndUpdate(char a){
		if(currentIndex >= seqReveal.size()){
			return 'e';
		}
		seqUserGet += a;
		return seqReveal[currentIndex++];
	};
	//saves the param and return 1 if correct guess, 0 if incorrect, -1 if there is nothing to compare it to
	int saveGuessAndCheck(char a){
		if(currentIndex > seqReveal.size()){
			return -1;
		}
		seqUserGet += a;
		return a == seqReveal[currentIndex++];
	};
	//todo error check
	bool checkGuessAt(int index){
		return seqUserGet[index] == seqReveal[index];
	}
	//make generic? make everything into bits so it can be speed with bitwise xor?
	std::string getResultsSoFar(){
		std::string a = "";
		for (int i = 0; i < currentIndex; i++){
			a += std::to_string(std::to_string(seqHeader[i]) == std::to_string(seqUserGet[i]));
		}
		return a;
	}

	//todo replace with generic callbacks and what not
	void getUserMadeSequence(unsigned int index, unsigned int offset, std::fstream & sqT){
		//Replace with a generic alg that works with the format of the data in the database or something
		//todo error checking 
		std::string a;
		sqT.seekg(index*userGeneratedSequenceLength);
		std::getline(sqT,a);
		seqHeader = a.substr(0,sequenceHeaderLength);
		seqReveal = a.substr(sequenceHeaderLength,sequenceRevealLength);
		isUserMade = 1;
		currentIndex = 0;
		seqUserGet = "";
	};
	//todo allow a user defined source of randomness
	void getRandomSequence(unsigned int seed){
		std::srand(seed);
		seqHeader = "";
		for(int i = 0; i < sequenceHeaderLength; i++){
			seqHeader += std::to_string(rand()%2);
		}
		seqReveal = "";
		for(int i = 0; i < sequenceRevealLength; i++){
			seqReveal += std::to_string(rand()%2);
		}
		isUserMade = 0;
		currentIndex = 0;
		seqUserGet = "";
	}
	//todo replace with generic callbacks and what not
	void updateResultsTableWithResults(std::fstream & toPut){
		//Replace with generics, make sure guesses are actually completed too
		std::string a = seqUserGet + ',' + seqReveal + ',' + seqHeader + ',' + std::to_string(isUserMade) + '\n';
		toPut << a;
	};
	//for quick and dirty single string formatting (\n at start and end)
	std::string prettyBlockFormat(){
		std::string a = "";
		a += "Sequence:\n" + getCurrentlyRevealed() + "\nGuesses:";
		a += seqUserGet + "\nCorrect:" + getResultsSoFar() + "\n";
		return a;
	}
	std::string prettyBlockFormatSpoilers(){
		std::string a = prettyBlockFormat();
		a += "R:" + std::to_string(isUserMade) + '\n';
		return a;
	}
};

//replace with database calls
bool postSequence(std::fstream &fs, std::string s){
	if(s.size() == userGeneratedSequenceLength){
		fs << s + '\n';
		return 1;
	}
	return 0;
}

//for example only
bool doSeq(std::fstream &squ){
	std::cout << "Choose a 0 or 1 " << userGeneratedSequenceLength << " times then hit enter" << std::endl;
	std::string a = "";
	std::cin >> a;
	if (a.size() < userGeneratedSequenceLength){
		std::cout << "\nnot enough numbers" << std::endl;
		return 0;
	}
	for(int i = 0; i < a.size(); i++){
		if(a[i] != '1' && a[i] != '0'){
			std::cout << "\nnot 0 or 1 at " << i << std::endl;
			return 0;
		}
	}
	if(a.size() != userGeneratedSequenceLength){
		a = a.substr(0,userGeneratedSequenceLength);
		std::cout << "Sequence too long, truncating to: " << a << std::endl;
	}
	if(postSequence(squ,a)){
		std::cout << "Saved sequence" << std::endl;
		return 1;
	}
	std::cout << "Nope?" << std::endl;
	return 0;
}
bool doPaRe(std::fstream & sq, std::fstream & re){
	std::vector<sequenceHolder> H;
	srand(time(NULL));
	sequenceHolder t;
	int j = 0;
	int h = 0;
	for(int i = 0; i < amountOfSequences/2; i++){
		j = rand()%std::max((int)((int)(sq.gcount())/(userGeneratedSequenceLength+1)),1);
		h = rand()%std::max((int)(userGeneratedSequenceLength-sequenceHeaderLength-sequenceRevealLength),1);
		t.getUserMadeSequence(j,h,sq);
		H.push_back(t);
		t.getRandomSequence(time(NULL)+i);
		H.push_back(t);
	}
	std::random_shuffle(H.begin(), H.end());
	int guessesH = 0;
	int correctH = 0;
	int guessesR = 0;
	int correctR = 0;
	int r = 0;
	char guess = '1';
	std::string deb = "";
	for(int i = 0; i < H.size(); i++){
		for(int h = 0; h < sequenceRevealLength; h++){
			std::cout << std::endl << "make your next guess" << std::endl;
			deb = H[i].getCurrentlyRevealed();
			std::cout << deb << std::endl;
			deb = "";
			for(int j = 0; j < sequenceHeaderLength; j++) deb+= " ";
			deb += H[i].getSeqUserGet();
			std::cout << deb;
			std::cout.flush();
			std::cin >> guess;
			r = H[i].saveGuessAndCheck(guess);
			if(r >= 0){
				if(H[i].getIsUserMade()){
					guessesH++;
					correctH += r;
				}
				else {
					guessesR++;
					correctR += r;
				}
			}
			else {
				return 0;
			}
		}
		H[i].updateResultsTableWithResults(re);
	}
	for(int i = 0; i < H.size(); i++){
		std::cout << H[i].prettyBlockFormatSpoilers();
	}
	std::cout << std::endl;
	std::cout << "You got " << correctH+correctR << "/" << guessesH+guessesR << " correct" << std::endl;
	std::cout << "You got " << correctH << "/" << guessesH << " human made ones correct" << std::endl;
	return 1;
}
int main(){
	std::fstream sequencesTable;
	std::fstream resultsTable;
	std::cout << "(1) Both make a sequence and test your pattern recognition skills,\n(2) Only make a sequence,\n(3) Only test your pattern recognition skills" << std::endl;
	int uc;
	std::cin >> uc;
	if(uc == 1){
		if (!resultsTable.is_open()){
			resultsTable.open("reDB.txt",std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::app);
		}
		if (!sequencesTable.is_open()){
			sequencesTable.open("sqDB.txt",std::fstream::in | std::fstream::out | std::fstream::app);
		}
		if(!doSeq(sequencesTable)){
			return 1;
		}
		if(!doPaRe(sequencesTable,resultsTable)){
			return 1;
		}
		
	}
	else if (uc == 2){
		if (!sequencesTable.is_open()){
			sequencesTable.open("sqDB.txt",std::fstream::in | std::fstream::out | std::fstream::app);
		}
		if(!doSeq(sequencesTable)){
			return 1;
		}
	}
	else if (uc == 3){
		if (!resultsTable.is_open()){
			resultsTable.open("reDB.txt",std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::app);
		}
		if (!sequencesTable.is_open()){
			sequencesTable.open("sqDB.txt",std::fstream::in | std::fstream::out | std::fstream::app);
		}
		if(!doPaRe(sequencesTable,resultsTable)){
			return 1;
		}
	}
	else {
		std::cout << "Idk what you chose" << std::endl;
		return 0;
	}

}