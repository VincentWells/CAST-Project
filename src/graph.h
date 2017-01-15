#include <vector>
#include <string>

class graph{
private:
	void score(int);
	std::vector<std::string> nodes; //list of sequences
	std::vector<std::vector<std::pair<int, double> > > edges; //0th item is list of nodes 0th node connects to, etc.
	std::vector<std::vector<double> > scores;
	double avg_score(std::vector<int>&, int);
	double avg_score(std::vector<int>&, std::string&, int);
	bool AddBest(std::vector<int>&, std::vector<int>&, std::vector<int>&, std::vector<int>&);
	bool RemWorst(std::vector<int>&);
protected:
public:
	constexpr static double theta = 0.75;
	void enscore();
	void add(std::vector<std::string>&);
	void connect();
	void print();
	std::vector<std::vector<int> > clusters;
	std::vector<std::vector<int> > cliques;
	void encluster();
	void CAST();
	graph(std::vector<std::string>&);
};
