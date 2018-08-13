#pragma once
#include<iostream>
#include"Structs.h"
#include<fstream>
#include<cstring>
#include<string>

using namespace std;

class Directory
{
private:
	string SizeTotal();
	METADATA* metadataDisc;
	string discName, actualUbicationString;
	int initialPositionForDirectory;
	int initialPositionForBloque;
	int initialPositionForMapByte;
	int directoryActualInBytes;
	int	 disponibleByteInMapBits();
	bool isFather;
	string SizeDisk();
	string SizeArchiveOrDirectory(int bytes);
	int DisponibleBloque();
	int DisponibleDirectory();
	int ExistDirectory(string directoryName, int father);
	int ReturnFirstDataBlockIfExistArchive(string name, int father);
	int ExistArchive(string name, int father);
	int GetBit(unsigned char byte, int position);
	int OfBite(unsigned char byte, int position);
	int OnBite(unsigned char byte, int position);
	void UpdateDirection(FILE_ENTRY newPosition, int directoryPosition);
	void BackDirectory();
	void PrintBitMap();
	int CantidadDeBloquesDisponibles();
	bool DeleteRec(int initialPosition);
public:
	int DeleteArchive(int initialPosition);
	void test();
	Directory(string discName);
	void PrincipalMain();
	bool CreateDirectory();
	bool ChangeDirectory();
	void DirectoriesandArchivesList(int father);
	bool DeleteDirectoryOrArchive(string nameToDelete);
	bool ImportArchive();
	bool ExportArchive();
	void ReadMetadataDisc();
	void DiscName(string name);
};