#ifndef BASEFUNCTION_H
#define BASEFUNCTION_H

#include <string>
#include <sstream>
#include <vector>
using namespace std;

// Definition
int CountSpaceNum(string buffer);
vector<string> &split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);


// Implement
int CountSpaceNum( string buffer )
{
	int count=0;
	for (int i=0;i<buffer.size();i++)
	{
		if (buffer[i]==' ')
			count++;
	}
	return count;
}

// c++ split ·µ»Øvector<string>
vector<string> &split(const string &s, char delim, vector<string> &elems)
{     
	stringstream ss(s);     
	string item;     
	while(getline(ss, item, delim)) 
	{         
		elems.push_back(item);     
	}     
	return elems; 
}   

vector<string> split(const string &s, char delim)
{     
	vector<std::string> elems;     
	return split(s, delim, elems); 
}

#endif