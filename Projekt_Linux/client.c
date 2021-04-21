#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) //komunikat odpowiadajacy za wyswietlenie bledu
{
    perror(msg);
    exit(0);
}
int charToInt(char c) { // char to int 
	int num = 0;
	num = c - '0';
	return num;
}
int main(int argc, char *argv[])
{
    int sockfd, portno, n; //definicja zmiennych
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    int kwota = 0;
    int saldo = 200;
    if (argc < 3) //jezeli liczba podanych parametrow jest mniejsza od 3 (bo podajemy ip oraz port)
    { 
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]); //numer portu - musi byc indetyczny co jest w serverze
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // tworzene soketa
    if (sockfd < 0) //jezeli wartosc jest mniejsza od 0 - blad z polaczeniem                                           // 	TO WSZYSTKO ODPOWIADA ZA POLACZENIE SIE POPRAWNIE Z SERVEREM
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) { //jezeli wartosc servera jest = 0 - blad z polaczeniem
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting"); // blad polaczenia 

    printf("Wprowadz login: "); // wprowadzamy login
    bzero(buffer,256); 
    fgets(buffer,255,stdin);
    char username[20];
    memcpy(username, buffer, 20);
    n = write(sockfd,buffer,strlen(buffer)); //wpis dlugosci bufera
    if (n < 0) //jezeli mniejsze od zera to blad
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255); //odczyt dlugosci buffera
    if (n < 0) // jezeli mniejsze od zera to blad
         error("ERROR reading from socket");
    if(!strcmp(buffer, "1")){ // jezeli sie zgadza login
        printf("wprowadz haslo: ");
        bzero(buffer,256);
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer)); //wpis dlugosci buffera
        bzero(buffer,256);
        n = read(sockfd,buffer,255); //odczyt od klienta
        if(!strcmp(buffer,"3")){ // jezeli sie zgadza haslo 
        	int pozostan = 1;
        	system("clear");
        	printf("Zostales poprawnie zalogowany jako: %s\n", username);
        	while(pozostan){
        		printf("Co chcesz zrobic na koncie?\n");
        		printf("1.Dodaj srodki\n");
        		printf("2.Wyjmij srodki\n");                           //	OKNO APLIKACJI UZYTKOWNIKA
        		printf("3.Sprawdz stan konta\n");
        		printf("4.Wyjdz\n\n");
                printf("Wybierz opcje: "); //wybor opcji
                bzero(buffer,256);
                fgets(buffer,255,stdin);
                n = write(sockfd,buffer,strlen(buffer));
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
                int zianie = buffer[0];
                switch (zianie) // W ZALEZNOSCI OD WYBORU
                {
                case 49: //dodanie srodkow
                	printf("Ile chcesz dodac ?\n");
                	bzero(buffer,256);
                	fgets(buffer,255,stdin);
                	int wplacono = 0;
                	n = write(sockfd,buffer,strlen(buffer));
                	bzero(buffer,256);
					n = read(sockfd,&wplacono,sizeof(int));
					printf("-----------------------------\n");
					printf("Transakcja wykonana poprawnie\n");
					printf("Twoj stan konta to: %d\n", (wplacono));
					printf("-----------------------------\n");
                	break;
                case 50: //wyjecie srodkow
                	printf("Ile chcesz wyjac ?\n");
                	bzero(buffer,256);
                	fgets(buffer,255,stdin);
                	int Wyplacone = 0;
                	n = write(sockfd,buffer,strlen(buffer));
                	bzero(buffer,256);
					n = read(sockfd,&Wyplacone,sizeof(int));
					if(Wyplacone >= 0){
						printf("-----------------------------\n");
						printf("Transakcja wykonana poprawnie\n");
						printf("Twoj stan konta to: %d\n", (Wyplacone));
						printf("-----------------------------\n");
					}
					if (Wyplacone < 0) {
						printf("Nie masz wystarczajacych srodkow\n");
					}
    				break;
                case 51: //sprawdzenie stanu konta
                	n = write(sockfd,buffer,strlen(buffer));
                	bzero(buffer,256);
                	n = read(sockfd,&saldo,sizeof(int));
                	printf("-----------------------------\n");
					printf("Masz na koncie: %d\n", (saldo));
					printf("-----------------------------\n");
					break;
                case 52: // wyjscie 
					pozostan = 0;
					break;
				}
        	}
        } else{ // komunikat o blednym hasle
        	printf("Podano niepoprawne haslo\n");
        }
    } else { //komunikat o blednym loginie
    	printf("Podano niepoprawny login\n");
    	close(sockfd);
    }
    return 0;
}
