#include "stdafx.h"
#include "Directory.h"

Directory::Directory(string discName)
{
	this->actualUbicationString = discName + ":/";
	this->DiscName(discName);
	this->isFather = true;
	this->ReadMetadataDisc();
}

void Directory::PrincipalMain()
{

	int opcion;
	bool continuar = true;
	do
	{
		cout << "===========================================================================" << endl;
		cout << "Disco: " << discName.substr(0, discName.length() - 4) << "                " << "Utilizado: " << this->SizeDisk() << ".          Total: " << this->SizeTotal() << endl << endl;
		cout << "Directorio Actual: " << actualUbicationString << endl << endl << endl;

		cout << "Que desea hacer:" << endl;
		cout << "1. Crear Directorio" << endl;
		cout << "2. Cambiar Directorio" << endl;
		cout << "3. Listar Directorios y Archivos" << endl;
		cout << "4. Eliminar Directorio/Archivo" << endl;
		cout << "5. Importar Archivo" << endl;
		cout << "6. Exportar Archivo" << endl;
		cout << "7. Mostrar Mapa de Bits" << endl;
		cout << "8. Regresar" << endl;
		cout << "9. Salir al menu principal" << endl;
		cin >> opcion;
		cout << "===========================================================================" << endl << endl;
		switch (opcion) {
		case 1:
		{
			this->CreateDirectory();
		}
		break;
		case 2:
		{
			this->ChangeDirectory();
		}
		break;
		case 3:
		{
			this->DirectoriesandArchivesList(this->directoryActualInBytes);
		}
		break;
		case 4:
		{
			string nameToDelete;
			cout << "Ingrese nombre del Directorio/ Archivo a eliminar" << endl;
			cin >> nameToDelete;
			this->DeleteDirectoryOrArchive(nameToDelete);
		}
		break;
		case 5:
		{
			this->ImportArchive();
		}
		break;
		case 6:
		{
			this->ExportArchive();
		}
		break;
		case 7:
		{
			this->PrintBitMap();
		}
		break;
		case 8:
		{
			this->BackDirectory();
		}
		break;
		case 9:
		{
			continuar = false;
		}
		break;
		}
		cout << endl << "===========================================================================" << endl;
		cout << "=======E==S==T==R==U==C==T==U==R==A=====D==E=====D==A==T==O==S=====2=======" << endl;
	} while (continuar);
}

string Directory::SizeTotal()
{
	int t = this->initialPositionForMapByte;
	int converter = 0;
	converter += this->metadataDisc->Entry_Amount * sizeof(FILE_ENTRY);
	converter += this->metadataDisc->Bit_Mat_Size * sizeof(DATA_BLOCK);
	return this->SizeArchiveOrDirectory(converter);
}

int Directory::disponibleByteInMapBits()
{

	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	int posLeer = this->initialPositionForMapByte;
	int disp = -1, contara = 0, temporal = 0;
	char leer[1];
	int active = -1;

	while (!archive.eof() && contara<this->metadataDisc->Bit_Mat_Size)
	{
		archive.seekg(posLeer);
		archive.read(leer, 1);
		for (int i = 7; i >= 0; i--)
		{
			active = this->GetBit(*leer, i);

			if (active == 0 && disp == -1)
			{
				return posLeer;
			}
			temporal++;
		}
		posLeer++;
		temporal = 0;
		contara++;
	}
	return disp;
}



string Directory::SizeDisk()
{
	int total = 0;
	string retornar = to_string(total);
	ifstream mapBits(this->discName, ios::binary | ios::in | ios::out);
	int contara = 0;
	int reading;
	int temporal;
	int cantBits = 0;
	int posActual = this->initialPositionForMapByte;
	while (!mapBits.eof() && cantBits<this->metadataDisc->Bit_Mat_Size)
	{
		mapBits.seekg(posActual);
		mapBits.read(reinterpret_cast<char*>(&reading), 1);
		for (int i = 0; i<8; i++)
		{
			temporal = this->GetBit(reading, i);
			if (temporal == 1)
			{
				contara++;
			}
		}
		cantBits++;
		posActual++;
	}
	total = sizeof(DATA_BLOCK)*contara;


	posActual = this->initialPositionForDirectory;
	cantBits = 0;
	contara = 0;
	FILE_ENTRY directories;
	while (!mapBits.eof() && cantBits<this->metadataDisc->Entry_Amount)
	{
		mapBits.seekg(posActual);
		mapBits.read(reinterpret_cast<char*>(&directories), sizeof(FILE_ENTRY));
		if (!directories.Libre)
		{
			contara++;
		}
		cantBits++;
		posActual += sizeof(FILE_ENTRY);
	}
	total += sizeof(FILE_ENTRY)*contara;
	retornar = this->SizeArchiveOrDirectory(total);
	return retornar;
}

