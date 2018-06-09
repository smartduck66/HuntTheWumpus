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
#include<map>	// pour bénéficier du container multimap qui n'est a priori pas défini dans std_lib_facilities.h

void tunnels(int room, int nb_rooms_maze, multimap<int, int>&current_maze)
{  
	// *********************************************************************************************************************************
	// Cette fonction alimente le container multimap 'current_maze' passé par référence créé dans le main en créant le nombre de tunnels
	// adéquats pour la 'room' passée en argument. Chaque 'room' ne peut avoir que 3 tunnels.
	// *********************************************************************************************************************************

	// On ne peut avoir que 3 tunnels possibles au maximum pour une pièce donnée
	// On vérifie donc leur nombre pour la pièce passée en paramètre en utilisant les algorithmes liés au container multimap
	auto z = current_maze.equal_range(room);
	int nb_tunnels_existants = distance(z.first, z.second);
	int new_tunnel = 0;

	// On crée alors le bon nombre de tunnels pour la room (0 éventuellement si 3 tunnels existent déjà...)
	for (int i=1;i<= 3-nb_tunnels_existants;++i) {
		
		// Création du sens room -> new room
		bool creation_possible = false;
				
		while (!creation_possible) {
			new_tunnel = randint(1, nb_rooms_maze);	// On se sert de la fonction randint définie par BS
													// Note : on peut "reseeder" éventuellement car la génération est identique à chaque lancement du programme
			
			// Le nouveau tunnel ne peut boucler sur lui-même
			if (new_tunnel != room) creation_possible = true;
			
			// Tunnel déjà existant pour la pièce en cours ? L'algo "any_of" est mal maîtrisé, remplacé donc par une boucle simple
			// On ne peut pas se baser sur le 'z = current_maze.equal_range(room)' dans le cas où aucune occurence de la room n'existe 
			// ET que deux tunnels identiques se créent dans la boucle 'i' en cours
			auto ze = current_maze.equal_range(room);
			for (auto t = ze.first; t != ze.second; ++t)
				if (t->second == new_tunnel) creation_possible = false;

			// Le nombre de tunnels de la pièce pointée par new_tunnel ne doit pas déjà être égal à 3
			auto zn = current_maze.equal_range(new_tunnel);
			if (distance(zn.first, zn.second) == 3) creation_possible = false;
				

		}
		current_maze.insert(make_pair(room, new_tunnel));	// On crée le tunnel dès que l'on sort du 'while'
		
		// Création du sens new room -> room si et seulement si le nombre de tunnels est également inférieur ou égal à 3
		// Cette étape 'inverse' est nécessaire pour éviter de créer de nouveaux tunnels quand l'appel à la fonction fera que 'room' sera égal à 'new_tunnel'
		auto zn = current_maze.equal_range(new_tunnel);
		if (distance(zn.first, zn.second) < 3) 
			current_maze.insert(make_pair(new_tunnel, room));
		
	}

	
}


// ***********************************************************************************************************************************************
int main()
{

	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));

		// Génération aléatoire du labyrinthe
		int nb_rooms = 20;
		auto maze = multimap<int, int>{};
		
		for (int room=1;room< nb_rooms;++room) {
			 
			// Création des tunnels de la room concernée
			tunnels(room, nb_rooms,maze);
			
		}
				
		// Vérification
		for (auto const& tunnels : maze) std::cout << tunnels.first << '-' << tunnels.second << '\n';
		
		// Placement du Wumpus, des fosses et des chauve-souris au hasard

		// Téléportation du joueur au démarrage dans une room vide

		// Boucle de jeu
		



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

