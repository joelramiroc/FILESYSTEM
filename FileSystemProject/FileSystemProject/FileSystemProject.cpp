// FileSystemProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "Disc.h"
#include "Directory.h"
#include <string>

using namespace std;

int main()
{
	string nameDisc = "";
	Disc* disc = new Disc();
	Directory* directory;
	
	int opcion;
	bool continuar = true;
	cout << "===================FILE SYSTEM======================" << endl;

	do
	{
		cout << "Que desea hacer:" << endl;
		cout << "1. Crear Disco" << endl;
		cout << "2. Usar Disco existente" << endl;
		cout << "3. Salir" << endl;
		cin >> opcion;

		switch (opcion)
		{
		case 1:
		{
			nameDisc = disc->CreateDisc();
		}
		break;
		case 2:
		{
			cout << "Ingrese nombre del disco a utilizar:" << endl;
			cin >> nameDisc;
			if (disc->ExistDisc(nameDisc))
			{
				Directory* di = new Directory(nameDisc);
				di->PrincipalMain();
			}else
			{
				cout << "No se encontro un disco con ese nombre." << endl;
			}
		}
		break;
		case 3:
		{
			continuar = false;
		}
		break;
		}
	} while (continuar);

return 0;
}

