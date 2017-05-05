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
#ifdef HAVE CONFIG H
#include <config.h>
#endif
#include <iostream>
#include <cstdlib>
#include "quadtree.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
using namespace std;
int main(int argc, char *argv[])
{
    //cout << Hello, world! << endl;
    Quadtree *q;
    fstream f;
    uint maxdim;
    maxdim= 65000;//(rangeof(int)); // limite dell’int
    //cout << maxdim << endl;
    q=new Quadtree(maxdim,512); //256cm il quadratone
    //cout << *q << endl;
    float xr=251,yr=251,theta=0,tempx,tempy;
    //nel quadrato simulato sono xr=47; yr=47; theta=0;
    //float xr=47,yr=47,theta=0,tempx,tempy;
    uint cnt=1;
    vector<float> r, angle;
    // il primo scan
    for(cnt=0; cnt<4;cnt++)
    {
        //primi 5 scan sono nella stessa posizione...non fare scanmatching(ROBUSTEZZA)
        q->getScan(cnt,r,angle);
        for (int k=0; (uint)k<r.size(); k++)
        {
            tempx=xr+r.at(k)*cos(theta+angle.at(k));
            tempy=yr+r.at(k)*sin(theta+angle.at(k)); // wrt absolute frame
            q->insertPt(xr,yr,tempx,tempy);
        }
    }
    // Limite di 65k file !
    int i = cnt;
    while ( i ) //loop
    {
        r.clear();angle.clear();
        if (!q->Scanmatcher2(xr,yr,theta,i,r,angle)) break;
        // No ScanMatcher
        //if(!q->getScan(i,r,angle))
        // break;
        // --–- DEBUG --–-
        // cout << xr << << yr << <<theta << endl<<endl<<endl;
        // cout << r.size();
        // --–-–-–-–-–-
        for (int k=0; (uint)k<r.size(); k++)
        {
            tempx=xr+r.at(k)*cos(theta+angle.at(k));
            tempy=yr+r.at(k)*sin(theta+angle.at(k)); // wrt absolute frame
            q->insertPt(xr,yr,tempx,tempy);
        }
        i++;
    }
    // Disegna robot
    char buffer[30];
    sprintf(buffer,"%s",argv[1]); //ogni ciclo 1 file
    string stringona= buffer;
    stringona= log/+stringona;
    f.open(stringona.c_str(), ios::out);
    f << *q;
    f << image.fillColor(\red\); ;
    f << image.draw( DrawableRectangle((<< xr << -x_o)*mul,( << yr << -y_o)*mul,( << xr << -x_o)*mul+mul,( << yr << -y_o)*mul+mul ) );;
    f.close();
    // delete q;
    return EXIT SUCCESS;
}
