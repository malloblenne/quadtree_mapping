#include "virtualscan.h"

#define PI 3.14159265 //credo ci sia da qualche parte


// input (xrobot,yr,angolorobot,arraydirette[m q],dimensione array) output (vector di punti)
void virtualsensorscan(float xr, float yr, float theta,float mq[][2], int dim, vector<int>& xpt,vector<int>& ypt)
{
  // ho il range del robot es da -90 a +90 (sarebbe da 0 a 180)
  const int minangle=-90; //in gradi
  const int maxangle=+90;
  const float gap=1; // es un grado tra una due raggi

  float angle=0; // angolo usato nei passi iterazione
  float m=0; // pendenza temp
  float q=0; // q temp
  float xfound,yfound, minr=100000,tempr,xmin,ymin; // temporanei
  for (float i=minangle; i<=maxangle; i+=gap)
    {
      // computo la retta robot-angolo corrispondente all'iterazione in cartesiano
      angle=theta+i; // inclinazione del robot + l'angolo del sensore
      // uso la formula y-y0=m(x-x0)
      m= tan(angle*PI/180); // angoli in rad
      q= yr-m*xr; // la q
      // fine retta

      // interseco con tutte le rette nell'array...poi confronto delle intersezioni quella con raggio min
      minr=10000; //raggio minimo
      for (int j=0; j<dim; j++)
	{
	  //interseca le due rette solita formula y=m1x+q1 y=m2x+q2 -> x=(q2-q1)/(m1-m2)
          xfound=(mq[j][1]-q)/(m-mq[j][0]);
          yfound=m*xfound+q;  //y=mx+q
          // trovato il punto in coordinate cartesiane
          // calcolo il raggio
          tempr=(sqrt( pow(xfound-xr,2) + pow(yfound-yr,2)));
          if ( tempr  < minr )  //fare nel caso un check che non sia > di un maxrange (es 10m)
	    {
              //nuovo minimo
              minr=tempr;
              xmin=xfound;
              ymin=yfound;
	    }
	}
      // ora metto il punto trovato nel vector
      xpt.push_back((int)xmin);  // al momento uso int quindi casto
      ypt.push_back((int)ymin);
      // e ri-incomincia il gioco
    } 

}
