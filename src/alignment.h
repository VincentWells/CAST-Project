#include <string>
#include <vector>

class alignment{
private:
	void gridInit(std::vector<std::vector<int> >&, std::string*, std::string*);
	void gridInitZero(std::vector<std::vector<int> >&);
	void graphInit(std::vector<std::vector<std::pair<int, int> > >&);
	std::vector<std::vector<std::vector<int> > > edges;
	const static int mu = -8;
	const static int sigma = -4;
	const static int epsilon = -3;
protected:
public:
	int max;
	std::vector<std::vector<std::pair<int, int> > > nodes;
	void reset();
	int getScore();
	void print(std::vector<std::vector<std::pair<int, int> > >&);
	std::pair<std::string, std::string> traverse(std::vector<std::vector<std::pair<int, int> > >&, std::string*, std::string*);
	void init(std::string*, std::string*);
	void populate();
	alignment(std::string*, std::string*);
	alignment();
};
