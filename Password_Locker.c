/*
______  ___   _____ _____  _    _  _________________        _     _____ _____  _   __ ___________       ______ _____
| ___ \/ _ \ /  ___/  ___|| |  | ||  _  | ___ \  _  \      | |   |  _  /  __ \| | / /|  ___| ___ \      |  _  \_   _|
| |_/ / /_\ \\ `--.\ `--. | |  | || | | | |_/ / | | |      | |   | | | | /  \/| |/ / | |__ | |_/ /      | | | | | |
|  __/|  _  | `--. \`--. \| |/\| || | | |    /| | | |      | |   | | | | |    |    \ |  __||    /       | | | | | |
| |   | | | |/\__/ /\__/ /\  /\  /\ \_/ / |\ \| |/ /       | |___\ \_/ / \__/\| |\  \| |___| |\ \       | |/ / _| |_
\_|   \_| |_/\____/\____/  \/  \/  \___/\_| \_|___/        \_____/\___/ \____/\_| \_/\____/\_| \_|      |___/  \___/



    _____   ___  ____________ _____ _____ _      _____       ______ ___________ _   _   ___  ____________ _____
   |  __ \ / _ \ | ___ \ ___ \_   _|  ___| |    |  ___|      | ___ \  ___| ___ \ \ | | / _ \ | ___ \  _  \_   _|
   | |  \// /_\ \| |_/ / |_/ / | | | |__ | |    | |__        | |_/ / |__ | |_/ /  \| |/ /_\ \| |_/ / | | | | |
   | | __ |  _  || ___ \    /  | | |  __|| |    |  __|       | ___ \  __||    /| . ` ||  _  ||    /| | | | | |
   | |_\ \| | | || |_/ / |\ \ _| |_| |___| |____| |___       | |_/ / |___| |\ \| |\  || | | || |\ \| |/ / _| |_
    \____/\_| |_/\____/\_| \_|\___/\____/\_____/\____/       \____/\____/\_| \_\_| \_/\_| |_/\_| \_|___/  \___/

*/

/// ---------------- P R O T O C O L L I ----------------

/*
   Per corregere l'assenza del file DefaultUser verra', dopo la conferma dell'utente, avviato il programma "Repair.bat" che creera' tutte le directory ed i file e necessari al programma per essere eseguito
*/
/*
   Per verificare l'integrita' del file utente di default controlla che:
   -  Al suo interno ci siano 80 caratteri (compreso l'eof (end of file)), non uno in piu' e non uno meno
   -  Come primo carattere non ci sia una '@'
   -  Come 40esimo carattere ci sia il trattino ('-')
   -  Come 41esimo carattere non ci sia una '@'
*/

/*
   La registrazione degli account deve avvenire nel formato: 39_CARATTERI_PER_USERNAME-39caratteri_per_la_password

   NOTA CHE:
   -  Gli spazi vuoti devono essere riempiti con @ quindi non sarà possibile inserire quel carattere ne' nell'username ne' nella password
   -  Il trattino separera' l'username dalla password di accesso
   -  L'username deve essere trasformato tutto in maiuscolo

*/
/*
   Le registrazione dei file ha un procedimento diverso rispetto alla registrazione dell'account del Password Locker

   NOTA CHE:
   -  In un prima parte verra' inserito solamente il nome del sito, effettuando gli opportuni controlli, in seguito verra' sommata la stringa ".txt" per creare un nome file assegnabile
   -  Il nome file deve essere trasformato tutto in maiuscolo
   -  Il nome del sito puo' arrivare ad un massimo di 29 caratteri

   L'username e la password nei Siti devono essere salvati in un modo simile a quelli del Password Locker:
   -  Gli USERNAME e le password possono arrivare a 39 caratteri (LS --> Lunghezza Stringa)
   -  Gli spazi inutilizzati devono essere riempiti con @ quindi non sarà possibile inserire quel carattere ne' alla fine dell'username ne' alla fine della password
   -  Il trattino separera' l'username dalla password di accesso
   -  L'username deve essere trasformato tutto in maiuscolo

   I PIN devono essere salvati in un modo particolare, infatti devono:
   -  Avere una lunghezza minima di 4 caratteri ed una massima di 8
   -  Non contenere caratteri diversi da numeri
   -  ESSERE SALVATI COME STRINGHE PER PERMETTERE DI AVERE LO ZERO COME PRIMO CARATTERE

*/

/*
   E' possibile utilizzare system(stringa), con stringa che e' un array di char e contiene per esempio "md NomeCartella", in questo modo e' possibile creare una cartella per ogni utente.
   Accertarsi di utilizzare la giusta sintassi, per accertarsi della correttezza della sintassi si può utilizzare una variabile globale di tipo "stringa" con di default un valore ad esempio:
   char CreateDirectory[LS+3] = {"md " + NomeUtente};
   con NomeUtente --> char NomeUtente[LS];

   Ricordarsi che se si vuole creare una sotto cartella e' necessario creare prima le due cartelle poi utilizzare sistem("move cartellaMov cartellaDest");
   per spostare la prima cartella nella seconda. Ricordati di cancellare la cartella che devi spostare in caso non si sia effettivamente spostata

   OPPURE: avvalersi di un piccolo file .bat che esegue automaticamente l'istruzione
*/


/// ----------------- L I B R E R I E -----------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <windows.h>


/// ---------- C O N S T A N T S ----------

#define LP 9   //8 caratteri + finestringa --> lunghezza massima del PIN
#define CMD 25 //24 caratteri + finestringa --> Utilizzato per il command ("Users/DefaultUser/Sites/" o "Users/DefaultUser/Pins/")
#define LS 150  //149 caratteri + finestringa --> gestione USERNAME e Password nei file e Login_Hint
#define LF 180  //29 caratteri + finestringa a cui verra' aggiunta l'estensione ".txt" --> gestione nomeFile
#define ROWL 500  //499 caratteri + finestringa --> utilizzato per leggere le righe del database da cui importare le credenziali

const char SiteCommand[LS + CMD] = "Users/DefaultUser/Sites/";  // --> Si potrebbe utilizzare in futuro per avere l'inizio dell'indirizzo dei file Siti
const char PinCommand[LS + CMD] = "Users/DefaultUser/Pins/";  // --> Si potrebbe utilizzare in futuro per avere l'inizio dell'indirizzo dei file PIN
//strrchr(nomeStringa, 'CarattereDaCercare');  //Restituisce l'indirizzo del puntatore dell'ultimo carattere appartenente alla stringa uguale a quello cercato, NULL se non c'e'. Utile per mettere il finestringa all'ultimo carattere ripetuto voluto


#define VERSION "Password_Locker-Version_Alpha.2.1"
#define VERSION_URL "https://bernardigabriele.altervista.org/Password_Locker/Latest_Version.txt"
#define DOWNLOAD_URL "https://bernardigabriele.altervista.org/Password_Locker/DOWNLOADABLE_VERSIONS/VersioneLettaDalFileLatest_Version.txt"


/// ------------------ S T R U C T S ------------------

typedef struct
{
   char ACCESS_USERNAME[LS];
   char ACCESS_PASSWORD[LS];
   char LOGIN_HINT[2*LS + 1];  // IN FASE DI SVILUPPO --> 200 caratteri + finestringa --> servira' per mostrare un' indizio sulla password
} User;

typedef struct
{
   char SITE_NAME[LS];
   char SITE_USERNAME[LS];
   char SITE_PASSWORD[LS];
} DataBaseSite;


/// --------- G L O B A L   V A R I A B L E S ---------

//Serviranno in futuro per la gestione delle scadenze delle password per avere come variabile globale la data odierna
int Day; // --> Conterra' il giorno attuale
int Month;  // --> Conterra' il mese attuale
int Year;   // --> Conterra' l'anno attuale

bool Block = false;  // IN FASE DI SVILUPPO --> Sara' impostata su true nel caso in cui l'utente abbia effettuato N tentativi errati di inserimento della password e non e' passato un giorno dall'ultimo tentativo
bool AlreadyRegistered = false;  // --> Variabile che verra' modificata dalla funzione per la registrazione, serve per controllare lo stato di registrazione di un utente
bool LoginHint = false; // IN FASE DI SVILUPPO --> Sara' impostata su true nel caso in cui l'utente abbia deciso di dare un indizio dopo N tentativi errati di inserimento della password
bool LoggedIn = false;  // --> Variabile che verra' modificata dalla funzione per il login, serve per controllare lo stato di login di un utente

int SitesAmount = 0;
int PinsAmount = 0;

User user;  // --> Conterra' l'USERNAME e la password corretti per la visualizzazione dopo la fase di login, non sara' dunque necessaria la "decodifica"


/// --------------- P R O T O T Y P E S ---------------

/// ---- DEVELOPER FUNCTIONS ----

void getDate();   // IN FASE DI SVILUPPO --> Servira' per ottenere la data odierna e aggiornare le apposite variabili globali

void RepairFunction();  // --> Avvia il file Repair.bat che crea i file e le directory mancanti

void checkRegistrationAndUpload();  // --> Verifica la presenza e la correttezza della registrazione di un utente e la carica all'interno della struct

int getAmountOfSites(); // --> Restituisce il numero di siti presenti nel file Sites.txt
void loadSites(char SITES[][LS]); // --> Aggiorna il vettore SITES

int getAmountOfPins();  // --> Restituisce il numero di PIN presenti nel file Pins.txt
void loadPins(char PIN[][LS]); // --> Aggiorna il vettore PIN

void getSiteOrPinNameFromCommand(char Command[], char SiteOrPinName[]);  // --> Sovrascrive la seconda stringa passata con il nome del sito o del PIN estrapolato dalla prima stringa command

void loadAccounts();    // IN FASE DI SVILUPPO --> Carichera' all'interno di un vettore di struct User tutte le credenziali degli utenti registrati

void importDataFromCSV(char FileName[]);  // --> Crea un vettore di struct e richiama il resto delle funzioni che serviranno per completare l'importazione
void getDataFromCSV(char ROW[], DataBaseSite *DATA_BASE_SITES);   // --> Estrapola i dati dalla riga ricevuta e li mette all'interno della struct ricevuta
void createAndFillSiteFromCSV(DataBaseSite DATA_BASE_SITES[], short int N);   // --> Crea il file, se ne esiste gia' uno aggiunge un "_" ed incrementa la lettera finale al valore successivo; in seguito ne inserisce i dati


/// ---- LOGIN FUNCTIONS ----

void loginPage(); // --> Menu per la registrazione o il login dell'utente e per svolgere le funzioni di base

void registerFunction();   // --> Serve per effettuare la registrazione di un account
void loginFunction(bool AskKeepLogged);   // --> Serve per effettuare il login di un account
void KeepLoggedFunction(bool CheckFromFile); // --> Serve per controllare o per chiedere se l'utente deve essere disconnesso ogni volta


/// ---- USER FUNCTIONS ----

void Menu(char SITES[][LS], char PIN[][LS]); // --> Menu attraverso il quale l'utente interagisce con il programma

void ManageAccount();   // --> Menu attraverso il quale l'utente puo' gestire l'account
void viewAccountData(); // --> Serve per visualizzare le credenziali di accesso
void removeAccount();   // --> Serve per cancellare tutti i dati di un account
void changePassword();  // --> Permette di cambiare la password dell'account del Password Locker



void createSite(char SITES[][LS]);   // --> Crea i file contenenti USERNAME e password del sito con il nome del file creato
void fillSite(char Command[]);   // --> Riempie il file del Sito appena creato con l'USERNAME e la password inseriti dall'utente

void searchSite(char SITES[][LS], int Option);  // --> Cerca il sito con il nome inserito dall'utente dal file Sites.txt, in caso non lo trova procede con la ricerca approfondita per eseguire una funzione in base all'opzione passata
bool advancedSearchSite(char SITES[][LS], char SITE_NAME[], char Command[]);  // --> Effettua una ricerca della parola cercata all'interno dei nomi dei siti
void readSite(char Command[]);  // --> Visualizza le credenziali di accesso del Sito richiesto

void manageSites(char SITES[][LS]); // --> Permette di effettuare diverse azioni relative ai Siti
bool renameSelectedSite(char SITES[][LS], char TempFileName[]);   // --> Permette di rinominare il file del sito
bool removeEverySite(char SITES[][LS]);   // --> Cancella tutti i siti registrati; restituisce true se sono stati cancellati i siti, false altrimenti


void createPin(char PIN[][LS]);   // --> Crea i file contenenti il PIN del sito con il nome del file creato
void fillPin(char Command[]);   // --> Riempie il file del PIN appena creato con il PIN inserito dall'utente

void searchPin(char PIN[][LS], int Option);   // --> Cerca il file del PIN con il nome inserito dall'utente dal file Pins.txt
bool advancedSearchPin(char PIN[][LS], char PIN_NAME[], char Command[]);   // --> Effettua una ricerca della parola cercata all'interno dei nomi dei PIN
void readPin(char Command[]);  // --> Visualizza il PIN di accesso del file richiesto

