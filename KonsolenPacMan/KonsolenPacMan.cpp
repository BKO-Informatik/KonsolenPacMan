#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

void verarbeiteTastendruck(int& y, int &x, int& punkte);
void zeigeSpielBeschreibung();
int  spielerSetztSpielgeschwindigkeit();
void zeigeAbschiedsgrussSpielende(const int& erzieltePunkte);

const u_int  REIHEN  = 23;
const u_int  SPALTEN = 73;
char tmp_spielfeld[REIHEN][SPALTEN];
char spielfeld[REIHEN][SPALTEN] = {
	"+######################################################################+",
	"|                                                                      |",
	"|                                                                      |",
	"|          ########   #########             #########                  |",
	"|                                                                      |",
	"| ##############                                                       |",
	"| #                                                                    |",
	"| #  ######################                                            |",
	"| #                                                                    |",
	"| ##############  #################                                    |",
	"|                                                                      |",
	"|#######   ######################################                      |",
	"|                                                                      |",
	"|####################################### ########                      |",
	"|                                                                      |",
	"|  ##############################################                      |",
	"|                                                                      |",
	"|      ########        ##                                              |",
	"|                                                                      |",
	"|                                                                      |",
	"|                                                                      |",
	"|                                                                      |",
	"+######################################################################+"
};

void zeigeSpielfeld( ) {
	/*
	for( int i = 0; i < REIHEN; i++ ) 
		cout << spielfeld[i] << endl;
	*/
	for( int i = 0; i < REIHEN; i++ ) {
		for( int j = 0; j < SPALTEN; j++ ) {
			cout << spielfeld[i][j];
		}
		cout << "\n";
	}
}

void gehezuXY( u_int x, u_int y ) {
	HANDLE hStdout = GetStdHandle( STD_OUTPUT_HANDLE );
	COORD position = { x, y };
	SetConsoleCursorPosition( hStdout, position );
}

struct sMoeglicherWegpunktZumHeld {
	u_int zaehler;
	u_int x;
	u_int y;
	u_int vorherigeWegpunktNummer;
};

struct sKoordinatenXY {
	u_int x;
	u_int y;
};

vector<sKoordinatenXY> vFeindWegPuffer;
vector<sMoeglicherWegpunktZumHeld>  vTmpWegPunkt;

void ergaenzeMoeglichenWegpunktZumHeld( u_int x, u_int y, u_int wc, u_int wegpunktNummer ) {
	if( tmp_spielfeld[y][x] == ' ' || tmp_spielfeld[y][x] == '.' ) {
		tmp_spielfeld[y][x] = '#';  // Tempraeres Spielfeld mit beliebigen Zeichen besetzen
		sMoeglicherWegpunktZumHeld wegpunkt;
		wegpunkt.x = x;
		wegpunkt.y = y;
		wegpunkt.zaehler = wc;
		wegpunkt.vorherigeWegpunktNummer = wegpunktNummer;
		vTmpWegPunkt.push_back( wegpunkt );
	}
}

void findeWegZumHeld( u_int feindX, u_int feindY, u_int heldX, u_int heldY ) {
	// memcpy( void *destination, const void *source, size_t num );
	memcpy( tmp_spielfeld, spielfeld, sizeof( spielfeld ) );
	vTmpWegPunkt.clear( );
	sMoeglicherWegpunktZumHeld wegpunkt;
	wegpunkt.x = feindX;
	wegpunkt.y = feindY;
	wegpunkt.zaehler = 0;
	wegpunkt.vorherigeWegpunktNummer = -1;  // -1 bedeutet Startpunkt beim Feind
	vTmpWegPunkt.push_back( wegpunkt );
	size_t i = 0;
	while( i < vTmpWegPunkt.size( ) ) {
		if( vTmpWegPunkt[i].x == heldX && vTmpWegPunkt[i].y == heldY ) {
		// Feind hat einen Weg zum Held gefunden da die X und Y
		// Koordinaten von Feind und Held gleich sind
			vFeindWegPuffer.clear( );  // alten WegPuffer loeschen
			sKoordinatenXY zielXY;
			while( vTmpWegPunkt[i].zaehler != 0 ) {
				zielXY.x = vTmpWegPunkt[i].x;
				zielXY.y = vTmpWegPunkt[i].y;
				vFeindWegPuffer.push_back( zielXY );  // Wegpunkte vom Held weg zum Feind werden gespeichert
				i = vTmpWegPunkt[i].vorherigeWegpunktNummer;             //
			}
			break;
		}
		// von der aktuellen Position wird ein moeglicher Wegpunkt in x+1, x-1, y+1 und y-1 erzeugt
		ergaenzeMoeglichenWegpunktZumHeld( vTmpWegPunkt[i].x + 1, vTmpWegPunkt[i].y,     vTmpWegPunkt[i].zaehler + 1, i );
		ergaenzeMoeglichenWegpunktZumHeld( vTmpWegPunkt[i].x - 1, vTmpWegPunkt[i].y,     vTmpWegPunkt[i].zaehler + 1, i );
		ergaenzeMoeglichenWegpunktZumHeld( vTmpWegPunkt[i].x,     vTmpWegPunkt[i].y + 1, vTmpWegPunkt[i].zaehler + 1, i );
		ergaenzeMoeglichenWegpunktZumHeld( vTmpWegPunkt[i].x,     vTmpWegPunkt[i].y - 1, vTmpWegPunkt[i].zaehler + 1, i );
		i++;
	}
	vTmpWegPunkt.clear( );
}

