//
//  main.cpp
//  encoder
//
//  Created by Sakib Rabbany on 2017-03-24.
//  Copyright © 2017 Sakib Rabbany. All rights reserved.
//

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>
#include <functional>
#include <map>
#include <fstream>
#include <string>

using namespace std;


struct Tree {
    int c;
    Tree* left;
    Tree* right;
};


struct Container {
    int weight;
    Tree* trie;
};

class ComparisonClass {
public:
    bool operator() (Container first, Container second) {
        return first.weight > second.weight;
    }
};

bool isLeaf (Tree* t) {
    return (t->right == NULL) && (t->left == NULL);
}

Tree* makeNode (int c, Tree* left, Tree* right) {
    Tree* retNode = new Tree();
    retNode->c = c;
    retNode->left = left;
    retNode->right = right;
    return retNode;
}

void initializeMinHeap(priority_queue<Container, vector<Container>, ComparisonClass>& minHeap) {
    for (int i = 0 ; i < 128 ; i++) {
        Tree* node = makeNode(i, NULL, NULL);
        int key = 1;
        Container cont = Container();
        cont.weight = key;
        cont.trie = node;
        minHeap.push(cont);
    }
}

void deleteMinHeap(priority_queue<Container, vector<Container>, ComparisonClass>& minHeap) {
    while (!minHeap.empty()) {
        Container first = minHeap.top();
        cout << "key: " << first.weight << " " << first.trie->c << endl;
        delete first.trie;
        minHeap.pop();
    }
}


Tree* makeHuff (priority_queue<Container, vector<Container>, ComparisonClass >& minHeap) {
    while (1) {
        if (minHeap.size() == 1) break;
        Container top = minHeap.top();
        Tree* firstTrie = top.trie;
        int firstWeight = top.weight;
        minHeap.pop();
        top = minHeap.top();
        Tree* secondTrie = top.trie;
        int secondWeight = top.weight;
        Tree* newRoot = makeNode(-1, firstTrie, secondTrie);
        int newWeight = firstWeight + secondWeight;
        minHeap.pop();
        Container cont = Container();
        cont.weight = newWeight;
        cont.trie = newRoot;
        minHeap.push(cont);
    }
    Tree* htrie = minHeap.top().trie;
    minHeap.pop();
    return htrie;
}


void deleteTree(Tree* huffTrie){
    if( huffTrie != NULL ) {
        deleteTree(huffTrie->left);
        deleteTree(huffTrie->right);
        //int k = huffTrie->c;
        //if (k != -1) cout << k << endl;
        delete huffTrie;
    }
}

void initializeFreq (vector <int>& freq) {
    for (int i = 0 ; i < 128 ; i++) {
        freq.push_back(0);
    }
}



void getCode (Tree* trie, map<int, vector<int> >& code, vector <int>& bits) {
    if (trie->left) {
        vector <int> leftVec = bits;
        leftVec.push_back(0);
        getCode(trie->left, code, leftVec);
    }
    if (trie->right) {
        vector <int> rightVec = bits;
        rightVec.push_back(1);
        getCode(trie->right, code, rightVec);
    }
    if (isLeaf(trie)) {
        pair<int, vector<int> > pr = make_pair(trie->c, bits);
        code.insert(pr);
    }
}

string getStr (int n) {
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}


void printBits (vector <int>& bits, string& output) {
    for (int i = 0 ; i < bits.size() ; i++) {
        int bit = bits[i];
        string bitStr = getStr(bit);
        output += bitStr;
    }
}



void readInput (string& input) {
    string line;
    while (getline(cin, line)){
        input += line;
        input += '\n';
    }
}

void printVec (vector<int> vec) {
    for (int i = 0 ; i < vec.size() ; i++) {
        cout << vec[i] << endl;
    }
}

void writeFreq (vector<int> vec) {
    ofstream freqFile ("huff.freq");
    for (int i = 0 ; i < vec.size() ; i++) {
        if (i == vec.size() - 1) {
            freqFile << vec[i];
        } else {
            freqFile << vec[i] << endl;
        }
    }
    freqFile.close();
}

void printMap(map<int ,vector<int> >& symTable){
    map<int ,vector<int> >::iterator it;
    for (it = symTable.begin() ; it !=symTable.end() ; it++){
        vector <int> bits = (*it).second;
        for (int i = 0 ; i < bits.size() ; i++) {
            cout << bits[i];
        }
        cout << endl;
    }
}