void managePin(char PIN[][LS]);  // --> Permette di effettuare diverse azioni relative ai PIN
bool renameSelectedPin(char PIN[][LS], char TempFileName[]);   // --> Permette di rinominare il file del PIN
bool removeEveryPin(char PIN[][LS]);  // --> Cancella tutti i PIN registrati; restituisce true se sono stati cancellati i PIN, false altrimenti

bool removeSelectedElement(char TempFileName[], bool ElementOption); // --> Cancella un sito o un PIN. Se il bool e' 0 allora e' un sito altrimenti e' un PIN


void getDataFileName(); // --> Ottiene il nome del file da cui estrapolare i dati controllandone l'esistenza


/// --------------------- M A I N ---------------------

int main()
{
   srand(time(0));

   FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "r");   //Apro il file Users.txt presente nella cartella Users in modalita lettura

   if(DefaultUserFile == NULL)   //Impossibile stabilire un collegamento con il file
   {
      system("color 40");
      printf("\n\n\n\n \t\t F A T A L   E R R O R   !");
      printf("\n\n\n\n \t\t Press any key to continue...");
      system("PAUSE > Null");

      RepairFunction();
   }

   else
   {
      fclose(DefaultUserFile);

      //system("date /t > Data.txt"); //Inserisce la data odierna nel file Data.txt nel formato: gg/mm/aaaa (lo '/t' va messo perche' di default quando scrivi "date" la puoi modificare mentre cosi' la copia senza modifiche al file)
      //getDate();  //Attraverso questa funzione copio la data odierna dal file e la metto nelle apposite variabili globali per giorno, mese e anno

      //DEBUGGER: printf("\n Inizio check registration function \n");
      checkRegistrationAndUpload();
      //DEBUGGER: printf("\n Fine check registration function \n");

      if(AlreadyRegistered)
         KeepLoggedFunction(1);


      if(!AlreadyRegistered || !LoggedIn)
         loginPage();

      if(LoggedIn)
      {
         SitesAmount = getAmountOfSites();
         //DEBUGGER: printf("\n\n There are: %i sites \n\n", SitesAmount);
         //DEBUGGER: system("PAUSE > Null");

         char SITES[SitesAmount][LS];
         loadSites(SITES);

         PinsAmount = getAmountOfPins();
         //DEBUGGER: printf("\n\n There are: %i PIN sites \n\n", PinsAmount);
         //DEBUGGER: system("PAUSE > Null");

         char PIN[PinsAmount][LS];
         loadPins(PIN);


         Menu(SITES, PIN);
      }
   }
}


/// ---------------- F U N C T I O N S ----------------

void RepairFunction()
{
   system("cls");
   system("color 80");

   char TempChar;

   printf("\n\n\n \t\t With this tool you can try to fix the problems by reinstalling the program");
   printf("\n \t\t\t\t BUT THIS WILL DELETE EVERY PROGRAM DATA");
   printf("\n \t\t\t\t\t (Including User's data) \n\n");

   do{
      printf("\n \t\t\t Enter 'Y' if you want to continue, 'N' otherwise: ");
      fflush(stdin);

      scanf("%c", &TempChar);
      TempChar = toupper(TempChar);
   }while(!(TempChar == 'Y' || TempChar == 'N'));

   if(TempChar == 'Y')
   {
      printf("\n \t\t\t\t THIS OPERATION CANNOT BE RESTORED !");
      printf("\n \t\t\t Try to contact out support first (pswlck.info@gmail.com) \n\n");

      do{
         printf("\n \t\t\t Enter 'Y' if you want to continue, 'N' otherwise: ");
         fflush(stdin);

         scanf("%c", &TempChar);
         TempChar = toupper(TempChar);
      }while(!(TempChar == 'Y' || TempChar == 'N'));

      if(TempChar == 'Y')
      {
         system("Repair.bat > Null");
         system("cls");

         system("color a0");
         printf("\n\n\n\n \t\t YOUR PROGRAM SHOULD NOW WORK !");
         printf("\n\n \t\t MANUALLY REBOOT THE PROGRAM TO APPLY THE CHANGES \n\n\n\n\n\n");

         for(;;)  //Per ogni carattere che viene inserito non fa nulla, in questo modo costringe l'utente a chiudere il programma
            system("PAUSE > Null");
      }
      else{
         system("cls");
         system("color f0");

         printf("\n\n\n\n \t\t Try to solve the problem and then manually reboot the program \n");
         printf(" \t\t You can also try to contact our support via email: pswlck.info@gmail.com \n\n\n\n");
         system("PAUSE > Null");
      }
   }

   else{
      system("cls");
      system("color f0");

      printf("\n\n\n\n \t\t Try to solve the problem and then manually reboot the program \n");
      printf(" \t\t You can also try to contact our support via email: pswlck.info@gmail.com \n\n\n\n");
      system("PAUSE > Null");
   }
}


void checkRegistrationAndUpload()
{
   short int N = 0;
   short int CommaPos = 0;
   char ROW[ROWL] = "";
   char TempUsername[ROWL] = "", TempPassword[ROWL] = "";
   bool Comma = false;
   FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "r");   //Non ci sara' bisogno di controllare la correttezza del collegamento con il file in quanto e' gia' stato verificato nel main qualche istante prima

   fgets(ROW, ROWL, DefaultUserFile);
   fclose(DefaultUserFile);
   //DEBUGGER: printf("\n\n ROW: _%s_ \n\n", ROW);

   for(short int i = 0; i < strlen(ROW); i++)
      if(ROW[i] == ',')
      {
         Comma = true;
         break;
      }

   if(!Comma || ROW[0] == '\0'){
      //DEBUGGER: printf("\n\n No username \n\n");

      AlreadyRegistered = false;

      //Sovrascrivo il file in modo tale da svuotarlo
      DefaultUserFile = fopen("Users/DefaultUser.txt", "w");

      system("DeleteDefaultUserSites.bat > Null");
      system("DeleteDefaultUserPins.bat > Null");
      system("cls");
   }
   else{
      while(ROW[N] != ','){
         strncat(TempUsername, &ROW[N], 1);  //Devo perforza concatenare la stringa con questo metodo senno' da' un warning siccome legge ROW[N] come intero
         N++;
      } TempUsername[N] = '\0';  //Imposto manualmente il finestringa
      //DEBUGGER: printf("\n Username: _%s_ \n", TempUsername);

      CommaPos = ++N;
      while(ROW[N] != '\n' && ROW[N] != '\0'){
         strncat(TempPassword, &ROW[N], 1);  //Devo perforza concatenare la stringa con questo metodo senno' da' un warning siccome legge ROW[N] come intero
         N++;
      } TempPassword[N - CommaPos] = '\0';   //Imposto manualmente il finestringa

      if(N - CommaPos == 0){
         //DEBUGGER: printf("\n\n No password \n\n");

         AlreadyRegistered = false;

         //Sovrascrivo il file in modo tale da svuotarlo
         DefaultUserFile = fopen("Users/DefaultUser.txt", "w");

         system("DeleteDefaultUserSites.bat > Null");
         system("DeleteDefaultUserPins.bat > Null");
         system("cls");
      }
      else{
         //DEBUGGER: printf("\n Password: _%s_ \n", TempPassword);

         strcpy(user.ACCESS_USERNAME, TempUsername);  //Copio l'username all'interno della struct
         strcpy(user.ACCESS_PASSWORD, TempPassword);  //Copio la password all'interno della struct

         AlreadyRegistered = true;


         //DEBUGGER: printf("\n\n USERNAME (struct): _%s_", user.ACCESS_USERNAME);
         //DEBUGGER: printf("\n\n PASSWORD (struct): _%s_ \n\n", user.ACCESS_PASSWORD);
         //DEBUGGER: printf("\n Default user loaded succesfully! \n");
      }
   }

   //DEBUGGER: system("PAUSE");
}

void KeepLoggedFunction(bool CheckFromFile)
{
   system("color 1f");
   fflush(stdin);

   char TempChar;

   if(!CheckFromFile)  //Se si desidera sovrascrivere il file
   {
      system("cls");
      FILE *StayLogged = fopen("Users/DefaultUser/Keep.txt", "w");

      do{
         printf("\n Do you want to stay logged in in the future? ");
         printf("\n (Please note that the auto-login will be deactivated at the next manual logout) ");
         printf("\n Type 'Y' if you want to activate this feature, 'N' otherwise: ");
         fflush(stdin);
         scanf("%c", &TempChar);
         TempChar = toupper(TempChar);
      }while(!(TempChar == 'Y' || TempChar == 'N'));

      if(TempChar == 'Y')  //Se la risposta e' si' allora scrivo la 'T' di True sul file
      {
         //DEBUGGER: printf("\n Yes --> Put('T') \n");
         fputc('T', StayLogged);
      }

      else  //Se la risposta e' no allora scrivo la 'F' di False sul file
      {
         //DEBUGGER: printf("\n No --> Put('F') \n");
         fputc('F', StayLogged);
      }

      fclose(StayLogged);
   }

   else  //Se si desidera leggere dal file
   {
      FILE *StayLogged = fopen("Users/DefaultUser/Keep.txt", "r");

      if(StayLogged == NULL)   //Impossibile stabilire un collegamento con il file
      {
         //DEBUGGER: printf("\n\n Creazione di un nuovo file \n\n");

         StayLogged = fopen("Users/DefaultUser/Keep.txt", "w");  //Creo il file vuoto
         fputc('F', StayLogged); //Ci metto di default il fatto che viene disconnesso ogni volta
         fclose(StayLogged);
      }

      else
      {
         TempChar = fgetc(StayLogged);
         fclose(StayLogged);

         if(TempChar == 'T')  //Se sul file c'e' scritto 'T' allora l'utente viene loggato automaticamente
            LoggedIn = true;

         else if(TempChar != 'F')
               {
                  StayLogged = fopen("Users/DefaultUser/Keep.txt", "w");  //Creo il file vuoto
                  fputc('F', StayLogged); //Ci metto di default il fatto che viene disconnesso ogni volta
                  fclose(StayLogged);
               }
      }
   }
}


int getAmountOfSites()
{
   int N = 0;
   char TempString[LS];

   system("UpdateDefaultUserSites.bat > Null");  //Utilizzo il file bat che direziona l'output del dir nel file Sites.txt

   FILE *Sites = fopen("Users/DefaultUser/Sites.txt", "r"); //Apro il file in modalita' lettura

   while(fgets(TempString, LS, Sites) != NULL)  //Conto il numero di siti presenti
   {
      //printf("\n %i^ sito: %s \n", N, TempString);
      N++;
   }

   fclose(Sites);

   return N;   //Ritorno il valore contato
}
void loadSites(char SITES[][LS])
{
   SitesAmount = getAmountOfSites();

   FILE *Sites = fopen("Users/DefaultUser/Sites.txt", "r"); //Apro il file in modalita' lettura, non c'e' bisogno di effettuare controlli in quanto la funzione getAmountOfSites() chiama il file bat che crea il file in questione

   for(short int N = 0; N < SitesAmount; N++)   //Carico i siti nell'array
   {
      fgets(SITES[N], LS, Sites);

      for(short int i = 0; i < LS; i++)   //Faccio finire manualmente la stringa dopo il ".txt" altrimenti c'e' il fineriga che fa andare a capo
         if(SITES[N][i] == '.' && SITES[N][i+1] == 't' && SITES[N][i+2] == 'x' && SITES[N][i+3] == 't')
         {
            SITES[N][i+4] = '\0';
            break;
         }
   }

   fclose(Sites);

   ///DEBUGGER:
   /*
   printf("\n \t\t\t LOAD SITE FUNCTION \n\n");

   printf(" Array: \n");
   for(short int i = 0; i < SitesAmount; i++)
      printf("\n %i^ sito: %s", i, SITES[i]);

   system("PAUSE > Null");
   */
}
void getSiteOrPinNameFromCommand(char Command[], char SiteOrPinName[])
{
   short int CounterSlash = 0, Start = 0;

   //DEBUGGER: printf("\n Command: _%s_", Command);
   strcpy(SiteOrPinName, "");

   while(CounterSlash <= 2)   //Posiziono il cursore al secondo slash
      if(Command[Start++] == '/')
         CounterSlash++;

   //DEBUGGER: printf("\n Start: %i", Start);

   for(short int i = Start; i < strlen(Command) - 3; i++)   //Inizio a copiare la stringa dal secondo slash e mi fermo prima dell'estensione del file
   {
      strncat(SiteOrPinName, &Command[i-1], 1);   //Devo perforza concatenare la stringa con questo metodo senno' da' un warning siccome legge TempFileName[x] come intero
      SiteOrPinName[i-Start] = '\0';   //Metto manualmente il finestringa
   }
}

