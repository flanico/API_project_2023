#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {false, true}boolean;

typedef struct macchine{
    struct macchine *left;
    struct macchine *right;
    u_int32_t km;
    u_int16_t num;
}carTree;

typedef struct albero{
    u_int32_t stazione;
    struct albero *left;
    struct albero *right;
    struct albero *father;
    u_int32_t maxKM;
    struct albero *tappaRaggiungibile;
    char X;
    carTree* macchine;
}tree;

typedef struct lista{
    u_int32_t stazione;
    struct lista* next;
}list;

typedef struct listaPunt{
    tree* Y;
    struct listaPunt *next;
}listaY;

void aggiungiStazione(tree **autostrada, u_int32_t stazione, u_int32_t* cars, u_int16_t NumCars);
void aggiungiMacchina(carTree **macchine, u_int32_t macchina);
tree* findMinValueNode(tree* node);
carTree* findMinValueNodeCar(carTree* node);
boolean demolisciStazione(tree** autostrada, u_int32_t stazione);
boolean rottamaMacchina(carTree** macchine, u_int32_t macchina);
tree* cercaStazione(tree** autostrada, u_int32_t stazione);
carTree** cercaStazioneCar(tree** autostrada, u_int32_t stazione);
void freeStation (tree* stazione);
void dealloc (carTree *macchine);
void pianificaPercorsoAvanti(tree** autostrada, u_int32_t partenza,u_int32_t arrivo);
void pianificaPercorsoIndietro(tree** autostrada, u_int32_t partenza,u_int32_t arrivo);
u_int32_t visitaAvanti(tree** autostrada, u_int32_t partenza, u_int32_t tappa, boolean firstSearch);
void stampaLista(list *lista);
u_int32_t visitaAlbero(tree* currRoot, u_int32_t arrivo,boolean firstSearch);
u_int32_t searchMax(carTree** macchine);
void stampaPercorsoIndietro(tree* partenza,u_int32_t arrivo);
void freeListaNewStation();

void ispezionaTappa(tree* tappaCorrente, u_int32_t partenza);
void visitaAlberoInd(tree* curr, u_int32_t partenza,tree* tappaCorrente);

void visitaAlberoInizializza(tree* currRoot, u_int32_t arrivo);
void inizializzaX(tree* tappaCorrente, u_int32_t partenza);


void mettiX(tree* tappaCorrente);
void visitaAlberoMettiX(tree* currRoot);

u_int32_t maxReach;
boolean connesso;
int numrighe = 0;
boolean demolisciSoloMacchineStazione;

int main() {
    tree *pointer = NULL;
    tree **autostrada = &pointer;
    boolean FINE = false;
    char command[20];

    while( !FINE ){

        if (scanf("%s",command) != true){FINE = true;}
        numrighe++;

        if(FINE == false && strcmp(command, "aggiungi-stazione")==0) {

            u_int32_t stazione, macchina;
            u_int16_t numCars;

            if(scanf(" %u", &stazione)){}
            if(scanf(" %hd", &numCars)) {}


            u_int32_t cars [numCars];
            for(int i = 0; i<numCars; i++){
                if(scanf(" %u", &macchina)){}
                cars[i] = macchina;
            }

            aggiungiStazione(autostrada, stazione, cars, numCars);

        }else if(FINE == false && strcmp(command, "demolisci-stazione")==0){

            int stazione;
            if(scanf(" %u", &stazione)){}
            boolean demolita;

            demolisciSoloMacchineStazione = true;
            demolita = demolisciStazione(autostrada,stazione);

            if(!demolita) {
                printf("non demolita\n");       //la stazione non è presente nell'albero
            }
            else {
                printf("demolita\n");
            }

        }else if(FINE == false && strcmp(command, "aggiungi-auto")==0){

            u_int32_t stazione;
            u_int32_t macchina;

            if(scanf(" %u", &stazione)){}
            if(scanf(" %u", &macchina)){}

            tree* currStation = cercaStazione(autostrada, stazione);
            if (currStation == NULL) printf("non aggiunta\n");
            else{
                carTree** macchine = &(currStation ->macchine);

                aggiungiMacchina(macchine, macchina);
                if( currStation->maxKM < macchina ) currStation->maxKM = macchina;
                printf("aggiunta\n");
            }

        }else if(FINE == false && strcmp(command, "rottama-auto")==0){

            u_int32_t stazione;
            u_int32_t macchina;

            if(scanf(" %u", &stazione)){}
            if(scanf(" %u", &macchina)){}


            tree* currStation = cercaStazione(autostrada, stazione);
            if (currStation == NULL) printf("non rottamata\n");
            else{
                carTree** macchine = &(currStation ->macchine);

                boolean changePossible = rottamaMacchina(macchine, macchina);
                if (changePossible){
                    currStation->maxKM = searchMax(macchine);
                }

            }

        }else if(FINE == false && strcmp(command, "pianifica-percorso")==0){

            u_int32_t partenza;
            u_int32_t arrivo;

            if(scanf(" %u", &partenza)){}
            if(scanf(" %u", &arrivo)){}

            if (partenza == arrivo){
                printf("%u\n",partenza);
            }
            if(partenza < arrivo){
                pianificaPercorsoAvanti(autostrada,partenza,arrivo);
            }
            else{
                pianificaPercorsoIndietro(autostrada,partenza,arrivo);
            }

        }else {
            FINE = true;
        }

    }

    return 0;
}


