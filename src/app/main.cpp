#include "mainQgis.h"
#include <iostream>
#include <fstream>
#include <QString>
bool isAuthenticated();

using namespace std;

int main(int argc, char * argv[])
	{
		if(isAuthenticated())
		{
			mainQgis mqgis(argc,argv);
		}
			
	
	return 0;
	}
	
bool isAuthenticated()
	{
		char* logFileUri="../python/logfile";
		std::string line;
		std::string loginMsg="user is authenticated";
		bool auth;
		
		ifstream logFile(logFileUri);
		
		if(logFile.is_open())
		{
			while(getline(logFile,line))
			{
				if(line==loginMsg)
				{
					auth= true;
				}
				else auth= false;
			}
		}
		return auth;
	}
