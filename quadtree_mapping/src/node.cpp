/***************************************************************************
 *   Copyright (C) 2007 by Mauro Brenna, Ivan Reguzzoni   *
 *   malloblenne@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "node.h"
#include <cassert>
#include <iostream> //
#include <iomanip> // per decimali corretti

using namespace QuadtreeLib;

Node::Node(bool l,uint v,uint oc, uint d, int x, int y):
    leaf(l),
    vis(v),
    occ(oc),
    dim(d)
{
    for (int i=0; i<4; i++)
    {
        this->children[i] = 0;
    }

    this->corner[Axis::X] = x;
    this->corner[Axis::Y] = y;

}

bool Node::isLeaf() const
{
    // Or all children are 0 or all are valid pointer
    return this->children[0] != 0;
}


Node:: ~Node()
{
    for (int i=0; i<4; i++)
    {
        if (this->children[i])
        {
            delete this->children[i];
            this->children[i] = 0;
        }
    }
}

// Visualizza Node
std::ostream &operator<< (std::ostream &output, Node &n)
{
    // uncomment e comment in modo opportuno. Ora è abilitato x Magick++
    if ( n.isLeaf() ) // leaf==1
    {
        /* output << "( " << n.getCornX() << ", " << n.getCornY() << ", " << n.getDim()
          << ", " << std::setprecision( 2 ) << std::setiosflags( std::ios::fixed | std::ios::showpoint )
          << n.occProbability() << ')';
        */
        // mul=moltiplicatore x pixel (pixel/cm) [dichiarato nel cpp di magick++]
        output << "image.fillColor(ColorGray("<< std::setprecision( 2 )
               << std::setiosflags( std::ios::fixed | std::ios::showpoint ) <<(1-n.occProbability() ) <<"));"
               << std::endl <<"image.draw( DrawableRectangle("<<n.getCornX()<<"*mul,"<<(n.getCornY()+n.getDim())
               <<"*mul,"<<(n.getCornX()+n.getDim())<<"*mul,"<<n.getCornY()<<"*mul )); "<< std::endl;
    }

    else
    {
       //  output << "[1: " << *(n.getChild(0)) << " 2:" << *(n.getChild(1))
       //    << " 3:" << *(n.getChild(2)) << " 4:" << *(n.getChild(3)) << "]"; *

       //output << *(n.getChild(Direction::NW)) << std::endl << *(n.getChild(Direction::NE))
       //        << std::endl << *(n.getChild(Direction::SW)) << std::endl << *(n.getChild(Direction::SE)) << std::endl;
       output << " Test " << std::endl;
    }

    return output;
}

float Node::occProbability() const
{
    assert (vis>=occ);
    //principalmente fa occ/vis
    //if (vis == 0) return 0.5; // presumo sia free...o si può mandare 0.5 unknown ;)
    // idea: si usa un bias...finchè ho occ=0 all'aumentare di vis abbasso la prob da 0.5 seguendo una legge
    // funziona meglio con gaussiana a mio parere ma implemento x comodità la retta (poi usa gsl)

    // retta passante x due punti... tra (0,0.5) e (bias,0)
    static const int bias=20; // x volte di vis sono suff affinchè sono convinto sia bianco.
    if (occ==0)//ricorda caso vis=0!!
    {
        if (vis < bias)
        {
            // retta (0,0.5),(bias,0) x due punti: (y-0)/(0.5-0)= (vis-bias)/(0-bias)
            return (0.5*(bias-vis)/(bias));
        }
        else return 0;
    }
    else return ((float)occ/vis);

}


