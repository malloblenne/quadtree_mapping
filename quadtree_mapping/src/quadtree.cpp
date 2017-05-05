/***************************************************************************
* Copyright (C) 2007 by Mauro Brenna, Ivan Reguzzoni *
* malloblenne@gmail.com *
* *
* This program is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by *
* the Free Software Foundation; either version 3 of the License, or *
* (at your option) any later version. *
* *
* This program is distributed in the hope that it will be useful, *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
* GNU General Public License for more details. *
* *
* You should have received a copy of the GNU General Public License *
* along with this program;If not, see <http://www.gnu.org/licenses/>. *
* *
***************************************************************************/
#include "quadtree.h"
using namespace std;
// --–-–-–-–-–-–-
// Costruttore
Quadtree::Quadtree(uint maxdim, uint initdim)
    : MAXDIM(maxdim),INITDIM(initdim) // inizializzazione delle const
{
    root = new Node(false,0,0,initdim,0,0); // coordinate assolute (0,0)
    int array[4][2];
    root->newCorner(array);
    uint newdim= initdim/2;
    for (int j=0; j<4; j++)
    {
        root->setChild(j,new Node(true,0,0,newdim,array[j][0],array[j][1]));
        // std::cout << Nodo : << j << cornerx: << array[j][0] << cornery: << array[j][1] <<
        std::endl;
    }
    //fine ho inizializzato 1 nodo radice + 4 nodi foglia
}
// --–-–-–-–-–-–-
//Distruttore
Quadtree:: ˜Quadtree()
{
    delete root;
}
// Visualizza Node
std::ostream &operator<< (std::ostream &output, Quadtree &q)
{
    output << "Image image( Geometry("<< q.getRoot()->getDim()<<"*mul ,"<< q.getRoot()->getDim()
           << "*mul), Color(\"white\") );"<< std::endl
           << "image.strokeColor(\"blue\");" << std::endl
           << "image.fillColor(\"green\");" << std::endl
           << "image.strokeWidth(1);" << std::endl << std::endl
           << *(q.getRoot());

    return output;
}
// Retrieve a scan
/*
INPUT
t=k*Ts where Ts=1/fs; sampling; k integer (the filename of log files)
OUTPUT
r= raggio ; angle= angolo ottenuto dal sensore x la misura
*/
bool Quadtree::getScan(uint k, std::vector<float> &r, std::vector<float> &angle)
{
    /*
1) apri il file di nome k o k.log
Del tipo:
r angle
r angle
......
2) parser salvi r e angle di ogni riga fino alla fine file pushando nel vector
3) Ottieni OUTPUT
*/
    char buffer[30];
    sprintf (buffer,"%d",k);
    string sbuff(buffer);
    string file = logfiles/ + sbuff + ".log";
    cerr << file << endl;
    ifstream* in = new ifstream(file.c str());// allocazione dinamica, dato che il nome del file si sa a run-time
    if (!in->is open())
    {
        // se il file non \‘e stato trovato
        // avverti l’utente
        cerr << Errore: file + file + non trovato << endl;
        return false;
    }
    string f; // float temp per salvare numero corrente
    float temp;
    while(*in >> f)
    {
        temp=atof(f.c str());
        r.push_back(temp/10); // aggiungila al vettore di float (raggio)
        *in >> f;
        temp=atof(f.c str());
        angle.push back(temp); // angles
        // formato file: raggio angolo /n ...
    }
    return true; // file founded and processed
}

