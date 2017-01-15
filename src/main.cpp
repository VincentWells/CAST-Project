#include "graph.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <istream>

using namespace std;

//find the proportion of item in a vector inside a given range relative to that range
double overlap(vector<int>& v, int s, int e){
	int count = 0;
	for(int i = 0; i < v.size(); i++){
		if(v[i] >= s && v[i] < e){
			count ++;
		}
	}
	//cout << "count " << count << endl;
	return (double)(count) / (e - s);
}

//unused method
double loose(vector<double>& v){
	double d = 0;
	for(int i = 0; i < v.size(); i++){
		d += v[i];
	}
	return 1 - d;
}

//print comparison of results
void print(graph& g, vector<int>& v){
	cout << v.size() << endl;
	cout << "theta: " << g.theta << "\t\t\t";
	for(int i = 0; i < g.cliques.size(); i++){
		cout << "CAST cluster " << i + 1 << "\t";
	}
	cout << endl;
	for(int j = 1; j < v.size(); j++){
		cout << "real cluster " << j << "\t\t";
		for(int k = 0; k < g.cliques.size(); k++){
			//cout << "real " << v[j-1] << " thru  " << v[j] << endl;
			cout << overlap(g.cliques[k], v[j-1], v[j]) * 100.0 << "\t\t";
		}
		cout << endl;
	}
}

int main(){
	string addr;
	cout << "provide file address: " << endl;
	cin >> addr;
	// "/home/me/C++ Projects/hw3/vincentWells/src/clusters.txt"
	ifstream file(addr.c_str());
	if(!file.is_open()){
		cout << "Error: invalid file address." << endl;
		return 0;
	}
	vector<vector<string> > v (1);
	vector<int> indices(0);
	int i = -1;
	int j = 0;
	for(string temp; getline(file, temp);){
		//cout << "got line" << endl << flush;
		//cout << temp << endl << flush;
		if(temp.substr(0, 8) == "template"){
			indices.push_back(j);
			i++;
			vector<string> u(0);
			v.push_back(u);
		} else if (temp.substr(0, 8) == "sequence"){
		} else if(temp.size() > 2){
			v[i].push_back(temp);
			j++;
		}
	}
	indices.push_back(j);
	file.close();
	//cout << "pre graph" << endl << flush;
	graph g(v[0]);
	for(int i = 1; i < v.size(); i++){
		g.add(v[i]);
	}
	g.enscore();
	g.connect();
	//cout << "graph built" << endl << flush;
	g.CAST();
	//cout << "graph cliqued" << endl << flush;
	g.print();
	//cout << "graph finished" << endl << flush;
	print(g, indices);
	return 0;
}
