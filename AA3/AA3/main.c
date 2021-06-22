#define _CRT_SECURE_NO_WARNING

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "functions.h"

int main() {
	char rutaArchivo[150] = "", ip[15] = "", cmd[100] = "", nombreAdaptador[100] = "";
	FILE *DNSips, *ipsOK;
	struct dns dns[10] = { 0, 0 };

	while (true) {
		printf("Introduce la ruta del archivo con las ip a leer: \n");
		if (scanf("%s", &rutaArchivo) > 0) {
			
			DNSips = fopen(rutaArchivo, "r"); // Abrimos fichero con todas las ip

			if (DNSips == NULL) {
				perror("Error");
				return(-1);
			}

			printf("\n---------------Contenido del archivo % s---------------\n", rutaArchivo);

			while (fgets(ip, sizeof(ip), DNSips) != NULL) {
				printf("%s", ip);
			}
			fclose(DNSips);

			printf("\n-----------------------------------------------------------------\n");

			strcpy(nombreAdaptador, adaptadorRed());


			DNSips = fopen(rutaArchivo, "r"); // Abrimos fichero con todas las ip
			ipsOK = fopen("ipsOK.txt", "a"); // Abrimos fichero donde pondremos las ip que dan respuesta

			if (DNSips == NULL) {
				perror("Error");
				return(-1);
			}

			if (ipsOK == NULL) {
				perror("Error");
				return(-1);
			}

			
			while (fgets(ip, sizeof(ip), DNSips) != NULL) { //Hacemos loop sobre todas las lineas del fichero
				if (ip[strlen(ip) - 1] == '\n') { //Borramos los caracteres '\n' para evitar problemas de lectura
					ip[strlen(ip) - 1] = '\0';
				}
				if (ip[0] != '\0') { //Con este desigual nos aseguramos de que no lea lineas en blanco
					printf("\nPinging ip %s...\n", ip);
					strcpy(cmd, "ping ");
					strcat(cmd, ip);
					strcat(cmd, " > output.txt");
					system(cmd); //Lanza el comando "ping [ip] > output.txt"
					if (readPings() == 1) { //Leemos el resultado
						fprintf(ipsOK, "%s\n", ip); // Apuntamos las ips con respuesta en un archivo temporal					
						
					}
				}
			}
			fclose(DNSips);
			fclose(ipsOK);

			ipsOK = fopen("ipsOK.txt", "r"); //Ahora que tenemos todas las ip posibles, vamos a leerlas y comparar velocidades
			int i = 0;
			while (fgets(ip, sizeof(ip), ipsOK) != NULL) { //Hacemos loop sobre todas las lineas del fichero
				if (ip[strlen(ip) - 1] == '\n') { //Borramos los caracteres '\n' para evitar problemas de lectura
					ip[strlen(ip) - 1] = '\0';
				}
				if (ip[0] != '\0') { //Con este desigual nos aseguramos de que no lea lineas en blanco
					strcpy(dns[i].id, ip);
					dns[i].avgSpeed = checkSpeed(dns[i].id);
					//printf("%s, %d\n", dns[i].id, dns[i].avgSpeed);
					i++;
				}
			}
			fclose(ipsOK);
			printf("\n--------------------------------------------------------\n");
			int fastest = dns[0].avgSpeed, location = 0; //Con este bloque de codigo, vamos a hacer loop a todas las dns del documento ipsOK.txt y veremos cual es la mas rapida.
			for (int j = 0; j < i; j++) {
				if (dns[j].avgSpeed < fastest) {
					fastest = dns[j].avgSpeed;
					location = j;
				}
			}
			printf("La DNS mas rapida es la %s, con una media de %dms de respuesta\n", dns[location].id, dns[location].avgSpeed);
			
			if (assignDNS(nombreAdaptador, dns[location].id) == 1) { printf("DNS %s aplicada correctamente\n", dns[location].id); }
			else { 
				perror("Error"); 
				return -1;
			}


			break;
		} else {
			printf("Ruta incorrecta\n");
		}
	}
	

}