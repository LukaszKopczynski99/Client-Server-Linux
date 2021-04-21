#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>
#include <time.h>
void error(const char *msg) //blad 
{
    perror(msg);
    exit(1);
}
struct user //struktura zawierajaca nazwe uzytkownika, haslo oraz saldo
{
	char *username; 
	char *password;
	int saldo;
};
int checkUser(struct user arr[],char user[]) //sprawdzanie uzytkownika
{
	
	for(int i = 0; i < 3;i++)
	{
		if(!strcmp(arr[i].username, user)) 
		{
			return (i+1);
		}
	}
	return 0;
};
int checkPass(int i, struct user arr[], char pass[]) // sprawdzanie poprawnosci hasla
{
		if(!strcmp(arr[i].password, pass)) 
		{
			return 1;
		}
		else 
		{
			return 0;
		}
};

int main(int argc, char *argv[]) 
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    int kwota;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int m;
    if (argc < 2) //w przypadku nie podania portu
    {
         fprintf(stderr, "ERROR, no port provided\n");
         exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) //blad otworzenia socketu
         error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5); //nasluch - wskazuje to ze gniazdo jest gotowe do przyjmownaia polaczen 
    clilen = sizeof(cli_addr);
  
    int pid;
    while (1) 
    {
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         struct user users[2];
         time_t t = time(NULL);
		 struct tm tm = *localtime(&t);
		 FILE *fptr;
         users[0].username = "lukasz"; // user 1 - lukasz 
         users[0].password = "lukasz";
		 fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_1.txt", "r"); //otwarcie salda dla uzytkownika pierwszego
		 fscanf(fptr, "%d", &users[0].saldo); //sprawdzenie zawartosci pliku 
		 fclose(fptr); // zamkniecie pliku

         users[1].username = "kacper"; //user 2 login
         users[1].password = "kacper"; //user 2 haslo
		 fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_2.txt", "r");//otwarcie salda dal uzytkownika drugiego
		 fscanf(fptr, "%d", &users[1].saldo); //sprawdzenie zawartosci pliku
		 fclose(fptr); //zamkniecie pliku

         if (newsockfd < 0)
                error("ERROR on accept");
         
         pid = fork();
         if (pid < 0) // problem z tworzeniem nowego procesu
	 {
              error("ERROR in new process creation");
         }
         if (pid == 0) {
            
            close(sockfd);
         
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255); //odbieranie zawartosci
            int licznik = 0;
            for(int i = 0; i < 255; i++) 
	    {
            	if(buffer[i] > 0)
		{
            		licznik++; //sprawdzenie ile w buferze jest zapisanych znakow
            	}
            }
            char slowo[licznik-1];
            for(int i = 0; i< (licznik-1); i++)
	    {
            	slowo[i] = buffer[i];
            }
            if(checkUser(users, slowo)){
            	int itemArr;
            	itemArr = checkUser(users,slowo) - 1;
				n = write(newsockfd, "1", 1);
				bzero(buffer, 256);
				n = read(newsockfd, buffer, 255); 			//MECHANIZM SPRAWDZANIA UZYTKOWNIKA
				licznik = 0;
				for(int i = 0; i < 255; i++) 
				{
				            	if(buffer[i] > 0)
						{
				            		licznik++;
				            	}
				}
				char haslo[licznik-1];
				for(int i = 0; i< (licznik-1); i++)
				{
					haslo[i] = buffer[i];
				}
				if(!strcmp(users[itemArr].password, haslo)) 
				{
		        	fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Logs.txt", "a"); //otworz plik logow
		        	fprintf(fptr, "%d-%02d-%02d %02d:%02d:%02d - Zalogowano jako: %s\n", tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, users[itemArr].username); //zapisz do logow
		        	fclose(fptr); //zamknij plik
					n = write(newsockfd, "3", 1);
					int zostan = 1;
					while(zostan)                         // MENU OBSLUGI KLIENTA
					{
						bzero(buffer, 256);
						n = read(newsockfd, buffer, 255);
						char operacja;
						operacja = buffer[0];
						n = write(newsockfd, &operacja, 1);
						if(operacja == 49){ //Dodanie kwoty
							bzero(buffer, 256);
							n = read(newsockfd, buffer, 255);
							char *ptr;
							ptr = buffer;
							int kwota = 0;
							sscanf(ptr, "%d", &kwota);
							fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Logs.txt", "a");
							fprintf(fptr, "%d-%02d-%02d %02d:%02d:%02d - Do konta uzytkownika %s dodano sume %d\n", tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, users[itemArr].username, kwota);
							fclose(fptr);
							users[itemArr].saldo += kwota; 
							n = write(newsockfd, &users[itemArr].saldo, sizeof(int));
							if(!strcmp(users[itemArr].username, "lukasz")){
								fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_1.txt", "w");
								printf("%d\n", users[itemArr].saldo);
								fprintf(fptr, "%d", users[itemArr].saldo);
								fclose(fptr);
							}
							if(!strcmp(users[itemArr].username, "kacper"))
							{
								fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_2.txt", "w");
								fprintf(fptr, "%d", users[itemArr].saldo);
								fclose(fptr);
							}
						}
						if(operacja == 50) //Wyplacenie
						{
							bzero(buffer, 256);
							n = read(newsockfd, buffer, 255);
							char *ptr;
							ptr = buffer;
							int kwota = 0;
							sscanf(ptr, "%d", &kwota);
							if(kwota <= users[itemArr].saldo){
								fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Logs.txt", "a");
								fprintf(fptr, "%d-%02d-%02d %02d:%02d:%02d - Z konta uzytkownika %s wybrano sume %d\n", tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, users[itemArr].username, kwota);
								fclose(fptr);
								users[itemArr].saldo -= kwota;
								n = write(newsockfd, &users[itemArr].saldo, sizeof(int));
								if(!strcmp(users[itemArr].username, "lukasz")){
									fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_1.txt", "w");
									fprintf(fptr, "%d", users[itemArr].saldo);
									fclose(fptr);
								}
								if(!strcmp(users[itemArr].username, "kacper")){
									fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_2.txt", "w");
									fprintf(fptr, "%d", users[itemArr].saldo);
									fclose(fptr);
								}
						} 
						else 
						{
								int aha = -1;
								n = write(newsockfd, &aha, sizeof(int));
						};
						}
						if(operacja == 51) //sprawdzenie stanu konta
						{
							bzero(buffer, 256);
							 fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_1.txt", "r");
							 fscanf(fptr, "%d", &users[0].saldo);
							 fclose(fptr);
							 fptr = fopen("/home/student/Desktop/Projekt_Linux/Pliki_testowe/Saldo_2.txt", "r");
							 fscanf(fptr, "%d", &users[1].saldo);
							 fclose(fptr);
							n = read(newsockfd, buffer, 255);
							char *ptr;
							ptr = buffer;
							int kwota = 0;
							sscanf(ptr, "%d", &kwota);
							n = write(newsockfd, &users[itemArr].saldo, sizeof(int));
						}
						if(operacja == 52) //wyjscie
						{
							zostan = 0;
						}
					}
					close(newsockfd);
				} 
				else 
				{
					n = write(newsockfd, "4", 1);
				}
				if (m < 0)
					 error("ERROR writing to socket");
				close(newsockfd);
            }	
		else 
		{
				n = write(newsockfd, "2", 1);
				if (n < 0)
					 error("ERROR writing to socket");
            }
          } 
		else 
		{
             close(newsockfd);
          }
    }

   return 0;
}
