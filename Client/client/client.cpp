// client.cpp�: d�finit le point d'entr�e pour l'application console.
//
#include "stdafx.h"
#include "World/World.hpp"

int main(int argc, char** argv)
{
	World* l_World = new World();
	if (argc == 3)
		l_World->Initialize(argv);

    return 0;
}