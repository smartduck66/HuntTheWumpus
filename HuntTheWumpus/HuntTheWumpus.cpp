//
// Book : Version du chapitre 18 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 09/06/2018 
// Commit en cours : exercice 12 pages 665 et 666 (Hunt The Wumpus)
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"

// Librairie restreinte fournie par BS
#include "std_lib_facilities.h"



// ***********************************************************************************************************************************************
int main()
{

	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		

		keep_window_open("");


	}

	catch (runtime_error& e)
	{	// this code is to produce error messages
		cout << e.what() << '\n';
		keep_window_open("q");	// For some Windows(tm) setups
	}

	catch (...) {	// this code is to produce error messages
		cout << "Erreur indéfinie" << endl;
		keep_window_open("q");	// For some Windows(tm) setups
	}

	keep_window_open();
}

