/*
* Author: Dylan Bonin
* Assignment Title: Huffmann Encoding
* Assignment Description: This program uses the Huffman encoding method to compress
*     text files and decompress Huffman encrypted files into text files.
* Due Date: 2/19/2023
* Date Created: 2/13/2023
* Date Last Modified: 2/18/2023
 */

/*
 * Data Abstraction:
 *         The user inputs a command (-huff or -unhuff) and the names of two files.
 *         The source file is entered first followed by the destination file. Data is
 *         abstracted for the source file.
 * Input:
 *         The program reads input in the format "-huff <source> <destination>
           or "-unhuff <source> <destination>" from the command line.
 * Process:
 *         If huffing, characters are read from the text file and are used to create a
 *         Huffman tree. If the compressed file will have less bytes than the original
 *         file, it will start encrypting. First a magic number will be written to the
 *         the destination, along with the number of characters, the Huffman tree, and
 *         lastly the encryption. If unhuffing, the magic number is checked to ensure
 *         the file was encrypted by this program. If it was, it will begin decrypting
 *         by reading the data, constructing the Huffman tree, and reading in bytes
 *         that will be converting to an encryption string, which will be converted to
 *         characters.
 * Output:
 *         Outputs error messages if the file can not be compressed or if it was not
 *         encrypted using this program. Otherwise, no direct output, but the correct
 *         data will be outputted to the destination file.
 * Assumptions:
 *         It is assumed that the user only wants to huff/unhuff one file at a time.
 *         It is assumed that the user only wants to huff/unhuff text files.
 *         It is assumed that the user use the command line in the format
 *         "-huff <source> <destination> or "-unhuff <source> <destination>".
 */

#include <iostream>
#include <queue>
#include <map>
#include <fstream>

using namespace std;

//node class used to create nodes for the Huffman tree
struct node {
    int count; //variable for the char frequency
    char c; //the char of the node
    node *left, *right; //the left and right children of the node
    
    /*
     * description: default constructor for the node
     * return: none
     * precondition: none, lptr and rptr are null by default
     * postcondition: creates node with the specificed lptr and
     *                rptr (or null by default) that has count
     *                0 and stores the '\0' character
     *
    */
    node(node *lptr = nullptr, node* rptr = nullptr) {
        count = 0;
        c = '\0';
        left = lptr;
        right = rptr;
        
        //if given left and right child, make the count equal to their sum
        if (left && right) {
            count = left->count + right->count;
        }
    }
};

//class used to compare two nodes
class cmp_node {
public:
    /*
     * description: this function compares two nodeds, returns true
     *              if first node has higher frequency than the second
     * return: true if node a has the higher count, false otherwise
     * precondition: node* a and b exist and are valid
     * postcondition: returns true if a has a higher count data member,
     *                false otherwise
     *
    */
    bool operator()(node *a, node *b) {
        return a->count > b->count;
    }
};

/*
 * description: converts a string that represents an 8 bit
 *              binary number into an actual byte (unsigned char).
 * return: unsigned char
 * precondition: codeString has size 8 and represents an 8 bit binary
                 number
 * postcondition: returns a byte (unsigned char) that was represented
 *                by the string.
 *
*/

unsigned char convertToByte (string codeString) {
    unsigned char number = 0; //initalizing byte

    //if there is a 1 in the string, we shift a 1
    //onto the byte. Otherwise, leave it be.
    for(int i=7; i>=0; i--)
    {
        unsigned char temp = 1;
        if (codeString[i] == '1')
        {
            temp <<= (7-i);
            number |= temp;
        }
    }
    
    return number;
}

/*
 * description: uses Huffman tree to create encodings for each character
 * return: void (N/A)
 * precondition: root is the root node of the huffman tree, str is valid,
 *               myMap is valid
 * postcondition: no return, but encodeTable will be populated with strings
                  of the encodings
 *
*/

void encodeData(node* root, string str, map<char, string>& myMap) {
    
    //if not null, insert the path we used to get to the root
    if (root == NULL) {
        return;
    }
    if (root->c != '\0') {
        myMap.insert(pair<char, string>(root->c, str));
    }
    
    //use recursion to traverse the tree and add to encode string
    encodeData(root->left, str + "0", myMap);
    encodeData(root->right, str + "1", myMap);
}