int main() {
	const char HELD = '0';
	const char FEIND = 'X';
	const char PUNKTE_SAMMELT_HELD = '.';   // DOTS sind zum Essen fuer den Held
	const int BILD_PUNKTESTAND_X = 31;
	const int BILD_PUNKTESTAND_Y = 1;
	const int STARTPOSITION_HELD_X = 15;
	const int STARTPOSITION_HELD_Y = 16;
	const int STARTPOSITION_FEIND_X = 1;
	const int STARTPOSITION_FEIND_Y = 1;
	int feindX = STARTPOSITION_FEIND_X;
	int feindY = STARTPOSITION_FEIND_Y;
	int heldX = STARTPOSITION_HELD_X;
	int heldY = STARTPOSITION_HELD_Y;
	int altX = 0;
	int altY = 0;
	int erzieltePunkte = 0;
	int bild = 0;
	bool spielSollLaufen = true;
	zeigeSpielBeschreibung( );
	int spielgeschwindigkeit = spielerSetztSpielgeschwindigkeit( );
	system( "cls" );
	zeigeSpielfeld( );
	gehezuXY( heldX, heldY );
	cout << HELD;
	findeWegZumHeld( feindX, feindY, heldX, heldY );
	while( spielSollLaufen == true ) {
		gehezuXY( heldX, heldY );
		cout << " ";
		altX = heldX;
		altY = heldY;
		verarbeiteTastendruck( heldY, heldX, erzieltePunkte );

		if( altX != heldX || altY != heldY ) {  // hat sich unser Held bewegt
			findeWegZumHeld( feindX, feindY, heldX, heldY );
		}
		gehezuXY( heldX, heldY );
		cout << HELD;
		spielfeld[feindY][feindX] = '.';
		gehezuXY(feindX, feindY);
		cout << PUNKTE_SAMMELT_HELD;  // Punkte die der Feind liegen laesst und die der Held sammeln kann
		if( bild%spielgeschwindigkeit == 0 && vFeindWegPuffer.size( ) != 0 ) {
			feindX = vFeindWegPuffer.back( ).x;
			feindY = vFeindWegPuffer.back( ).y;
			vFeindWegPuffer.pop_back( );
			bild = 0;
		}
		gehezuXY( feindX, feindY );
		cout << "X";
		if( feindX == heldX && feindY == heldY ) {  // verloren
			zeigeAbschiedsgrussSpielende( erzieltePunkte );
			spielSollLaufen = false;
		}
		gehezuXY( BILD_PUNKTESTAND_X, BILD_PUNKTESTAND_Y );
		cout << erzieltePunkte;
		Sleep( 100 );
		bild++;
	}
	return 0;
}

void verarbeiteTastendruck( int& y, int &x, int& punkte ) {
	if( GetAsyncKeyState( VK_UP ) ) {
		if( spielfeld[y - 1][x] == '.' ) {
			y--;
			punkte++;
		}
		else if( spielfeld[y - 1][x] == ' ' ) {
			y--;
		}
	}
	if( GetAsyncKeyState( VK_DOWN ) ) {
		if( spielfeld[y + 1][x] == '.' ) {
			y++;
			punkte++;
		}
		else if( spielfeld[y + 1][x] == ' ' ) {
			y++;
		}
	}
	if( GetAsyncKeyState( VK_LEFT ) ) {
		if( spielfeld[y][x - 1] == '.' ) {
			x--;
			punkte++;
		}
		else if( spielfeld[y][x - 1] == ' ' ) {
			x--;
		}
	}
	if( GetAsyncKeyState( VK_RIGHT ) ) {
		if( spielfeld[y][x + 1] == '.' ) {
			x++;
			punkte++;
		}
		else if( spielfeld[y][x + 1] == ' ' ) {
			x++;
		}
	}
	// BEAMEN
	/*
	if( GetAsyncKeyState( VK_SPACE ) ) {
	x = X_HELD_STARTPOINT;
	y = Y_HELD_STARTPOINT;
	}
	*/
}

void zeigeSpielBeschreibung( ) {
	std::cout << "Hinweise:\n1. Benutze die Pfeiltasten um den Held zu bewegen.\n";
	std::cout << "2. Dein Gegner spuckt Punkte die Dein Held aufsammeln kann, um Punkte zu sammeln.\n";
	std::cout << "3. Lass Deinen Helden nicht vom Gegner fangen und somit vernichten.\n\n";
}

int spielerSetztSpielgeschwindigkeit( ) {
	while( 1 ) {
		const int HARDMODE_SPIELGESCHWINDIGKEIT   = 1;
		const int NORMALMODE_SPIELGESCHWINDIGKEIT = 2;
		const int EASYMODE_SPIELGESCHWINDIGKEIT   = 5;
		cout << "1 -> Hardmode\n2 -> Normalermodus\n3 -> Easybeasy\n\nEingabe: ";
		string tmp;
		getline( cin, tmp );
		try {
			int geschwindigkeit = std::stoi(tmp);
			if( geschwindigkeit == 1 )
				return HARDMODE_SPIELGESCHWINDIGKEIT;
			else if( geschwindigkeit == 2 )
				return NORMALMODE_SPIELGESCHWINDIGKEIT;
			else if( geschwindigkeit == 3 )
				return EASYMODE_SPIELGESCHWINDIGKEIT;
			else {
				system( "cls" );
				continue;
			}
		}
		catch (...) {
			system( "cls" );
		    continue;
		}
	    continue;
	}
}

void zeigeAbschiedsgrussSpielende( const int& erzieltePunkte ) {
	system( "cls" );
	cout << "Du bist geschnappt worden.\n";
	cout << "Immerhin konntest Du " << erzieltePunkte << " sammeln, junger Padawan.\n";
	cout << "Ende mit enter...." << endl;
	string s;
	getline( cin, s );
}