/**
 * scorre l'albero delle stazioni e la aggiunge solo se non già esistente
 * @param autostrada
 * @param stazione
 * @param cars
 * @param NumCars
 */
void aggiungiStazione(tree **autostrada, u_int32_t stazione, u_int32_t* cars, u_int16_t NumCars){
    tree* pre=NULL,
            *cur;

    cur = *autostrada;
    while(cur!=NULL){
        pre=cur;
        if (cur -> stazione == stazione){
            printf("non aggiunta\n");
            return;
        }
        else if(cur -> stazione > stazione){
            cur = cur -> left;
        }
        else cur = cur -> right;
    }

    //ho verificato che non esiste una stazione con lo stessa distanza: devo creare un nuovo nodo
    tree* newStation = malloc(sizeof(tree));
    newStation -> stazione = stazione;
    u_int32_t maxKM = 0;
    for (int i = 0; i < NumCars; i++){
        if (cars[i] > maxKM) maxKM = cars[i];
        aggiungiMacchina(&newStation->macchine, cars[i]);
    }

    newStation -> maxKM = maxKM;
    newStation -> right = NULL;
    newStation -> left = NULL;
    newStation -> father = pre;

    if (pre==NULL) {
        *autostrada = newStation;
    }
    else if (pre -> stazione > stazione) pre -> left = newStation;
    else pre->right=newStation;

    printf("aggiunta\n");
}

//semplice inserimento nell'albero delle auto
void aggiungiMacchina(carTree **macchine, u_int32_t macchina){
    carTree * pre=NULL,
            *cur;

    cur=*macchine;
    while(cur!=NULL){
        pre=cur;
        if (cur -> km == macchina){
            cur -> num++;       //caso in cui ho una macchina con la stessa autonomia già presente nel BST
            return;
        }
        else if( cur -> km > macchina){
            cur=cur->left;
        }
        else cur=cur->right;
    }
    //creo il nuovo nodo perchè non ho trovato una macchina con la stessa autonomia
    carTree* newCar = malloc(sizeof(carTree));
    newCar -> km = macchina;
    newCar -> num = 1;

    newCar -> right = NULL;
    newCar -> left = NULL;

    if (pre==NULL) *macchine = newCar;
    else if (pre -> km > macchina) pre -> left = newCar;
    else if (pre -> km < macchina) pre->right=newCar;
}

/**
 * semplice rimozione ricorsiva di un elemento dall'albero delle stazioni
 * @param autostrada
 * @param stazione
 * @return
 */
boolean demolisciStazione(tree** autostrada, u_int32_t stazione){
    if (*autostrada == NULL) {
        return false;
    }
    if (stazione < (*autostrada)->stazione) {
        return demolisciStazione(&((*autostrada)->left), stazione);
    } else if (stazione > (*autostrada)->stazione) {
        return demolisciStazione(&((*autostrada)->right), stazione);
    } else {
        if (demolisciSoloMacchineStazione){
            dealloc((*autostrada)->macchine);
            (*autostrada)->macchine = NULL;
            demolisciSoloMacchineStazione = false;
        }
        if ((*autostrada)->left == NULL) {
            tree* temp = (*autostrada)->right;
            if(temp != NULL){
                temp -> father = (*autostrada)->father;
            }
            freeStation(*autostrada);
            *autostrada = temp;
        } else if ((*autostrada)->right == NULL) {
            tree* temp = (*autostrada)->left;
            temp -> father = (*autostrada)->father;
            freeStation(*autostrada);
            *autostrada = temp;
        } else {
            tree* temp = findMinValueNode((*autostrada)->right);
            (*autostrada)->stazione = temp->stazione;
            (*autostrada)->maxKM = temp->maxKM;
            (*autostrada)->macchine = temp->macchine;
            demolisciStazione(&((*autostrada)->right), temp->stazione);
        }
        return true;
    }
}

