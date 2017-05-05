/***************************************************************************
* Copyright (C) 2007 by Mauro Brenna, Ivan Reguzzoni
* malloblenne@gmail.com
*
*
*
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
*
*
*
*
*
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
*
*
*
*
* You should have received a copy of the GNU General Public License
* along with this program;If not, see <http://www.gnu.org/licenses/>.
*
*
*
***************************************************************************/
#ifndef NODE H
#define NODE H
#include <iostream>
#include <vector>
#include <cmath>
class Node
{
    friend std::ostream &operator<<(std::ostream &, Node &);
public:
    Node(bool l,uint v,uint oc, uint d, int x, int y);
    Node();
    bool insertPt (const float&xr,const float &yr, const float &xpt, const float &ypt);
    bool insertFinalPt (const float &xpt, const float &ypt);
    bool visit (const float &xpt, const float &ypt);
    bool clean (const float &xpt, const float &ypt);
    uint getDim() const {return dim;}
    //int setLeaf(bool tf ) { leaf=tf; }
    bool getIfLeaf () const { return leaf;}
    int getCornX() const {return corner[0];}
    int getCornY() const {return corner[1];}
    void setChild(int i,Node *n) {children[i]=n;}
    Node* getChild(int pos) { return children[pos];}
    void newCorner(int array[4][2]);
    float occProbability() const;
    void incVis(int inc=1) {vis=vis+inc;} // incrementa vis default=1
    float score_occprob(const float &xr,const float &yr, const float &theta,const std::vector<float>
                        &r,const std::vector<float> &angle);
    float score_distance(const float &xr,const float &yr, const float &theta,const std::vector<float>
                         &r,const std::vector<float> &angle);
    float score_nearestdist(const float &xr,const float &yr, const float &theta, const std::vector<float>
                            &r, const std::vector<float> &angle);
private:
    Node* intUpdate (const float &xr, const float &yr, const float &xpt, const float &ypt);
    void getNearestCorner(const float &xpt, const float &ypt, int &cornerx,int &cornery);
    void getMaxresChildren(std::vector<int> &x,std::vector<int> &y);
    float getNearestDistance(const float &xpt, const float &ypt);
    float ptOccProbability (const float &xpt,const float &ypt );
    bool leaf; // if leaf=true else branch
    bool updated;
    Node *children[4]; // i figli del nodo NW=0 NE=1 SW=2 SE=3
    uint vis; //visited
    uint occ; //occupied
    uint dim; // dimensione del lato! (non area)
    int corner[2]; // (x,y) dell’angolo basso-sinistro del quadrato
    static const uint MAXRES=1; // 1 cm risoluzione max
};
#endif /* NODE H */