void writeCode(map<int ,vector<int> >& code){
    int count = 0;
    ofstream codeFile ("huff.code");
    map<int ,vector<int> >::iterator it;
    for (it = code.begin() ; it != code.end() ; it++){
        vector <int> bits = (*it).second;
        for (int i = 0 ; i < bits.size() ; i++) {
            codeFile << bits[i];
        }
        if (count == 127) {
            continue;
        } else {
            codeFile << endl;
            count++;
        }
    }
    codeFile.close();
}



void makeMinHeapWithFrequency (priority_queue<Container, vector<Container>, ComparisonClass >& minHeap, vector <int>& freq) {
    for (int i = 0 ; i < 128 ; i++) {
        Tree* node = makeNode(i, NULL, NULL);
        int key = freq[i];
        Container cont = Container();
        cont.weight = key;
        cont.trie = node;
        minHeap.push(cont);
    }
}

void encodeInitial (vector <int>& freq, map <int, vector <int> >& code, string::iterator& inputIter, string& input, string& output) {
    int charCounter = 0;
    int charMax = 128;
    while (1) {
        if (charCounter == charMax) break;
        if (inputIter == input.end()) {
            throw string("EOF");
        }
        int index = static_cast<unsigned int>(*inputIter);
        map <int, vector <int> >::iterator mapIter;
        mapIter = code.find(index);
        vector<int> bits = (*mapIter).second;
        printBits(bits, output);
        freq[index]++;
        charCounter++;
        inputIter++;
    }
}

void getNewCode (vector <int>& freq,
                 map <int, vector <int> >& code,
                 priority_queue<Container, vector<Container>, ComparisonClass >& minHeap)
{
    makeMinHeapWithFrequency(minHeap, freq);
    Tree* huffTrie = makeHuff(minHeap);
    code.clear();
    vector <int> bits;
    getCode(huffTrie, code, bits);
    bits.clear();
    deleteTree(huffTrie);
}


void encodeRest (vector <int>& freq,
                 map <int, vector <int> >& code,
                 string::iterator& inputIter,
                 string& input,
                 priority_queue<Container, vector<Container>, ComparisonClass >& minHeap,
                 string& output)
{
    // t1
    getNewCode(freq, code, minHeap);
    ofstream huffCode;
    int counter = 0;
    int maxCount = 128;
    while (1) {
        if (counter == maxCount) {
            // t1++
            getNewCode(freq, code, minHeap);
            maxCount *= 2;
            counter = 0;
        }
        if (inputIter == input.end()) {
            throw string("EOF");
        }
        int index = static_cast<unsigned int>(*inputIter);
        map <int, vector <int> >::iterator mapIter;
        mapIter = code.find(index);
        vector<int> bits = (*mapIter).second;
        printBits(bits, output);
        freq[index]++;
        counter++;
        inputIter++;
    }
}

void encode (vector <int>& freq,
             map <int, vector <int> >& code,
             string::iterator& inputIter,
             string& input,
             priority_queue<Container, vector<Container>, ComparisonClass >& minHeap,
             string& output){
    encodeInitial(freq, code, inputIter, input, output);
    encodeRest(freq, code, inputIter, input, minHeap, output);
}

void writeWpl (vector <int>& freq, map <int, vector <int> >& code, string& input) {
    ofstream wplFile ("huff.wpl");
    if (input.size() < 128) {
        wplFile << 896;
    } else {
        int wpl = 0;
        map <int, vector <int> >::iterator codeIter;
        for (codeIter = code.begin() ; codeIter != code.end() ; codeIter++) {
            int index = (*codeIter).first;
            vector <int> bits = (*codeIter).second;
            int length = int(bits.size());
            wpl += freq[index] * length;
        }
        wplFile << wpl;
    }
    wplFile.close();
}


void writeEncode (string& output) {
    cout << output << endl;
    ofstream encodeFile ("code.txt");
    encodeFile << output;
    encodeFile.close();
}

int main(int argc, const char * argv[]) {
    string output = "";
    map <int, vector <int> > code;
    vector <int> freq;
    initializeFreq(freq);
    
    priority_queue<Container, vector<Container>, ComparisonClass > minHeap;
    initializeMinHeap(minHeap);
    
    
    // t0
    Tree* huffTrie = makeHuff(minHeap);
    vector <int> bits;
    getCode(huffTrie, code, bits);
    deleteTree(huffTrie);
    bits.clear();

    string input = "";
    readInput (input);
    string::iterator inputIter = input.begin();
    
    
    
    try {
        //throws when input runs out
        encode(freq, code, inputIter, input, minHeap, output);
    } catch (const string& msg) {
        writeFreq(freq);
        writeCode(code);
        writeWpl(freq, code, input);
        writeEncode(output);
    }
    
    return 0;
}