bool Node::insertPt(const float &xr,const float &yr, const float &xpt, const float &ypt)
{
    //std::cout << " entrato" << std::endl;
    //per ricorsione
    if (this->leaf==false && this->dim <= 2*maxRes)
    {
        for (int i=0; i<4; i++)
        {
            children[i]->incOcc(1);
            children[i]->incVis(1);
        }
    }
    else if ( leaf==true && dim > maxRes )
    {
        if (dim <= maxRes*2)
        {
            // VOGLIO RADDOPPIARE LO SPESSORE DEL NERO
            leaf=false; // diventa un ramo
            //creo 4 nodi
            int newcorn[4][2]; // coordinate dei nuovi angoli
            getNewCorner(newcorn); // this->
            int newdim=dim/2; // nuove lunghezze lati
            for (int j=0; j<4; j++)
            {
                children[j]=new Node(true,1,1,newdim,newcorn[j][0],newcorn[j][1]);
            }
        }
        else
        {
            //devo spezzare perchè non sono a maxRes
            leaf=false; // diventa un ramo
            //creo 4 nodi
            int newcorn[4][2]; // coordinate dei nuovi angoli
            getNewCorner(newcorn); // this->
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
    }
    else //this->leaf == false Stai visit il ramo almeno per la 2a volta (già creato)
    {
        //aggiornointersezioni e scegli dove andare
        Node *prox=intUpdate(xr,yr,xpt,ypt);
        //std::cout << *prox << std::endl;
        return prox->insertPt(xr,yr,xpt,ypt);
    }

}





//Aggiorna le intersezioni e ritorna il punto per il prox inserimento in profondità
// funzione si spera giusta
Node* Node:: intUpdate (const float &xr, const float &yr, const float &xpt, const float &ypt)
{
    //aggiorna il vis solo delle foglie
    assert (children[0]!=0 && children[1]!=0 && children[2]!=0 && children[3]!=0) ; // leaf=0

    Node *next; // prossimo nodo nella ricorsione dell'insert
    int cornerx, cornery,tempdim; // var utili x brevita
    float temp;
    bool ptfall=false, robotfall=false, touch=false; // indica se cade il punto o il robot nel quadrato
    float intersection[2]; //punto di intersezione quadrato retta R-PT
    bool mzero=false,minf=false; // pendenza retta R-PT se m=0 o m=inf
    // CREO IL RETTANGOLO 'AREA DI MINACCIA DEL SEGMENTO R-PT
    /*
Vertici:
C D
A B
*/
    float menace[4][2];

    if (xr <= xpt ) //xr è a sinistra o uguale
    {
        if (yr <= ypt) // yr è in basso o uguale
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
    else // xr è a destra di xpt
    {
        if (yr <= ypt) // xr è a dx e m retta < 0
        {
            menace[0][0]=xpt; menace[0][1]=yr;//(x,y) di A
            menace[1][0]=xr; menace[1][1]=yr; //(x,y) di B
            menace[2][0]=xr; menace[2][1]=ypt; //(x,y) di C
            menace[3][0]=xpt; menace[3][1]=ypt; //(x,y) di D
        }
        else  // xr è a dx ed m >0
        {
            menace[0][0]=xpt; menace[0][1]=ypt;//(x,y) di A
            menace[1][0]=xr; menace[1][1]=ypt; //(x,y) di B
            menace[2][0]=xr; menace[2][1]=yr; //(x,y) di C
            menace[3][0]=xpt; menace[3][1]=yr; //(x,y) di D
        }
    }

    //FINE CREAZIONE
    //sono inizializzate a false
    //FORSE È MEGLIO SE SI GUARDANO I TRONCAMENTI A INT PER SAPERE SE m=0...ALTRIMENTI 1.00001 1.0002 SN !=
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
        //std::cout << "figlio considerato:" << cornerx << ", "<<cornery << ", " << tempdim << std::endl;

        // CASO CADUTA PUNTO
        if ( (xpt >= cornerx) && (xpt < (cornerx+tempdim)) && (ypt >= cornery) && (ypt < (cornery+tempdim)) )
        {
            // per forza se ci cade questo è il next da valutare
            ptfall=true;
            next=children[i];
        }

        // CASO CADUTA ROBOT NEL QUADRATO
        if ( (xr >= cornerx) && (xr < (cornerx+tempdim)) && (yr >= cornery) && (yr < (cornery+tempdim)) )
        {
            // se cade robot e nn punto questo ramo è sicuramente attraversato, aggiornare vis
            robotfall=true; // prima di aggiornare serve 1 controllo (caso entrambi bool=true)

        }

        //Applico decisione del caso caduta robot
        if (robotfall==true && ptfall==false)
        {
            children[i]->incVis(1); // incremento di 1 il vis del nodo
            if ((children[i]->isLeaf()==false))
            {
                Node *tempzero; // in teoria forse non serve neppure guardarlo
                tempzero=children[i]->intUpdate(xr,yr,xpt,ypt); // attenzione qui next=0 xk non trova il punto!!
            }
        }
        else if ( robotfall==false && ptfall==false) // può essere attraversato dalla retta o meno
        {
            //std::cout << "sto considerando  " << *(children[i]) << std::endl; // cancellare
            //Guardo se la retta interseca il quadrato in 0,1 punti.
            // in 0 punti. Non lo tocca -> nessun aggiornamento
            // in 1 punto. Potrebbe toccarlo
            // valuto g(x)=retta passante per (xr,yr) (xpt,ypt)
            //Voglio fare se g(x)=corner[1] (la y dell'angolo)
            // uso : ((y-yr)/(ypt-yr) = (x-xr)/(xpt-xr))
            temp=0; // il risultato potrebbe esser float..lo tronco

            // avendo scartato gli altri2casi basta che si verifichi una ipotesi. (basta che tocca1lato)

            //verifica che il quadrato sia all'interno del segmento robot-punto e tocchi il segmento
            // e non la retta generata.
            touch=false; // VEDI SE RETTA INTERSECA UN LATO DEL QUADRATO

            //se interno al segmento verifico se lo tocca o meno
            if (mzero == false ) temp=( ((((double)(cornery-yr))/(double)(ypt-yr))*(xpt-xr))+xr ) ; // rappr una x
            //std:: cout << temp << std::endl;
            if ( ((mzero ==false) && (temp < ((float)(cornerx+tempdim)))  && (temp > ((float)cornerx)))  ||  ((minf==true) && (temp < ((float)(cornerx+tempdim)))  && (temp >= ((float)cornerx)) ) )
            { // tocchi la retta y=cornery
                touch=true;
                intersection[0]=temp; intersection[1]=(float)cornery;  // interseca in (x,y)
            }
            else
            {
                if (mzero==false) temp= ( ((double)((cornery+tempdim)-yr)/(double)(ypt-yr))*(xpt-xr)+xr ) ; // rappr una x
                //std:: cout << temp << std::endl;
                if ( ((mzero==false) && (temp < ((float)(cornerx+tempdim)))  && (temp > ((float)cornerx)))  ||  ((minf==true) && (temp < ((float)(cornerx+tempdim)))  && (temp >= ((float)cornerx)) ) )
                { // tocchi la retta y=corner[1]+tempdim all'interno del quadrato
                    touch=true;
                    intersection[0]=temp; intersection[1]=(float)(cornery+tempdim);  // interseca in (x,y)
                }
                else
                {
                    // Voglio fare se g(corner[0])
                    if (minf == false) temp = ( ((((double)(cornerx-xr))/(double)(xpt-xr))*(ypt-yr))+yr ); //rappr una y
                    //std:: cout << temp << std::endl;
                    if ( ((minf == false) && (temp < ((float)(cornery+tempdim))) && (temp > ((float)cornery)))  ||  ((mzero==true) && (temp < ((float)(cornery+tempdim))) && (temp >= ((float)cornery)) ))
                    {  // tocchi la retta x= corner[0] all'interno del quadrato considerato
                        touch=true;
                        intersection[0]=(float)cornerx; intersection[1]=temp;  // interseca in (x,y)
                    }
                    else
                    {
                        if (minf ==false ) temp = ( ((((double)((cornerx+tempdim)-xr))/(double)(xpt-xr))*(ypt-yr))+yr ); //rappr una y
                        //std:: cout << temp << std::endl;
                        if ( ((minf== false) && (temp < ((float)(cornery+tempdim))) && (temp > ((float)cornery))) ||  ((mzero==true) && (temp < ((float)(cornery+tempdim))) && (temp >= ((float)cornery)) ) )
                        {  // tocchi la retta x= corner[0]+tempdim all'interno del quadrato considerato
                            touch=true;
                            intersection[0]=(float)(cornerx+tempdim); intersection[1]=temp;// interseca in (x,y)
                        }
                    }
                }
            }
            // FINE CHECK TOUCH

            // problema nel caso non sia foglia.... facciamo un check...bisogna chiamare ricorsivamente l'update
            if ((children[i]->isLeaf()==false) && (touch==true))
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
                    // verifico la coordinata x di corner se sta tra i due (c' 1dim di mezzo)
                    if ((cornerx > (xtemp[0]-tempdim) )&& ( cornerx < xtemp[1]) )
                    {

                        //check sulla y...se c' intersezione (yr=ypt) Se  compreso:
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
                    // Controllo se la y  nel range
                    if (((cornery +tempdim )> (ytemp[0]) )&& ( cornery  < ytemp[1]) ) // dovrebbe prendere anche i casi strani.
                    {
                        //check sulla x...se c' intersezione (xr=xpt) Se  compreso:
                        if ( (cornerx <= xr) && ((cornerx+tempdim) > xr)) children[i]->incVis(1);
                    }

                }
            }

            // c' touch che toglie disguinge da sottoinsieme caso minf e mzero
            // se toccato verifico che il punto intersezione sta nell'area minacciata
            if ( (children[i]->isLeaf()==true) && (touch==true) && (intersection[0] > ((float)menace[0][0])) && (intersection[0] < ((float)menace[1][0])))
            { // tra xA e xB
                if ( (intersection[1] > ((float)menace[1][1])) && (intersection[1] < ((float)menace[2][1])) ) // tra yB e yC
                {
                    // effetivamente allora soddisfa entrambe le ipotesi quindi lo aggiorno a bianco
                    children[i]->incVis(1);
                }
            }
        }

    }

    return next; // ritorno il next

}



