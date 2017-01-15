#include "graph.h"
#include "alignment.h"
#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <ctime>
#include <thread>

using namespace std;

/*
  struct candidate{  //used to store candidate nodes
  int ni; //node index
  vector<int> parents;
  };
*/
graph::graph(vector<string>& v){
	//cout << "check1" << endl << flush;
	nodes.resize(0);
	edges.resize(0);
	cliques.resize(0);
	clusters.resize(0);
	//cout << "check2" << endl << flush;
	add(v);
//	enscore();
//	cout << "check3" << endl << flush;
//	connect();
	//cout << "check4" << endl << flush;
}

//calls the alignments of each pair of sequences using multithreading (4 threads).
void graph::enscore(){
	scores.resize(nodes.size());
	double s;
	for(int k = 0; k < scores.size(); k++){
		scores[k].resize(scores.size());
	}
	vector<thread> threads(4);
	for (int l = 0; l < threads.size(); l++){
		thread t(&graph::score, this, l);
		threads[l] = move(t);
	}
	for(auto& thread : threads){
		thread.join();
	}
	/*
	cout << "Score table:" << endl;
	for(int j = 0; j < scores[0].size(); j++){
		cout << "\t" << j;
	}
	cout << endl;
	for(int i = 0; i < scores.size(); i++){
		cout << i << "\t";
		for(int j = 0; j < scores[i].size(); j++){
			cout << scores[i][j] << "\t";
		}
		cout << endl;
	}
	*/
}

//this method will compute the necessary alignments. 4 instances should run concurrently on different sets
void graph::score(int s){
	for(int i = s; i < scores.size(); i += 4){
		scores[i][i] = 1;
		//cout << i << endl;
		for(int j = i + 1; j < nodes.size(); j++){
			//cout << "score" << " i " << i << " j " << j << endl << flush;
			//cout << nodes[i] << endl;
			//cout << nodes[j] << endl;
			alignment a(&nodes[i], &nodes[j]);
			a.populate();
			scores[i][j] = (double)(a.getScore() + 8 * a.max) / (double)(12 * a.max);
			scores[j][i] = scores[i][j];
			//cout << "score of i v j: " << i << " v " << j << " is " << scores[i][j] << endl;
		}
	}
	//cout << "scored" << " i " << i << " j " << j << endl << flush;
}

//method to add a list of nodes
void graph::add(vector<string>& v){
	for(int i = 0; i < v.size(); i++){
		nodes.push_back(v[i]);
	}
}

//method to connect nodes via edges
void graph::connect(){
	//cout << "connect 1" << endl << flush;
	edges.resize(nodes.size());
	double s;
	for(int i = 0; i < nodes.size(); i++){
		for(int j = 0; j < nodes.size(); j++){
			cout << "i: " << i << "\tj: " << j << endl << flush;
			if(i != j){
				s = scores[i][j];
				if(s >= theta){
					edges[i].push_back(make_pair(j, s));
				}
			}
		}
	}
	//cout << "connect 2" << endl << flush;
}

void graph::CAST(){
	if(nodes.size() == 0){
		cout << "Error: graph not built properly" << endl;
		return;
	}
	vector<int> filled (nodes.size(), 1);
	int n = 0;
	vector<int> c (0);
	bool finished = false;
	bool skip = false;
	while(!finished){
		skip = false;
		//cout << "loop start" << endl << flush;
		vector<int> used (nodes.size(), 1);
		//cout << "n: " << n << endl;
		//cout << "used: " << used.size() << endl;
		vector<int> cands (0);  //list of adjacent nodes that aren't in clust c
		c.push_back(n);
		used[n] = 2;
		//cout << "loop init" << endl << flush;
		for (int i = 0; i < edges[n].size(); i++){
			//cout << "test" << endl << flush;
			if(filled[edges[n][i].first] == 1 && used[edges[n][i].first] == 1){
				cands.push_back(edges[n][i].first);
				used[edges[n][i].first] = 0;
			}
		}
		if(cands.size() == 0){
			skip = true;
		}
		if(!skip){
			if(AddBest(cands, c, used, filled)){
				//cout << "loop inited" << endl << flush;
				//cout << "c size" << c.size() << endl;
				while((AddBest(cands, c, used, filled) || RemWorst(c)) && c.size() > 1) {
					//cout << "cluster size "<< c.size() << endl << flush;
				}
			}
		}
		//store the cluster
		cliques.push_back(c);
		//make the cluster's nodes are taken
		for(int k = 0; k < c.size(); k++){
			filled[c[k]] = 0;
			//cout << "c[k] " << c[k] << endl;
		}
		c.clear();
		//find the next node to put in a cluster
		finished = true;
		for (int j = 0; j < filled.size(); j++){
			//cout << filled[j] << endl;
			if (filled[j] == 1){
				n = j;
				finished = false;
				break;
			}
		}
		//cout << "loop end" << endl << flush;
	}
}