string Directory::SizeArchiveOrDirectory(int bytes)
{
	float size;
	string retornar = to_string(bytes) + " B(s)";
	if (bytes >= 1000000000)
	{
		size = bytes / 1000000000.00;
		retornar = to_string(size) + " GB(s)";
	}
	else if (bytes >= 1000000)
	{
		size = bytes / 1000000.00;
		retornar = to_string(size) + " MB(s)";
	}
	else if (bytes >= 1000)
	{
		size = bytes / 1000.00;
		retornar = to_string(size) + " kB(s)";
	}

	return retornar;
}

int Directory::DisponibleBloque()
{
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	int posLeer = this->initialPositionForMapByte;

	int disp = -1, contara = 0, temporal = 0;
	char leer[1];
	int active = -1;

	while (!archive.eof() && contara<this->metadataDisc->Bit_Mat_Size)
	{
		archive.seekg(posLeer);
		archive.read(leer, 1);
		posLeer++;
		for (int i = 7; i >= 0; i--)
		{
			active = this->GetBit(*leer, i);
			if (active == 0 && disp == -1)
			{
				disp = temporal;
				return disp;
			}
			temporal++;
		}
	}
	return disp;
}

int Directory::DisponibleDirectory()
{
	FILE_ENTRY file;
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	int cantEntry = this->metadataDisc->Entry_Amount;

	archive.seekg(this->initialPositionForDirectory);
	bool continuar = true;
	int temporal;
	while (!archive.eof() && continuar && cantEntry>0)
	{
		temporal = archive.tellg();
		archive.read(reinterpret_cast<char *>(&file), sizeof(FILE_ENTRY));
		if (file.Libre)
		{
			return temporal;
		}
		cantEntry--;
	}
	return -1;
}

int Directory::ExistDirectory(string directoryName, int father)
{
	ifstream archive(discName, ios::in | ios::out | ios::binary);
	int actualposition = this->directoryActualInBytes;
	FILE_ENTRY directory, fatherD;
	if (this->directoryActualInBytes == -1)
	{
		actualposition = this->initialPositionForDirectory;
	}
	else
	{
		archive.seekg(actualposition);
		archive.read(reinterpret_cast<char*>(&fatherD), sizeof(FILE_ENTRY));
		if (fatherD.First_Son == -1)
		{
			return -1;
		}
		actualposition = fatherD.First_Son;
	}

	string nombreAComparar, type;
	int c = 0;
	while (!archive.eof() && c<this->metadataDisc->Entry_Amount)
	{
		archive.seekg(actualposition);
		archive.read(reinterpret_cast<char*>(&directory), sizeof(FILE_ENTRY));
		nombreAComparar.assign(directory.Name);
		type.assign(directory.Type);
		if (nombreAComparar == directoryName && directory.Father == father && type == "D")
		{
			return actualposition;
		}
		if (father == -1)
		{
			actualposition += sizeof(FILE_ENTRY);
		}
		else
		{
			actualposition = directory.Right_Brother;
		}
		c++;
	}
	return -1;
}

