//
// Book : Version du chapitre 18 de l'ouvrage
// "Programming -- Principles and Practice Using C++" de Bjarne Stroustrup (2ème édition : 2014)
// Commit initial : 09/06/2018 
// Commit en cours : 16/06/2018 - Exercice 12 pages 665 et 666 (Hunt The Wumpus)
// Note : version très "procédurale". Evolution à faire : data abstraction (ex : Maze en classe)
// Caractères spéciaux : [ ]   '\n'   {  }   ||   ~   _     @

// Librairie indispensable sous Windows et VC++
#include "stdafx.h"

// Librairie restreinte fournie par BS
//#include "std_lib_facilities.h"
#include<iostream>
#include<iterator>
#include<vector>
#include<string>
#include<utility>
#include<algorithm>
#include<ctime>
#include<map>	// pour bénéficier du container multimap 

using namespace std;

int nb_aleatoire(const int min, const int max)
{
	// Cette fonction helper gère le tirage de nombres aléatoires entre deux bornes passées en arguments
	// On préfère la fonction C rand() à randint(min, max) incluse dans std_lib_facilities.h
	// Le seeding réalisée dans le main() permet une génération pseudo-aléatoire (meilleurs outils à partir de C++ 11)
	int a= rand() % max + min;
	return a;
}

inline void keep_window_open() // From "std_lib_facilities.h" of BS
{
	cin.clear();
	cout << "Please enter a character to exit\n";
	char ch;
	cin >> ch;
	return;
}

inline void keep_window_open(string s)
{
	if (s == "") return;
	cin.clear();
	cin.ignore(120, '\n');
	for (;;) {
		cout << "Please enter " << s << " to exit\n";
		string ss;
		while (cin >> ss && ss != s)
			cout << "Please enter " << s << " to exit\n";
		return;
	}
}

void tunnels(const int room, const int nb_rooms_maze, multimap<int, int>&current_maze)
{  
	// ***********************************************************************************************************************************
	// Cette fonction alimente le container multimap 'current_maze' passé par référence (créé dans le main) en créant le nombre de tunnels
	// adéquats pour la 'room' passée en argument. Chaque 'room' ne peut avoir que 3 tunnels.
	// ***********************************************************************************************************************************

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
			new_tunnel = nb_aleatoire(1, nb_rooms_maze);	
			
			// Le nouveau tunnel ne peut boucler sur lui-même
			if (new_tunnel != room) creation_possible = true;
			
			// Tunnel déjà existant pour la pièce en cours ? L'algo "any_of" est mal maîtrisé, remplacé donc par une boucle simple
			// On ne peut pas se baser sur le 'z = current_maze.equal_range(room)' dans le cas où aucune occurence de la room n'existe 
			// ET que deux tunnels identiques se créent dans la boucle 'i' en cours
			auto ze = current_maze.equal_range(room);
			// if (any_of(ze.first, ze.second, [&new_tunnel](auto const& ze) { return ze.second == &new_tunnel; })) creation_possible = false;
			for (auto t = ze.first; t != ze.second; ++t)
				if (t->second == new_tunnel) creation_possible = false;
			
			// Le nombre de tunnels de la pièce pointée par new_tunnel ne doit pas déjà être égal à 3...
			auto zn = current_maze.equal_range(new_tunnel);
			if (distance(zn.first, zn.second) == 3) {
				creation_possible = false;
				
				// ... Néanmoins, pour éviter une boucle infinie (un ou plusieurs tunnels n'auront pas alors 3 issues)
				if (size(current_maze)>static_cast<unsigned int>((room-1)*3)) creation_possible = true;
			}
			
		}
		current_maze.insert(make_pair(room, new_tunnel));	// On crée le tunnel dès que l'on sort du 'while'
		
		// Création du sens new room -> room si et seulement si le nombre de tunnels est également inférieur ou égal à 3
		// Cette étape 'inverse' est nécessaire pour éviter de créer de nouveaux tunnels quand l'appel à la fonction fera que 'room' sera égal à 'new_tunnel'
		auto zx = current_maze.equal_range(new_tunnel);
		if (distance(zx.first, zx.second) < 3) 
			current_maze.insert(make_pair(new_tunnel, room));
		
	}

}

bool is_wumpus_here(const int room, const int wumpus_room)
{
	// Cette fonction helper teste si le wumpus se trouve dans la pièce où entre le joueur
	return (room == wumpus_room);

}

