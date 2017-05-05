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
#include "node.h"
#include <cassert>
#include <iostream>
#include <iomanip> // per decimali corretti
Node::Node(bool l,uint v,uint oc, uint d, int x, int y)
{
    leaf = l;
    vis = v;
    occ = oc;
    dim = d;
    updated = false;
    corner[0] = x;
    corner[1] = y;
    for(int n=0; n<4; n++)
    {
        children[n] = NULL;
    }
}

// --–-–-–-–-–-–-
Node:: ˜Node()
{
    for(int n=0; n<4; n++)
    {
        delete children[n];
        children[n] = NULL;
    }
}
// --–-–-–-–-–-–
// Visualizza Node
std::ostream &operator<< (std::ostream &output, Node &n)
{
    // uncomment e comment in modo opportuno. Ora \‘e abilitato x Magick++
    if ( n.getIfLeaf () ) // leaf==1
    {
        // output << ( << n.getCornX() << , << n.getCornY() << , << n.getDim()
        // << , << std::setprecision( 2 ) << std::setiosflags( std::ios::fixed | std::ios::showpoint)
        // << n.occProbability() << ’)’;
        //
        // mul=moltiplicatore x pixel (pixel/cm) [dichiarato nel cpp di magick++]
        //
        // output << image.fillColor(ColorGray(<< std::setprecision( 2 )
        // << std::setiosflags( std::ios::fixed | std::ios::showpoint ) <<(1-n.occProbability() )  <<));
        // << std::endl <<image.draw( DrawableRectangle(<<(int)n.getCornX()<<*mul,<<(int)((int)n.getCornY()+(int)n.getDim())
        // <<*mul,<<(int)((int)n.getCornX()+(int)n.getDim())<<*mul,<<(int)n.getCornY()<<*mul)); << std::endl;
        output << "image.fillColor(ColorGray("<< std::setprecision( 2 )
               << std::setiosflags( std::ios::fixed | std::ios::showpoint ) <<(1-n.occProbability() ) <<"));"
               << std::endl <<"image.draw( DrawableRectangle("<<n.getCornX()<<"*mul,"<<(n.getCornY()+n.getDim())
               <<"*mul,"<<(n.getCornX()+n.getDim())<<"*mul,"<<n.getCornY()<<"*mul )); "<< std::endl;
    }
    else
    {
        // output << [1: << *(n.getChild(0)) << 2: << *(n.getChild(1))
        // << 3: << *(n.getChild(2)) << 4: << *(n.getChild(3)) << ];
        output << *(n.getChild(0)) << std::endl << *(n.getChild(1))
               << std::endl << *(n.getChild(2)) << std::endl << *(n.getChild(3)) << std::endl;
    }
    return output;
}

// --–-–-–-–-–-–
float Node::occProbability() const
{
    assert (vis>=occ);
    // principalmente fa occ/vis
    // if (vis == 0) return 0.5; // presumo sia free...o si pu\‘o mandare 0.5 unknown
    // idea: si usa un bias...finch\‘e ho occ=0 all’aumentare di vis abbasso la prob da 0.5 seguendo una legge
    // retta passante x due punti... tra (0,0.5) e (bias,0)
    static const uint BIAS=20; // x volte di vis sono suff affinch\‘e mi convinca che sia bianco.
    if (occ==0)//ricorda caso vis=0!!
    {
        if (vis < BIAS)
        {
            // retta (0,0.5),(BIAS,0) x due punti: (y-0)/(0.5-0)= (vis-BIAS)/(0-BIAS)
            return (0.5*(BIAS-vis)/(BIAS));
        }
        else return 0;
    }
    else return ((float)occ/vis);
}