int Directory::ReturnFirstDataBlockIfExistArchive(string name, int father)
{
	FILE_ENTRY principal;
	int posA = father;
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	bool con = true;
	if (father == -1)
	{
		posA = this->initialPositionForDirectory;
	}
	else
	{
		archive.seekg(posA);
		if (!archive.eof())
		{
			archive.read(reinterpret_cast<char*>(&principal), sizeof(FILE_ENTRY));
		}
		if (principal.First_Son == -1)
			con = false;
	}

	if (con)
	{
		string comparar = "", type = "";
		bool continuar = true;
		while (!archive.eof() && continuar)
		{
			archive.seekg(posA);
			archive.read(reinterpret_cast<char*>(&principal), sizeof(FILE_ENTRY));
			comparar.assign(principal.Name);
			type.assign(principal.Type);
			if (comparar == name && type == "A")
			{
				return posA;
			}
			if (principal.Right_Brother == -1 && father != -1)
			{
				return -1;
			}
			else if (father == -1)
			{
				posA += sizeof(FILE_ENTRY);
			}
			else
			{
				posA = principal.Right_Brother;
			}
		}
	}
	return -1;
}

int Directory::ExistArchive(string name, int father)
{

	FILE_ENTRY principal;
	int posA = father;
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	bool con = true;
	if (father == -1)
	{
		posA = this->initialPositionForDirectory;
	}
	else
	{
		archive.seekg(posA);
		if (!archive.eof())
		{
			archive.read(reinterpret_cast<char*>(&principal), sizeof(FILE_ENTRY));
		}
		if (principal.First_Son == -1)
		{
			con = false;
		}
		else
		{
			posA = principal.First_Son;
		}
	}

	if (con)
	{
		string comparar = "", type = "";
		bool continuar = true;
		while (!archive.eof() && continuar)
		{
			archive.seekg(posA);
			archive.read(reinterpret_cast<char*>(&principal), sizeof(FILE_ENTRY));
			comparar.assign(principal.Name);
			type.assign(principal.Type);
			if (comparar == name && type == "A")
			{
				return posA;
			}
			if (principal.Right_Brother == -1 && father != -1)
			{
				return -1;
			}
			else if (father == -1)
			{
				posA += sizeof(FILE_ENTRY);
			}
			else
			{
				posA = principal.Right_Brother;
			}
		}
	}
	return -1;
}

int Directory::GetBit(unsigned char byte, int position)
{
	return (byte >> position) & 0x1;
}

int Directory::OfBite(unsigned char byte, int position)
{
	return (byte ^= (1 << position));
}

int Directory::OnBite(unsigned char byte, int position)
{
	return (byte |= (1 << position));
}

void Directory::UpdateDirection(FILE_ENTRY newPosition, int directoryPosition)
{
	this->directoryActualInBytes = directoryPosition;
	string directoryName;
	directoryName.assign(newPosition.Name);
	this->actualUbicationString += "/ " + directoryName;
	this->isFather = false;
}

void Directory::BackDirectory()
{
	if (!this->isFather)
	{
		FILE_ENTRY father;
		ifstream archive(this->discName, ios::binary | ios::in | ios::out);
		archive.seekg(this->directoryActualInBytes);
		if (!archive.eof())
		{
			archive.read(reinterpret_cast<char*>(&father), sizeof(FILE_ENTRY));
		}
		if (father.Father == -1)
		{
			this->directoryActualInBytes = father.Father;
			this->actualUbicationString = discName.substr(0, discName.length() - 4) + ":/";
			this->isFather = true;
		}
		else
		{
			this->directoryActualInBytes = father.Father;
			int fathers = strlen(father.Name) + 2;
			int directorylength = this->actualUbicationString.length();
			int reduce = directorylength - fathers;
			string newDirection = "";
			for (int i = 0; i < reduce; i++)
			{
				newDirection += this->actualUbicationString.at(i);
			}
			this->actualUbicationString = newDirection;
		}
	}
	else
	{
		cout << "Se encuentra en el directorio principal" << endl;
	}
}

void Directory::PrintBitMap()
{
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);
	int posLeer = this->initialPositionForMapByte;

	int disp = -1, contara = 0, temporal = 0;
	char leer[1];
	int sactive = -1;

	while (!archive.eof() && contara<this->metadataDisc->Bit_Mat_Size)
	{
		archive.seekg(posLeer);
		archive.read(leer, 1);
		posLeer++;
		for (int i = 7; i >= 0; i--)
		{
			sactive = this->GetBit(*leer, i);
			cout << sactive;
		}
		cout << endl;
		contara++;
	}
	cout << endl;
}