int getAmountOfPins()
{
   int N = 0;
   char TempString[LS];

   system("UpdateDefaultUserPins.bat > Null");  //Utilizzo il file bat che direziona l'output del dir nel file Pins.txt

   FILE *Pin = fopen("Users/DefaultUser/Pins.txt", "r"); //Apro il file in modalita' lettura

   while(fgets(TempString, LS, Pin) != NULL)  //Conto il numero dei PIN presenti
   {
      //printf("\n %i^ sito: %s \n", N, TempString);
      N++;
   }

   fclose(Pin);

   return N;   //Ritorno il valore contato
}
void loadPins(char PIN[][LS])
{
   PinsAmount = getAmountOfPins();

   FILE *Pin = fopen("Users/DefaultUser/Pins.txt", "r"); //Apro il file in modalita' lettura, non c'e' bisogno di effettuare controlli in quanto la funzione getAmountOfPins() chiama il file bat che crea il file in questione

   for(short int N = 0; N < PinsAmount; N++)   //Carico i siti nell'array
   {
      fgets(PIN[N], LS, Pin);

      for(short int i = 0; i < LS; i++)   //Faccio finire manualmente la stringa dopo il ".txt" altrimenti c'e' il fineriga che fa andare a capo
         if(PIN[N][i] == '.' && PIN[N][i+1] == 't' && PIN[N][i+2] == 'x' && PIN[N][i+3] == 't')
         {
            PIN[N][i+4] = '\0';
            break;
         }
   }

   fclose(Pin);

   ///DEBUGGER:
   /*
   printf("\n \t\t\t LOAD SITE FUNCTION \n\n");

   printf(" Array: \n");
   for(short int i = 0; i < SitesAmount; i++)
      printf("\n %i^ sito: %s", i, SITES[i]);

   system("PAUSE > Null");
   */
}


/// ---------- M E N U   L O G I N ----------