// --–-–-–-–-–-–-
// Restituisce l’occProbability di un punto dato
float Node:: ptOccProbability(const float &xpt,const float &ypt )
{
    //std::cout<<Punto considerato x ptoccprob: << xpt << << ypt << std::endl;
    if (this->leaf==true)
        if (this->occProbability()<= 0.5)
            return 0;
        else
            return this->occProbability();
    else // a branch
    {
        int cornerx,cornery;
        uint tempdim=this->getDim()/2;
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cade il punto
            cornerx=children[i]->getCornX();
            cornery=children[i]->getCornY();
            tempdim=children[i]->getDim();
            //std::cout << dom:appartiene a << cornerx << << cornery << << tempdim << std::endl;
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                       (cornery+tempdim)) )
                return children[i]->ptOccProbability(xpt,ypt);
        }
        //assert( false); // deve cadere in un punto del quadrato per forza
        //Sono fuori dal quadtree con questo punto. Probabilmente o \‘e distante dai punti nella mappa oppure i punti della mappa inseriti sono vicini al bordo e non posso comunque stabilire a che probabilit\‘a sono.
        return 0; // errore
    }
}
// --–-–-–-–-–-–-
// Score che sfrutta il metodo ptOccProbability
float Node::score_occprob(const float &xr,const float &yr, const float &theta, const std::vector<float> &r, const std::vector<float> &angle)
{
    //scorrendo l’indice i del vector fino a vector size
    // 1) computi l’endpoint. fai theta+angle ed \‘e m=tan(quello) \‘e m retta... anzi
    // usi r*cos(angle) r*sin(angle) hai la x e y in coordinate relative... sommi a quelle di xr e yr trovi coord assolute endpoint
    // 2) (scoretotale fuori inizializzata a 0)
    // scoretotale+=ptOccProbability(xendpoint,yendpoint);
    // 3) fine ciclo ritorni valore
    float tempx, tempy;
    float totscore=0 ;
    for ( int i=0; (uint)i<r.size() ; i++) // there’s a warning but i is always positive.
    {
        tempx=xr+r.at(i)*cos(theta+angle.at(i));
        tempy=yr+r.at(i)*sin(theta+angle.at(i)); // wrt absolute frame
        totscore+=ptOccProbability(tempx,tempy); // total score. Correlation function between last scan and map.
    }
    return totscore;
}
// --–-–-–-–-–-–-
void Node::getMaxresChildren(std::vector<int> &x,std::vector<int> &y)
{
    if (this->getIfLeaf ()) // controlla se \‘e nodo foglia
    {
        if (this->dim <= Node::MAXRES) // se \‘e alla massima profondit\‘a
        {
            // poi fai un check se \‘e occprobability >0.50
            if (this->occProbability() > 0.50)
            {
                x.push_back(corner[0]);
                y.push_back(corner[1]);
                //std::cout << inserito << std::endl;
            }
            return;
        }
        else
            return;
    }
    else // \‘e un ramo devo scendere
    {
        for (int n=0; n<4; n++) // controlla ogni sottoramo possibile
        {
            children[n]->getMaxresChildren(x,y);
        }
    }
}
void Node::getNearestCorner(const float &xpt, const float &ypt, int &cornerx,int &cornery)
{
    if (this->getIfLeaf ()) // controlla se \‘e nodo foglia
    {
        if (this->dim <= Node::MAXRES) // se \‘e alla massima profondit\‘a
        {
            //\‘e dove andrebbe a cadere sicuramente il punto.
            cornerx=corner[0];
            cornery=corner[1];
            return;
        }
        else //sono in foglia non a risoluzione massima
        {
            // non esiste il sottoalbero, nell’insert viene creato.
            // scelgo di ritornare il floor del punto
            cornerx=(int)floor(xpt);
            cornery=(int)floor(ypt);
            return;
        }
    }
    else // \‘e un ramo devo scendere in uno solo dei figli.
    {
        uint tempdim=0;
        int tcornerx=0,tcornery=0; //temporanei servono per l’if
        for (int i=0; i<4; i++) // controlla ogni sottoramo possibile
        {
            // verifico sempre se nel quadrato cade il punto
            tcornerx=children[i]->getCornX();
            tcornery=children[i]->getCornY();
            tempdim=children[i]->getDim();
            if ( (xpt >= tcornerx) && (xpt < (tcornerx+tempdim)) && (ypt >= tcornery) &&
                 (ypt < (tcornery+tempdim)) )
                return children[i]->getNearestCorner(xpt,ypt,cornerx,cornery);
        }
    }
    // fine
}
// Metodo per trovare la distanza minima che sfrutta il quadtree
float Node::getNearestDistance(const float &xpt, const float &ypt)
{
    //fine ricorsione
    static const float BIAS=0.5;
    if (this->getIfLeaf () && this->dim <= Node::MAXRES )
    {
        if (this->occProbability() > BIAS)
            return sqrt( pow( (xpt - this->getCornX()) ,2) + pow( (ypt - this->getCornY()),2));
    }
    else if (this->getIfLeaf ())
        return -1; // le distanze non sono mai negative. Lo uso per check
    else
    {
        //idea prendi il punto (xpt,ypt) confrontalo con i cornerx,y del nodo in cui sei.
        // fai un rank di questi nodi e visitali in sequenza.
        //int childrencorners[4][2];
        float childrendistance[4]; // 4 \‘e il numero dei figli
        float tempdistance=10000;
        float tempmin=10000;
        int tempidx=0;
        bool taken[4]={false,false,false,false};
        int childrenvisitorder[4]={0,1,2,3};
        for (int i=0; i<4; i++)
        {
            childrendistance[i]=sqrt( pow( (xpt - children[i]->getCornX()) ,2) + pow( (ypt -
                                                                                       children[i]->getCornY()),2));
        }
        //trova il migliore
        //taken[4]={false,false,false,false};
        taken[0]=false,taken[1]=false,taken[2]=false;taken[3]=false;
        int indexvisit=0;
        // Forza il figlio in cui cade il punto a esser il migliore o allunghi l’algoritmo!
        int cornerx,cornery;
        uint tempdim;
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cade il punto
            cornerx=children[i]->getCornX();
            cornery=children[i]->getCornY();
            tempdim=children[i]->getDim();
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt
                                                                                       < (cornery+tempdim)) )
            {
                //indexvisit \‘e =0
                childrenvisitorder[indexvisit]=i; // forzi a visitare prima questo
                indexvisit++;
                taken[i]=true;
                break; // e non controllare +. Ce ne pu\‘o esser solo 1.
            }
        }
        // Fine verifica caduta figlio in un nodo
        for (int j=0; j<4; j++)
        {
            //riporto il minimo in alto
            tempmin=100000;
            for (int k=0; k<4; k++)
            {
                if ( taken[k]==false && childrendistance[k] < tempmin)
                {
                    tempmin=childrendistance[k];
                    tempidx=k;
                }
            } // end for k
            taken[tempidx]=true; // lo prendo e non lo considero piu nei minimi.
            childrenvisitorder[indexvisit]=tempidx;
            indexvisit++; // non \‘e detto sia=j vedi se il figlio cade in un nodo
        }
        //visita in base al rank
        for (int i=0; i<4; i++)
        {
            tempdistance=children[childrenvisitorder[i]]->getNearestDistance(xpt,ypt);
            if ( tempdistance != -1)
                return tempdistance;
        }
        // se non trovi nulla tra questi figli.
        return -1;
    }
    return -1; // non arriva mai qui. solo con break
}
// Score che sfrutta la distanza con getNearestDistance
float Node::score_nearestdist(const float &xr,const float &yr, const float &theta, const std::vector<float>
                              &r, const std::vector<float> &angle)
{
    float totscore=0;
    float tempx,tempy;
    for ( int i=0; (uint)i< r.size(); i++) // per ogni beam dello scan
    {
        tempx=(xr+ r.at(i)*cos(theta+angle.at(i)) );
        tempy=(yr+ r.at(i)*sin(theta+angle.at(i)) ); // wrt absolute frame
        totscore=totscore - this->getNearestDistance(tempx,tempy);
    }
    return totscore;
}
// --–-–-–-–-–-–-
// Score che sfrutta la distanza
float Node::score_distance(const float &xr,const float &yr, const float &theta, const std::vector<float>
                           &r, const std::vector<float> &angle)
{
    float totscore=0;
    std::vector<int> x,y;
    // cerca la lista di figli a massima risoluzione e ritorna cornerx e cornery
    this->getMaxresChildren(x,y);
    // --–- DEBUG --–-
    // std:: cout << Finito di inserire valori nel vettore << x.size() << std::endl;
    // --–-–-–-–-–-
    // METODO GREZZO LE CONFRONTO CON TUTTE (COSTOSO)
    int distance=0, tempdistance=1000;
    int tempx,tempy; // ERRORE!!
    //int sigma;
    for ( int i=0; (uint)i<r.size(); i++ ) // per ogni beam dello scan
    {
        // trasformo in interi i punti terminali
        //tempx=(int)floor(xr+ r.at(i)*cos(theta+angle.at(i)) );
        //tempy=(int)floor(yr+ r.at(i)*sin(theta+angle.at(i)) ); // wrt absolute frame
        tempx=(xr+ r.at(i)*cos(theta+angle.at(i)) );
        tempy=(yr+ r.at(i)*sin(theta+angle.at(i)) ); // wrt absolute frame
        // cerco il cornerX,Y piu vicino.
        int cornerx=0,cornery=0;
        this->getNearestCorner(tempx,tempy,cornerx,cornery);
        // trovo la cella nella mappa di dimensioni MAXRES pi\‘u vicina a lui
        // cio\‘e quella con la distanza minore!!!
        distance=(pow((cornerx-x.at(0) ),2)+pow((cornery-y.at(0)),2));
        for(int h=1; (uint)h<x.size(); h++)
        {
            tempdistance=(pow((tempx-x.at(h) ),2)+pow((tempy-y.at(h)),2));
            if (tempdistance < distance) distance=tempdistance;
        }
        // ho trovato la distanza minore!
        // ora la sommo opportunamente nella totscore
        totscore=totscore - sqrt(distance);
    }
    return totscore;
}
// INSERISCI SOLO IL PUNTO (SENZA VISITATI)
bool Node::insertFinalPt(const float &xpt, const float &ypt)
{
    //std::cout << entrato << std::endl;
    //per ricorsione
    if (this->leaf==true && this->dim <=MAXRES)
    {
        // not vis++ just to avoud double increment with brasenham
        if (occ==0 ) //succede perch\‘e il nodo \‘e di default a occ=0,vis=1
        {
            // anche se non si fa l’if in due volte diventa nero.
            occ++;
            updated=true; // vis \‘e gi\‘a a uno
        }
        else
        {
            occ++;
            updated=false; // per sicurezza
        }
        return true;
    }
    else if ( leaf==true && dim > MAXRES )
    {
        //devo spezzare perch\‘e non sono a MAXRES
        leaf=false; // diventa un ramo
        //creo 4 nodi
        int newcorn[4][2]; // coordinate dei nuovi angoli
        newCorner(newcorn); // this->
        int newdim=dim/2; // nuove lunghezze lati
        for (int j=0; j<4; j++)
            children[j]=new Node(true,1,0,newdim,newcorn[j][0],newcorn[j][1]);
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cade il punto
            int cornerx=children[i]->getCornX();
            int cornery=children[i]->getCornY();
            uint tempdim=children[i]->getDim();
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                       (cornery+tempdim)) )
                return children[i]->insertFinalPt(xpt,ypt);
        }
    }
    else //this->leaf == false Stai visit il ramo almeno per la 2a volta (gi\‘a creato)
    {
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cade il punto
            int cornerx=children[i]->getCornX();
            int cornery=children[i]->getCornY();
            uint tempdim=children[i]->getDim();
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                       (cornery+tempdim)) )
                return children[i]->insertFinalPt(xpt,ypt);
        }
    }
    return true;
}
bool Node::visit(const float &xpt, const float &ypt)
{
    //std::cout << entrato << std::endl;
    //per ricorsione
    if (this->leaf==true && !updated)
    {
        vis++;
        updated = true;
        return true;
    }
    else if (this->leaf==true && updated)
        return true; // non aggiorno i visitati
    else
    {
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cadeil punto
            int cornerx=children[i]->getCornX();
            int cornery=children[i]->getCornY();
            uint tempdim=children[i]->getDim();
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                       (cornery+tempdim)) )
                return children[i]->visit(xpt,ypt);
        }
    }
    return true;
}
bool Node::clean(const float &xpt, const float &ypt)
{
    //std::cout << entrato << std::endl;
    //per ricorsione
    if (this->leaf==true)
    {
        updated = false;
        return true;
    }
    else
    {
        for ( int i=0; i<4; i++) // x ogni figlio
        {
            // verifico sempre se nel quadrato cade il punto
            int cornerx=children[i]->getCornX();
            int cornery=children[i]->getCornY();
            uint tempdim=children[i]->getDim();
            if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                       (cornery+tempdim)) )
                return children[i]->clean(xpt,ypt);
        }
    }
    return true;
}
bool Node::insertPt(const float &xr,const float &yr, const float &xpt, const float &ypt)
{
    //std::cout << entrato << std::endl;
    //per ricorsione
    if (this->leaf==true && this->dim <= MAXRES)
    {
        // punto trovato
        if ( occ == 0 )
        {
            vis=1; //causa creazione nodi
            occ++; // * (cambiare i valori...)
            return true;
        }
        else
        {
            vis++; //*
            occ++; //*
            return true;
        }
    }
    else if ( leaf==true && dim > MAXRES)
    {
        //devo spezzare perch\‘e non sono a MAXRES
        leaf=false; // diventa un ramo
        //creo 4 nodi
        int newcorn[4][2]; // coordinate dei nuovi angoli
        newCorner(newcorn); // this->
        int newdim=dim/2; // nuove lunghezze lati
        for (int j=0; j<4; j++)
        {
            children[j]=new Node(true,1,0,newdim,newcorn[j][0],newcorn[j][1]);
        }
        //aggiornointersezioni e scegli dove andare
        Node *prox=intUpdate(xr,yr,xpt,ypt);
        // andiamo avanti con la ricorsione sul nodo prescelto
        return prox->insertPt(xr,yr,xpt,ypt);
    }
    else //this->leaf == false Stai visit il ramo almeno per la 2a volta (gi\‘a creato)
    {
        //aggiornointersezioni e scegli dove andare
        Node *prox=intUpdate(xr,yr,xpt,ypt);
        //std::cout << *prox << std::endl;
        return prox->insertPt(xr,yr,xpt,ypt);
    }
}
// --–-–-–-–-–-–-
// Aggiorna le intersezioni e ritorna il punto per il prox inserimento in profondit\‘a
Node* Node:: intUpdate (const float &xr, const float &yr, const float &xpt, const float &ypt)
{
    //aggiorna il vis solo delle foglie
    assert (children[0]!=0 && children[1]!=0 && children[2]!=0 && children[3]!=0) ; // leaf=0
    Node *next; // prossimo nodo nella ricorsione dell’insert
    int cornerx, cornery,tempdim; // var utili x brevita
    float temp;
    bool ptfall=false, robotfall=false, touch=false; // indica se cade il punto o il robot nel quadrato
    float intersection[2]; //punto di intersezione quadrato retta R-PT
    bool mzero=false,minf=false; // pendenza retta R-PT se m=0 o m=inf
    // CREO IL RETTANGOLO ’AREA DI MINACCIA DEL SEGMENTO R-PT
    /*
Vertici:
C D
A B
*/
    float menace[4][2];
    if (xr <= xpt ) //xr \‘e a sinistra o uguale
    {
        if (yr <= ypt) // yr \‘e in basso o uguale
        {
            menace[0][0]=xr; menace[0][1]=yr;//(x,y) di A
            menace[1][0]=xpt; menace[1][1]=yr; //(x,y) di B
            menace[2][0]=xpt; menace[2][1]=ypt; //(x,y) di C
            menace[3][0]=xr; menace[3][1]=ypt; //(x,y) di D
        }
        else // xr < xpt ed retta m<0
        {
            menace[0][0]=xr; menace[0][1]=ypt;//(x,y) di A
            menace[1][0]=xpt; menace[1][1]=ypt; //(x,y) di B
            menace[2][0]=xpt; menace[2][1]=yr; //(x,y) di C
            menace[3][0]=xr; menace[3][1]=yr; //(x,y) di D
        }
    }
    else // xr \‘e a destra di xpt
    {
        if (yr <= ypt) // xr \‘e a dx e m retta < 0
        {
            menace[0][0]=xpt; menace[0][1]=yr;//(x,y) di A
            menace[1][0]=xr; menace[1][1]=yr; //(x,y) di B
            menace[2][0]=xr; menace[2][1]=ypt; //(x,y) di C
            menace[3][0]=xpt; menace[3][1]=ypt; //(x,y) di D
        }
        else // xr \‘e a dx ed m >0
        {
            menace[0][0]=xpt; menace[0][1]=ypt;//(x,y) di A
            menace[1][0]=xr; menace[1][1]=ypt; //(x,y) di B
            menace[2][0]=xr; menace[2][1]=yr; //(x,y) di C
            menace[3][0]=xpt; menace[3][1]=yr; //(x,y) di D
        }
    }
    //FINE CREAZIONE
    //sono inizializzate a false
    if (yr == ypt) mzero= true; // posso dividere per (ypt-yr)
    if (xr == xpt) minf= true; // posso dividere per (xpt -xr)
    // CONTROLLO SU PENDENZA RETTA
    // FINE CONTROLLO
    for ( int i=0; i<4; i++) // x ogni figlio
    {
        // verifico sempre se nel quadrato cade il robot e/o il punto
        // qualche var che aiuta
        ptfall=false; robotfall=false;
        cornerx=children[i]->getCornX();
        cornery=children[i]->getCornY();
        tempdim=children[i]->getDim();
        //std::cout << figlio considerato: << cornerx << , <<cornery << , << tempdim <<
        std::endl;
        // CASO CADUTA PUNTO
        if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt <
                                                                                   (cornery+tempdim)) )
        {
            // per forza se ci cade questo \‘e il next da valutare
            ptfall=true;
            next=children[i];
        }
        // CASO CADUTA ROBOT NEL QUADRATO
        if ( (xr >= cornerx) && (xr < (cornerx+tempdim)) && (yr >= cornery) && (yr < (cornery+
                                                                                      tempdim)) )
        {
            // se cade robot e nn punto questo ramo \‘e sicuramente attraversato, aggiornare vis
            robotfall=true; // prima di aggiornare serve 1 controllo (caso entrambi bool=true)
        }
        //Applico decisione del caso caduta robot
        if (robotfall==true && ptfall==false)
        {
            children[i]->incVis(1); // incremento di 1 il vis del nodo
            if ((children[i]->getIfLeaf ()==false))
            {
                Node *tempzero; //non serve neppure guardarlo
                tempzero=children[i]->intUpdate(xr,yr,xpt,ypt); // attenzione qui next=0 xk non trova il punto!!
            }
        }
        else if ( robotfall==false && ptfall==false) // pu\‘o essere attraversato dalla retta o meno
        {
            //std::cout << sto considerando << *(children[i]) << std::endl; // cancellare
            //Guardo se la retta interseca il quadrato in 0,1 punti.
            // in 0 punti. Non lo tocca -> nessun aggiornamento
            // in 1 punto. Potrebbe toccarlo
            // valuto g(x)=retta passante per (xr,yr) (xpt,ypt)
            //Voglio fare se g(x)=corner[1] (la y dell’angolo)
            // uso : ((y-yr)/(ypt-yr) = (x-xr)/(xpt-xr))
            temp=0; // il risultato potrebbe esser float..lo tronco
            // avendo scartato gli altri2casi basta che si verifichi una ipotesi. (basta che tocca1lato)
            //verifica che il quadrato sia all’interno del segmento robot-punto e tocchi il segmento
            // e non la retta generata.
            touch=false; // VEDI SE RETTA INTERSECA UN LATO DEL QUADRATO
            //se interno al segmento verifico se lo tocca o meno
            if (mzero == false ) temp=( ((((double)(cornery-yr))/(double)(ypt-yr))*(xpt-xr))+xr ) ;
            // rappr una x
            //std:: cout << temp << std::endl;
            if ( ((mzero ==false) && (temp < ((float)(cornerx+tempdim))) && (temp > ((float)cornerx)))
                 || ((minf==true) && (temp < ((float)(cornerx+tempdim))) && (temp >= ((float)cornerx)) )
                 )
            { // tocchi la retta y=cornery
                touch=true;
                intersection[0]=temp; intersection[1]=(float)cornery; // interseca in (x,y)
            }
            else
            {
                if (mzero==false) temp= ( ((double)((cornery+tempdim)-yr)/(double)(ypt-yr))*(xptxr)+
                                          xr ) ; // rappr una x
                //std:: cout << temp << std::endl;
                if ( ((mzero==false) && (temp < ((float)(cornerx+tempdim))) && (temp >
                                                                                ((float)cornerx))) || ((minf==true) && (temp < ((float)(cornerx+tempdim))) && (temp >=
                                                                                                                                                               ((float)cornerx)) ) )
                { // tocchi la retta y=corner[1]+tempdim all’interno del quadrato
                    touch=true;
                    intersection[0]=temp; intersection[1]=(float)(cornery+tempdim); // interseca in (x,y)
                }
                else
                {
                    // Voglio fare se g(corner[0])
                    if (minf == false) temp = ( ((((double)(cornerx-xr))/(double)(xpt-xr))*(ypt-yr))+yr
                                                ); //rappr una y
                    //std:: cout << temp << std::endl;
                    if ( ((minf == false) && (temp < ((float)(cornery+tempdim))) && (temp >
                                                                                     ((float)cornery))) || ((mzero==true) && (temp < ((float)(cornery+tempdim))) && (temp >=
                                                                                                                                                                     ((float)cornery)) ))
                    { // tocchi la retta x= corner[0] all’interno del quadrato considerato
                        touch=true;
                        intersection[0]=(float)cornerx; intersection[1]=temp; // interseca in (x,y)
                    }
                    else
                    {
                        if (minf ==false ) temp = ( ((((double)((cornerx+tempdim)-xr))/(double)(xptxr))*(
                                                         ypt-yr))+yr ); //rappr una y
                        //std:: cout << temp << std::endl;
                        if ( ((minf== false) && (temp < ((float)(cornery+tempdim))) && (temp >
                                                                                        ((float)cornery))) || ((mzero==true) && (temp < ((float)(cornery+tempdim))) && (temp >=
                                                                                                                                                                        ((float)cornery)) ) )
                        { // tocchi la retta x= corner[0]+tempdim all’interno del quadrato considerato
                            touch=true;
                            intersection[0]=(float)(cornerx+tempdim); intersection[1]=temp;// interseca in
                            (x,y)
                        }
                    }
                }
            }
            // FINE CHECK TOUCH
            // problema nel caso non sia foglia.... facciamo un check...bisogna chiamare  ricorsivamente l’update
            if ((children[i]->getIfLeaf ()==false) && (touch==true))
            {
                Node *tempzero; // in teoria forse non serve neppure guardarlo
                tempzero=children[i]->intUpdate(xr,yr,xpt,ypt); // attenzione qui next=0 xk non trova il punto!!
            }
            //Gestione casi minf o mzero == true
            if ( (mzero==true) || (minf==true) )
            {
                //un caso alla volta
                // se true entrambi sono lo stesso punto...non faccio nulla
                if ((mzero == true) && (minf == false))
                {
                    //assert(touch==false); // in teoria
                    touch=false; //lo gestisco qui
                    // devi vedere se xr<xpt o viceversa...
                    float xtemp[2]; //uso una var temporanea x togliere casi
                    if (xr<xpt) {xtemp[0]=xr; xtemp[1]=xpt;} else {xtemp[0]=xpt; xtemp[1]=xr;}
                    // verifico la coordinata x di corner se sta tra i due (c’ 1dim di mezzo)
                    if ((cornerx > (xtemp[0]-tempdim) )&& ( cornerx < xtemp[1]) )
                    {
                        //check sulla y...se c’ intersezione (yr=ypt) Se compreso:
                        if ( (cornery <= yr) && ((cornery+tempdim) > yr)) children[i]->incVis(1);
                    }
                }
                else if ( (minf==true) && (mzero== false))
                {
                    // Caso simmetrico
                    //assert (touch==false);// in teoria (assert?)
                    touch=false; //lo gestisco qui
                    float ytemp[2]; // var temp come sopra (ytemp[0]=basso)
                    if (yr<ypt) {ytemp[0]=yr; ytemp[1]=ypt;} else {ytemp[0]=ypt; ytemp[1]=yr;}
                    // Controllo se la y nel range
                    if (((cornery +tempdim )> (ytemp[0]) )&& ( cornery < ytemp[1]) ) // dovrebbe prendere anche i casi strani.
                    {
                        //check sulla x...se c’ intersezione (xr=xpt) Se compreso:
                        if ( (cornerx <= xr) && ((cornerx+tempdim) > xr)) children[i]->incVis(1);
                    }
                }
            }
            // c’ touch che toglie disguinge da sottoinsieme caso minf e mzero
            // se toccato verifico che il punto intersezione sta nell’area minacciata
            if ( (children[i]->getIfLeaf ()==true) && (touch==true) && (intersection[0] > ((float)menace[0][0]))
                 && (intersection[0] < ((float)menace[1][0])))
            { // tra xA e xB
                if ( (intersection[1] > ((float)menace[1][1])) && (intersection[1] < ((float)menace[2][1]))
                     ) // tra yB e yC
                {
                    // effetivamente allora soddisfa entrambe le ipotesi quindi lo aggiorno a bianco
                    children[i]->incVis(1);
                }
            }
        }
    }
    return next; // ritorno il next
}
// --–-–-–-–-–-–-
// Da this->dim e x,y (corner) restituisce i nuovi corner dei 4 lati dei 4 subsquare
void Node:: newCorner(int array[4][2])
{
    // ricordare il -1 (da 0 a 64 -> dim32 -> coordinata a 31)
    /* DISPOSIZIONE
| | |
| 1 | 2 |
| | |
| | |
| | |
| 3 | 4 |
| | |
| | |
*/
    // array[4][2] contiene coppie di corner[0] e [1]
    int newdim=dim/2; //la nuova dimensione sar\‘a la met\‘a della precedente
    array[0][0]=corner[0]; //corner[0] (x) di 1
    array[0][1]=corner[1]+newdim; //corner[1] (y) di 1
    array[1][0]=corner[0]+newdim; //corner[0] (x) di 2
    array[1][1]=corner[1]+newdim; //corner[1] (y) di 2
    array[2][0]=corner[0]; //corner[0] (x) di 3
    array[2][1]=corner[1]; //corner[1] (y) di 3
    array[3][0]=corner[0]+newdim; //corner[0] (x) di 4
    array[3][1]=corner[1]; //corner[1] (y) di 4
}