int Directory::CantidadDeBloquesDisponibles()
{
	ifstream mapBits(this->discName, ios::binary | ios::in | ios::out);
	int contara = 0;
	int cantBits = 0;
	int reading;
	int temporal;
	int posActual = this->initialPositionForMapByte;
	while (!mapBits.eof() && cantBits<this->metadataDisc->Bit_Mat_Size)
	{
		mapBits.seekg(posActual);
		mapBits.read(reinterpret_cast<char*>(&reading), 1);
		for (int i = 0; i<8; i++)
		{
			temporal = this->GetBit(reading, i);
			if (temporal == 0)
			{
				contara++;
			}
		}
		posActual++;
		cantBits++;
	}
	return contara;
}
bool Directory::DeleteRec(int initialPosition)
{
	if (initialPosition == -1)
	{
		return true;
	}
	FILE_ENTRY nuevoFile, leidoFile;
	int temporal = initialPosition;
	ifstream leyendo(this->discName, ios::in | ios::binary);
	ofstream escribiendo(this->discName, ios::out | ios::in | ios::binary);
	if (!escribiendo.is_open())
	{
		escribiendo.open(this->discName);
	}
	leyendo.seekg(temporal);
	leyendo.read(reinterpret_cast<char*>(&leidoFile), sizeof(FILE_ENTRY));
	string type;
	type.assign(leidoFile.Type);
	if (type == "A")
	{
		this->DeleteArchive(leidoFile.First_Data_Bloque);
	}

	if (leidoFile.First_Son != -1)
	{
		this->DeleteRec(leidoFile.First_Son);
	}

	if (leidoFile.Right_Brother != -1)
	{
		this->DeleteRec(leidoFile.Right_Brother);
	}
	escribiendo.seekp(temporal);
	escribiendo.write(reinterpret_cast<char*>(&nuevoFile), sizeof(FILE_ENTRY));
	escribiendo.close();
	return true;
}


int Directory::DeleteArchive(int initialPosition)
{
	ifstream leyendo(this->discName, ios::in | ios::binary);
	DATA_BLOCK nuevo, leido;
	ofstream escribiendo(this->discName, ios::out | ios::in | ios::binary);
	if (!escribiendo.is_open())
	{
		escribiendo.open(this->discName);
	}
	int posA;
	int actual = initialPosition;

	while (actual != -1)
	{
		posA = actual;
		leyendo.seekg(posA);
		leyendo.read(reinterpret_cast<char*>(&leido), sizeof(DATA_BLOCK));
		escribiendo.seekp(posA);
		escribiendo.write(reinterpret_cast<char*>(&nuevo), sizeof(DATA_BLOCK));

		char tmp[1];
		int byteApagando = (posA - initialPositionForBloque) / sizeof(DATA_BLOCK) + 1;
		int temporal = byteApagando;
		int contara = 0;
		while (temporal >= 8)
		{
			temporal -= 8;
			contara++;
		}
		int test;
		int posFinalToOffByte;

		posFinalToOffByte = byteApagando / 8 + initialPositionForMapByte;
		if (byteApagando % 8 == 0)
		{
			posFinalToOffByte = byteApagando / 8 + initialPositionForMapByte - 1;
		}
		leyendo.seekg(posFinalToOffByte);
		leyendo.read(tmp, 1);
		escribiendo.seekp(posFinalToOffByte);
		test = ((int)(tmp[0]));
		int newByte;
		if (temporal == 0)
		{
			newByte = this->OfBite(*tmp, temporal);
		}
		else
		{
			newByte = this->OfBite(*tmp, 8 - temporal);
		}
		escribiendo.write(reinterpret_cast<char*>(&newByte), 1);
		actual = leido.siguiente;
	}
	escribiendo.close();
	return 1;
}

void Directory::test()
{

	ofstream escribir(discName, ios::out | ios::in | ios::binary);
	ifstream leer(discName, ios::in | ios::binary);
	METADATA escrito, leido;
	escrito.Bit_Mat_Size = 20;
	escrito.Block_Amount = 160;
	escrito.Block_Size = 1024;
	escrito.Entry_Amount = 10;
	escribir.seekp(0, ios::end);
	escribir.write(reinterpret_cast<char*>(&escrito), sizeof(METADATA));
	escribir.close();
	leer.seekg(0, ios::beg);
	leer.read(reinterpret_cast<char*>(&leido), sizeof(METADATA));
}