bool is_pit_here(const int room, const vector<int>& pit_rooms)
{
	// Cette fonction helper teste si une fosse se trouve dans la pièce où entre le joueur
	// Utilisation d'un algorithme de recherche plus compact qu'une boucle (A tour of C++ : page 109)
	 return find(pit_rooms.begin(), pit_rooms.end(),room) != pit_rooms.end();

}

bool is_bat_here(const int room, const vector<int>& bat_rooms)
{
	// Cette fonction helper teste si une chauve-souris se trouve dans la pièce où entre le joueur
	return find(bat_rooms.begin(), bat_rooms.end(), room) != bat_rooms.end();
}

void tunnels_in_room(const int room, vector<int>& tunnels, const multimap<int, int>&current_maze)
{
	// Cette fonction helper popule le vecteur passé en référence avec les tunnels disponibles dans une pièce donnée
	auto ze = current_maze.equal_range(room);
	
	for (auto t = ze.first; t != ze.second; ++t)
		tunnels.push_back(t->second);
	
}

bool is_new_room_valid(const int room, const vector<int>& tunnels)
{
	// Cette fonction helper vérifie si le mouvement souhaité par le joueur est valide
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		if (tunnels[i] == room) return true;

	}

	cout << "Ce tunnel n'est pas accessible !" << endl;
	return false;

}

bool hazard(int room, const int wumpus_room, const vector<int>& pit_rooms, const vector<int>& bat_rooms, int& player_room, const int nb_rooms_maze)
{
	// Cette fonction helper vérifie les dangers de la salle où le joueur a souhaité bouger
	// Si l'un des cas testés retourne false, le joueur est décédé
	// int room n'est pas passé en const car peut être modifié dans le cas de la chauve-souris ;
	// player_room également pour repartir dans la boucle principale du jeu
	// *****************************************************************************************
	
	if (is_wumpus_here(room, wumpus_room)) {
		cout << "Désolé... Le Wumpus est dans cette salle :-( Tu es mort !" << endl; 
		return false;
	}
	
	if (is_pit_here(room, pit_rooms)) {
		cout << "Désolé... Il y a une fosse dans cette salle :-( Tu es mort !" << endl;
		return false;
	}

	if (is_bat_here(room, bat_rooms)) {
		room = nb_aleatoire(1, nb_rooms_maze);
		cout << "Une chauve-souris se trouve dans cette salle... Elle te transporte dans la salle "<<room << endl;
		return hazard(room, wumpus_room, pit_rooms, bat_rooms, player_room, nb_rooms_maze);	// On fait appel à la récursion pour vérifier les dangers de la salle où l'on est transporté
	}

	// La salle est "clear" : on modifie donc la salle du joueur et on repart dans la boucle du jeu
	player_room = room;
	return true;

}

int wumpus_move(const int w_room, const multimap<int, int>&current_maze)
{
	// Fonction Helper qui fait bouger le wumpus s'il est réveillé par un tir de flèche
	// ********************************************************************************
	
	// On récupère les tunnels disponibles à partir de la salle où dort le Wumpus
	vector <int> tunnels_dispo{};
	tunnels_in_room(w_room, tunnels_dispo, current_maze);

	// On tire au hasard le rang de la salle, compris entre 0 et la taille du vecteur précédent et
	// on retourne la nouvelle salle où le wumpus a bougé
	return tunnels_dispo[nb_aleatoire(0, size(tunnels_dispo)-1)];

}

tuple<bool, bool> shoot(const int room, const vector<int>& tunnels, int& wumpus_room, const multimap<int, int>&current_maze)
{
	// Cette fonction helper gère le tir
	// tuple<bool, bool> : le premier argument de retour correspond à la vie du Wumpus, le second à la vie du joueur
	// On passe wumpus_room par référence NON CONST car elle peut être modifiée si le Wumpus est réveillé et bouge
	// *************************************************************************************************************
	
	// Le Wumpus est-il présent dans l'une des salles adjacentes ?
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		if (tunnels[i] == wumpus_room) {
			cout << "BRAVO !! Tu as tué le Wumpus qui se trouvait en salle " << wumpus_room << ". C'est gagné !!" << endl;
			return make_tuple(false, true);
		}
	}
	
	// Le Wumpus se réveille et bouge si la flèche a été tirée vers une salle accessible de la pièce où il se trouve
	for (unsigned int i = 0; i< size(tunnels); ++i) {

		// On récupère les salles accessibles de chaque pièce où une flèche a été décochée
		auto ze = current_maze.equal_range(tunnels[i]);
		for (auto t = ze.first; t != ze.second; ++t)
			if (t->second == wumpus_room) {
				wumpus_room=wumpus_move(wumpus_room, current_maze);
				cout << "Le Wumpus a été réveillé par l'une de tes flèches qui a atterri dans une pièce adjacente à l'endroit où il se trouve. Il a bougé..." << endl;
				return make_tuple(true, true);
			}
	}
	
	// Rien ne se passe...
	cout << "Le tir n'a eu aucun effet." << endl;
	return make_tuple(true,true);

}