tree* findMinValueNode(tree* node) {
    tree* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

carTree* findMinValueNodeCar(carTree* node) {
    carTree* current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

carTree** cercaStazioneCar(tree** autostrada, u_int32_t stazione){
    if (*autostrada == NULL) {
        return NULL;
    }
    if (stazione == (*autostrada) -> stazione) {
        return &((*autostrada)->macchine);
    }

    if (stazione < (*autostrada)->stazione) {
        return cercaStazioneCar(&((*autostrada)->left), stazione);
    } else if (stazione > (*autostrada)->stazione) {
        return cercaStazioneCar(&((*autostrada)->right), stazione);
    }

    return NULL;
}

/**
 * ritorna il puntatore alla stazione al km passato per parametro altriment se
 * non esiste ritorna null
 * @param autostrada
 * @param stazione
 * @return
 */
tree* cercaStazione(tree** autostrada, u_int32_t stazione){
    if (*autostrada == NULL) {
        return NULL;
    }
    if (stazione == (*autostrada) -> stazione) {
        return *autostrada;
    }

    if (stazione < (*autostrada)->stazione) {
        return cercaStazione(&((*autostrada)->left), stazione);
    } else if (stazione > (*autostrada)->stazione) {
        return cercaStazione(&((*autostrada)->right), stazione);
    }

    return NULL;
}


boolean rottamaMacchina(carTree** macchine, u_int32_t macchina){
    if (*macchine == NULL) {
        printf("non rottamata\n");
        return false;
    }
    if (macchina == (*macchine)->km && (*macchine)->num > 1) {
        (*macchine)->num--;
        printf("rottamata\n");
        return false; // non devo modificare il max
    }
    if (macchina < (*macchine)->km) {
        return rottamaMacchina(&(*macchine)->left, macchina);
    } else if (macchina > (*macchine)->km) {
        return rottamaMacchina(&((*macchine)->right), macchina);
    }
    else {

        boolean changeMaxPossible = (*macchine)->right == NULL;
        if ((*macchine)->left == NULL) {
            carTree* temp = (*macchine)->right;
            free(*macchine);
            *macchine = temp;
            printf("rottamata\n");
        } else if ((*macchine)->right == NULL) {
            carTree* temp = (*macchine)->left;
            free(*macchine);
            *macchine = temp;
            printf("rottamata\n");
        } else {
            carTree* temp = findMinValueNodeCar((*macchine)->right);
            (*macchine)->km = temp->km;
            (*macchine)->num = temp->num;
            rottamaMacchina(&((*macchine)->right), temp->km);

        }
        return changeMaxPossible;
    }

}

void freeStation (tree* stazione){
    free(stazione);
}

void dealloc (carTree* macchine){
    if(macchine == NULL)return;
    if(macchine->left != NULL)dealloc(macchine->left);
    if(macchine->right != NULL)dealloc(macchine->right);
    free(macchine);
}

void pianificaPercorsoAvanti(tree** autostrada, u_int32_t partenza,u_int32_t arrivo){

    list* lista = malloc(sizeof (list)),
            *curr;
    lista -> stazione = arrivo;
    lista -> next = NULL;

    u_int32_t tappa = arrivo;
    boolean firstSearch = true;
    connesso = true;
    tree* start = cercaStazione(autostrada, partenza);
    maxReach = partenza + start ->maxKM;


    while(tappa != partenza) {

        if(start->stazione + start->maxKM >= tappa){ //se la prima stazione può raggiungere la tappa
            tappa = start->stazione;
        }
        else {
            tappa = visitaAvanti(autostrada, partenza, tappa, firstSearch);

            if (firstSearch == true && connesso == false) {
                printf("nessun percorso\n");
                free(lista);
                return;
            }
        }

        curr = malloc(sizeof (list));
        curr -> stazione = tappa;
        curr -> next = lista;
        lista = curr;
        firstSearch = false;
    }
    stampaLista(lista);
}

//returns 0 se l'autostrada non è connessa
u_int32_t visitaAvanti(tree** autostrada, u_int32_t partenza, u_int32_t tappa, boolean firstSearch){
    if(partenza == tappa){
        return partenza;
    }

    tree* start = cercaStazione(autostrada, partenza);

    tree* curr = start;
    u_int32_t tappacorrente;

    while(curr -> stazione + curr -> maxKM < tappa){
        if (curr -> right != NULL){
            tappacorrente = visitaAlbero(curr->right,tappa,firstSearch);

            if (firstSearch == true && connesso == false) return 0;
            if (tappacorrente != 0) return tappacorrente;

        }
        while (curr != curr -> father -> left){
            curr = curr -> father;
        }
        curr = curr -> father;

        if (firstSearch == true && maxReach < curr->stazione){
            connesso = false;
            return 0;
        }
        if (maxReach < curr->stazione + curr->maxKM) maxReach = curr->stazione + curr->maxKM;
    }
    return curr -> stazione;
}

//ritorno 0 se non lo raggiungo
u_int32_t visitaAlbero(tree* currRoot, u_int32_t arrivo,boolean firstSearch){
    u_int32_t tappa = 0;
    if(currRoot == NULL ){
        return tappa;
    }
    tappa = visitaAlbero(currRoot->left,arrivo,firstSearch);

    if (tappa != 0) return tappa;

    //prima controllo che la stazione corrente sia raggiungibile dalle precedenti
    if (firstSearch == true && maxReach < currRoot->stazione) {
        connesso = false;
        return 0;
    }
    //aggiorno il chilometro massimo raggiungibile utilizzando anche la stazione corrente
    if (maxReach < currRoot->stazione + currRoot->maxKM)
        maxReach = currRoot->stazione + currRoot->maxKM;

    //se non ho ancora trovato la tappa verifico se la corrente lo è
    if ((currRoot->stazione + currRoot->maxKM >= arrivo)){
        return currRoot->stazione;
    }
    tappa = visitaAlbero(currRoot->right,arrivo,firstSearch);
    return tappa;
}

void stampaLista(list *lista){

    list* prec;

    while(lista != NULL){
        prec = lista;
        printf("%u",lista-> stazione);
        if(lista -> next != NULL)  printf(" ");
        lista = lista -> next;
        free(prec);
    }
    printf("\n");
}

u_int32_t searchMax(carTree** macchine){
    if(*macchine == NULL){
        return 0;
    }
    if((*macchine)->right == NULL) return (*macchine)->km;
    return searchMax(&(*macchine)->right);
}


listaY *newTappeCorrenti = NULL;
listaY *codaTappeCorrenti;
u_int32_t lastY;

void pianificaPercorsoIndietro(tree** autostrada, u_int32_t partenza,u_int32_t arrivo){

    tree* tappaCorrente = cercaStazione(autostrada,arrivo);

    inizializzaX(tappaCorrente,partenza);

    while( tappaCorrente -> stazione < partenza ){

        ispezionaTappa(tappaCorrente,partenza);


        if(newTappeCorrenti == NULL){
            printf("nessun percorso\n");
            return;
        }

        mettiX(newTappeCorrenti->Y);

        tappaCorrente = newTappeCorrenti->Y;

        listaY *temp = newTappeCorrenti -> next;
        free(newTappeCorrenti);
        newTappeCorrenti = temp;

    }

    freeListaNewStation();

    tree* Partenza = cercaStazione(autostrada, partenza);
    stampaPercorsoIndietro(Partenza,arrivo);

}

void freeListaNewStation(){
    listaY *temp;
    while(newTappeCorrenti != NULL){
        temp = newTappeCorrenti;
        newTappeCorrenti = newTappeCorrenti -> next;
        free(temp);
    }
}


void ispezionaTappa(tree* tappaCorrente, u_int32_t partenza){

    tree* curr = tappaCorrente;


    while(curr -> stazione < partenza){
        if (curr -> right != NULL){
            visitaAlberoInd(curr->right,partenza,tappaCorrente);
        }

        if (curr -> father == NULL) return;

        while (curr != curr -> father -> left){
            curr = curr -> father;
            if (curr->father ==NULL ) return;
        }
        curr = curr -> father;

        // X significa che il nodo non è più di interesse
        // Y significa che il nodo è già stato scritto e quindi non va sovrascritto
        if( curr -> X != 'X' && curr -> X != 'Y' && curr -> stazione <= tappaCorrente -> stazione + curr -> maxKM) {

            if (newTappeCorrenti == NULL){
                newTappeCorrenti = malloc(sizeof (listaY));
                codaTappeCorrenti = newTappeCorrenti;
            }
            else {
                codaTappeCorrenti -> next = malloc(sizeof (listaY));
                codaTappeCorrenti = codaTappeCorrenti -> next;
            }

            codaTappeCorrenti -> Y = curr;
            codaTappeCorrenti -> next = NULL;

            curr -> tappaRaggiungibile = tappaCorrente;
            curr -> X = 'Y';
            lastY = curr ->stazione;
        }

    }
}

void visitaAlberoInd(tree* curr, u_int32_t partenza,tree* tappaCorrente){

    if(curr == NULL ){
        return;
    }

    visitaAlberoInd(curr->left,partenza,tappaCorrente);

    if( curr -> X != 'X' && curr -> X != 'Y' && curr -> stazione <= tappaCorrente -> stazione + curr -> maxKM) {

        if (newTappeCorrenti == NULL){
            newTappeCorrenti = malloc(sizeof (listaY));
            codaTappeCorrenti = newTappeCorrenti;
        }
        else {
            codaTappeCorrenti -> next = malloc(sizeof (listaY));
            codaTappeCorrenti = codaTappeCorrenti -> next;
        }

        codaTappeCorrenti -> Y = curr;
        codaTappeCorrenti -> next = NULL;

        curr -> tappaRaggiungibile = tappaCorrente;
        curr -> X = 'Y';
        lastY = curr ->stazione;
    }

    visitaAlberoInd(curr->right,partenza,tappaCorrente);

}


void inizializzaX(tree* tappaCorrente, u_int32_t partenza) {
    tree* curr = tappaCorrente;
    while(curr -> stazione <= partenza) {
        if (curr->right != NULL) {
            visitaAlberoInizializza(curr->right, partenza);
        }

        if (curr -> father == NULL) return;

        while (curr->father != NULL && curr != curr->father->left) {
            curr = curr->father;
        }

        if (curr -> father == NULL) return;

        curr = curr->father;
        curr->X = 'F';
    }
}

void visitaAlberoInizializza(tree* currRoot, u_int32_t partenza){

    if(currRoot == NULL){
        return;
    }

    visitaAlberoInizializza(currRoot->left,partenza);

    if (currRoot->stazione > partenza) return;

    currRoot -> X = 'F';

    visitaAlberoInizializza(currRoot->right,partenza);
}

void mettiX(tree* tappaCorrente){
    tree* curr = tappaCorrente;
    while(curr -> stazione < lastY) {
        if (curr->right != NULL) {
            visitaAlberoMettiX(curr->right);
        }


        if (curr -> father == NULL || curr -> father ->stazione >= lastY) return;

        while (curr != curr->father->left) {
            curr = curr->father;
            if(curr->father == NULL) return;
        }
        curr = curr->father;

        if (curr -> stazione >= lastY) return;

        if (curr->X == 'F') curr->X = 'X';
    }
}

void visitaAlberoMettiX(tree* currRoot){
    if(currRoot == NULL ){
        return;
    }

    visitaAlberoMettiX(currRoot->left);

    if(currRoot -> stazione >= lastY) return;

    if(currRoot -> X == 'F') currRoot -> X = 'X';

    visitaAlberoMettiX(currRoot->right);
}


void stampaPercorsoIndietro(tree* partenza,u_int32_t arrivo){

    tree* curr = partenza;

    list* testa = malloc(sizeof (list)),
        *punt;
    punt = testa;
    testa -> stazione = partenza->stazione;
    testa -> next = NULL;

    while (curr -> stazione > arrivo){

       if (curr -> X != 'Y'){ //se non è raggiungibile

           //dealloco la lista
           punt = testa;
           while(punt != NULL){
               testa = punt -> next;
               free(punt);
               punt = testa;
           }
           printf("nessun percorso\n");
           return;
       }

        punt -> next = malloc(sizeof (list));
        punt = punt -> next;
        punt -> next = NULL;
        punt -> stazione = curr -> tappaRaggiungibile -> stazione;
        curr = curr -> tappaRaggiungibile;

    }
    stampaLista(testa);
}