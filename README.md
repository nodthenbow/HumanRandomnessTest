# HumanRandomnessTest
Simple program to test how random people are and how well they can see patterns in other's randomness
Copied from the cpp file:
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
