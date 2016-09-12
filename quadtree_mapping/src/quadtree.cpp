/***************************************************************************
 *   Copyright (C) 2007 by Mauro Brenna, Ivan Reguzzoni   				   *
 *   malloblenne@gmail.com   											   *
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
#include "quadtree.h"

using namespace QuadtreeLib;

// Costruttore
Quadtree::Quadtree(unsigned int maxdim, unsigned int initdim)
    : maxDim(maxdim)
{
    this->root = new Node(false,0,0,initdim,0,0); // Absolute coordinates (0,0)

    int array[4][2];

    root->getNewCorner(array);

    const unsigned int newdim = initdim / 2;

    for (int j=0; j<4; j++)
    {
        root->setChild(j,new Node(true,0,0,newdim,array[j][0],array[j][1]));
    }
    //End initilization of root and 4 leaves
}

//Distruttore
Quadtree:: ~Quadtree()
{
    delete this->root;
    this->root = 0;
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


bool Quadtree::insertPt (const float &xr,const float &yr, const float &xpt, const float &ypt)
{
    // Check that the point is inside the quadtree
    // If so then call insertion from node, else make new nodes and recall this function
    bool limit=false;
    if (outOfBorder(xr,yr,root,limit) || outOfBorder(xpt,ypt,root,limit))
    {
        //sono fuori dal quadrato
        if (limit==true) // ho sforato la dim max...nn posso fare nulla
        {
            std::cout << "sono qui!!!!!!!!!!!" << std::endl;
            return false;
        }
        else // non ho sforato
        {
            std::cout << "sono passato per outofborder" << std::endl;
            return this->insertPt(xr,yr,xpt,ypt); // richiamo questa stessa funzione..potrebbe riaccadere che esco ancora...servono magari 2 ingrandimenti
        }
    }
    else // caso normale
    {
        // uso la insert del Node
        //std::cout << "sono qui" << std::endl;
        return root->insertPt(xr,yr,xpt,ypt);
    }

} 

// Node *n è sempre uguale a root del quadtree
bool Quadtree::outOfBorder(const float &x,const float &y,Node *n, bool &limitReached)
{
    if ((x < n->getCornX()) || (y < n->getCornY()) || (x > (n->getCornX() + n->getDim()) ) || (y > (n->getCornY()+n->getDim()) ) )
    {
        // allora siamo fuori dai limiti
        //1) devo sapere dove cade il punto e avere il nuovo corner del padre
        unsigned int dimroot= (n->getDim())*2;
        if (dimroot > maxDim)
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
    else return false;  // return false...non è fuori dai limiti

}


void Quadtree:: fatherCorner(Node *n,float x, float y)
{
    // sorry la funzione è cambiata
    // childpos è la posizione 0,1,2,3 nell'array del padre del *n
    // xcorn e ycorn sono i corner del padre

    // posso uscire dal quadrato in alto in basso a sx o dx.

    if (x < n->getCornX()) // punto cade a sinistra
    {
        if (y > (n->getCornY()+n->getDim())) // pt cade a sx e in alto
        {
            // il figlio va a finire in 4 cioè pos 3
            createNodesandPutN(n,3 );
        }
        else // se il punto cade a sinistra e in basso o sx e basta
        {
            //lo metto in childpos=1 (posizione 2)
            createNodesandPutN(n,1 );
        }
    }
    else // cade a destra o è dentro
    {
        if (y > (n->getCornY()+n->getDim())) // pt cade a dx e in alto
        {
            // il figlio va a finire in 3 cioè pos 2
            createNodesandPutN(n,2 );
        }
        else // se il punto cade a dx e in basso o dx e basta
        {
            //lo metto in childpos=0
            createNodesandPutN(n,0 );
        }
    }

}

// ma in realtà x e y non li usi....si possono cancellare dalla funzione e rendere + snella
void Quadtree:: createNodesandPutN(Node *n,Direction::e childpos)
{
    unsigned int fatherdim=(n->getDim())*2;
    Node *temproot;
    switch (childpos)
    {
    case Direction::NW: temproot= new Node(false,0,0,fatherdim,n->getCornX(),(n->getCornY()-n->getDim())); break;
    case Direction::NE: temproot= new Node(false,0,0,fatherdim,(n->getCornX()-n->getDim()),(n->getCornY()-n->getDim())); break;
    case Direction::SW: temproot= new Node(false,0,0,fatherdim,n->getCornX() ,n->getCornY()); break;
    case Direction::SE: temproot= new Node(false,0,0,fatherdim,(n->getCornX() -n->getDim()),n->getCornY()); break;
    default : break;
    };
    //inoltre si devono creare 3 nodi nuovi e associare al posto giusto i nodi
    int array[4][2];
    temproot->getNewCorner( array);
    // abbiamo i nuovi corner
    Node *children[4];
    for (int j=0; j<4; j++)
    {
        if (j!= static_cast<int>(childpos))
            children[j]=new Node(true,0,0,n->getDim(),array[j][0],array[j][1]);
        else
            children[j]=n; // associo n in pos giusta
    }
    for (int j=0; j<4; j++)
    {
        temproot->setChild(j,children[j]); // li associo tutti
    }

    // adesso è associato...dovrei mettere questa come root
    root=temproot;
}