/*
 * description: uses Huffman tree to create decodings for each character
 * return: void (N/A)
 * precondition: root is the root node of the huffman tree, str is valid,
 *               myMap is valid
 * postcondition: no return, but dencodeTable will be populated with strings
                  of the encodings
 *
*/

void dencodeData(node* root, string str, map<string, char>& myMap) {

    //if not null, insert the path we used to get to the root
    if (root == NULL) {
        return;
    }
    if (root->c != '\0') {
        myMap.insert(pair<string, char>(str, root->c));
    }
    
    //use recursion to traverse the tree and add to dencode string
    dencodeData(root->left, str + "0", myMap);
    dencodeData(root->right, str + "1", myMap);
}

/*
 * description: converts a byte to a string that represents that byte
 *                       in binary form.
 * return: returns the string that represents the byte
 * precondition: theByte is a valid byte
 * postcondition: returns string that represents the byte.
 *
*/

string byteToString(unsigned char theByte) {
    string theString;
    int output[8];
    
    //populate output with the bits from the char
    for (int i = 0; i < 8; ++i) {
      output[i] = (theByte >> i) & 1;
    }
    //add it to the string
    for (int i = 7; i >= 0; i--) {
        theString = theString + to_string(output[i]);
    }
    
    return theString;
}

/*
 * description: main driver for the program
 * return: returns 0 as an exit code
 * precondition: argc and argv represent the number of commands
 *               and array that holds the commands respectively
 * postcondition: 0 is returned at the end of the program.
 *
*/