// ***********************************************************************************************************************************************
int main()
{

	try
	{
		// A mettre dans la 1ère ligne du "main" pour afficher les caractères accentués sous la console Windows
		std::locale::global(std::locale("fr-FR"));
		
		// Initialisation du jeu ************************************************************************************************************** 
		// Génération aléatoire du labyrinthe
		auto maze = multimap<int, int>{};
		int nb_rooms = 20;
		srand((int)time(0));	// Seeding du moteur de génération aléatoire en partant de l'heure
		
		for (int room=1;room< nb_rooms;++room) {
			 
			// Création des tunnels de la room concernée
			tunnels(room, nb_rooms,maze);
			
		}
		
		// Vérification 'visuelle' du labyrinthe
		// for (auto const& tunnels : maze) cout << tunnels.first << '-' << tunnels.second << '\n';
		cout << "Taille du réseau créé : " << size(maze) << endl;

		// Placement du Wumpus, des fosses et des chauve-souris au hasard
		int wumpus_room = nb_aleatoire(1, nb_rooms);

		int nb_pits = 3;
		vector <int> pit_rooms{};
		for (int i = 0; i< nb_pits; ++i)
			pit_rooms.push_back(nb_aleatoire(1, nb_rooms));

		int nb_bats = 3;
		vector <int> bat_rooms{};
		for (int i = 0; i< nb_bats; ++i)
			bat_rooms.push_back(nb_aleatoire(1, nb_rooms));

		// Téléportation du joueur au démarrage dans une room vide
		bool player_room_full = true;
		int player_room = 0;

		while (player_room_full) {
			player_room = nb_aleatoire(1, nb_rooms);
			
			if (is_wumpus_here(player_room, wumpus_room) || is_pit_here(player_room, pit_rooms) || is_bat_here(player_room, bat_rooms))
				player_room_full = true;
			else 
				player_room_full = false;

		}

		// **********************************************************************************************************************************
		// Boucle de jeu
		// Tant que le wumpus ET le joueur sont vivants, on continue le jeu ; sinon, le jeu est terminé au bénéfice de l'un ou de l'autre :-)
		bool wumpus_alive = true;
		bool player_alive = true;
		int nb_arrows = 5;

		while (wumpus_alive && player_alive) {
		
			// Position du joueur et tunnels disponibles
			vector <int> tunnels_dispo{};
			tunnels_in_room(player_room, tunnels_dispo,maze);
			cout << "Tu es en salle " << player_room << " dotée des tunnels suivants : [ ";
			for (auto x : tunnels_dispo) cout << x<<' ';
			cout << "]. Move (mx), Shoot (s) or Quit (q) ?";
			
			// Saisie de l'option Move (mx), Shoot (s) or Quit (q)
			string option{};
			cin >> option;
			
			string choix = option.substr(0, 1);
			if (choix == "q") {
				cout << "Bye" << endl;
				player_alive=false;

			} else if (choix == "m") {
				// Si le numéro du tunnel choisi est valide, on transporte le joueur dans la nouvelle salle
				// Son "état de vie" peut y être affecté en fonction de la présence du wumpus ou d'une fosse ; sinon il est transféré dans la nouvelle salle (player_room updaté)
				int new_room = stoi(option.substr(1, size(option)-1));
				if (is_new_room_valid(new_room, tunnels_dispo)) player_alive = hazard(new_room, wumpus_room, pit_rooms, bat_rooms, player_room, nb_rooms);
							
			} else if (choix == "s") {
				// Il faut qu'il reste des flèches pour en tirer une...
				if (!nb_arrows)
					cout << "Tu n'as plus de flèches !"<<endl;
				
				else {
					--nb_arrows;	// ... sinon on les décrémente et on tire !
					cout << "Nombre de flèches restantes : "<< nb_arrows << endl;

					// On doit vérifier la survivance du wumpus ET du joueur post-tir : d'où le besoin de retourner deux valeur bool en utilisant un t-uple
					auto live = shoot(player_room, tunnels_dispo, wumpus_room, maze);
					wumpus_alive = get<0>(live);
					player_alive = get<1>(live);
				}

			}
		
		}

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

