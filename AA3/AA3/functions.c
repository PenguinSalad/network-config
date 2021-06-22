#define _CRT_SECURE_NO_WARNING
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>




void removeSpaces(char* str)
{
	// Cuenta los caracteres que no son espacios 
	int count = 0;

	// Recorre todo el string. Si i no es un espacio, colocalo al index count++ 
	for (int i = 0; str[i]; i++)
		if (str[i] != ' ')
			str[count++] = str[i];
	str[count] = '\0';
}

int readPings() {
	FILE* fp;
	char line[512] = "";
	bool ping = false;

	fp = fopen("output.txt", "r"); //Abrir fichero

	if (fp == NULL) {
		perror("Error");
		return(-1);
	}
	while (fgets(line, sizeof(line), fp) != NULL) { //Buscar la linea del output donde salga "ms"
		if (strstr(line, "ms")) {
			ping = true;
		}
	}

	if (ping) {
		printf("PING CORRECTO\n"); //Si hay una linea donde salga "ms", es que el ping es correcto
		return(1);
	}
	else {
		printf("PING INCORRECTO\n"); //Si no sale, es incorrecto
		return(0);
	}
	return 0;
}

const char* adaptadorRed() {
	char nombreAdaptador[100] = "", cmd[200] = "", line[512] = "";
	int dnsNumber = 1;
	FILE *fp, *ipsOK;

	printf("Introduce el nombre del adaptador que quieres modificar: \n"); //Preguntamos que adaptador quiere ver
	if (scanf("%s", nombreAdaptador) > 0) {
		strcpy(cmd, "netsh interface ipv4 show dns "); 
		strcat(cmd, nombreAdaptador);
		strcat(cmd, " > output.txt");
		system(cmd); //Lanzamos el comando "netsh interface ipv4 show dns [nombreAdaptador] > output.txt"
	}

	fp = fopen("output.txt", "r");
	if (fp == NULL) {
		perror("Error");
		return(-1);
	}
	ipsOK = fopen("ipsOK.txt", "w");
	if (ipsOK == NULL) {
		perror("Error");
		return(-1);
	}

	printf("\n---------------Viendo adaptador: %s---------------\n", nombreAdaptador);

	while (fgets(line, sizeof(line), fp) != NULL) {
		if (strstr(line, "DNS")) { //Buscamos la linea donde sale la dns
			char* token = strtok(line, ":");
			token = strtok(NULL, ":");
			removeSpaces(token); // Con esta funcion quitamos los espacios de la linea que puedan molestar a la hora de formatear
			printf("DNS %d: %s\n", dnsNumber, token);
			fprintf(ipsOK,"%s", token);
			fgets(line, sizeof(line), fp);
			while (strstr(line, "Registrar") == NULL) { //Buscamos todas las posibles DNS
				dnsNumber++;
				removeSpaces(line); // Con esta funcion quitamos los espacios de la linea que puedan molestar a la hora de formatear
				printf("DNS %d: %s\n", dnsNumber, line);
				fprintf(ipsOK, "%s", line);
				fgets(line, sizeof(line), fp);
			}

		}
	}
	fclose(fp);
	fclose(ipsOK);

	printf("\n--------------------------------------------------------\n");
	return nombreAdaptador;


}

int checkSpeed(char* dns) { //Give dns, return avg 
	char cmd[200] = "", line[512] = "", mediaChar[5] = "";;
	int avgSpeed;
	FILE* fp;

	strcpy(cmd, "ping ");
	strcat(cmd, dns);
	strcat(cmd, " > output.txt");
	system(cmd);

	//Abrimos el fichero output.txt
	fp = fopen("output.txt", "r");
	if (fp == NULL) {
		perror("Error");
		return(-1);
	}
	//Analizar resultado ping
	while (fgets(line, sizeof(line), fp) != NULL) {
		if (strstr(line, "Media")) {
			char* token = strtok(line, ",");
			for (int i = 0; i < 2; i++) { //Coneguimos el 3 valor que es el de la media
				token = strtok(NULL, ",");
			}

			size_t i = 0, j = 0;
			


			while (token[i] != '\0') {
				if (isdigit(token[i])) {  //Separamos los numeros del resto y lo almacenamos en una variable para la posterior comparacion
					mediaChar[j] = token[i];
					j++;
				}
				i++;
			}

		}
	}
	avgSpeed = atoi(mediaChar); //char to int
	fclose(fp); //Cerramos el fichero

	return avgSpeed;
}

int assignDNS(char* nombreAdaptador, char* fastestDNS) {
	char cmd[200] = "";

	strcpy(cmd, "netsh interface ip set dns name=\"");
	strcat(cmd, nombreAdaptador);
	strcat(cmd, "\" source=\"static\" address=\"");
	strcat(cmd, fastestDNS);
	strcat(cmd, "\"");
	
	if (system(cmd) != -1) { return 1; }  //Lanzamos el comando netsh interface ip set dns name="[nombreAdaptador]" source="static" address="[fastestDNS]"
	else { return -1; } //system() devuelve -1 si hay un error
}