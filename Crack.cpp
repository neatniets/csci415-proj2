#include <string.h>
#include <iostream>
#include <fstream>

#include "rainbow.hpp"

const int CHAIN = 15;

int main( int argc, char **argv){

	char pass[11];
	salt_t salt;
	char *temp[2];
	int i = 0;
	int counter = 0;
	const char *test;
	char *red;
	int difCH = 0;
	int track = 0;
	std::string rain;
	std::string rainVal[100];
	std::string startVal[100];

	std::cout << "Enter a password that's less than 10 characters long that will be hashed: " << std::endl;
	std::cin >> pass;

	std::cout << "Enter a salt value between 0-4095: " << std::endl;
	std::cin >> salt;

	temp[0] = hash(salt,pass);

	std::cout << "The hashed value of the password + salt is: " << temp[0] << std::endl;
	std::cout << "Now cracking the hash with a rainbow table to find what the plaintext is..." << std::endl;

	std::ifstream rainTable("rainbow-table-16.txt");

	while(!rainTable.eof()){

		rainTable >> rain;                      //This gets the salt from the rainbow table

		if(rain!= std::to_string(salt)){        //if not the same salt as hashed value then ignore
			rainTable >> rain;              //next two values
			rainTable >> rain;
		}

		else{                           //salt is the same
			rainTable >> rain;
			rainVal[i] = rain;      //place rainbow password values into an arry
			rainTable >> rain;
			startVal[i] = rain;     // place startpoint values into an array
			i++;                    // keep this final value as to determine the end of the arrays
		}

	}

	rainTable.close();              //we don't need the file anymore

	counter = 0;
	int counterCH = CHAIN;

	while(counterCH >= 0){

		difCH = CHAIN - counterCH;                      //this is needed to keep track of how many times reduce function needs to be used

		if(difCH == 0){
			red = reduce(counterCH, temp[0]);       //test if the reduced of the hashed is the password
		}

		else{
			counter = counterCH;

			while(difCH > 0){                               //how many more times a value needs to be reduced and hashed
				if(counter == counterCH){               //this is checking if this is the first time reducing and hashing the original value
					red = reduce(counter, temp[0]);
					red = hash(salt, red);
				}
				else{                                   //this is for everytime after the first reduce and hash
					red = reduce(counter,red);
					red = hash(salt, red);
				}
				difCH--;
				counter++;

			}
			red = reduce(CHAIN, red);                       //the value needs to be reduced one last time to end the chain
		}

		counter = 0;
		track = 0;                                              //this is so it can track what positiion the reduced chain was equal to
		while(counter < i){                                     //check if the reduced-password-hash is equal to anything in the rainbow table

			if(rainVal[counter] == red){                    //if we find a match then stop both while loops;

				counterCH = -5;
				counter = i;
			}
			track++;
			counter++;
		}
		counter = 0;
		counterCH--;

	}

	if(counterCH < -1){                     //there is a match in the rainbow table that caused the while loop to end

		track--;
		counter = 0;
		test = startVal[track].c_str();         //gets the starting point that was in the rainbow table;

		red = hash(salt, test);                 //start hashing the startpoint

		while(counter < CHAIN){                 //hash chain the startpoint until the matching hash is found

			if(*red == *temp[0]){                   //if they equal right away then stop the while loop
				counter = CHAIN;


			}
			else{                                   //keep chaining until the matching hashes are found
				red = reduce(counter, red);
				test = red;
				red = hash(salt, red);
				counter++;
			}

		}
		std::cout<<"Program found the same plaintext in the rainbow table!!"<<std::endl;
		std::cout<<"The plaintext password is: " << test << std::endl;

	}

	else {
		std::cout<<"Password not found with rainbow table..." << std::endl;
	}


}



