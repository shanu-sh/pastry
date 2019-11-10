#include <bits/stdc++.h>
std::vector<string> splitstring(string str,char delim)
{
	std::vector<string> vec;
	int i=0;
	string s="";
	while(i<str.length())
	{
		if(str[i]==delim)
		{
			vec.push_back(s);
			s="";
			i++;
		}
		else
		{
			s=s+str[i];
			i++;
		}
	}
	vec.push_back(s);
	return vec;
}