//basic funtion to add best valid candidates to the cluster
bool graph::AddBest(vector<int>& cands, vector<int>& contents, vector<int>& visited, vector<int>& filled){
	double max = 0;
	int node;
	double t;
	/*
	for(int i = 0; i < visited.size(); i++){
		cout << i << " : " << visited[i] << endl;
	}
	*/
	for(int i = 0; i < cands.size(); i++){
		t = avg_score(contents, nodes[cands[i]], cands[i]);
		//cout << "T " << t << endl;
		//cout << cands[i] << endl;
		if(t > max && visited[cands[i]] != 2){
			//cout << "might add" << endl;
			max = t;
			node = cands[i];
		}
	}
	if (max >= theta){
		contents.push_back(node);
		visited[node] = 2;
		//add new candidate nodes
		for(int j = 0; j < edges[node].size(); j++){
			if(visited[edges[node][j].first] == 1 && filled[edges[node][j].first] == 1){
				cands.push_back(edges[node][j].first);
				visited[edges[node][j].first] = 0;
			}
		}
		//cout << "adding " << node << endl;
		return true;
	} else {
		return false;
	}
}

//remove worst item from the cluster if it falls beneath the minimum theta
bool graph::RemWorst(vector<int>& v){
	double min = 1;
	double t;
	int index;
	string str;
	for(int i = 0; i < v.size(); i++){
		str = nodes[v[i]];
		t = avg_score(v, i);
		//cout << "t " << t << endl;
		if(min > t){
			min = t;
			index = i;
		}
	}
	if(min < theta){
		v.erase(v.begin() + index);
		return true;
	} else {
		return false;
	}
}

//returns average scores of a vector (note: str param is unused)
double graph::avg_score(vector<int>& v, string& str, int index){
	//string s = str;
	double score = 0;
	for(int i = 0; i < v.size(); i++){
		score += scores[index][v[i]];
	}
	return score / v.size();
}

//returns average scores of a vector, skipping given index
double graph::avg_score(vector<int>& v, int skip){
	string s = nodes[v[skip]];
	double score = 0;
	for(int i = 0; i < v.size(); i++){
		if(i != skip){
			score += scores[v[i]][v[skip]];
		}
	}
	return score / (v.size() - 1);
}

//unused function in current version
void graph::encluster(){
	vector<int> t (nodes.size(), 1);
	int i = 0;
	int e = 0;
	int p;
	bool empty;
	stack<int> s;
	while(true){
		vector<int> v;
		empty = true;
		for(int j = 0; j < t.size(); j++){
			if(t[j] != 0){
				e = j;
				empty = false;
				break;
			}
		}
		if(empty){
			break;
		}
		s.push(e);
		t[e] = 0;
		while(!s.empty()){
			p = s.size();
			for(int k = 0; k < edges[e].size(); k++){
				if(t[edges[e][k].first] != 0){
					s.push(edges[e][k].first);
					v.push_back(edges[e][k].first);
				}
			}
			if(p == s.size()){
				s.pop();
			}
			e = s.top();
		}
		clusters.push_back(v);
		i++;
	}
}

void graph::print(){
	for (int i = 0; i < cliques.size(); i++){
		cout << "Cluster " << i << " :\t";
		for(int j = 0; j < cliques[i].size(); j++){
			cout << cliques[i][j] << "\t";
		}
		cout << endl;
	}
}