int main(int argc, char** argv) {
    priority_queue<node*, vector<node*>, cmp_node> n; //priority
    //queue used to create huffman tree
    node* p; //temporary node p used to help construct huffman tree
    char ch; //temporary char to read in from file
    map<char, int> charMap; //map used to store chars with their
    map<string, char> dencodeTable; //map used to hold encoding with the encoding string
                                    //as the key
    //frequency
    ifstream myFile; //input file stream used to read from text
    //file
    fstream outputFile; //output file stream used to write to binary
    //file
    ofstream finalFile; //output file stream to write to textfile
    //while unhuffing
    int magicNum = 312341; //arbitrary random number used as the
    //magic number for our huffman encoding
    char eofChar = 13; //eof character to signify when we are done
    //reading from the binary file
    unsigned char myByte = 0; //temporary byte used to write bytes
    //to the binary file
    string command = argv[1]; //first command line argument
    string iFileName = argv[2]; //second command line argument
    string oFileName = argv[3]; //third command line argument
    map<char, int>::iterator itr; //iterator used to loop through char map;

    
    
    //if we are huffing
    if (command == "-huff") {
        map<char, string> encodeTable; //map used to hold encodings with char as the key
        int numByteOrig = 0; //int variable used to find number
        //of bytes in original file
        int numByteComp = 8; //int variable used to track number of
        //bytes in binary file
        //will start at 8 bytes because magic number(4) and numChar(4).
        string encodCountString = ""; //string used to represent
        //our encoding that we will write to binary file
        //used to find how many bytes we will write to binary file
        //map<char, int>::iterator itr; //iterator used to loop through char map;
        
        
        myFile.open(iFileName);
        
        //while reading from file
        while (myFile.get(ch)) {
            numByteOrig++;
            
            //if its not in map, add it
            if (charMap.find(ch) == charMap.end()) {
                charMap.insert(pair<char, int>(ch, 1));
            }
            
            //if its in the map, update its frequency
            else if (charMap.find(ch) != charMap.end()) {
                itr = charMap.find(ch);
                (*itr).second = (*itr).second + 1;
            }
        }
        
        //insert EOF character
        charMap.insert(pair<char, int>(eofChar, 1));
        numByteComp = numByteComp + charMap.size() * 5; //5 bytes for each
        // char/int pair we put in encoding file
        myFile.close();
        
        
        //create all the nodes in the tree and add to priority queue
        itr = charMap.begin();
        while (itr != charMap.end()) {
            p = new node();
            p->c = itr->first;
            p->count = itr->second;
            n.push(p);
            
            itr++;
        }
        
        
        node *n1, *n2; //temporary nodes used to read from top of queue
        //create huffman tree using the priority queue
        while (n.size() > 1) {
            n1 = n.top();
            n.pop();
            
            n2 = n.top();
            n.pop();
            p = new node(n1, n2);
            n.push(p);
        }
        
        //create our encoding map
        encodeTable.clear();
        encodeData(p, "", encodeTable);
        
        //reopen file to create encoding string representation
        myFile.open(iFileName);
        
        //create string and make it divisible by 8 (to represent bytes)
        while (myFile.get(ch)) {
            encodCountString = encodCountString + encodeTable[ch];
        }
        while ( (encodCountString.size() % 8) != 0) {
            encodCountString = encodCountString + 'd';
        }
        numByteComp = numByteComp + (encodCountString.size() / 8);
        myFile.close();
        
        //if our compressed file would be bigger than our original, don't compress
        if (numByteComp > numByteOrig) {
            cout << "File will not compress" << endl;
            return 0;
        }
        
        //starting encoding
        int mapSize = charMap.size(); //represents size of our character map
        //open binary file, write magic num and number of characters
        outputFile.open(oFileName, ios::out | ios::binary);
        outputFile.write((char*)&magicNum, sizeof(magicNum));
        outputFile.write((char*)&mapSize, sizeof(mapSize));
        
        //write huffman tree
        itr = charMap.begin();
        
        //write each character and its frequency to the binary file
        while (itr != charMap.end()) {
            outputFile.write((char*)&(itr->first), sizeof(itr->first));
            outputFile.write((char*)&(itr->second), sizeof(itr->second));
            
            itr++;
        }

        //read from original file again
        myFile.open(iFileName);
        string encodString = ""; //string variable used to represent the bytes we read from
        //the file. It will be used to write bytes to the file
        
        //write to compression file
        while (myFile.get(ch)) {
            //get the encoding for our character
            map<char, string>::iterator itr4 = encodeTable.find(ch);
            string chEncode = (*itr4).second;

            //if our encoding is greater than 8 characters long, split it and write
            //the first 8 to the binary file
            while ( encodString.size() >= 8) {
                string tempString = encodString.substr(8); //represents rest of string
                //after we cut it
                encodString = encodString.substr(0, 8); //repesents first 8 characters of string
                
                //convert string to byte and write it to file
                myByte = convertToByte(encodString);
                outputFile.write((char*)&(myByte), sizeof(myByte));
                encodString = tempString;
            }
            
            //if size is less than 8, add them
            if ( (encodString.size() + chEncode.size()) <= 8) {
                encodString = encodString + chEncode;
                
                //if its equal to 8, write it
                if (encodString.size() == 8) {
                    myByte = convertToByte(encodString);
                    outputFile.write((char*)&(myByte), sizeof(myByte));
                    encodString = "";
                }
            }
            
            //if combined size is greater than 8, cut it and write first 8 characters to file
            else if ( (encodString.size() + chEncode.size()) > 8) {
                for (int i = 0; i < chEncode.size(); i++) {
                    string str2 = chEncode.substr(0, i);
                    
                    if ( (encodString.size() + str2.size()) == 8) {
                        encodString = encodString + str2;
                        myByte = convertToByte(encodString);
                        string str3 = chEncode.substr(i); //temp string used to rest of string
                        //after we cut it
                        outputFile.write((char*)&(myByte), sizeof(myByte));
                        encodString = str3;
                        str3 = "";
                        str2 = "";
                        break;
                    }
                }
            }
            
            
        }
        myByte = 0; //reset byte

        //write eof character to file
        string eofString = encodeTable[eofChar]; //string to represent the eof character
        //if we can add it onto the current byte string, write it to file
        if (encodString.size() + eofString.size() == 8) {
            encodString = encodString.size() + eofString.size();
            myByte = convertToByte(encodString);
            outputFile.write((char*)&(myByte), sizeof(myByte));
        }
        //if it will be less than 8, add zeros onto the end of the byte
        else if ( (encodString.size() + eofString.size()) < 8) {
            encodString = encodString.size() + eofString.size();
            
            while (encodString.size() < 8) {
                encodString.append("0");
            }
            
            myByte = convertToByte(encodString);
            outputFile.write((char*)&(myByte), sizeof(myByte));
        }
        //if it will be greater than 8, split it into two string
        else if (encodString.size() + eofString.size() > 8 ) {
            for (int i = 0; i < eofString.size(); i++) {
                string str2 = eofString.substr(0, i);
                
                if ( (encodString.size() + str2.size()) == 8) {
                    encodString = encodString + str2;
                    string str3 = eofString.substr(i); //temporary string to store other half
                    myByte = convertToByte(encodString);
                    outputFile.write((char*)&(myByte), sizeof(myByte));
                    //if it's less than 8, add zeroes to make it byte size
                    while (str3.size() < 8) {
                        str3.append("0");
                    }
                    myByte = convertToByte(str3);
                    outputFile.write((char*)&(myByte), sizeof(myByte));
                    break;
                }
            }
        }
        
        //close the files
        myFile.close();
        outputFile.close();
    }
    
    
    
    //UNHUFF
    //reading decompressed file
    else if (command == "-unhuff") {
        outputFile.open(iFileName, ios::in | ios::binary);
        int firstNum; //variable to read the first number
        int numLets; //variable to read number of characters
        char chaa; //temporary character to store read letters
        int freqq; //temporary int to read frequency
        map<string, char> dencodeTable; //map used to hold encoding
        //with the encoding string as the key
        
        //read the magic number, see if it matches
        outputFile.read((char*)&firstNum, sizeof(firstNum));
        
        //if it doesn't match, end the program.
        if (firstNum != magicNum) {
            cout << "Input file was not Huffman Endoded." << endl;
            return 0;
        }
        
         
        //read in number of characters
        outputFile.read((char*)&numLets, sizeof(numLets));

        //read in each character and its frequency, put it into a map
        for (int i = 0; i < numLets; i++) {
            outputFile.read((char*)&chaa, sizeof(chaa));
            outputFile.read((char*)&freqq, sizeof(freqq));
            charMap.insert(pair<char, int>(chaa, freqq));
        }
        
        
        map<char, int>::iterator itr = charMap.begin(); //iterator variable to iterate
                                                         //through map
        
        //add nodes to priority queue to create Huffman tree
        while (itr != charMap.end()) {
            p = new node();
            p->c = itr->first;
            p->count = itr->second;
            n.push(p);
            
            itr++;
        }
        
        
        node *n3, *n4; //temporary node variables used to create huffman tree
        
        //create huffman tree by combining nodes with smallest values
        while (n.size() > 1) {
            n3 = n.top();
            n.pop();
            
            n4 = n.top();
            n.pop();
            p = new node(n3, n4);
            n.push(p);
        }
        
        //reset string and create dencodeTable
        dencodeTable.clear();
        dencodeData(p, "", dencodeTable);
        
        
        //unsigned char myByte = 0; //
        string byteString; //create temporary string to represent bytes
        string readingString; //create string that represents current reading of
                              //the encoding
        //read the first byte character from the file
        //and convert it to a string
        outputFile.read((char*)&myByte, sizeof(myByte));
        byteString = byteToString(myByte);
        
        
        //open the destination file
        finalFile.open(oFileName);
        
        //read the encryption and convert it to characters to write to output file
        while (ch != eofChar) {
            //add bytestring to readingstring, see if we encounter a letter
            for (int i = 0; i < byteString.size(); i++) {
                readingString = readingString + byteString[i];
                
                if (dencodeTable.find(readingString) != dencodeTable.end()) {
                    ch = dencodeTable[readingString];
                    if (ch == eofChar) {
                        break;
                    }
                    //output the character to the file, reset string
                    finalFile << ch;
                    readingString = "";
                }
            }
            
            //end if eof character
            if (ch == eofChar) {
                break;
            }
            
            //reset string, read next byte, convert it to string
            byteString = "";
            outputFile.read((char*)&myByte, sizeof(myByte));
            byteString = byteToString(myByte);
        }
        
        //close both files
        finalFile.close();
        outputFile.close();
    }

    
    return 0;
}
