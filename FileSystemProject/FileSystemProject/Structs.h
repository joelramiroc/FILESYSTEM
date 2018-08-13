#pragma once

struct DATA_BLOCK
{
	char DATA[1020];
	int siguiente = -1;
};

struct FILE_ENTRY
{
	char Name[30];
	int Size = -1,
		Father = -2,
		First_Son = -1,
		Right_Brother = -1,
		First_Data_Bloque = -1;
	char Type[1];
	bool Libre = true;
};

struct METADATA
{
	int Bit_Mat_Size;
	int Entry_Amount;
	int Block_Size = 1024;
	int Block_Amount;
	int Disc_Size;
};

struct FAT_VARIABLES
{
	int Cant_Inputs,
		Cant_Data_Bloques;
};

struct BitMap
{
	char * size;
};

