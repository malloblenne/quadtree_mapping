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

#ifdef HAVE_CONFIG_H
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

#include "virtualortoscan.h"


using namespace std;


int main(int argc, char *argv[])
{
  //cout << "Hello, world!" << endl;
  Quadtree *q;
  fstream f;
  uint maxdim;
  maxdim= 65000;//(rangeof(int)); // limite dell'int
  //cout << maxdim << endl;
  q=new Quadtree(maxdim,64); //256cm il quadratone
  //cout << *q << endl;
  bool success=false;
  string stringona;
  char buffer[20];
  //uso un quadrato
  // rette delimitanti il quadrato y=10 y=50 x=10 x=50 (nn ho pensato alle rette infinite sorry)
  //float array[4][2]={ {0, 10}, {0, 50}, {40,-390 }, {40,-190} }; //m e q
  float array[4][2]={ {-1, 30}, {+1,10 }, {+1,-10 }, {-1,50} }; //m e q
  vector<float> xscan, yscan; // due vettori
  int xr,yr,theta; // pose del robot (in interi)
  int numfile=0;
  
  //un ciclo 
  xr=20; yr=15; theta=0;
  // xr=47; yr=47; theta=0;
  virtualortoscan(xr,yr,theta,10,10,50,50, xscan,yscan);
 
 for(int i=0;i < xscan.size(); i++) cout << xscan.at(i) << "  " << yscan.at(i) << endl;

  for(int i=0;i < xscan.size(); i++)
     {
      q->insertPt(xr,yr,xscan.at(i),yscan.at(i)); 
      sprintf(buffer,"%04d",i); //ogni ciclo 1 file
      stringona= buffer;
      stringona= "log/"+stringona;
      f.open(stringona.c_str(), ios::out);
      f << *q;
      f << "image.fillColor(\"red\");" ;
      f << "image.draw( DrawableRectangle("<< xr<<"*mul,"<<yr<<"*mul,"<< xr<<"*mul+mul,"<<yr<<"*mul+mul ) );";
      f.close();
      numfile++;
     }
  // fine un ciclo

  
 //un ciclo 
  xr=47; yr=41; theta=45;
  virtualortoscan(xr,yr,theta,10,10,50,50, xscan,yscan);
  
  for(int j=numfile;j < xscan.size(); j++)
     {
      q->insertPt(xr,yr,xscan.at(j),yscan.at(j)); 
      sprintf(buffer,"%04d",j); //ogni ciclo 1 file
      stringona= buffer;
      stringona= "log/"+stringona;
      f.open(stringona.c_str(), ios::out);
      f << *q;
      f << "image.fillColor(\"red\");" ;
      f << "image.draw( DrawableRectangle("<< xr<<"*mul,"<<yr<<"*mul,"<< xr<<"*mul+mul,"<<yr<<"*mul+mul ) );";
      f.close();
      numfile++;
     }
  // fine un ciclo

  return EXIT_SUCCESS;

}
