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


void printBits (vector <int>& bits, ostream& out) {
    for (int i = 0 ; i < bits.size() ; i++) {
        out << bits[i];
    }
}



void readInput (string& input) {
    ifstream encodedFile ("code.txt");
    getline(encodedFile, input);
    encodedFile.close();
}

void printVec (vector<int> vec) {
    for (int i = 0 ; i < vec.size() ; i++) {
        cout << vec[i] << endl;
    }
}

void printMap(map<int ,vector<int> >& symTable){
    map<int ,vector<int> >::iterator it;
    for (it = symTable.begin() ; it !=symTable.end() ; it++){
        printBits((*it).second, cout);
    }
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



Tree* getNewTree (vector <int>& freq,
                 priority_queue<Container, vector<Container>, ComparisonClass >& minHeap)
{
    makeMinHeapWithFrequency(minHeap, freq);
    Tree* huffTrie = makeHuff(minHeap);
    return huffTrie;
}

int getIndex (Tree* trie, string::iterator& inputIter) {
    int index;
    if (isLeaf(trie)) {
        index = trie->c;
    } else {
        char path = (*inputIter);
        if (path == '0') {
            inputIter++;
            index = getIndex(trie->left, inputIter);
        } else if (path == '1') {
            inputIter++;
            index = getIndex(trie->right, inputIter);
        }
    }
    return index;
}

void decodeInitial (Tree* trie, vector <int>& freq, string::iterator& inputIter, string& input) {
    int counter = 0;
    int maxCounter = 128;
    
    while (1) {
        if (counter == maxCounter) break;
        if (inputIter == input.end()) throw string("EOF");
        int index = getIndex(trie, inputIter);
        freq[index]++;
        counter++;
        char c = index;
        cout << c;
    }
}

void decodeRest (
                 vector<int>& freq,
                 string::iterator& inputIter,
                 string& input,
                 priority_queue<Container, vector<Container>, ComparisonClass >& minHeap)
{
    //map<int, vector<int> > code;
    Tree* huffTrie = getNewTree(freq, minHeap);
    //vector <int> bits;
    //getCode(, code, bits);
    //printMap(code);
    
    int counter = 0;
    int maxCounter = 128;
    
    while (1) {
        if (counter == maxCounter) {
            maxCounter *= 2;
            counter = 0;
            deleteTree(huffTrie);
            huffTrie = getNewTree(freq,  minHeap);
        }
        if (inputIter == input.end()) throw string("EOF");
        
        int index = getIndex(huffTrie, inputIter);
        freq[index]++;
        counter++;
        char c = index;
        cout << c;
    }
}

int main(int argc, const char * argv[]) {
    string input = "";
    readInput (input);
    string::iterator inputIter = input.begin();
    
    vector <int> freq;
    initializeFreq(freq);
    
    priority_queue<Container, vector<Container>, ComparisonClass > minHeap;
    initializeMinHeap(minHeap);

    Tree* huffTrie = makeHuff(minHeap);

    try {
        decodeInitial(huffTrie, freq, inputIter, input);
        deleteTree(huffTrie);
        decodeRest(freq, inputIter, input, minHeap);
    } catch (const string& msg) {
        deleteTree(huffTrie);
    }
    
    return 0;
}

