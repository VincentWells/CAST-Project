#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include "alignment.h"
using namespace std;

void alignment::print(vector<vector<pair<int, int> > >& v){
	for(int i = 0; i < v.size(); i++){
		for(int j = 0; j < v[i].size(); j++){
			cout << v[i][j].first << ", " << v[i][j].second << "\t";
		}
		cout << endl;
	}
}

//initialize the edges to the proper values
void alignment::gridInit(vector<vector<int> >& v, string* seq1, string* seq2){
	//cout << "gi s" << endl;
	//cout << seq1->size() << ">=" << seq2->size() << endl << flush;
	//cout << v.size() << ">=" << v[0].size() << endl << flush;
	for(int i = 0; i < v.size(); i++){
		//cout << "gi loop" << endl;
		for(int j = 0; j < v[i].size(); j++){
			//cout << "i " << i << " " << v.size() << endl;
			//cout << "j " << j << " " << v[i].size() << endl;
			if(seq1->at(i) == seq2->at(j)){
				v[i][j] = 4;
			} else {
				v[i][j] = mu;
			}
		}
	}
}

//unused method, would reset values to 0
void alignment::gridInitZero(vector<vector<int> >& v){
	for(int i = 0; i < v.size(); i++){
		for(int j = 0; j < v[0].size(); j++){
			v[i][j] = epsilon;
		}
	}

}

//init the nodes to "empty" values
void alignment::graphInit(vector<vector<pair<int, int> > >& v){
	for(int i = 0; i < v.size(); i++){
		for(int j = 0; j < v[i].size(); j++){
			v[i][j].first = 0;
			v[i][j].second = -1;
		}
	}
}

//initialize the graph 
void alignment::init(string* seq1, string* seq2){
	if(seq1->size() < seq2->size()){
		alignment::max = seq2->size();
	} else {
		alignment::max = seq1->size();
	}
	//cout << "init s" << endl;
	int e = alignment::epsilon;
	//cout << "seq1 size " << seq1->size() << endl;
	edges.clear();

	//this code was originally designed to have 3 layers of edges for a different project.  In order to save time rewriting, the edges vector has an extra dimension of size 1.
	edges.resize(1, vector<vector<int> >(seq1->size(), vector<int>(seq2->size())));
	//cout << "x: " << edges.size() << " y: " << edges[0].size() << " z: " << edges[0][0].size() << endl;
	gridInit(edges[0], seq1, seq2);
	//cout << "init c0" << endl;
	//gridInitZero(edges[1]);
	//gridInitZero(edges[2]);
	//cout << "init c1" << endl;
	nodes.clear();
	nodes.resize(seq1->size() + 1, vector<pair<int, int> >(seq2->size() + 1, make_pair(0, -1)));
	//cout << "init c2" << endl;
	//graphInit(nodes);
	//cout << "init e" << endl;
}

//to reuse the same graph, this function deletes all edges and nodes.
void alignment::reset(){
	nodes.clear();
	edges.clear();
}

//computes the optimal alignment score
void alignment::populate(){
	int diagonal;
	int down;
	int right;
	//initialize
	//cout << "fill s" << endl;
	nodes[0][0].first = 0;
	nodes[0][0].second = 0;
	for(int i = 1; i < nodes[0].size(); i++){
		nodes[0][i].second = 2;
		if(nodes[0][i].second == nodes[0][i - 1].second){
			nodes[0][i].first = nodes[0][i - 1].first + epsilon;
		} else {
			nodes[0][i].first = nodes[0][i - 1].first + sigma;
		}
	}
	//fill in the rest of the graph
	for(int j = 1; j < nodes.size(); j++){
		nodes[j][0].second = 1;
		if(nodes[j][0].second == nodes[j - 1][0].second){
			nodes[j][0].first = nodes[j - 1][0].first + epsilon;
		} else {
			nodes[j][0].first = nodes[j - 1][0].first + sigma;
		}
		for(int k = 1; k < nodes[0].size(); k++){
			//cout << "y: " << j << endl;
			//cout << "x: " << k << endl;
			diagonal = nodes[j - 1][k - 1].first + edges[0][j - 1][k - 1];
			if(nodes[j][k - 1].second == 2){
				right = nodes[j][k - 1].first  + epsilon;
			} else {
				right = nodes[j][k - 1].first + sigma;
			}
			if(nodes[j - 1][k].second == 1){
				down = nodes[j - 1][k].first + epsilon;
			} else {
				down = nodes[j - 1][k].first + sigma;
			}
			if(diagonal >= down){
				if(diagonal >= right){
					nodes[j][k].first = diagonal;
					nodes[j][k].second = 0;
				} else {
					nodes[j][k].first = right;
					nodes[j][k].second = 2;
				}
			} else {
				if(down > right){
					nodes[j][k].first = down;
					nodes[j][k].second = 1;
				} else {
					nodes[j][k].first = right;
					nodes[j][k].second = 2;
				}
			}
			//cout << "added " << v[j][k].second << endl;
		}
	}
	//cout << "fill e" << endl;
}

//return the final score after population
int alignment::getScore(){
	//cout << "score: " << nodes[nodes.size() - 1][nodes[0].size() - 1].first << endl;
	if(nodes.size() == 0 || nodes[0].size() == 0){
		return 0;
	}
	return nodes[nodes.size() - 1][nodes[0].size() - 1].first;
}

//this method would be used to find the path which led to the optimal alignment; not usd in the CAST project
pair<string, string> alignment::traverse(vector<vector<pair<int, int> > >& v, string* seq1, string* seq2){
	stack<char> rstr1;
	stack<char> rstr2;
	//print(v);
	int y = v.size() - 1;
	int x = v[0].size() - 1;
	int p;
	char c;
	while(x != 0 || y != 0){
		//cout << "x: " << x << endl;
		//cout << "y: " << y << endl;
		p = v[y][x].second;
		//cout << "p: " << p << endl;
		if(p == 0){
			x--;
			y--;
			c = seq1->at(y);
			rstr1.push(c);
			c = seq2->at(x);
			rstr2.push(c);
		} else if(p == 2){
			x--;
			rstr1.push('-');
			c = seq2->at(x);
			rstr2.push(c);
		} else {
			y--;
			c = seq1->at(y);
			rstr1.push(c);
			rstr2.push('-');
		}
	}
	string str1;
	string str2;
	while(!rstr1.empty()){
		c = rstr1.top();
		str1 += c;
		rstr1.pop();
	}
	while(!rstr2.empty()){
		c = rstr2.top();
		str2 += c;
		rstr2.pop();
	}
	return make_pair(str1, str2);
}

alignment::alignment(string* s1, string* s2){
	string seq1 = *s1;
	string seq2 = *s2;
	//ensure that seq1 is always greater than seq2
	alignment::max = seq1.size();
	init(&seq1, &seq2);
	//populate(nodes);
	//pair<string, string> seqs = traverse(nodes, &seq1, &seq2);
}

alignment::alignment(){
	alignment::max = 0;
}

//test GCCCAGTTATGTCAGGGGGCACGAGCATGCAGA GCCGCCGTCGTTTTCAGCAGTTATGTCAGAT

