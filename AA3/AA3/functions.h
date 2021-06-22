struct dns {
	char id[50];
	int avgSpeed;
};


void removeSpaces(char* str);

int readPings();

const char* adaptadorRed();

int checkSpeed(char* dns);

int assignDNS(char* nombreAdaptador, char* fastestDNS);