// Da this->dim e x,y (corner) restituisce i nuovi corner dei 4 lati dei 4 subsquare
void Node::getNewCorner(int array[4][2])
{

    const int newdim = dim / 2; //this

    array[Direction::NW][Axis::X] = corner[Axis::X];
    array[Direction::NW][Axis::Y] = corner[Axis::Y] + newdim;

    array[Direction::NE][Axis::X] = corner[Axis::X] + newdim;
    array[Direction::NE][Axis::Y] = corner[Axis::Y] + newdim;

    array[Direction::SW][Axis::X] = corner[Axis::X];
    array[Direction::SW][Axis::Y] = corner[Axis::Y];

    array[Direction::SE][Axis::X] = corner[Axis::X] + newdim;
    array[Direction::SE][Axis::Y] = corner[Axis::Y];
}


uint Node::getDim() const
{
    return this->dim;
}


int Node::getCornX() const
{
    return this->corner[Axis::X];
}

int Node::getCornY() const
{
    return this->corner[Axis::Y];
}

void Node::setChild(Direction::e direction, Node *n)
{
    this->children[direction] = n;
}

Node* Node::getChild(Direction::e direction) const
{
    return this->children[direction];
}


void Node::incVis(int inc)
{
    this->vis += inc;
}

void Node::incOcc(int inc)
{
    this->occ += inc;
}