bool Directory::CreateDirectory()
{
	int directoryNew = this->DisponibleDirectory();

	if (directoryNew != -1)
	{
		FILE_ENTRY directory;
		cout << "Ingrese nombre del directorio:" << endl;
		cin >> directory.Name;
		directory.First_Data_Bloque = -1;
		directory.First_Son = -1;
		directory.Libre = false;
		directory.Right_Brother = -1;
		char D[1];
		D[0] = 'D';
		memcpy(directory.Type, D, 1);
		directory.Size = sizeof(FILE_ENTRY);

		if (this->isFather)
		{
			directory.Father = -1;
		}
		else
		{
			FILE_ENTRY father;
			ifstream archive(this->discName, ios::binary | ios::in);
			archive.seekg(this->directoryActualInBytes);
			bool continuar = true;
			int actualPosition = -1;
			while (!archive.eof() && continuar)
			{
				actualPosition = archive.tellg();
				archive.read(reinterpret_cast<char*>(&father), sizeof(FILE_ENTRY));

				if (father.First_Son == -1)
				{
					ofstream update(this->discName, ios::out | ios::in | ios::binary);
					if (!update.is_open())
					{
						ofstream update(this->discName, ios::out | ios::in | ios::binary);
					}
					FILE_ENTRY newSon;
					update.seekp(actualPosition);
					memcpy(newSon.Name, father.Name, 30);
					newSon.First_Data_Bloque = father.First_Data_Bloque;
					newSon.Libre = father.Libre;
					newSon.Father = father.Father;
					newSon.Right_Brother = father.Right_Brother;
					newSon.Size = father.Size;
					memcpy(newSon.Type, father.Type, 1);
					newSon.First_Son = directoryNew;
					update.write(reinterpret_cast<char*>(&newSon), sizeof(FILE_ENTRY));
					update.close();
					directory.Father = actualPosition;
					continuar = false;
				}
				else
				{
					directory.Father = actualPosition;
					FILE_ENTRY brother;
					archive.seekg(father.First_Son);
					actualPosition = archive.tellg();
					while (!archive.eof() && continuar)
					{
						archive.seekg(actualPosition);
						archive.read(reinterpret_cast<char*>(&brother), sizeof(FILE_ENTRY));
						if (brother.Right_Brother == -1)
						{
							ofstream update(this->discName, ios::out | ios::in | ios::binary);
							if (!update.is_open())
							{
								ofstream update(this->discName, ios::out | ios::in | ios::binary);
							}
							FILE_ENTRY newBrother;
							update.seekp(actualPosition);
							strcpy_s(newBrother.Name, brother.Name);
							newBrother.First_Data_Bloque = brother.First_Data_Bloque;
							newBrother.Libre = brother.Libre;
							newBrother.Father = brother.Father;
							newBrother.Right_Brother = directoryNew;
							newBrother.Size = brother.Size;
							memcpy(newBrother.Type, brother.Type, 1);
							newBrother.First_Son = brother.First_Son;
							update.write(reinterpret_cast<char*>(&newBrother), sizeof(FILE_ENTRY));
							update.close();
							continuar = false;
						}
						else
						{
							actualPosition = brother.Right_Brother;
						}
					}
				}
			}
		}

		ofstream archive(this->discName, ios::out | ios::in | ios::binary);
		if (!archive.is_open())
		{
			ofstream fs(this->discName, ios::out | ios::in | ios::binary);
		}

		archive.seekp(directoryNew);
		archive.write(reinterpret_cast<char*>(&directory), sizeof(FILE_ENTRY));
		archive.close();
		cout << "Directorio creado satisfactoriamente." << endl;
		return true;
	}
	cout << "No hay directorios disponibles" << endl;
	return false;
}

