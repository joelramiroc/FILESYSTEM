#include "stdafx.h"
#include "Disc.h"


Disc::Disc()
{

}

bool Disc::CreateDisc()
{
	string discName;
	cout << "Ingrese nombre del Disco:" << endl;
	cin >> discName;
	if (ExistDisc(discName))
	{
		cout << "Ya existe un disco con ese nombre" << endl;
		return false;
	}

	createMetaDataDisk(discName);
	cout << "Se creo exitosamente el nuevo disco" << endl;
	return true;
}

bool Disc::createMetaDataDisk(string discName)
{

	ofstream disc(discName + ".dat", ios::out | ios::binary);
	if (!disc.is_open())
	{
		disc.open(discName + ".dat");
	}

	METADATA *metadata = new METADATA();
	cout << "Ingrese cantidad de entradas que permitira" << endl;
	cin >> metadata->Entry_Amount;
	cout << "Ingrese cantidad de bloques que permitira" << endl;
	cin >> metadata->Block_Amount;
	metadata->Bit_Mat_Size = metadata->Block_Amount / 8;
	metadata->Block_Amount = metadata->Bit_Mat_Size * 8;

	disc.seekp(0, ios::end);
	disc.write(reinterpret_cast<char*>(metadata), sizeof(METADATA));
	//bitmap
	int t = 0;
	for (int i = 0; i < metadata->Bit_Mat_Size; i++) {
		disc.write(reinterpret_cast<char*>(&t), 1);
	}

	//fe
	for (int i = 0; i < metadata->Entry_Amount; i++) {
		FILE_ENTRY *fileEntryEscribir = new FILE_ENTRY();
		disc.write((char*)fileEntryEscribir, sizeof(FILE_ENTRY));
	}
	//bloques

	for (int i = 0; i < metadata->Block_Amount; i++) {
		DATA_BLOCK *dataEscribir = new DATA_BLOCK();
		disc.write((char*)dataEscribir, sizeof(DATA_BLOCK));
	}

	disc.close();
	return true;
}

bool Disc::ExistDisc(string directoryName)
{
	string temporalName = directoryName + ".dat";
	ifstream file(temporalName, ios::in | ios::binary);
	if (!file)
		return false;
	return true;
}
