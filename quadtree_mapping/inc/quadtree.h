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
#ifndef _QUADTREE_H
#define _QUADTREE_H

#include "node.h"

class Quadtree
{

friend std::ostream &operator<<(std::ostream &, Quadtree &);

public:
Quadtree(uint maxdim, uint initdim);
~Quadtree();
//inserimento un punto alla volta (controlla anche se sfora la dim..e provvede)
bool insertPt (const float &xr,const float &yr, const float &xpt, const float &ypt); 
// da implementare un inserimento di scansione o da file
// insert(file)
Node* getRoot() {return root;}
private:

void createNodesandPutN(Node *n,int childpos);
void fatherCorner(Node *n,float x, float y); // il nome nn c'entra ormai nulla(leggi il chiamante)
bool outOfBorder(const float &x,const float &y, Node *n, bool &limitReached); // check se il punto o il robot sono fuori e crea quadrato + grosso
Node *root; // radice dell'albero
const uint maxDim; // max dim del quadtree. Serve qnd lo ingrandisco
const uint initDim; // dimensione quadtree iniziale..Poi si può espandere
 
};

#endif /* _QUADTREE_H */