bool Directory::ChangeDirectory()
{
	string directoryName;
	this->DirectoriesandArchivesList(this->directoryActualInBytes);
	cout << "Ingrese nombre del directorio al que se desea cambiar(solo disponibles los de letra D)" << endl;
	cin >> directoryName;
	if (directoryName == "../")
	{
		this->BackDirectory();
		return true;
	}
	int directoryPosition = this->ExistDirectory(directoryName, this->directoryActualInBytes);
	if (directoryPosition != -1)
	{
		FILE_ENTRY newPosition;
		ifstream archive(this->discName, ios::binary | ios::in | ios::out);
		archive.seekg(directoryPosition);
		if (!archive.eof())
		{
			archive.read(reinterpret_cast<char*>(&newPosition), sizeof(FILE_ENTRY));
		}
		if (newPosition.Type[0] == 'D')
		{
			this->UpdateDirection(newPosition, directoryPosition);
			return true;
		}
		else
		{
			cout << "El nombre que usted a ingresado es de un archivo y no de una carpeta." << endl;
		}
	}
	else
	{
		cout << "No se encontro el directorio seleccionado" << endl;
	}
	return false;
}

void Directory::DirectoriesandArchivesList(int father)
{
	ifstream archive(this->discName, ios::binary | ios::in | ios::out);

	int temporalposition = this->directoryActualInBytes;
	if (this->directoryActualInBytes == -1)
	{
		temporalposition = this->initialPositionForDirectory;
	}
	else
	{
		FILE_ENTRY firstSon;
		archive.seekg(temporalposition);
		archive.read(reinterpret_cast<char *>(&firstSon), sizeof(FILE_ENTRY));
		if (firstSon.First_Son == -1)
		{
			return;
		}
		temporalposition = firstSon.First_Son;
	}

	FILE_ENTRY file;
	string name;
	bool continuar = true;
	archive.seekg(temporalposition);
	int comparar = this->metadataDisc->Entry_Amount * sizeof(FILE_ENTRY) + this->initialPositionForDirectory;
	while (!archive.eof() && continuar && temporalposition < comparar)
	{
		archive.seekg(temporalposition);
		archive.read(reinterpret_cast<char *>(&file), sizeof(FILE_ENTRY));
		name.assign(file.Name);
		if (!file.Libre && file.Father == father || (this->isFather && file.Father == -1))
		{
			cout << file.Name << "             " << file.Type << "            " << this->SizeArchiveOrDirectory(file.Size) << endl;
		}

		if (file.Right_Brother != -1)
		{
			temporalposition = file.Right_Brother;
		}
		else if (father == -1)
		{
			temporalposition += sizeof(FILE_ENTRY);
		}
		else
		{
			continuar = false;
		}
	}
}

bool Directory::DeleteDirectoryOrArchive(string nameToDelete)
{
	int existDirectoryArchive = this->ExistDirectory(nameToDelete, this->directoryActualInBytes);
	if (existDirectoryArchive == -1)
	{
		existDirectoryArchive = this->ExistArchive(nameToDelete, this->directoryActualInBytes);
	}

	if (existDirectoryArchive != -1)
	{
		FILE_ENTRY leidoFile, nuevoFile, father, brother;
		ifstream leyendo(this->discName, ios::in | ios::binary);
		ofstream escribiendo(this->discName, ios::out | ios::in | ios::binary);
		if (!escribiendo.is_open())
		{
			escribiendo.open(this->discName);
		}
		leyendo.seekg(existDirectoryArchive);
		leyendo.read(reinterpret_cast<char*>(&leidoFile), sizeof(FILE_ENTRY));
		escribiendo.seekp(existDirectoryArchive);
		escribiendo.write(reinterpret_cast<char*>(&nuevoFile), sizeof(FILE_ENTRY));
		if (leidoFile.Father != -1)
		{
			leyendo.seekg(leidoFile.Father);
			leyendo.read(reinterpret_cast<char*>(&father), sizeof(FILE_ENTRY));
			int possA;
			if (father.First_Son != existDirectoryArchive)
			{
				possA = father.First_Son;
				bool c = true;
				while (c)
				{
					leyendo.seekg(possA);
					leyendo.read(reinterpret_cast<char*>(&brother), sizeof(FILE_ENTRY));
					if (brother.Right_Brother == existDirectoryArchive)
					{
						brother.Right_Brother = leidoFile.Right_Brother;
						escribiendo.seekp(possA);
						escribiendo.write(reinterpret_cast<char*>(&brother), sizeof(FILE_ENTRY));
						c = false;
					}
					else
					{
						possA = brother.Right_Brother;
					}
				}

			}
			else
			{
				father.First_Son = leidoFile.Right_Brother;
				escribiendo.seekp(leidoFile.Father);
				escribiendo.write(reinterpret_cast<char*>(&father), sizeof(FILE_ENTRY));
			}
		}
		escribiendo.close();
		if (leidoFile.First_Son != -1)
		{
			this->DeleteRec(leidoFile.First_Son);
		}
		else
		{
			this->DeleteArchive(leidoFile.First_Data_Bloque);
		}
		return true;
	}
	else
	{
		cout << "No se encontro el Directorio/ Archivo" << endl;
	}
	return false;
}