void loginPage()
{
   int Choose = 0;
   float Real;
   char TempChar;


   system("cls");
   system("color 1f");

   checkRegistrationAndUpload(); //Richiamo per essere sempre aggiornato la funzione per verificare la registrazione di un utente

   ///Se non c'e' alcun account registrato allora questo menu e' quello di registrazione dell'account
   ///Se c'e' un account registrato allora questo menu e' quello di login dell'account

   printf("\n\n\n");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +");
   printf("\n \t\t |                                                    |");

   if(!AlreadyRegistered)
      printf("\n \t\t |        THIS IS THE ACCOUNT REGISTRATION PAGE       |");
   else
      printf("\n \t\t |          THIS IS THE ACCOUNT LOGIN PAGE            |");

   printf("\n \t\t |                                                    |");
   printf("\n \t\t |        What action do you want to perform ?        |");
   printf("\n \t\t |         (digit the number of your choose)          |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");

   if(!AlreadyRegistered)
      printf("\n \t\t |         1 --> REGISTER ACCOUNT                     |");
   else
      printf("\n \t\t |         1 --> LOGIN ACCOUNT                        |");

   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         9 --> ADAPT SCREEN                         |");  //Consiste semplicemente nel ricaricare la funzione in modo tale che il programma si adatti allo schermo intero o a quello in finestra
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         10 --> EXIT THE PROGRAM                    |");
   printf("\n \t\t |                                                    |");

   if(AlreadyRegistered)
      printf("\n \t\t |         100 --> REPAIR THE PROGRAM                 |");  //Sara' necessario effettuare il login per accedere a questo tool

   printf("\n \t\t |                                                    |");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- + \n\n");

   do
   {
      printf("\t\t Type here your decision: ");
      fflush(stdin);
      scanf("%f", &Real);
      Choose = Real;
      printf("\n");
   }while(!(Choose == Real && (Choose == 1 || Choose == 9 || Choose == 10 || Choose == 100)));


   switch(Choose)
   {
      case 1:
      {
         checkRegistrationAndUpload();

         if(!AlreadyRegistered)
            registerFunction();

         else
            loginFunction(1);
      }break;

      case 10:
      {
         printf("\n\n You are trying to exit the program \n");

         do{
            printf("\n Are you sure you want to continue (Y --> Yes, N --> No)? ");
            fflush(stdin);
            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'Y')  //Se non si vuole cancellare l'account
         {
            system("cls");
            system("color a0");

            printf("\n\n\n\n \t\t EXIT COMPLETED SUCCESSFULLY ! \n\n\n");
         }
      }break;

      case 100:
      {
         system("cls");

         if(AlreadyRegistered)
         {
            loginFunction(0);
            RepairFunction();
         }
      }break;
   }

   if(!LoggedIn && Choose != 10) //Utilizzo questo metodo perche' sono in grado di uscire dal programma in qualsiasi momento (ho effettuato svariati test ed e' quello che funziona meglio)
      loginPage();
}
void registerFunction()
{
   system("cls");
   system("color 1f");

   char TempChar;
   char TempArrUsername[LS], TempArrPassword[LS];
   bool RightArr = true;


   ///INSERIMENTO USERNAME
   do{
      RightArr = true;

      printf("\n\n Insert the username (NO SPACES, the character ',' isn't allowed): ");
      fflush(stdin);
      scanf("%s", TempArrUsername);


      if(strlen(TempArrUsername) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE USERNAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
      }

      else
      {
         for(short int i = 0; i < strlen(TempArrUsername); i++)
            if(TempArrUsername[i] == ',')
            {
               RightArr = false;
               break;
            }

         if(RightArr)
         {
            printf("\n This will be the username: %s \n", TempArrUsername);

            do{
               printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
               fflush(stdin);

               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')
               RightArr = false;
         }
      }
   }while(!RightArr);

   //Trasformo tutto in maiuscolo
   for(short int i = 0; i < strlen(TempArrUsername); i++)
      TempArrUsername[i] = toupper(TempArrUsername[i]);
   strcpy(user.ACCESS_USERNAME, TempArrUsername);

   //DEBUGGER: printf("\n USERNAME: %s", TempArrUsername);

   system("cls");

   ///INSERIMENTO PASSWORD
   do{
      RightArr = true;

      printf("\n\n Insert the password (NO SPACES, the character ',' isn't allowed): ");
      fflush(stdin);
      scanf("%s", TempArrPassword);

      if(strlen(TempArrPassword) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE PASSWORD IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
      }

      else
      {
         for(short int i = 0; i < strlen(TempArrPassword); i++)
            if(TempArrPassword[i] == ',')
            {
               RightArr = false;
               break;
            }

         if(RightArr)
         {
            printf("\n This will be the password: %s \n", TempArrPassword);

            do{
               printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
               fflush(stdin);

               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')
               RightArr = false;
         }
      }
   }while(!RightArr);
   strcpy(user.ACCESS_PASSWORD, TempArrPassword);

   //DEBUGGER: printf("\n USERNAME: _%s_ \t PASSWORD: _%s_", TempArrUsername, TempArrPassword);
   //DEBUGGER: system("PAUSE > Null");

   ///COPIO IL RISULTATO SUL FILE
   FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "w");   //Apro il file DefaultUser.txt presente nella cartella Users in modalita scrittura
   fputs(TempArrUsername, DefaultUserFile);
   fputc(',', DefaultUserFile);
   fputs(TempArrPassword, DefaultUserFile);
   fclose(DefaultUserFile);

   AlreadyRegistered = true;  //Imposto su true il valore dell'inserimento dei dati
   LoggedIn = true;  //Imposto su true il valore del login


   ///CHIEDO SE SI VUOLE RIMANERE LOGGATI
   KeepLoggedFunction(0);
}
void loginFunction(bool AskKeepLogged)
{
   system("cls");
   system("color 1f");

   char TempArr[LS];
   bool RightArr = true, Right = true;


   ///INSERIMENTO USERNAME
   do{
      if(!Right)
      {
         system("cls");

         printf("\n Wrong Username! \n");
      }

      Right = true;

      //Controllo che non ci siano ','
      do{
         RightArr = true;

         printf("\n\n Insert the username: ");
         fflush(stdin);
         scanf("%s", &TempArr);

         for(short int i = 0; i < strlen(TempArr); i++)
            if(TempArr[i] == ',')
            {
               RightArr = false;
               break;
            }
      }while(!RightArr);

      //Trasformo tutto in maiuscolo
      for(short int i = 0; i < strlen(TempArr); i++)
         TempArr[i] = toupper(TempArr[i]);


      ///CONTROLLO DELLA CORRETTEZZA
      if(strlen(user.ACCESS_USERNAME) != strlen(TempArr)) //Se le lunghezze sono diverse allora anche gli username sono diversi
         Right = false;

      else if(strcmp(TempArr, user.ACCESS_USERNAME) != 0)  //Le due lunghezze coincidono --> controllo l'uguaglianza delle due stringhe
         Right = false;

   }while(!Right);

   //DEBUGGER: printf("\n Right Username \n\n");


   ///INSERIMENTO PASSWORD
   do{
      if(!Right)
         printf("\n Wrong Password! \n");

      Right = true;

      //Controllo che non ci siano ','
      do{
         RightArr = true;

         printf("\n\n Insert the password: ");
         fflush(stdin);
         scanf("%s", &TempArr);

         for(short int i = 0; i < strlen(TempArr); i++)
            if(TempArr[i] == ',')
            {
               RightArr = false;
               break;
            }
      }while(!RightArr);


      ///CONTROLLO DELLA CORRETTEZZA
      if(strlen(user.ACCESS_PASSWORD) != strlen(TempArr)) //Se le lunghezze sono diverse allora anche le password sono diverse
         Right = false;

      else if(strcmp(TempArr, user.ACCESS_PASSWORD) != 0) //Le due lunghezze coincidono --> controllo l'uguaglianza delle due stringhe
         Right = false;

   }while(!Right);
   LoggedIn = true;


   if(AskKeepLogged)
      KeepLoggedFunction(0);
}


/// ----------- M E N U   U S E R -----------

void Menu(char SITES[][LS], char PIN[][LS])
{
   system("cls");
   system("color 1f");

   int Choose, Integer;
   float Real;
   char TempChar;

   printf("\n\n\n");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |        What action do you want to perform ?        |");
   printf("\n \t\t |         (digit the number of your choose)          |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         1 --> ADD NEW SITE                         |");
   if(SitesAmount)   //Se ci sono dei siti salvati allora mostro le voci nel menu che lavorano con i siti esistenti
   {
      printf("\n \t\t |         2 --> SEARCH SITE                          |");
      printf("\n \t\t |         3 --> MANAGE SITES                         |");
   }

   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         4 --> ADD NEW PIN                          |");
   if(PinsAmount)   //Se ci sono dei PIN salvati allora mostro le voci nel menu che lavorano con i PIN esistenti
   {
      printf("\n \t\t |         5 --> SEARCH PIN                           |");
      printf("\n \t\t |         6 --> MANAGE PIN                           |");
   }

   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         7 --> MANAGE THE ACCOUNT                   |");
   printf("\n \t\t |         8 --> IMPORT DATA FROM BROWSER             |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         10 --> EXIT THE PROGRAM                    |");
   printf("\n \t\t |                                                    |");
   //printf("\n \t\t |         100 --> REPAIR THE PROGRAM                 |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- + \n\n");

   do
   {
      printf("\t\t Type here your decision: ");
      fflush(stdin);
      scanf("%f", &Real);
      Choose = Real;
      printf("\n");
   }while(!(Choose == Real && ((Choose >= 1 && Choose <= 8) || Choose == 10 || Choose == 100)));


   switch(Choose)
   {
      case 1:
      {
         createSite(SITES);
      }break;

      case 2:
      {
         if(SitesAmount)
            searchSite(SITES, 1);
      }break;

      case 3:
      {
         if(SitesAmount)
            manageSites(SITES);
      }break;

      case 4:
      {
         createPin(PIN);
      }break;

      case 5:
      {
         if(PinsAmount)
            searchPin(PIN, 1);
      }break;

      case 6:
      {
         if(PinsAmount)
            managePin(PIN);
      }break;

      case 7:
      {
         ManageAccount();
      }break;

      case 8:
      {
         getDataFileName();
      }break;

      case 10:
      {
         printf("\n\n You are trying to exit the program \n");

         do{
            printf("\n Do you want to continue (Y --> Yes, N --> No)? ");
            fflush(stdin);
            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'Y')
         {
            system("cls");
            system("color a0");

            printf("\n\n\n\n \t\t EXIT COMPLETED SUCCESSFULLY ! \n\n\n");
         }
      }break;

      case 100:
      {
         system("cls");
         RepairFunction();
      }break;
   }

   if(Choose != 10 && Choose != 100)  //Utilizzo questo metodo perche' sono in grado di uscire dal programma in qualsiasi momento (ho effettuato svariati test ed e' quello che funziona meglio)
      Menu(SITES, PIN);
}

void ManageAccount()
{
   system("cls");
   system("color 1f");

   int Choose;
   float Real;
   char TempChar;


   printf("\n\n\n");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |        What action do you want to perform ?        |");
   printf("\n \t\t |         (digit the number of your choose)          |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         1 --> VIEW LOGIN CREDENTIALS               |");
   printf("\n \t\t |         2 --> CHANGE PASSWORD                      |");
   //printf("\n \t\t |         3 --> ADD LOGIN HINT                       |");   --> Da implementare
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         7 --> LOG OUT                              |");
   printf("\n \t\t |         8 --> REMOVE ACCOUNT                       |");
   printf("\n \t\t |         9 --> VIEW ACCOUNT INFO                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         10 --> GO BACK                             |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- + \n\n");

   do
   {
      printf("\t\t Type here your decision: ");
      fflush(stdin);
      scanf("%f", &Real);
      Choose = Real;
      printf("\n");
   }while(!(Choose == Real && (Choose == 1 || Choose == 2 || (Choose >= 7 && Choose <= 10))));


   switch(Choose)
   {
      case 1:
      {
         viewAccountData();
      }break;

      case 2:
      {
         changePassword();
      }break;

      case 7:
      {
         printf("\n\n You are trying to log out from the account: %s", user.ACCESS_USERNAME);
         printf("\n This action will deactivate the auto-login feature but you can reactivate it at the next login \n");

         do{
            printf("\n Are you sure you want to continue (Y --> Yes, N --> No)? ");
            fflush(stdin);
            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'Y')  //Se si vuole fare il logout
         {
            system("cls");
            system("color a0");

            LoggedIn = false;

            FILE *StayLogged = fopen("Users/DefaultUser/Keep.txt", "w");
            fputc('F', StayLogged);
            fclose(StayLogged);

            printf("\n\n\n\n \t\t LOGGED OUT SUCCESSFULLY ! \n\n");
            printf("\n\n\n\n \t\t Press any key to continue...");

            system("PAUSE > Null");


            loginPage();
         }
      }break;

      case 8:
      {
         system("cls");

         printf("\n\n You are trying to delete the existent account named: %s \n", user.ACCESS_USERNAME);
         printf(" This will also DELETE ALL the registered DATA ! \n");

         do{
            printf("\n Are you sure you want to continue (Y --> Yes, N --> No)? ");
            fflush(stdin);
            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'N')  //Se non si vuole cancellare l'account
         {
            system("cls");
            system("color f0");

            printf("\n\n\n\n \t\t THE ACCOUNT WON'T BE DELETED ! \n\n");
            printf("\n\n\n\n \t\t Press any key to continue...");

            system("PAUSE > Null");
         }
         else
         {
            printf("\n\n\n This is the LAST WARNING before COMPLETELY DELETING the account: %s \n", user.ACCESS_USERNAME);
            printf("\t NO DATA CAN BE RECOVERED AFTER THE DELETION ! \n");

            do{
               printf("\n Are you SURE you want to continue (Y --> Yes, N --> No)? ");
               fflush(stdin);
               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')  //Se non si vuole cancellare l'account
            {
               system("cls");
               system("color f0");

               printf("\n\n\n\n \t\t THE ACCOUNT WON'T BE DELETED ! \n\n");
               printf("\n\n\n\n \t\t Press any key to continue...");

               system("PAUSE > Null");
            }
            else{
               removeAccount();
            }
         }
      }break;

      case 9:
      {
         //In fase di sviluppo
      }

      case 10:
      {
         system("exit");
      }
   }

   if(Choose != 7 && Choose != 8 && Choose != 10)  //Utilizzo questo metodo perche' sono in grado di uscire dal programma in qualsiasi momento (ho effettuato svariati test ed e' quello che funziona meglio)
      ManageAccount();
}
void viewAccountData()
{
   system("cls");
   system("color 1f");


   printf("\n\n \t THESE ARE YOU LOGIN CREDENTIALS: \n");
   printf("\n\n \t USER ACCESS USERNAME: %s \n", user.ACCESS_USERNAME);
   printf("\n \t USER ACCESS PASSWORD: %s \n\n\n", user.ACCESS_PASSWORD);


   //printf("\n\n Press any key to continue...");

   system("PAUSE > Null");
}
void removeAccount()
{
   system("cls");

   //Sovrascrivo il file in modo tale da svuotarlo
   FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "w");
   fclose(DefaultUserFile);

   //Sovrascrivo il file in modo tale da svuotarlo ed in seguito metto l'accesso automatico disattivato
   FILE *StayLogged = fopen("Users/DefaultUser/Keep.txt", "w");
   fputc('F', StayLogged);
   fclose(StayLogged);

   system("DeleteDefaultUserSites.bat > Null");  //Avvio il programma per cancellare le cartelle dei siti
   system("DeleteDefaultUserPins.bat > Null");  //Avvio il programma per cancellare le cartelle dei PIN

   ///IMPOSTO TUTTE LE VARIABILI BOOLEANE DEL CASO SU FALSE
   AlreadyRegistered = false;
   LoggedIn = false;

   system("cls");
   system("color a0");
   printf("\n\n\n\n \t\t ACCOUNT DELETED SUCCESSFULLY !");
   printf("\n\n\n\n \t\t Press any key to continue...");

   system("PAUSE > Null");

   loginPage();
}
void changePassword()
{
   system("cls");
   system("color 1f");

   char TempChar;
   char ROW[ROWL], TempArrPassword[LS];
   bool Right = true;

   ///CONTROLLO DELLA CORRETTEZZA
   do{
      Right = true;

      printf("\n\n Insert the OLD password or enter 'X' to exit: ");
      fflush(stdin);
      scanf("%s", &TempArrPassword);

      if(!(strlen(TempArrPassword) == 1 && (TempArrPassword[0] == 'X' || TempArrPassword[0] == 'x')))  //Se non si vuole uscire
      {
         if(strlen(user.ACCESS_PASSWORD) != strlen(TempArrPassword)) //Se le lunghezze sono diverse allora anche le password sono diverse
            Right = false;

         else  //Le due lunghezze coincidono
            for(short int i = 0; i < strlen(TempArrPassword); i++)   //Controllo che le password corrispondano
               if(TempArrPassword[i] != user.ACCESS_PASSWORD[i])
               {
                  Right = false;
                  break;
               }

         if(!Right)
         {
            system("cls");
            printf("\n WRONG PASSWORD !");
         }
      }
   }while(!Right);

   if(!(strlen(TempArrPassword) == 1 && (TempArrPassword[0] == 'X' || TempArrPassword[0] == 'x')))  //Se non si vuole uscire
   {
      do{
         Right = true;

         printf("\n\n Insert the new password (NO SPACES, the character ',' isn't allowed) or enter 'X' to exit: ");
         scanf("%s", TempArrPassword);

         if(strlen(TempArrPassword) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
         {
            printf("\n THE NEW PASSWORD IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
            Right = false;
         }

         else if(!(strlen(TempArrPassword) == 1 && (TempArrPassword[0] == 'X' || TempArrPassword[0] == 'x')))  //Se non si vuole uscire
               {
                  ///CONTROLLO CHE SIA DIVERSA DA QUELLA PRECEDENTE
                  if(strlen(user.ACCESS_PASSWORD) == strlen(TempArrPassword)) //Se le lunghezze sono uguali
                     if(strcmp(TempArrPassword, user.ACCESS_PASSWORD) == 0)
                     {
                        Right = false;

                        system("cls");
                        printf("\n\n THE NEW PASSWORD IS THE SAME AS THE OLD ONE !");
                     }

                  if(Right)
                  {
                     for(short int i = 0; i < strlen(TempArrPassword); i++)
                        if(TempArrPassword[i] == ',')
                        {
                           Right = false;
                           break;
                        }

                     if(Right)
                     {
                        printf("\n This will be the new password: %s \n", TempArrPassword);

                        do{
                           printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
                           fflush(stdin);

                           scanf("%c", &TempChar);
                           TempChar = toupper(TempChar);
                        }while(!(TempChar == 'Y' || TempChar == 'N'));

                        if(TempChar == 'N')
                        {
                           Right = false;
                           system("cls");
                        }
                     }
                  }
               }
      }while(!Right);
   }


   if(!(strlen(TempArrPassword) == 1 && (TempArrPassword[0] == 'X' || TempArrPassword[0] == 'x'))) //Se non si vuole uscire
   {
      //Aggiorno la struct con la nuova password
      strcpy(user.ACCESS_PASSWORD, "");
      strcpy(user.ACCESS_PASSWORD, TempArrPassword);

      //DEBUGGER: printf(" NEW PASSWORD: %s", TempArrPassword);

      FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "r");
      fgets(ROW, ROWL, DefaultUserFile);
      fclose(DefaultUserFile);

      short int CommaPos = 0;
      while(ROW[CommaPos] != ',')
      {
         //DEBUGGER: printf("\n ROW[%i]: %c \n", CommaPos, ROW[CommaPos]);
         CommaPos++;
      }
      CommaPos++;

      for(short int i = 0; i < strlen(TempArrPassword); i++)   //Sovrascrivo la riga con la password nuova
      {
         ROW[CommaPos + i] = TempArrPassword[i];
      }
      ROW[CommaPos + strlen(TempArrPassword)] = '\0';

      DefaultUserFile = fopen("Users/DefaultUser.txt", "w");
      fputs(ROW, DefaultUserFile);  //Butto il risultato dentro al file
      fclose(DefaultUserFile);
   }

   //DEBUGGER: system("PAUSE > Null");
   //DEBUGGER: system("cls");
}



void createSite(char SITES[][LS])
{
   /// CONTROLLARE LA SINGOLARITA'
   system("cls");
   system("color 1f");

   char TempChar;
   char TempSiteName[LS];
   char TempFileName[LF];
   char Command[LF + CMD] = "Users/DefaultUser/Sites/";  //Inizializzo il valore del comando al percorso del file
   bool RightArr = false;

   //DEBUGGER: printf("\n \t\t\t CREATE SITE FUNCTION \n\n");

   do{
      system("cls");
      RightArr = true;

      //printf("\n\n Insert the name of the site (Max %i characters, NO SPACES) or enter 'X' to exit: ", LF-1);
      printf("\n\n Insert the name of the site (NO SPACES) or enter 'X' to exit: ");
      fflush(stdin);
      scanf("%s", TempSiteName);

      if(strlen(TempSiteName) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE SITE NAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
      }

      else if(!(strlen(TempSiteName) == 1 && (TempSiteName[0] == 'X' || TempSiteName[0] == 'x')))  //Se l'utente non ha digitato 'X' o 'x' allora continuo
            {
               //Trasformo tutto in maiuscolo
               for(short int i = 0; i < strlen(TempSiteName); i++)
                  TempSiteName[i] = toupper(TempSiteName[i]);

               //Copio la stringa nell'array piu' grande e ci metto l'estensione
               strcpy(TempFileName, TempSiteName);
               strcat(TempFileName, ".txt");

               //DEBUGGER: printf("\n File name: %s \n", TempFileName);

               for(short int N = 0; N < SitesAmount; N++)
               {
                  //DEBUGGER: printf("\n File name in array: %s \n", SITES[N]);

                  if(strcmp(SITES[N], TempFileName) == 0)   //Se c'e' gia' un sito registrato con lo stesso nome
                  {
                     printf("\n THE INSERTED SITE ALREADY EXISTS !");
                     RightArr = false;

                     printf("\n\n DO YOU WANT TO OVERWRITE IT?");
                     do{
                        printf("\n Enter 'Y' to overwrite the existent site, 'N' otherwise: ");
                        fflush(stdin);

                        scanf("%c", &TempChar);
                        TempChar = toupper(TempChar);
                     }while(!(TempChar == 'Y' || TempChar == 'N'));

                     if(TempChar == 'Y')
                        RightArr = true;  //Continuo con l'esecuzione del codice in modo tale da sovrascrivere il file
                  }
               }
            }
   }while(!RightArr);

   if(!(strlen(TempSiteName) == 1 && (TempSiteName[0] == 'X' || TempSiteName[0] == 'x'))) //Se l'utente non ha digitato 'X' o 'x' allora continuo
   {
      //DEBUGGER: printf("\n Command value: %s \n", Command);

      //Assegno il valore a Command
      strcat(Command, TempFileName);   //Concateno le due stringhe

      //DEBUGGER: printf("\n Command value: %s \n", Command);

      FILE *Site = fopen(Command, "w");   //Creo il file desiderato
      fclose(Site);

      fillSite(Command);

      //Aggiorno l'array SITES
      loadSites(SITES);
   }

   //DEBUGGER: system("PAUSE > Null");
   //DEBUGGER: system("cls");
}
void fillSite(char Command[])
{
   system("cls");
   system("color 1f");

   char TempChar;
   char TempArrUsername[LS], TempArrPassword[LS];
   bool RightArr = true;


   ///INSERIMENTO USERNAME
   do{
      RightArr = true;

      printf("\n\n Insert the username (NO SPACES, the character ',' isn't allowed): ");
      fflush(stdin);
      scanf("%s", TempArrUsername);

      if(strlen(TempArrUsername) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE USERNAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
         system("cls");
      }

      else
      {
         for(short int i = 0; i < strlen(TempArrUsername); i++)
            if(TempArrUsername[i] == ',')
            {
               RightArr = false;
               break;
            }

         if(RightArr)
         {
            printf("\n This will be the username: %s \n", TempArrUsername);

            do{
               printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
               fflush(stdin);

               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')
            {
               RightArr = false;
               system("cls");
            }
         }
      }
   }while(!RightArr);

   //Trasformo tutto in maiuscolo --> Capire perche' se lo tolgo la funzione per visualizzare i dati da' memoria sporca
   for(short int i = 0; i < strlen(TempArrUsername); i++)
      TempArrUsername[i] = toupper(TempArrUsername[i]);

   //DEBUGGER: printf("\n USERNAME: _%s_", TempArrUsername);
   //DEBUGGER: system("PAUSE > Null");
   system("cls");

   ///INSERIMENTO PASSWORD
   do{
      RightArr = true;

      printf("\n\n Insert the password (NO SPACES, the character ',' isn't allowed): ");
      fflush(stdin);
      scanf("%s", TempArrPassword);

      if(strlen(TempArrPassword) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE PASSWORD IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
      }

      else
      {
         for(short int i = 0; i < strlen(TempArrPassword); i++)
            if(TempArrPassword[i] == ',')
            {
               RightArr = false;
               break;
            }

         if(RightArr)
         {
            printf("\n This will be the password: %s \n", TempArrPassword);

            do{
               printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
               fflush(stdin);

               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')
               RightArr = false;
         }
      }
   }while(!RightArr);

   //DEBUGGER: printf(" PASSWORD: _%s_", TempArrPassword);
   //DEBUGGER: system("PAUSE > Null");

   ///COPIO IL RISULTATO SUL FILE
   FILE *Site = fopen(Command, "w");   //Apro il file desiderato utilizzando la variabile Command che contiene Indirizzo, Nome Sito ed Estensione
   fputs(TempArrUsername, Site);
   fputc(',', Site);
   fputs(TempArrPassword, Site);
   fclose(Site);
}

void searchSite(char SITES[][LS], int Option)
{
   system("cls");
   system("color 1f");

   char TempChar;
   char TempSiteName[LS];
   char TempFileName[LF];
   char Command[LF + CMD] = "Users/DefaultUser/Sites/";  //Inizializzo il valore del comando al percorso del file

   bool RightArr = false;
   bool Found = false;
   bool TempBool;

   loadSites(SITES);
   //DEBUGGER: printf("\n \t\t\t SEARCH SITE FUNCTION \n\n");

   do{
      system("cls");
      system("color 1f");

      do{
         RightArr = true;

         if(Option == 1 || Option == 4)
            printf("\n\n Insert the name of the site (NO SPACES)(Enter 'X' to exit)(Enter '*' to view all): ");
         else
            printf("\n\n Insert the name of the site (NO SPACES)(Enter 'X' to exit): ");
         fflush(stdin);
         scanf(" %s", TempSiteName);

         if(strlen(TempSiteName) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
         {
            printf("\n THE SITE NAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
            RightArr = false;
         }
      }while(!RightArr);

      //DEBUGGER: printf("\n Searched site: %s", TempSiteName);

      //Trasformo tutto in maiuscolo
      for(short int i = 0; i < strlen(TempSiteName); i++)
         TempSiteName[i] = toupper(TempSiteName[i]);

      //DEBUGGER: printf("\n Searched site: %s", TempSiteName);


      if(strlen(TempSiteName) == 1 && TempSiteName[0] == 'X')  //Se e' stata inserita la 'X'
         break;

      /// ----------- SE E' STATO INSERITO IL CARATTERE '*' -----------
      else if(strlen(TempSiteName) == 1 && TempSiteName[0] == '*')
            {
               Found = true;

               switch(Option)
               {
                  case 1:
                  {
                     system("cls");

                     printf("\n\n \t HERE YOU CAN SEE ALL THE REGISTERED SITES \n\n");

                     for(short int i = 0; i < SitesAmount; i++)
                     {
                        printf("\n \t %i^ \t ", i+1);

                        for(short int c = 0; c < (strlen(SITES[i]) - 4); c++) //Serve per stampare il nome del file senza l'estensione
                           printf("%c", SITES[i][c]);
                     }

                     printf("\n\n\n \t Press any key to continue... \n\n");
                     system("PAUSE > Null");
                  }break;

                  case 2:
                  {
                     if(SitesAmount != 1) //Se ce n'e' piu' di uno
                     {
                        system("cls");
                        system("color 40");

                        printf("\n \t UNABLE TO RENAME EVERY SITE !");
                        printf("\n \t SEARCH JUST ONE ! \n");

                        printf("\n\n\n \t Press any key to continue... \n\n");
                        system("PAUSE > Null");
                        system("color 1f");


                        Found = false;
                     }
                     else{ //Se ce n'e' solo uno
                        system("cls");
                        strcat(Command, SITES[0]);   //Assegno il valore a Command

                        TempBool = renameSelectedSite(SITES, Command);
                     }
                  }break;

                  case 3:
                  {
                     if(SitesAmount != 1) //Se ce n'e' piu' di uno
                     {
                        system("cls");
                        system("color 40");

                        printf("\n \t UNABLE TO EDIT EVERY SITE !");
                        printf("\n \t SEARCH JUST ONE ! \n");

                        printf("\n\n\n \t Press any key to continue... \n\n");
                        system("PAUSE > Null");
                        system("color 1f");


                        Found = false;
                     }
                     else{ //Se ce n'e' solo uno
                        system("cls");
                        strcat(Command, SITES[0]);   //Assegno il valore a Command

                        //TempBool = editSelectedSite(SITES, Command);
                     }
                  }break;

                  case 4:
                  {
                     TempBool = removeEverySite(SITES);
                     loadSites(SITES);
                  }break;
               }
               break;
            }

            /// ----------- SE E' STATO INSERITO IL NOME DI UN SITO -----------
            else
            {
               //Copio la stringa nell'array piu' grande e ci metto l'estensione
               strcpy(TempFileName, TempSiteName);
               strcat(TempFileName, ".txt");


               for(short int i = 0; i < SitesAmount; i++)
               {
                  if(strcmp(SITES[i], TempFileName) == 0)
                  {
                     //Assegno il valore a Command
                     strcat(Command, TempFileName);

                     switch(Option)
                     {
                        case 1:
                        {
                           readSite(Command);   //Visualizzo il sito trovato
                        }break;

                        case 2:
                        {
                           TempBool = renameSelectedSite(SITES, Command);
                           loadSites(SITES);
                        }break;

                        case 3:
                        {
                           //TempBool = editSelectedSite(Command);
                        }break;

                        case 4:
                        {
                           TempBool = removeSelectedElement(Command, 0);   //Rimuovo il sito trovato
                           loadSites(SITES);
                        }break;
                     }

                     Found = true;
                     break;
                  }
               }

               /// ----------- SE BISOGNA RICORRERE ALLA RICERCA AVANZATA -----------
               if(!Found)
               {
                  if(!advancedSearchSite(SITES, TempSiteName, Command))
                  {
                     printf("\n Searched site not found !");
                     system("PAUSE > Null");
                  }
                  else
                  {
                     Found = true;

                     switch(Option)
                     {
                        case 1:  //Visualizzo i dati
                        {
                           readSite(Command);   //Visualizzo il sito trovato
                        }break;

                        case 2:
                        {
                           TempBool = renameSelectedSite(SITES, Command);
                           loadSites(SITES);
                        }break;

                        case 3:
                        {
                           //TempBool = editSelectedSite(Command);
                        }break;

                        case 4:
                        {
                           TempBool = removeSelectedElement(Command, 0);   //Rimuovo il sito trovato
                           loadSites(SITES);
                        }break;
                     }

                     break;
                  }
               }
            }
   }while(!(strlen(TempSiteName) != 1 && TempSiteName[0] != 'X'));
}
bool advancedSearchSite(char SITES[][LS], char SITE_NAME[], char Command[])
{
   int SearchedLenght = strlen(SITE_NAME);
   char TempChar;
   bool Found = false;
   bool ThereIs;

   //DEBUGGER: printf("\nLunghezza parola da cercare (%s): %i \n", SITE_NAME, SearchedLenght);

   for(short int i = 0; i < SitesAmount; i++)
   {
      //DEBUGGER: printf("\nSITO CHE CONTROLLO: %s \n", SITES[i]);

      //Escludo l'etensione del file dal calcolo della lunghezza
      if(strlen(SITES[i]) - 4 > SearchedLenght)  //Se la lunghezza del sito e' maggiore della parola cercata allora controllo la presenza di quella parola all'interno del nome del sito
      {
         //DEBUGGER: printf("\nLUNGHEZZA CONTROLLO SITO: %s --> %i \n", SITES[i], strlen(SITES[i]));

         for(short int j = 0; j <= (strlen(SITES[i]) - SearchedLenght); j++)   //Avanzo di una lettera ogni volta e controllo la presenza della parola cercata all'interno del nome del sito
         {
            //DEBUGGER: printf("\n J = %i, (strlen(SITES[i]) - SearchedLenght - 4) = %i \n", j, (strlen(SITES[i]) - SearchedLenght - 4));

            ThereIs = true;

            for(short int k = j; k < SearchedLenght + j; k++)   //Controllo la presenza della parola cercata all'interno del nome del sito
            {
               //DEBUGGER: printf("\nSITES[i][k] = %c --> SITE_NAME[k-j] = %c", SITES[i][k], SITE_NAME[k-j]);

               if(SITES[i][k] != SITE_NAME[k-j])
               {
                  ThereIs = false;  //La parola non e' contenuta
                  break;
               }
            }

            if(ThereIs)
            {
               char tempFoundSite[strlen(SITES[i]) - 4];
               strncpy(tempFoundSite, SITES[i], strlen(SITES[i]) - 4);
               tempFoundSite[strlen(SITES[i]) - 4] = '\0';

               do
               {
                  printf("\n Did You mean %s? ", tempFoundSite);
                  fflush(stdin);
                  printf("\n Enter 'Y' if Yes, 'N' otherwise: ");
                  scanf("%c", &TempChar); printf("\n");
                  TempChar = toupper(TempChar);
               }while(!(TempChar == 'Y' || TempChar == 'N'));

               if(TempChar == 'Y')  //Se e' il sito effettivamente cercato allora aggiorno la variabile command e ritorno true
               {
                  //Assegno il valore a Command
                  strcat(Command, SITES[i]);

                  return true;
               }
               else  //Altrimenti continuo la ricerca
                  break;
            }
         }
      }
   }

   return false;
}
void readSite(char Command[])
{
   short int N = 0, CommaPos = 0;
   char ROW[ROWL] = "", TempUsername[LS] = "", TempPassword[LS] = "";
   bool Comma = false;
   FILE *Site = fopen(Command, "r");

   fgets(ROW, ROWL, Site); //Carico la riga all'interno della variabile apposita
   fclose(Site);

   //DEBUGGER: printf("\n\n ROW: _%s_ \n\n", ROW);

   for(short int i = 0; i < strlen(ROW); i++)
      if(ROW[i] == ',')
      {
         Comma = true;
         break;
      }

   if(!Comma)
   {
      system("cls");
      system("color 40");

      printf("\n\n \t\t ERROR! UNABLE TO READ CREDENTIALS! \n\n");
      printf("\n\n \t\t\t Press any key to continue...");
      system("PAUSE > Null");

      system("color 1f");
      system("cls");
   }
   else{
      while(ROW[N] != ','){
         strncat(TempUsername, &ROW[N], 1);  //Devo perforza concatenare la stringa con questo metodo senno' da' un warning siccome legge ROW[N] come intero
         N++;
      } TempUsername[N] = '\0';  //Imposto manualmente il finestringa
      //DEBUGGER: printf("\n Username: _%s_ \n", TempUsername);

      CommaPos = ++N;
      while(ROW[N] != '\n' && ROW[N] != '\0'){
         strncat(TempPassword, &ROW[N], 1);  //Devo perforza concatenare la stringa con questo metodo senno' da' un warning siccome legge ROW[N] come intero
         N++;
      } TempPassword[N - CommaPos] = '\0';   //Imposto manualmente il finestringa
      //DEBUGGER: printf("\n Password: _%s_ \n", TempPassword);

      printf("\n\n THESE ARE THE LOGIN CREDENTIALS: \n");
      if(!strcmp(TempUsername, "") == 0)
         printf("\n USERNAME: %s", TempUsername);
      printf("\n PASSWORD: %s", TempPassword);

      system("PAUSE > Null");
   }
}

void manageSites(char SITES[][LS])
{
   system("cls");
   system("color 1f");

   int Choose, Integer;
   float Real;

   printf("\n\n\n");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |        What action do you want to perform ?        |");
   printf("\n \t\t |         (digit the number of your choose)          |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         1 --> RENAME A SITE                        |");
   printf("\n \t\t |         2 --> EDIT A SITE                          |");
   printf("\n \t\t |         3 --> REMOVE A SITE                        |");
   printf("\n \t\t |         4 --> REMOVE EVERY SITE                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         10 --> GO BACK                             |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- + \n\n");

   do
   {
      printf("\t\t Type here your decision: ");
      fflush(stdin);
      scanf("%f", &Real);
      Choose = Real;
      printf("\n");
   }while(!(Choose == Real && ((Choose >= 1 && Choose <= 4) || Choose == 10)));

   switch(Choose)
   {
      case 1:
      {
         searchSite(SITES, 2);   // --> Rename
      }break;

      case 2:
      {
         searchSite(SITES, 3);   // --> Edit
      }break;

      case 3:
      {
         searchSite(SITES, 4);   // --> Remove
      }break;

      case 4:
      {
         Integer = removeEverySite(SITES);   // --> Remove All
      }break;

      case 10:
      {
         system("exit");
      }
   }
}
bool renameSelectedSite(char SITES[][LS], char TempFileName[])
{
   system("cls");

   char TempFileName2[LF], SiteName[LS], NewSiteName[LS];
   bool RightArr = true;

   getSiteOrPinNameFromCommand(TempFileName, SiteName); //Ottengo il nome del sito

   printf("\n The actual name of the site is: %s ", SiteName);
   do{
      RightArr = true;

      printf("\n Insert the new name of the site (NO SPACES) or enter 'X' to exit: ");
      fflush(stdin);
      scanf("%s", NewSiteName);

      //Trasformo tutto in maiuscolo
      for(short int i = 0; i < strlen(NewSiteName); i++)
         NewSiteName[i] = toupper(NewSiteName[i]);

      if(!(strlen(NewSiteName) == 1 && (NewSiteName[0] == 'X' || NewSiteName[0] == 'x')))
      {
         if(strlen(NewSiteName) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
         {
            printf("\n THE SITE NAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
            RightArr = false;
         }
         else if(strlen(SiteName) == strlen(NewSiteName) && strcmp(SiteName, NewSiteName) == 0) //Il nome nuovo del sito e' uguale a quello vecchio
               {
                  printf("\n THE NEW SITE NAME MUST BE DIFFERENT FROM THE OLD ONE !");
                  RightArr = false;
               }
               else  //Controllo che non ci sia un sito registrato con lo stesso nome
               {
                  //Copio la stringa nell'array piu' grande e ci metto l'estensione
                  strcpy(TempFileName2, NewSiteName);
                  strcat(TempFileName2, ".txt");

                  //DEBUGGER: printf("\n File name: %s \n", TempFileName);

                  for(short int N = 0; N < SitesAmount; N++)
                  {
                     //DEBUGGER: printf("\n File name in array: %s \n", SITES[N]);

                     if(strcmp(SITES[N], TempFileName2) == 0)   //Se c'e' gia' un sito registrato con lo stesso nome
                     {
                        printf("\n THE INSERTED SITE ALREADY EXISTS !");
                        RightArr = false;
                        break;
                     }
                  }
               }
      }
   }while(!RightArr);

   if(!(strlen(NewSiteName) == 1 && (NewSiteName[0] == 'X' || NewSiteName[0] == 'x')))  //Se l'utente non ha digitato 'X' o 'x' allora continuo
   {
      for(short int i = 0; i < strlen(TempFileName); i++)   //Sostituisco nell'indirizzo del file gli '/' con i '\' in modo tale che sia eseguibile su cmd
         if(TempFileName[i] == '/')
            TempFileName[i] = '\\';

      char Command[2*LS + CMD + 2] = "rename ";
      strcat(Command, TempFileName);
      strcat(Command, " ");
      strcat(Command, TempFileName2);
      //DEBUGGER: printf("\n Command: %s \n", Command);
      //DEBUGGER: system("PAUSE > Null");

      system(Command);

      system("cls");
      system("color 20");
      printf("\n\n\n\n \t\t SITE RENAMED SUCCESSFULLY \n\n");
      printf("\n\n\n\n \t\t Press any key to continue...");

      system("PAUSE > Null");
      system("color 1f");


      return true;
   }
   system("cls");
   return false;
}
bool removeEverySite(char SITES[][LS])
{
   char TempChar;

   printf("\n\n\n \t Do you want to delete every registered site (%i site/sites)? \n", SitesAmount);

   do{
      printf("\n \t Enter 'Y' if YES, 'N' if NO: ");
      fflush(stdin);

      scanf("%c", &TempChar);
      TempChar = toupper(TempChar);
   }while(!(TempChar == 'Y' || TempChar == 'N'));

   if(TempChar == 'Y')
   {
      if(SitesAmount != 1)
      {
         printf("\n\n\n \t THIS OPERATION CANNOT BE RESTORED !");
         printf("\n \t Are you sure You want to continue? \n");

         do{
            printf("\n \t Enter 'Y' if YES, 'N' if NO: ");
            fflush(stdin);

            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'Y')
         {
            system("DeleteDefaultUserSites.bat > Null");
            loadSites(SITES);

            system("cls");
            system("color a0");
            printf("\n\n\n\n \t\t EVERY SITE WAS DELETED CORRECTLY \n\n");
            printf("\n\n\n\n \t\t Press any key to continue...");
            system("PAUSE > Null");
            return true;
         }
      }
      else{
         system("DeleteDefaultUserSites.bat > Null");
         loadSites(SITES);

         system("cls");
         system("color a0");
         printf("\n\n\n\n \t\t EVERY SITE WAS DELETED CORRECTLY \n\n");
         printf("\n\n\n\n \t\t Press any key to continue...");
         system("PAUSE > Null");
         return true;
      }
   }

   return false;
}


void createPin(char PIN[][LS])
{
   /// CONTROLLARE LA SINGOLARITA'
   system("cls");
   system("color 1f");

   char TempChar;
   char TempPinName[LS];
   char TempFileName[LF];
   char Command[LF + CMD] = "Users/DefaultUser/Pins/";  //Inizializzo il valore del comando al percorso del file
   bool RightArr = false;

   //DEBUGGER: printf("\n \t\t\t CREATE PIN SITE FUNCTION \n\n");

   do{
      system("cls");
      RightArr = true;

      //printf("\n\n Insert the name of the PIN (Max %i characters, NO SPACES) or enter 'X' to exit: ", LF-1);
      printf("\n\n Insert the name of the PIN (NO SPACES) or enter 'X' to exit: ");
      fflush(stdin);
      scanf("%s", TempPinName);

      if(strlen(TempPinName) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
      {
         printf("\n THE PIN NAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
         RightArr = false;
      }

      else if(!(strlen(TempPinName) == 1 && (TempPinName[0] == 'X' || TempPinName[0] == 'x')))  //Se l'utente non ha digitato 'X' o 'x' allora continuo
            {
               //Trasformo tutto in maiuscolo
               for(short int i = 0; i < strlen(TempPinName); i++)
                  TempPinName[i] = toupper(TempPinName[i]);

               //Copio la stringa nell'array piu' grande e ci metto l'estensione
               strcpy(TempFileName, TempPinName);
               strcat(TempFileName, ".txt");

               //DEBUGGER: printf("\n File name: %s \n", TempFileName);

               for(short int N = 0; N < PinsAmount; N++)
               {
                  //DEBUGGER: printf("\n File name in array: %s \n", PIN[N]);

                  if(strcmp(PIN[N], TempFileName) == 0)   //Se c'e' gia' un sito con PIN registrato con lo stesso nome
                  {
                     printf("\n THE INSERTED PIN SITE ALREADY EXISTS !");
                     RightArr = false;

                     printf("\n\n DO YOU WANT TO OVERWRITE IT?");
                     do{
                        printf("\n Enter 'Y' to overwrite the existent PIN, 'N' otherwise: ");
                        fflush(stdin);

                        scanf("%c", &TempChar);
                        TempChar = toupper(TempChar);
                     }while(!(TempChar == 'Y' || TempChar == 'N'));

                     if(TempChar == 'Y')
                        RightArr = true;  //Continuo con l'esecuzione del codice in modo tale da sovrascrivere il file
                  }
               }
            }
   }while(!RightArr);

   if(!(strlen(TempPinName) == 1 && (TempPinName[0] == 'X' || TempPinName[0] == 'x'))) //Se l'utente non ha digitato 'X' o 'x' allora continuo
   {
      //DEBUGGER: printf("\n Command value: %s \n", Command);

      //Assegno il valore a Command
      strcat(Command, TempFileName);

      //DEBUGGER: printf("\n Command value: %s \n", Command);

      FILE *Pin = fopen(Command, "wb");   //Creo il file desiderato
      fclose(Pin);

      fillPin(Command);

      //Aggiorno l'array PIN
      loadPins(PIN);
   }

   //DEBUGGER: system("PAUSE > Null");
   //DEBUGGER: system("cls");
}
void fillPin(char Command[])
{
   system("cls");
   system("color 1f");

   char PIN[LP];
   long long int PIN_int;

   char TempChar;
   bool RightNum = true;


   do{
      RightNum = true;

      printf("\n\n Insert the PIN (From a minimum of 4 to a maximum of 8 numbers)(NO LETTERS and NO SPACES): ");
      fflush(stdin);
      scanf("%s", &PIN);

      if(strlen(PIN) < 4 || strlen(PIN) > 8) //Se il PIN ha meno di 4 cifre o piu' di 8
      {
         system("cls");
         printf("\n THE PIN MUST CONTAIN FROM 4 TO 8 NUMBERS ! \n");
         RightNum = false;
      }

      else
      {
         char *charPointer;   //Variabile stringa temporanea che viene dichiarata come puntatore all'inizio di un'area di memoria
         unsigned long integerPart = strtoul(PIN, &charPointer, 10); //Questo metodo restituisce la parte intera della prima variabile passata (deve essere una stringa) ed inserisce il resto nella seconda variabile passata (deve essere una stringa), il 10 sta per indicare la base decimale

         if(*charPointer != '\0')   //Se questa stringa non e' vuota allora il PIN conteneva dei caratteri diversi dai numeri
         {
            system("cls");
            printf("\n THE PIN MUST CONTAIN ONLY NUMBERS ! \n");
            RightNum = false;
         }

         else
         {
            printf("\n This will be the PIN: %s \n", PIN);

            do{
               printf("\n Enter 'Y' if it's fine, 'N' if you want to change it: ");
               fflush(stdin);

               scanf("%c", &TempChar);
               TempChar = toupper(TempChar);
            }while(!(TempChar == 'Y' || TempChar == 'N'));

            if(TempChar == 'N')
               RightNum = false;
         }
      }
   }while(!RightNum);

   /*
   PIN_int = atoi(PIN);

   ///COPIO IL RISULTATO SUL FILE
   FILE *PinSite = fopen(Command, "wb");   //Apro il file desiderato utilizzando la variabile Command che contiene Indirizzo, Nome Sito con PIN ed Estensione
   fwrite(&PIN_int, sizeof(long long int), 1, PinSite);
   */
   FILE *Pin = fopen(Command, "w");   //Apro il file desiderato utilizzando la variabile Command che contiene Indirizzo, Nome Sito con PIN ed Estensione
   fputs(PIN, Pin); //Copio il record su file
   fclose(Pin);
}

void searchPin(char PIN[][LS], int Option)
{
   system("cls");
   system("color 1f");

   char TempChar;
   char TempPinName[LF];
   char TempFileName[LS];
   char Command[LS + CMD] = "Users/DefaultUser/Pins/";  //Inizializzo il valore del comando al percorso del file

   bool RightArr = false;
   bool Found = false;
   bool TempBool;

   loadPins(PIN);
   //DEBUGGER: printf("\n \t\t\t SEARCH PIN FUNCTION \n\n");


   do{
      system("cls");
      system("color 1f");

      do{
         RightArr = true;

         if(Option == 1 || Option == 4)
            printf("\n\n Insert the name of the PIN (NO SPACES)(Enter 'X' to exit)(Enter '*' to view all): ");
         else
            printf("\n\n Insert the name of the PIN (NO SPACES)(Enter 'X' to exit): ");
         fflush(stdin);
         scanf("%s", TempPinName);

         if(strlen(TempPinName) > (LF-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
         {
            printf("\n THE PIN NAME IS TOO LONG !");
            RightArr = false;
         }
      }while(!RightArr);

      //DEBUGGER: printf("\n Searched PIN: %s", TempPinName);

      //Trasformo tutto in maiuscolo
      for(short int i = 0; i < strlen(TempPinName); i++)
         TempPinName[i] = toupper(TempPinName[i]);

      //DEBUGGER: printf("\n Searched PIN: %s", TempPinName);


      if(strlen(TempPinName) == 1 && TempPinName[0] == 'X')  //Se e' stata inserita la 'X'
         break;

      /// ----------- SE E' STATO INSERITO IL CARATTERE '*' -----------
      else if(strlen(TempPinName) == 1 && TempPinName[0] == '*')
            {
               Found = true;

               switch(Option)
               {
                  case 1:
                  {
                     system("cls");

                     printf("\n\n \t HERE YOU CAN SEE ALL THE REGISTERED PINS \n\n");

                     for(short int i = 0; i < PinsAmount; i++)
                     {
                        printf("\n \t %i^ \t ", i+1);

                        for(short int c = 0; c < (strlen(PIN[i]) - 4); c++) //Serve per stampare il nome del file senza l'estensione
                           printf("%c", PIN[i][c]);
                     }

                     printf("\n\n\n \t Press any key to continue... \n\n");
                     system("PAUSE > Null");
                  }break;

                  case 2:
                  {
                     if(PinsAmount != 1) //Se ce n'e' piu' di uno
                     {
                        system("cls");
                        system("color 40");

                        printf("\n \t UNABLE TO RENAME EVERY PIN !");
                        printf("\n \t SEARCH JUST ONE ! \n");

                        printf("\n\n\n \t Press any key to continue... \n\n");
                        system("PAUSE > Null");
                        system("color 1f");


                        Found = false;
                     }
                     else{ //Se ce n'e' solo uno
                        system("cls");
                        strcat(Command, PIN[0]);   //Assegno il valore a Command

                        TempBool = renameSelectedPin(PIN, Command);
                     }
                  }break;

                  case 3:
                  {
                     if(PinsAmount != 1) //Se ce n'e' piu' di uno
                     {
                        system("cls");
                        system("color 40");

                        printf("\n \t UNABLE TO EDIT EVERY PIN !");
                        printf("\n \t SEARCH JUST ONE ! \n");

                        printf("\n\n\n \t Press any key to continue... \n\n");
                        system("PAUSE > Null");
                        system("color 1f");


                        Found = false;
                     }
                     else{ //Se ce n'e' solo uno
                        system("cls");
                        strcat(Command, PIN[0]);   //Assegno il valore a Command

                        //TempBool = editSelectedPin(PIN, Command);
                     }
                  }break;

                  case 4:
                  {
                     TempBool = removeEveryPin(PIN);
                     loadPins(PIN);
                  }break;
               }
               break;
            }

            /// ----------- SE E' STATO INSERITO IL NOME DI UN SITO -----------
            else
            {
               //Copio la stringa nell'array piu' grande e ci metto l'estensione
               strcpy(TempFileName, TempPinName);
               strcat(TempFileName, ".txt");


               for(short int i = 0; i < PinsAmount; i++)
               {
                  if(strcmp(PIN[i], TempFileName) == 0)
                  {
                     //Assegno il valore a Command
                     strcat(Command, TempFileName);

                     switch(Option)
                     {
                        case 1:
                        {
                           readPin(Command);   //Visualizzo il PIN trovato
                        }break;

                        case 2:
                        {
                           TempBool = renameSelectedPin(PIN, Command);
                           loadPins(PIN);
                        }break;

                        case 3:
                        {
                           //TempBool = editSelectedPin(Command);
                        }break;

                        case 4:
                        {
                           TempBool = removeSelectedElement(Command, 1);   //Rimuovo il PIN trovato
                           loadPins(PIN);
                        }break;
                     }

                     Found = true;
                     break;
                  }
               }

               /// ----------- SE BISOGNA RICORRERE ALLA RICERCA AVANZATA -----------
               if(!Found)
               {
                  if(!advancedSearchPin(PIN, TempPinName, Command))
                  {
                     printf("\n Searched PIN not found !");
                     system("PAUSE > Null");
                  }
                  else
                  {
                     Found = true;

                     switch(Option)
                     {
                        case 1:  //Visualizzo i dati
                        {
                           readPin(Command);   //Visualizzo il PIN trovato
                        }break;

                        case 2:
                        {
                           TempBool = renameSelectedPin(PIN, Command);
                           loadPins(PIN);
                        }break;

                        case 3:
                        {
                           //TempBool = editSelectedPin(Command);
                        }break;

                        case 4:
                        {
                           TempBool = removeSelectedElement(Command, 1);   //Rimuovo il PIN trovato
                           loadPins(PIN);
                        }break;
                     }

                     break;
                  }
               }
            }
   }while(!(strlen(TempPinName) != 1 && TempPinName[0] != 'X'));
}
bool advancedSearchPin(char PIN[][LS], char PIN_NAME[], char Command[])
{
   int SearchedLenght = strlen(PIN_NAME);
   char TempChar;
   bool Found = false;
   bool ThereIs;

   //DEBUGGER: printf("\nLunghezza parola da cercare (%s): %i \n", PIN_NAME, SearchedLenght);

   for(short int i = 0; i < PinsAmount; i++)
   {
      //DEBUGGER: printf("\nPIN CHE CONTROLLO: %s \n", PIN[i]);

      //Escludo l'etensione del file dal calcolo della lunghezza
      if(strlen(PIN[i]) - 4 > SearchedLenght)  //Se la lunghezza del PIN e' maggiore della parola cercata allora controllo la presenza di quella parola all'interno del nome del PIN
      {
         //DEBUGGER: printf("\nLUNGHEZZA CONTROLLO SITO: %s --> %i \n", SITES[i], strlen(SITES[i]));

         for(short int j = 0; j <= (strlen(PIN[i]) - SearchedLenght); j++)   //Avanzo di una lettera ogni volta e controllo la presenza della parola cercata all'interno del nome del PIN
         {
            //DEBUGGER: printf("\n J = %i, (strlen(PIN[i]) - SearchedLenght - 4) = %i \n", j, (strlen(PIN[i]) - SearchedLenght - 4));

            ThereIs = true;

            for(short int k = j; k < SearchedLenght + j; k++)   //Controllo la presenza della parola cercata all'interno del nome del PIN
            {
               //DEBUGGER: printf("\nPIN[i][k] = %c --> PIN_NAME[k-j] = %c", PIN[i][k], PIN_NAME[k-j]);

               if(PIN[i][k] != PIN_NAME[k-j])
               {
                  ThereIs = false;  //La parola non e' contenuta
                  break;
               }
            }

            if(ThereIs)
            {
               char tempFoundPin[strlen(PIN[i]) - 4];
               strncpy(tempFoundPin, PIN[i], strlen(PIN[i]) - 4);
               tempFoundPin[strlen(PIN[i]) - 4] = '\0';

               do
               {
                  printf("\n Did You mean %s? ", tempFoundPin);
                  fflush(stdin);
                  printf("\n Enter 'Y' if Yes, 'N' otherwise: ");
                  scanf("%c", &TempChar); printf("\n");
                  TempChar = toupper(TempChar);
               }while(!(TempChar == 'Y' || TempChar == 'N'));

               if(TempChar == 'Y')  //Se e' il PIN effettivamente cercato allora aggiorno la variabile command e ritorno true
               {
                  //Assegno il valore a Command
                  strcat(Command, PIN[i]);

                  return true;
               }
               else  //Altrimenti continuo la ricerca
                  break;
            }
         }
      }
   }

   return false;
}
void readPin(char Command[])
{
   char PIN[LP];

   FILE *Pin = fopen(Command, "r");

   fgets(PIN, LP-1, Pin);

   fclose(Pin);


   printf("\n\n THIS IS THE PIN: %s", PIN);

   system("PAUSE > Null");
}

void managePin(char PIN[][LS])
{
   system("cls");
   system("color 1f");

   int Choose, Integer;
   float Real;

   printf("\n\n\n");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |        What action do you want to perform ?        |");
   printf("\n \t\t |         (digit the number of your choose)          |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         1 --> RENAME A PIN                         |");
   printf("\n \t\t |         2 --> EDIT A PIN                           |");
   printf("\n \t\t |         3 --> REMOVE A PIN                         |");
   printf("\n \t\t |         4 --> REMOVE EVERY PIN                     |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |         10 --> GO BACK                             |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t |                                                    |");
   printf("\n \t\t + -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- + \n\n");

   do
   {
      printf("\t\t Type here your decision: ");
      fflush(stdin);
      scanf("%f", &Real);
      Choose = Real;
      printf("\n");
   }while(!(Choose == Real && ((Choose >= 1 && Choose <= 4) || Choose == 10)));


   switch(Choose)
   {
      case 1:
      {
         searchPin(PIN, 2);   // --> Rename
      }break;

      case 2:
      {
         searchPin(PIN, 3);   // --> Edit
      }break;

      case 3:
      {
         searchPin(PIN, 4);   // --> Remove
      }break;

      case 4:
      {
         Integer = removeEveryPin(PIN);   // --> Remove All
      }break;

      case 10:
      {
         system("exit");
      }
   }
}
bool renameSelectedPin(char PIN[][LS], char TempFileName[])
{
   system("cls");

   char TempFileName2[LF], PinName[LS], NewPinName[LS];
   bool RightArr = true;

   getSiteOrPinNameFromCommand(TempFileName, PinName);   //Ottengo il nome del PIN

   printf("\n The actual name of the PIN is: %s ", PinName);
   do{
      RightArr = true;

      printf("\n Insert the new name of the PIN (NO SPACES) or enter 'X' to exit: ");
      fflush(stdin);
      scanf("%s", NewPinName);

      //Trasformo tutto in maiuscolo
      for(short int i = 0; i < strlen(NewPinName); i++)
         NewPinName[i] = toupper(NewPinName[i]);

      if(!(strlen(NewPinName) == 1 && (NewPinName[0] == 'X' || NewPinName[0] == 'x')))
      {
         if(strlen(NewPinName) > (LS-1)) //Avviso l'utente di un inserimento di caratteri superiore al consentito
         {
            printf("\n THE PIN NAME IS TOO LONG (MAX %i CHARACTERS) !", LS-1);
            RightArr = false;
         }
         else if(strlen(PinName) == strlen(NewPinName) && strcmp(PinName, NewPinName) == 0) //Il nome nuovo del PIN e' uguale a quello vecchio
               {
                  printf("\n THE NEW PIN NAME MUST BE DIFFERENT FROM THE OLD ONE !");
                  RightArr = false;
               }
               else  //Controllo che non ci sia un PIN registrato con lo stesso nome
               {
                  //Copio la stringa nell'array piu' grande e ci metto l'estensione
                  strcpy(TempFileName2, NewPinName);
                  strcat(TempFileName2, ".txt");

                  //DEBUGGER: printf("\n File name: %s \n", TempFileName);

                  for(short int N = 0; N < SitesAmount; N++)
                  {
                     //DEBUGGER: printf("\n File name in array: %s \n", SITES[N]);

                     if(strcmp(PIN[N], TempFileName2) == 0)   //Se c'e' gia' un sito registrato con lo stesso nome
                     {
                        printf("\n THE INSERTED PIN ALREADY EXISTS !");
                        RightArr = false;
                        break;
                     }
                  }
               }
      }
   }while(!RightArr);

   if(!(strlen(NewPinName) == 1 && (NewPinName[0] == 'X' || NewPinName[0] == 'x')))  //Se l'utente non ha digitato 'X' o 'x' allora continuo
   {
      for(short int i = 0; i < strlen(TempFileName); i++)   //Sostituisco nell'indirizzo del file gli '/' con i '\' in modo tale che sia eseguibile su cmd
         if(TempFileName[i] == '/')
            TempFileName[i] = '\\';

      char Command[2*LS + CMD + 2] = "rename ";
      strcat(Command, TempFileName);
      strcat(Command, " ");
      strcat(Command, TempFileName2);
      //DEBUGGER: printf("\n Command: %s \n", Command);
      //DEBUGGER: system("PAUSE > Null");

      system(Command);

      system("cls");
      system("color 20");
      printf("\n\n\n\n \t\t PIN RENAMED SUCCESSFULLY \n\n");
      printf("\n\n\n\n \t\t Press any key to continue...");

      system("PAUSE > Null");
      system("color 1f");


      return true;
   }
   system("cls");
   return false;
}
bool removeEveryPin(char PIN[][LS])
{
   char TempChar;

   printf("\n\n\n \t Do you want to delete every registered PIN (%i PIN/PINS)? \n", PinsAmount);

   do{
      printf("\n \t Enter 'Y' if YES, 'N' if NO: ");
      fflush(stdin);

      scanf("%c", &TempChar);
      TempChar = toupper(TempChar);
   }while(!(TempChar == 'Y' || TempChar == 'N'));

   if(TempChar == 'Y')
   {
      if(PinsAmount != 1)
      {
         printf("\n\n\n \t THIS OPERATION CANNOT BE RESTORED !");
         printf("\n \t Are you sure You want to continue? \n");

         do{
            printf("\n \t Enter 'Y' if YES, 'N' if NO: ");
            fflush(stdin);

            scanf("%c", &TempChar);
            TempChar = toupper(TempChar);
         }while(!(TempChar == 'Y' || TempChar == 'N'));

         if(TempChar == 'Y')
         {
            system("DeleteDefaultUserPins.bat > Null");
            loadPins(PIN);

            system("cls");
            system("color a0");
            printf("\n\n\n\n \t\t EVERY PIN WAS DELETED CORRECTLY \n\n");
            printf("\n\n\n\n \t\t Press any key to continue...");
            system("PAUSE > Null");

            return true;
         }
      }
      else{
         system("DeleteDefaultUserPins.bat > Null");
         loadPins(PIN);

         system("cls");
         system("color a0");
         printf("\n\n\n\n \t\t EVERY PIN WAS DELETED CORRECTLY \n\n");
         printf("\n\n\n\n \t\t Press any key to continue...");
         system("PAUSE > Null");

         return true;
      }
   }

   return false;
}

bool removeSelectedElement(char TempFileName[], bool ElementOption)
{
   char TempChar;
   char Element[LS]; // Conterra' site o PIN
   char ELEMENT[LS]; // Conterra' SITE o PIN

   if(ElementOption == 0){
      strcpy(Element, "site");
      strcpy(ELEMENT, "SITE");
   }
   else{
      strcpy(Element, "PIN");
      strcpy(ELEMENT, "PIN");
   }

   printf("\n\n");
   do{
      printf("\n Do You want to remove this %s (Y --> Yes, N --> No)? ", Element);
      fflush(stdin);
      scanf("%c", &TempChar);
      TempChar = toupper(TempChar);
   }while(!(TempChar == 'Y' || TempChar == 'N'));

   if(TempChar == 'Y')  //Se si vuole cancellare il Sito/PIN
   {
      for(short int i = 0; i < strlen(TempFileName); i++)   //Sostituisco nell'indirizzo del file gli '/' con i '\' in modo tale che sia eseguibile su cmd
         if(TempFileName[i] == '/')
            TempFileName[i] = '\\';

      char Command[LS + CMD + 2] = "del ";
      strcat(Command, TempFileName);
      //DEBUGGER: printf("\n Command: _%s_ \n", Command);
      //DEBUGGER: system("PAUSE > Null");

      system(Command);

      system("cls");
      system("color 20");
      printf("\n\n\n\n \t\t %s REMOVED SUCCESSFULLY \n\n", ELEMENT);
      printf("\n\n\n\n \t\t Press any key to continue...");

      system("PAUSE > Null");
      system("color 1f");


      return true;
   }

   system("cls");
   system("color f0");
   printf("\n\n\n\n \t\t THE %s HAS NOT BEEN DELETED \n\n", ELEMENT);
   printf("\n\n\n\n \t\t Press any key to continue...");

   system("PAUSE > Null");
   system("color 1f");

   return false;
}


void getDataFileName()
{
   system("cls");
   system("color 1f");

   char FileName[LF*2] = "../";
   char TempFileName[LF*2] = "";
   bool True = true;

   char ParentDirectory[PATH_MAX]; //PATH_MAX --> Massimo valore dell'indirizzo che puo' raggiungere

   getcwd(ParentDirectory, sizeof(ParentDirectory));  //Ottengo la CWD (Current Working Directory) e la metto dentro alla variabile "ParentDirectory" su cui lavorero' successivamente
   char *LastSlash = strrchr(ParentDirectory, '\\');  //Puntatore che punta all'ultimo slash della stringa "ParentDirectory"
   *LastSlash = '\0';   //Sostituisco l'ultimo slash della stringa "ParentDirectory" con il finestringa

   fflush(stdin);

   printf("\n WARNING!\n");
   printf("\n Before using this tool you have to copy the file generated by your browser \n in the same directory where the Password Locker folder is saved:");
   printf("\n \t %s \n\n\n\n", ParentDirectory);

   do{
      True = true;

      printf("\n Insert the name of the file with the needed data (without the extension .csv) or enter 'X' to exit: ");
      fflush(stdin);
      gets(TempFileName);

      if(strlen(TempFileName) == 1 && (TempFileName[0] == 'X' || TempFileName[0] == 'x'))
      {
         True = false;
         break;
      }
      else
      {
         strcat(FileName, TempFileName);
         strcat(FileName, ".csv");

         FILE *TempFile = fopen(FileName, "r");

         if(TempFile == NULL) //Impossibile aprire il file
         {
            True = false;
            printf("\n Unable to open the file... Try again (read the warning for more information)! \n", FileName);
         }
         else{
            fclose(TempFile);
         }
      }
   }while(!True);

   if(True)
      importDataFromCSV(FileName);
}
void importDataFromCSV(char FileName[])
{
   char ROW[ROWL];
   short int Amount = 0;
   FILE *DataBase = fopen(FileName, "r"); //Apro il file in modalita' lettura

   while(fgets(ROW, ROWL, DataBase) != NULL)  //Conto il numero di righe presenti
   {
      Amount++;
   }
   rewind(DataBase);

   printf("\n Ci sono %i righe \n", Amount);
   DataBaseSite DATA_BASE_SITES[Amount-1]; //Dichiaro una struct di N-1 elementi (non conto la prima riga in quanto e' un'intestazione)
   //DEBUGGER: printf("\n Ho dichiarato l'array struct \n");

   short int N = 0;
   while(fgets(ROW, ROWL, DataBase) != NULL)  //Passo la riga alla funzione per la stampa bella
   {
      //DEBUGGER: printf("\n Test %i", N);

      if(N != 0)
      {
         //DEBUGGER: printf("\n %s \n", ROW);
         getDataFromCSV(ROW, &DATA_BASE_SITES[N-1]);
      }
      N++;
   }
   fclose(DataBase);

   createAndFillSiteFromCSV(DATA_BASE_SITES, Amount);

   system("cls");
   system("color a0");
   printf("\n\n\n\n \t\t DATA LOADED SUCCESSFULLY !");
   printf("\n\n \t RESTART THE PROGRAM TO APPLY THE CHANGES !");
   printf("\n\n\n\n \t\t Press any key to continue... \n\n\n\n");
   system("PAUSE > Null");
}
void getDataFromCSV(char ROW[], DataBaseSite *DATA_BASE_SITES)
{
   short int CommaAmount = 0;
   short int CommaPos[4] = {0, 0, 0, 0};  //CommaPos[0] = Dove finisce il nome del sito, CommaPos[1] = Dove finisce l'URL del sito, CommaPos[2] = Dove finisce l'username, CommaPos[3] = Dove finisce la password
   char SiteUsername[LS] = "", SitePassword[LS] = "";

   //DEBUGGER: printf("\n Row: %s \n", ROW);


   strcpy(DATA_BASE_SITES->SITE_NAME, "");   //Svuoto la stringa
   strcpy(DATA_BASE_SITES->SITE_USERNAME, "");  //Svuoto la stringa
   strcpy(DATA_BASE_SITES->SITE_PASSWORD, "");  //Svuoto la stringa

   for(short int i = 0; i < strlen(ROW); i++)
   {

      if(ROW[i] == ',')
      {
         CommaPos[CommaAmount++] = i;

         if(CommaAmount == 1) //Nome Sito
         {
            strncpy(DATA_BASE_SITES->SITE_NAME, ROW, i);
            DATA_BASE_SITES->SITE_NAME[i] = '\0';

            for(short int j = 0; j < strlen(DATA_BASE_SITES->SITE_NAME); j++)
               DATA_BASE_SITES->SITE_NAME[j] = toupper(DATA_BASE_SITES->SITE_NAME[j]);

            //DEBUGGER: printf("\n Site name (STRUCT): _%s_ \n", DATA_BASE_SITES->SITE_NAME);
         }

         else if(CommaAmount == 3 && !(CommaPos[2] + 1 == CommaPos[3]))  //Username Sito
         {
            short int UsernameLength = CommaPos[CommaAmount - 1] - CommaPos[CommaAmount - 2] - 1;

            for(short int j = CommaPos[CommaAmount - 2] + 1; j <= CommaPos[CommaAmount - 1] - 1; j++)
            {
               SiteUsername[j - (CommaPos[CommaAmount - 2] + 1)] = ROW[j];
            }
            SiteUsername[UsernameLength] = '\0';

            strcpy(DATA_BASE_SITES->SITE_USERNAME, SiteUsername);

            for(short int j = 0; j < strlen(DATA_BASE_SITES->SITE_USERNAME); j++)
               DATA_BASE_SITES->SITE_USERNAME[j] = toupper(DATA_BASE_SITES->SITE_USERNAME[j]);

            //DEBUGGER: printf(" Site Username: _%s_ \n", SiteUsername);
            //DEBUGGER: printf(" Site Username (STRUCT): _%s_ \n", DATA_BASE_SITES->SITE_USERNAME);
         }

         else if(CommaAmount == 4)  //Password Sito
         {
            int PasswordLength = CommaPos[CommaAmount - 1] - CommaPos[CommaAmount - 2] - 1;

            for(short int j = CommaPos[CommaAmount - 2] + 1; j <= CommaPos[CommaAmount - 1] - 1; j++)
            {
               SitePassword[j - (CommaPos[CommaAmount - 2] + 1)] = ROW[j];
            }
            SitePassword[PasswordLength] = '\0';

            strcpy(DATA_BASE_SITES->SITE_PASSWORD, SitePassword);

            //DEBUGGER: printf(" Site Password: _%s_ \n", SitePassword);
            //DEBUGGER: printf(" Site Password (STRUCT): _%s_ \n", DATA_BASE_SITES->SITE_PASSWORD);

            break;
         }
      }
   }
}
void createAndFillSiteFromCSV(DataBaseSite DATA_BASE_SITES[], short int N)
{
   int Reps = 0, SITE_NAME_L = 0;
   char Command[LS + CMD + 10] = "Users/DefaultUser/Sites/";  //Inizializzo il valore del comando al percorso del file
   char REPS[4] = "";
   FILE *CSV_Site;

   system("cls");
   system("color 0e");
   printf("\n\n\n \t\t\t LOADING DATA ... \n\n\n");

   for(short int i = 0; i < N; i++)
   {
      //DEBUGGER: printf("\n %i^ NOME: %s, USERNAME: %s, PASSWORD: %s", i+1, DATA_BASE_SITES[i].SITE_NAME, DATA_BASE_SITES[i].SITE_USERNAME, DATA_BASE_SITES[i].SITE_PASSWORD);

      strcpy(Command, "Users/DefaultUser/Sites/");  //Reimposto il valore del comando al percorso del file
      //DEBUGGER: printf("\n Command: _%s_ \n", Command);
      SITE_NAME_L = strlen(DATA_BASE_SITES[i].SITE_NAME);
      strcpy(REPS, "");

      if(strcmp(DATA_BASE_SITES[i].SITE_NAME, DATA_BASE_SITES[i+1].SITE_NAME) == 0){ //Se il successivo ha lo stesso nome
         Reps++;
         sprintf(REPS, "%i", Reps);   //Converte un numero in una striga di char

         strcat(DATA_BASE_SITES[i].SITE_NAME, "_");
         strcat(DATA_BASE_SITES[i].SITE_NAME, REPS);
      }
      else{ //Se il successivo ha un nome diverso
         Reps = 0;
      }
      strcat(Command, DATA_BASE_SITES[i].SITE_NAME);
      strcat(Command, ".txt");
      //DEBUGGER: printf("\n Command: _%s_ \n", Command);

      //Copio tutto su file
      CSV_Site = fopen(Command, "w");
      fputs(DATA_BASE_SITES[i].SITE_USERNAME, CSV_Site);
      fputc(',', CSV_Site);
      fputs(DATA_BASE_SITES[i].SITE_PASSWORD, CSV_Site);
      fclose(CSV_Site);

      //DEBUGGER: printf("\n %i^ site data uploaded successfully \n", i+1);
   }
}


/// ------ DA IMPLEMENTARE ------

void getDate()
{
   FILE *Data = fopen("Data.txt", "r");

   char TempDate[11];   //Questa stringa conterra' l'intera data
   char TempDay[3], TempMonth[3], TempYear[5];  //Queste stringhe conterranno rispettivamente il giorno, il mese e l'anno

   fgets(TempDate, 11, Data);

   fclose(Data);

   TempDate[11] = '\0';

   //DEBUGGER: printf(" Temp Date: %s \n The size is: %d\n\n", TempDate, sizeof(TempDate));


   TempDay[0] = TempDate[0];
   TempDay[1] = TempDate[1];
   TempDay[2] = '\0';

   TempMonth[0] = TempDate[3];
   TempMonth[1] = TempDate[4];
   TempMonth[2] = '\0';

   TempYear[0] = TempDate[6];
   TempYear[1] = TempDate[7];
   TempYear[2] = TempDate[8];
   TempYear[3] = TempDate[9];
   TempYear[4] = '\0';

   //DEBUGGER: printf("\n Day: %s", TempDay);
   //DEBUGGER: printf("\n Month: %s", TempMonth);
   //DEBUGGER: printf("\n Year: %s \n\n", TempYear);


   //Il metodo atoi(stringa) (Ascii to Integer) serve per convertire una stringa in intero
   Day = atoi(TempDay);
   Month = atoi(TempMonth);
   Year = atoi(TempYear);


   //DEBUGGER: printf("\n Today's date is: %i - %i - %i \n\n\n", Day, Month, Year);

   //DEBUGGER: system("Pause");
   //DEBUGGER: system("cls");
}

void loadAccounts()
{
   //E' stata solo abbozzata, non ci metto su le mani da quando ho lavorato per la prima volta su questo progetto
   int N;

   checkRegistrationAndUpload();

   ///CALCOLO DELLA QUANTITA' DI UTENTI
   if(AlreadyRegistered)
   {
      printf("\n\n\n\n \t\t PLEASE WAIT WHILE THE PROGRAM LOADS... \n\n\n\n");

      FILE *DefaultUserFile = fopen("Users/DefaultUser.txt", "r");   //Apro il file Users.txt presente nella cartella Users in modalita lettura

      N = 0;
      while(!feof(DefaultUserFile))
      {
         N++;
      }

      User USERS[N];
      char TempString[41];

      short int Counter = 0;

      while(!feof(DefaultUserFile))
      {
         fgets(TempString, (2*LS), DefaultUserFile);   //Copio la stringa contenente Username, Trattino e Password all'interno di una sringa temporanea

         for(short int i = 0; i < LS; i++)
         {
            USERS[Counter].ACCESS_USERNAME[i] = TempString[i];
         }

         for(short int i = LS; i < (2*LS - 1); i++)
         {
            USERS[Counter].ACCESS_PASSWORD[i] = TempString[i];
         }

         Counter++;
      }

      fclose(DefaultUserFile);
   }
}
