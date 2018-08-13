#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include "Structs.h"
#include<string>

using namespace std;
class Disc
{
private:
	string discName;
public:
	Disc();
	bool CreateDisc();
	bool ExistDisc(string directoryName);
	bool createMetaDataDisk(string fileName);
};