bool Directory::ImportArchive() {

	int disponibleDirectory = this->DisponibleDirectory();
	int disponibleBloque = this->DisponibleBloque();

	if (disponibleDirectory != -1 && disponibleBloque != -1)
	{
		string path;
		cout << "Ingrese nombre del archivo" << endl;
		cin >> path;
		ifstream discDat(path, ios::in | ios::binary);
		if (discDat)
		{
			discDat.seekg(0, ios::end);
			int sizeOnBytes = discDat.tellg();
			int cantidadDeBloquesDisponibles = this->CantidadDeBloquesDisponibles();
			if (sizeOnBytes <= (cantidadDeBloquesDisponibles * 1020))
			{
				FILE_ENTRY dad;
				ifstream principal(this->discName, ios::in | ios::binary);
				if (!this->isFather)
				{
					principal.seekg(this->directoryActualInBytes);
					principal.read(reinterpret_cast<char*>(&dad), sizeof(FILE_ENTRY));
				}

				FILE_ENTRY newArchive;
				newArchive.Father = this->directoryActualInBytes;
				newArchive.First_Son = -1;
				newArchive.Libre = false;
				strcpy_s(newArchive.Name, path.c_str());
				newArchive.Right_Brother = -1;
				newArchive.Size = sizeOnBytes + sizeof(FILE_ENTRY);
				char A[1];
				A[0] = 'A';
				memcpy(newArchive.Type, A, 1);

				ofstream update(this->discName, ios::out | ios::in | ios::binary);
				if (!update.is_open())
				{
					ofstream update(this->discName, ios::out | ios::in | ios::binary);
				}
				if (!this->isFather)
				{

					if (dad.First_Son != -1)
					{
						bool vacio = true;
						int poss = dad.First_Son;
						while (vacio)
						{
							principal.seekg(poss);
							principal.read(reinterpret_cast<char*>(&dad), sizeof(FILE_ENTRY));
							if (dad.Right_Brother == -1)
							{
								dad.Right_Brother = disponibleDirectory;
								update.seekp(poss);
								update.write(reinterpret_cast<char*>(&dad), sizeof(FILE_ENTRY));
								vacio = false;
							}
							else
							{
								poss = dad.Right_Brother;
							}
						}

					}
					else
					{
						dad.First_Son = disponibleDirectory;
						update.seekp(this->directoryActualInBytes);
						update.write(reinterpret_cast<char*>(&dad), sizeof(FILE_ENTRY));
					}
				}
				update.seekp(disponibleDirectory);
				int newBloque = this->DisponibleBloque() * sizeof(DATA_BLOCK) + sizeof(METADATA) + this->metadataDisc->Bit_Mat_Size + this->metadataDisc->Entry_Amount * sizeof(FILE_ENTRY);
				newArchive.First_Data_Bloque = newBloque;
				update.write(reinterpret_cast<char*>(&newArchive), sizeof(FILE_ENTRY));
				float bloquesAllenar = sizeOnBytes / 1020.00;
				for (int i = 0; i<bloquesAllenar; i++)
				{
					int byteApagar = this->disponibleByteInMapBits();
					int active;
					ifstream byteToModify(this->discName, ios::in | ios::binary);
					byteToModify.seekg(byteApagar);
					char* tmp = new char[1];
					byteToModify.read(tmp, 1);
					for (int i = 7; i >= 0; i--)
					{

						active = this->GetBit(*tmp, i);

						if (active == 0)
						{
							active = this->OnBite(*tmp, i);
							update.seekp(byteApagar);
							update.write(reinterpret_cast<char*>(&active), 1);
							update.close();
							i = 0;
						}
					}

					update.open(this->discName, ios::out | ios::in | ios::binary);
					disponibleBloque = this->DisponibleBloque() * sizeof(DATA_BLOCK) + sizeof(METADATA) + this->metadataDisc->Bit_Mat_Size + this->metadataDisc->Entry_Amount * sizeof(FILE_ENTRY);
					update.seekp(newBloque);
					if (!update.eof())
					{
						DATA_BLOCK bloque;
						if (i + 1 >= bloquesAllenar)
						{
							bloque.siguiente = -1;
						}
						else
						{
							bloque.siguiente = disponibleBloque;
						}

						int initialCopy = i * 1020;
						discDat.seekg(initialCopy);
						char read[1020];
						if (!discDat.eof())
						{
							discDat.read(read, 1020);
						}
						memcpy(&bloque.DATA, read, 1020);
						update.write(reinterpret_cast<char*>(&bloque), sizeof(DATA_BLOCK));
						newBloque = disponibleBloque;
					}
				}

				update.close();
				return true;
			}
			else
			{
				cout << "No hay suficiente espacio disponible en el disco para guardar este archivo" << endl;
			}
		}
	}
	else
	{
		cout << "No hay directorios disponibles" << endl;
	}
	return false;
}

