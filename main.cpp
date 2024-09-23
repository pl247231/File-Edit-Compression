#include<iostream>
#include<fstream>
#include<vector>
#include<list>
#include<utility>
#include<functional>
#include<algorithm>
#include<string>

using namespace std;

class Hashset {
public:
	Hashset() : table(10) {
		elements = 0;
		buckets = 10;
	}
	bool contains(int key) {
		list<int>& bucket = table[Hashfunction(to_string(key)) % buckets];
		for (int element : bucket) {
			if (element == key) {
				return true;
			}
		}
		return false;
	}
	void insert(int key) {
		if (!contains(key)) {
			table[Hashfunction(to_string(key)) % buckets].push_back(key);
			elements++;
			nums.push_back(key);
		}
		if ((double)elements / buckets > 0.75) {
			resize();
		}
	}
	void resize() {
		//make new table with bigger capacity
		vector<list<int>> newtable(2 * buckets);
		//rehash all elements
		for (int num : nums) {
			newtable[Hashfunction(to_string(num)) % (2 * buckets)].push_back(num);
		}
		buckets *= 2;
		table = newtable;
	}
	int size() {
		return elements;
	}
	bool empty() {
		return elements == 0;
	}
	int getelement(int index) {
		return nums[index];
	}
private:
	vector<list<int>> table;
	int elements;
	int buckets;
	//allows fast iteration through elements
	vector<int> nums;
	size_t Hashfunction(string key) {
		return hash<string>()(key);
	}
};

class Map {
public:
	Map() : table(1000) {
		numelements = 0;
		buckets = 1000;
	}
	Hashset& get(string key) {
		//return empty Hashset if not found
		list<pair<string, Hashset>>& bucket = table[Hashfunction(key) % buckets];
		for (auto& element: bucket) {
			if (element.first == key) {
				return (element.second);
			}
		}
		pair<string, Hashset> element = { key, Hashset() };
		bucket.push_back(element);
		return (bucket.back()).second;
	}
	void resize() {
		//make new table with bigger capacity
		vector<list<pair<string, Hashset>>> newtable(2 * buckets);
		//rehash all elements
		for (auto list : table) {
			for (auto element : list) {
				newtable[Hashfunction(element.first) % (2 * buckets)].push_back(element);
			}
			
		}
		buckets *= 2;
		table = newtable;
	}
private:
	vector<list<pair<string, Hashset>>> table;
	int numelements;
	int buckets;
	size_t Hashfunction(string key) {
		return hash<string>()(key);
	}
};

bool getInt(istream& inf, int& n)
{
	char ch;
	if (!inf.get(ch) || !isascii(ch) || !isdigit(ch))
		return false;
	inf.unget();
	inf >> n;
	return true;
}

bool getCommand(istream& inf, char& cmd, char& delim, int& length, int& offset)
{
	//returns false if command is incorrectly formatted
	if (!inf.get(cmd))
	{
		cmd = 'x';  // signals end of file
		return true;
	}
	switch (cmd)
	{
	case '+':
		return inf.get(delim).good();
	case '#':
	{
		char ch;
		return getInt(inf, offset) && inf.get(ch) && ch == ',' && getInt(inf, length);
	}
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

bool instring(char ch, string str) {
	for (char c : str) {
		if (c == ch) {
			return true;
		}
	}
	return false;
}

string adddelim(string str) {
	//use char that doesnt appear in string as delimiter
	char delim = 0;
	while (instring(delim, str)) {
		delim++;
	}
	return delim + str + delim;
	
}

void createRevision(istream& fold, istream& fnew, ostream& frevision) {
	string oldfile, newfile;
	char readin;
	//read in files to strings
	while (fold.get(readin))
	{
		oldfile += readin;
	}
	while (fnew.get(readin)) {
		newfile += readin;
	}

	Map ngrams;

	//add all substrings of length 8 into map
	for (int i = 0; i < oldfile.length() - 8 + 1; i++) {
		ngrams.get(oldfile.substr(i, 8)).insert(i);
	}

	//string that holds consective chars that need to be added, uses one + instruction instead of multiple
	string toadd = "";

	//iterate through newstring and attempt to find matches
	int index = 0;
	while(index < newfile.length()) {
		if (!(ngrams.get(newfile.substr(index, 8)).empty())){
			//if found
			if (toadd != "") {
				//add chars that are non copyable
				frevision << "+" << adddelim(toadd);
				toadd = "";
			}
			int longestmatch = 0;
			int indexoflongest;
			for (int i = 0; i < ngrams.get(newfile.substr(index, 8)).size(); i++) {
				int indexinold = ngrams.get(newfile.substr(index, 8)).getelement(i);
				int indexofendold = indexinold + 8;
				int indexofendnew = index + 8;
				int extralength = 0;
				//continue pairing if the strings continue matching
				while (indexofendnew < newfile.length() && indexofendold < oldfile.length() && oldfile[indexofendold] == newfile[indexofendnew]) {
					extralength++;
					indexofendnew++;
					indexofendold++;
				}
				if (longestmatch < 8 + extralength) {
					longestmatch = 8 + extralength;
					indexoflongest = indexinold;
				}
				else if(longestmatch == 8 + extralength) {
					indexoflongest = min(indexoflongest, indexinold);
				}
			}
			//take the longest match
			frevision << "#" << indexoflongest << "," << longestmatch;
			index += longestmatch;
		}
		else{
			//add current char to string of non copyable chars
			toadd += newfile[index];
			index++;
		}
	}
	if (toadd != "") {
		//add non copyable chars
		frevision << "+" << adddelim(toadd);
	}
}

bool revise(istream& fold, istream& frevision, ostream& fnew) {
	char cmd, delim, readin;
	int length, offset;
	string oldfile;
	//read old file into a string
	while (fold.get(readin))
	{
		oldfile += readin;
	}
	while (true) {
		if (!getCommand(frevision, cmd, delim, length, offset)) {
			//incorrectly formatted command
			return false;
		}
		else if(cmd == 'x'){
			//end of file
			return true;
		}
		else if (cmd == '+') {
			char temp;
			bool reacheddelim = false;
			while (frevision.get(temp)) {
				//write characters until delim is reached
				if (temp == delim) {
					reacheddelim = true;
					break;
				}
				fnew << temp;
			}
			if (!reacheddelim) {
				//if no second delim found, invalid
				cout << delim;
				cout << "asd";
				return false;
			}
		}
		else {
			//command == #
			if (offset >= oldfile.length() || offset + length > oldfile.length()) {
				//invalid offset or length, outside of range of file
				return false;
			}
			fnew << oldfile.substr(offset, length);
		}
	}
	return true;
}

int main() {
	bool createrevise = 1;
	if (createrevise) {
		ifstream oldfile("original.txt", ios::binary);
		ifstream newfile("edited.txt", ios::binary);
		ofstream revfile("revise.txt", ios::binary);
		createRevision(oldfile, newfile, revfile);
	}
	else { //create edited file
		ifstream infile("original.txt", ios::binary);
		ifstream revisefile("revise.txt", ios::binary);
		ofstream outfile("edited.txt", ios::binary);   // outfile is a name of our choosing.
		cout << revise(infile, revisefile, outfile);
	}
}
