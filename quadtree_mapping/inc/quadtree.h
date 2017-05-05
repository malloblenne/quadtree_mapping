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
#ifndef QUADTREE H
#define QUADTREE H
#include <cmath>
#include <vector>
#include "node.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
//Per le funzioni random si usa la GNU Scientific Library
#include <gsl/gsl rng.h>
#include <gsl/gsl randist.h> // per le gaussiane
//
using namespace std;
// Struct utilizzata nel metodo generatePoseandRanK
struct posewithscore
{
    float x;
    float y;
    float theta;
    float score;
};
class Quadtree
{
    friend std::ostream &operator<<(std::ostream &, Quadtree &);
public:
    Quadtree(uint maxdim, uint initdim);
    ˜Quadtree();
    //inserimento un punto alla volta (controlla anche se sfora la dim...e provvede)
    bool insertPt (const float &xr,const float &yr, const float &xpt, const float &ypt);
    bool Scanmatcher1(float &xr, float &yr, float &theta, uint k, std::vector<float> &r, std::vector<float>
                      &angle );
    bool Scanmatcher2(float &xr, float &yr, float &theta, uint k, std::vector<float> &r, std::vector<float>
                      &angle );
    bool getScan(uint k, std::vector<float> &r, std::vector<float> &angle);
private:
    Node* getRoot() {return root;}
    void createNodesandPutN(Node *n,int childpos);
    void fatherCorner(Node *n,float x, float y);
    bool outOfBorder(const float &x,const float &y, Node *n, bool &limitReached); // check se il punto o il robot sono fuori e crea quadrato + grosso
    void generatePoseandRank(posewithscore* array,const int ARRAYDIM, const float startingpose,
                             const float generatefromone,const float sigmacart,const float sigmarot, const std::vector<float>
                             &r, const std::vector<float> &angle );
    Node *root; // radice dell’albero
    const uint MAXDIM; // max dim del quadtree. Serve quando lo ingrandisce
    const uint INITDIM; // dimensione quadtree iniziale..Poi si pu\‘o espandere
};
#endif /* QUADTREE H */