bool Directory::ExportArchive()
{
	string origen;
	this->DirectoriesandArchivesList(this->directoryActualInBytes);
	cout << "Ingrese nombre del archivo con su formato que desea copiar" << endl;
	cin >> origen;
	ifstream discDat(discName, ios::in | ios::binary);
	FILE_ENTRY archivoExportar;
	int posALeerArchivoExportar = this->ExistArchive(origen, this->directoryActualInBytes);
	discDat.seekg(posALeerArchivoExportar);
	discDat.read(reinterpret_cast<char*>(&archivoExportar), sizeof(FILE_ENTRY));
	int bloqueInitial = archivoExportar.First_Data_Bloque;
	float sizeTotal = archivoExportar.Size + 0.00;

	if (bloqueInitial != -1)
	{
		string nameArchive;
		cout << "Ingrese nombre del archivo con su formato" << endl;
		cin >> nameArchive;
		DATA_BLOCK leyendo;
		bool continuar = true;
		int cantBloquesZise = 0;
		ofstream update(nameArchive, ios::out | ios::binary);
		while (!discDat.eof() && continuar && sizeTotal >= 1020)
		{
			discDat.seekg(bloqueInitial);
			discDat.read(reinterpret_cast<char*>(&leyendo), sizeof(DATA_BLOCK));
			update.write(leyendo.DATA, 1020);

			if (leyendo.siguiente == -1)
			{
				continuar = false;
			}
			else {
				bloqueInitial = leyendo.siguiente;
			}
			sizeTotal -= 1020;
		}
		discDat.seekg(bloqueInitial);
		discDat.read(reinterpret_cast<char*>(&leyendo), sizeof(DATA_BLOCK));
		update.write(leyendo.DATA, sizeTotal);
		update.close();
		return true;
	}
	else
	{
		cout << "No se encontro el archivo." << endl;
	}
	return false;
}

void Directory::ReadMetadataDisc()
{
	ifstream discDat(discName, ios::in | ios::binary);
	this->metadataDisc = new METADATA();
	discDat.seekg(0, ios::beg);
	discDat.read(reinterpret_cast<char *>(this->metadataDisc), sizeof(METADATA));
	this->initialPositionForMapByte = sizeof(METADATA);
	this->initialPositionForDirectory = metadataDisc->Bit_Mat_Size + sizeof(METADATA);
	this->initialPositionForBloque = this->initialPositionForDirectory + metadataDisc->Entry_Amount * sizeof(FILE_ENTRY);
	this->directoryActualInBytes = -1;
}

void Directory::DiscName(string name)
{
	if (name.length() <5)
	{
		this->discName = name + ".dat";
		return;
	}
	string comparar = "";

	for (int i = name.length() - 1; i> name.length() - 5; i--)
	{
		comparar += name.at(i);
	}

	if (comparar != "tad.")
	{
		this->discName = name + ".dat";
	}
}