//Scanmatching greedy tutorial Grisetti
/*
Ingresso: pose tempo t-1 x(t-1) [where t=k*Ts Ts=1/fs; frequency sampling]
Output: pose tempo t x(t)
*/
bool Quadtree::Scanmatcher1(float &xr, float &yr, float &theta,uint k, std::vector<float> &r,
                            std::vector<float> &angle)
{
    // chiedo al laser o un logfile o altro di darmi i punti di questa scansione z(t)
    //std::vector<float> r, // raggi endpoint trovati wrt robot
    // angle; // angoli delle miser wrt robot [coordinate relative robot]
    // --–- Debug --–-
    // cout << endl << Sono in ScanMatcher <Metodo 1> << endl << endl;
    // --–-–-–-–-–-
    if (!(getScan(k,r,angle))) // where t = k*Ts Ts=1/fs; frequency sampling
        return false;
    float bestxr = xr, // the best value for pose at time t
            bestyr = yr,
            besttheta = theta,
            bestmovexr = 0,
            bestmoveyr = 0,
            bestmovetheta = 0;
    // score e’ una semplice funzione da max/min-imizzare
    float bestscore = root->score distance(bestxr,bestyr,besttheta,r,angle);
    float testxr,testyr,testtheta; //test pose
    float testscore;
    // --–- PARTE RANDOM --–-
    //
    // const float MOV=2, ROT=10*3.14/180; // 2 cm 2gradi
    // const int ITER=100;
    // float testrot,testmove;
    // for (int i=0; i<ITER; i++)
    // {
    // switch (rand()%2)
    // {
    // case 0: testrot= theta + ((float)rand() / (float)(RAND MAX))*ROT; break;
    // case 1: testrot= theta - ((float)rand() / (float)(RAND MAX))*ROT; break;
    // default: break;
    // }
    // testmove=((float)rand() / (float)(RAND MAX))*MOV;
    // testxr= xr + testmove*cos(testrot);
    // testyr= yr + testmove*sin(testrot);
    // testscore=root->score distance(testxr,testyr,testrot,r,angle);
    //
    // if (bestscore < testscore)
    // {
    // bestxr=testxr;
    // bestyr=testyr;
    // besttheta=testrot;
    // bestscore=testscore;
    // }
    // }
    //
    // --–- FINE PARTE RANDOM --–-
    // Ho i dati dello scan e la posizione precedente...procedo con un algoritmo di discesa del gra-
    diente
            const int MAXITERATIONS=60;
    //float searchstep=INITIALSEARCHSTEP;
    int iterations = 0;
    int cntswitch = 0; // counter per switch (0-5)
    const float MOVE = 0.1; // minor spostamento possibile in avanti,dietro,sx,dx
    [DEFAULT = 0.1]
            const float ROTATE = 1*(3.14/180); // minor spost possibile in gradi [DE-
    FAULT = 1]
            testscore = 0;
    float maxmovescore;
    bool end = false;
    while ( (iterations < MAXITERATIONS) && (end==false) )
    {
        maxmovescore = bestscore;
        bestmovexr = bestxr;
        bestmoveyr = bestyr;
        bestmovetheta = besttheta;
        cntswitch = 0;
        for(cntswitch=0;cntswitch<6;cntswitch++)
        {
            // 0 = backward
            // 1 = forward
            // 2 = left
            // 3 = right
            // 4 = rotate left
            // 5 = rotate right
            switch(cntswitch)
            {
            case 0: { testxr = bestxr-MOVE; testyr = bestyr; testtheta = besttheta;};
                break;
            case 1: { testxr = bestxr+MOVE; testyr = bestyr; testtheta = besttheta;};
                break;
            case 2: { testxr = bestxr; testyr = bestyr-MOVE; testtheta = besttheta;};
                break;
            case 3: { testxr = bestxr; testyr = bestyr+MOVE; testtheta = besttheta;};
                break;
            case 4: { testxr = bestxr; testyr = bestyr; testtheta = besttheta-ROTATE;};
                break;
            case 5: { testxr = bestxr; testyr = bestyr; testtheta = besttheta+ROTATE;};
                break;
            default: break;
            }

            // Calcolo la score dopo avere effettuato lo spostamento
            testscore = root->score distance(testxr,testyr,testtheta,r,angle);
            // --–- DEBUG --–-
            //cout<< maxmovescore --> << maxmovescore << testscore --> << testscore << endl;
            // --–-–-–-–-–-
            // Trovata score migliore tra le 6 proposte
            if ( maxmovescore < testscore )
            {
                maxmovescore = testscore;
                bestmovexr = testxr;
                bestmoveyr = testyr;
                bestmovetheta = testtheta;
            }
        }
        // --–- DEBUG --–-
        //cout << endl << bestscore --> << bestscore << maxmovescore --> << maxmovescore << endl << --–-–- << endl;
        // --–-–-–-–-–-
        // Aggiorno la score nel caso ne trovi una migliore della precedende
        if ( bestscore < maxmovescore )
        {
            bestscore = maxmovescore;
            bestxr = bestmovexr;
            bestyr = bestmoveyr;
            besttheta = bestmovetheta;
            end = false;
            iterations++;
        }
        else
        {
            end=true; // se alla fine si aggiorna qualcosa riporto a false
        }
    }
    //return updated pose
    xr = bestxr;
    yr = bestyr;
    theta = besttheta;
    return true;
}
// Metodo utilizzato da Scanmatching che sfrutta distrib normali. Genera le pose e valuta la score
// restituendo un array ordinato per score
/*
Ingresso: array[ARRAYDIM] di pose e score, da quante pose generarne altre, quante generarne
da ogni pose (lei stessa non e’ compresa), deviazione standard delle gaussiane, l’ultimo scan ri-
cevuto.
Output: array ordinato in base allo score piu’ alto.
*/
void Quadtree::generatePoseandRank(posewithscore* array, const int ARRAYDIM, const
                                   float startingpose, const float generatefromone, const float sigmacart, const float sigmarot, con-
                                   st std::vector<float> &r, const std::vector<float> &angle )
{
    //Ipotesi: array gi\‘a ordinato in base alla score
    for(int i=0;i<(ARRAYDIM -1); i++ ) assert( array[i].score>=array[i+1].score);
    //1) Genero le pose e valuto le score
    posewithscore temp;
    int index=startingpose;
    temp.x=0;temp.y=0;temp.score=-20000;
    //inizializzo random per gaussiane
    gsl rng * ra = gsl rng alloc (gsl rng taus);
    for (int i=0; i<startingpose;i++)
    {
        for(int j=0; j<generatefromone; j++)
        {
            //Per ogni pose in i genero generatefromone pose in piu’
            temp.x=array[i].x + gsl ran gaussian( ra, sigmacart);
            temp.y=array[i].y + gsl ran gaussian( ra, sigmacart);
            temp.theta=array[i].theta + gsl ran gaussian( ra, sigmarot);
            // VALUTO LA SCORE
            temp.score=root->score distance(temp.x,temp.y,temp.theta,r,angle);
            //Inserisco nel posto giusto
            array[index].x=temp.x;
            array[index].y=temp.y;
            array[index].theta=temp.theta;
            array[index].score=temp.score;
            index++; // aggiorno la posizione dove inserire nuovi dati
        }
    }
    // Finita la generazione si ordina in base alla score
    //utilizzo shellsort, alla peggio O(nˆ2) http://en.wikipedia.org/wiki/Shell sort
    int i, j, increment;
    increment =ARRAYDIM / 2;
    while (increment > 0)
    {
        for (i=increment; i < ARRAYDIM; i++)
        {
            j = i;
            temp = array[i];
            while ((j >= increment) && (array[j-increment].score < temp.score)) // < e non >
            {
                array[j] = array[j - increment];
                j = j - increment;
            }
            array[j] = temp;
        }
        if (increment == 2)
            increment = 1;
        else
            increment = (int) (increment / 2.2);
    }
    //Fine ordine, algoritmo terminato
    //postcondizione deve essere ordinato
}
//Scanmatching che sfrutta distribuzioni normali
/*
Ingresso: pose tempo t-1 x(t-1) [where t=k*Ts Ts=1/fs; frequency sampling]
Output: pose tempo t x(t)
*/
bool Quadtree::Scanmatcher2(float &xr, float &yr, float &theta,uint k, std::vector<float> &r,
                            std::vector<float> &angle)
{
    if (!(getScan(k,r,angle))) // where t=k*Ts Ts=1/fs; frequency sampling
        return false;
    // --–- DEBUG --–-
    //cout << SEI ENTRATO NELL FUNZIONE SCANMATCHER PER TEST << endl;
    // --–-–-–-–-–-
    //pose iniziale
    float testxr=xr,
            testyr=yr,
            testtheta=theta;
    //calcolo la score
    float testscore=root->score distance(testxr,testyr,testtheta,r,angle);
    const int ARRAYDIM=100;
    posewithscore array[ARRAYDIM];
    for (int i=0; i< ARRAYDIM; i++)
    {
        //inizializzo l’array
        array[i].x=0;
        array[i].y=0;
        array[i].theta=0;
        array[i].score=-20000; // in score distance il massimo \‘e 0.
    }
    //pongo la posizione test nel primo spazio dell’array
    array[0].x=testxr;
    array[0].y=testyr;
    array[0].theta=testtheta;
    array[0].score=testscore;
    //faccio i calcoli
    const int MAXITERATIONS=3;
    const int STARTING=20;
    const int GENERATE=ARRAYDIM/STARTING -1; //es 50/10 -1 = 4 x ogni pose
    int sigmacart=2; // per movimenti ortogonali agli assi cartesiani
    int sigmarot=2; // per movimenti di rotazione.
    //inizio con generarne ARRAYDIM-1 dalla iniziale
    this->generatePoseandRank(array,ARRAYDIM, 1, ARRAYDIM-1, sigmacart*2,sigmarot*2,r,
                              angle );
    for (int divisor=1; divisor <= MAXITERATIONS; divisor++ )
    {
        this->generatePoseandRank(array,ARRAYDIM, STARTING, GENERATE,sigmacart/divisor
                                  ,sigmarot/divisor,r, angle );
    }
    // uso la migliore tra tutte
    xr=array[0].x;
    yr=array[0].y;
    theta=array[0].theta;
    // alla fine ritorno la migliore
    return true;
}
// Inserisce punto usando Bresenham (in ingresso i punti a coord cart assolute)
bool Quadtree::insertPt (const float &xr,const float &yr, const float &xpt, const float &ypt)
{
    // verifico che il punto non sia fuori dal quadrato...
    // se \‘e all’interno chiamo l’insert del nodo, altrimenti devo creare nuovi nodi...e richiamare
    questa!! funzione (potrebbe servire ancora di incrementare la dim)
            bool limit=false;
    if (outOfBorder(xr,yr,root,limit) || outOfBorder(xpt,ypt,root,limit))
    {
        //sono fuori dal quadrato
        if (limit==true) // ho sforato la dim max...nn posso fare nulla
        {
            // --–- DEBUG --–-
            // std::cout << sono qui!!!!!!!!!!! << std::endl;
            // --–-–-–-–-–-
            return false;
        }
        else // non ho sforato
        {
            // --–- DEBUG --–-
            // std::cout << sono passato per outofborder << std::endl;
            // --–-–-–-–-–-
            return this->insertPt(xr,yr,xpt,ypt); // richiamo questa stessa funzione..potrebbe riaccadere che esco ancora...servono magari 2 ingrandimenti
        }
    }
    else // caso normale
    {
        /* IDEA
1) Inserisci punto nuovo senza aggiornare visitati.(Crea i rami e i sottoalberi)
2) Aggiorno i visitati usando Bresenham (flag per visitati)
3) Riuso Bresenham e pulisco la flag dei visitati.
N.B.: Questo procedimento serve per non aggiornare pi\‘u volte il vis di un nodo.
*/
        // uso la insert del Node
        // [1]
        root->insertFinalPt(xpt,ypt);
        // --–- DEBUG --–-
        //std:: cout << Punto inserito: << xpt << <<ypt << std::endl;
        // --–-–-–-–-–-
        // [2]

        int x0,x1,y0,y1; // le variabili per bresenham sono intere
        int temp; // valore temporaneo
        /*
x0= (int)round(xr);
y0 = (int)round(yr);
x1 = (int)round(xpt);
y1 = (int)round(ypt);
*/
        // Il round d\‘a problemi ad esempio se inserisci un punto a 17.5
        x0= (int)floor(xr);
        y0 = (int)floor(yr);
        x1 = (int)floor(xpt);
        y1 = (int)floor(ypt);
        int xpttemp=(int)floor(xpt);
        int ypttemp=(int)floor(ypt);
        bool steep= (abs(y1 - y0) > abs(x1 - x0)); // pendenza
        if (steep)
        {
            // swap(x0, y0) swap(x1, y1)
            temp=x0; x0=y0; y0=temp;
            temp=x1; x1=y1; y1=temp;
        }
        if (x0 > x1)
        {
            //swap(x0, x1) swap(y0, y1)
            temp=x0; x0=x1; x1=temp;
            temp=y0; y0=y1; y1=temp;
        }
        int deltax = x1 - x0;
        int deltay = abs(y1 - y0);
        int error = -deltax / 2;
        int ystep;
        int y = y0;
        if (y0 < y1)
            ystep = 1;
        else ystep = -1;
        for (int x=x0; x<=x1; x++)
        {
            if (steep)
            {
                if (((y==xpttemp) &&( x==ypttemp)) || !( (y<=(xpttemp+1) && y>=(xpttemp-1))
                                                         && (x<=(ypttemp+1) && x>=(ypttemp-1))))
                    root->visit(y,x);
            }
            else
            {
                if (((x==xpttemp) &&( y==ypttemp)) || !( (x<=(xpttemp+1) && x>=(xpttemp-1))
                                                         && (y<=(ypttemp+1) && y>=(ypttemp-1))))
                    root->visit(x,y);}
            error = error + deltay;
            if (error > 0)
            {
                y = y + ystep;
                error = error - deltax;
            }
        }
        // [3]
        deltax = x1 - x0;
        deltay = abs(y1 - y0);
        error = -deltax / 2;
        y = y0;
        if (y0 < y1)
            ystep = 1;
        else ystep = -1;
        for (int x=x0; x<=x1; x++)
        {
            if (steep)
            {
                if (((y==xpttemp) &&( x==ypttemp)) || !( (y<=(xpttemp+1) && y>=(xpttemp-1))
                                                         && (x<=(ypttemp+1) && x>=(ypttemp-1))))
                    root->clean(y,x);
            }
            else
            {
                if (((x==xpttemp) &&( y==ypttemp)) || !( (x<=(xpttemp+1) && x>=(xpttemp-1))
                                                         && (y<=(ypttemp+1) && y>=(ypttemp-1))))
                    root->clean(x,y);
            }
            error = error + deltay;
            if (error > 0)
            {
                y = y + ystep;
                error = error - deltax;
            }
        }
        return true;
    }
}
// Node *n \‘e sempre uguale a root del quadtree
bool Quadtree::outOfBorder(const float &x,const float &y,Node *n, bool &limitReached)
{
    if ((x < n->getCornX()) || (y < n->getCornY()) || (x >= (n->getCornX() + n->getDim())
                                                       ) || (y >= (n->getCornY()+n->getDim()) ) )
    {
        // allora siamo fuori dai limiti
        //1) devo sapere dove cade il punto e avere il nuovo corner del padre
        uint dimroot= (n->getDim())*2;
        if (dimroot > MAXDIM)
        {
            limitReached=true; // non posso ingrandirmi...devo segnalarlo al quadtree
            return true; //controllo a livello sopra su limitreached
        }
        else
        {
            //caso tipico...
            // richiamo la funzione che crea la root nuova di dimensioni doppie
            fatherCorner(n,x,y);
            return true; // sono uscito dai limiti
        }
    }
    else return false; // return false...non \‘e fuori dai limiti
}
void Quadtree:: fatherCorner(Node *n,float x, float y)
{
    // childpos \‘e la posizione 0,1,2,3 nell’array del padre del *n
    // xcorn e ycorn sono i corner del padre
    // posso uscire dal quadrato in alto in basso a sx o dx.
    if (x < n->getCornX()) // punto cade a sinistra
    {
        if (y > (n->getCornY()+n->getDim())) // pt cade a sx e in alto
        {
            // il figlio va a finire in 4 cio\‘e pos 3
            createNodesandPutN(n,3 );
        }
        else // se il punto cade a sinistra e in basso o sx e basta
        {
            //lo metto in childpos=1 (posizione 2)
            createNodesandPutN(n,1 );
        }
    }
    else // cade a destra o \‘e dentro
    {
        if (y > (n->getCornY()+n->getDim())) // pt cade a dx e in alto
        {
            // il figlio va a finire in 3 cio\‘e pos 2
            createNodesandPutN(n,2 );
        }
        else // se il punto cade a dx e in basso o dx e basta
        {
            //lo metto in childpos=0
            createNodesandPutN(n,0 );
        }
    }
}
void Quadtree:: createNodesandPutN(Node *n,int childpos)
{
    uint fatherdim=(n->getDim())*2;
    Node *temproot;
    switch (childpos)
    {
    case 0: temproot= new Node(false,0,0,fatherdim,n->getCornX(),(n->getCornY()-n->getDim()));
        break;
    case 1: temproot= new Node(false,0,0,fatherdim,(n->getCornX()-n->getDim()),(n->getCornY()-
                                                                                n->getDim())); break;
    case 2: temproot= new Node(false,0,0,fatherdim,n->getCornX() ,n->getCornY()); break;
    case 3: temproot= new Node(false,0,0,fatherdim,(n->getCornX() -n->getDim()),n->getCornY());
        break;
    default : break;
    };
    //inoltre si devono creare 3 nodi nuovi e associare al posto giusto i nodi
    int array[4][2];
    temproot->newCorner( array);
    // abbiamo i nuovi corner
    Node *children[4];
    for (int j=0; j<4; j++)
    {
        if (j!= childpos)
            children[j]=new Node(true,0,0,n->getDim(),array[j][0],array[j][1]);
        else
            children[j]=n; // associo n in pos giusta
    }
    for (int j=0; j<4; j++)
    {
        temproot->setChild(j,children[j]); // li associo tutti
    }
    // adesso \‘e associato...dovrei mettere questa come root
    root=temproot;
}
