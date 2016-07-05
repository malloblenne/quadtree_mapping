#include "virtualortoscan.h"

#define PI 3.14159265 //credo ci sia da qualche parte


void virtualortoscan (float xr, float yr, float theta, float xlow, float ylow, float xhigh, float yhigh, vector<float>& xpt, vector<float>& ypt)
{
 if ( (xr>xlow) && (xr<xhigh) && (yr>ylow) && (yr<yhigh) )
   {cout << "Il robot è nella stanza" <<endl;}
 
 // stimo l'angolo
  // ho il range del robot es da -90 a +90 (sarebbe da 0 a 180)
  const int minangle=-120; //in gradi
  const int maxangle=+120;
  const float gap=4;  //0.36; // es un grado tra una due raggi
  float angle=0,modangle=0,m,q; // angolo usato nei passi iterazione
  float tx[2],ty[2], tr[2]; // valori temporanei x,y raggio
  
  for (float i=minangle; i<=maxangle; i+=gap)
    {
      // computo la retta robot-angolo corrispondente all'iterazione in cartesiano
      angle=theta+i; // inclinazione del robot + l'angolo del sensore

      //ho l'angolo in gradi...faccio un check sulla m se pendenza pos o neg
      modangle=(int)angle%360; // mi serve x le verifiche SBAGLIATO!
      if( (modangle>=0) && (modangle<90) )
        {
         //posso toccare o parete destra (xh) o alta (yh)
         if (modangle==0)
           {tx[0]=xhigh; ty[0]=yr; tr[0]=xhigh-xr; tr[1]=tr[0]+1; //usato x escludere
           }
         else
             {
              // faccio i calcoli per yh e xh
              m= tan(angle*PI/180); // angoli in rad
              q= yr-m*xr; // la q
              // valori per yh
              ty[0]=yhigh; tx[0]=(yhigh-q)/m; tr[0]=(sqrt( pow(tx[0]-xr,2) + pow(ty[0]-yr,2)));
              //valori per xh
              tx[1]=xhigh; ty[1]=m*tx[1]+q; tr[1]=(sqrt( pow(tx[1]-xr,2) + pow(ty[1]-yr,2)));
             }
        }
     else if (  (modangle>=90) && (modangle<180) )
        {
         // posso toccare yh o xl
         if (modangle==90) {tx[0]=xr; ty[0]=yhigh; tr[0]=yhigh-yr; tr[1]=tr[0]+1; //usato x escludere
            }
         else
           {
             // faccio i calcoli per yh e xl
              m= tan(angle*PI/180); // angoli in rad
              q= yr-m*xr; // la q
              // valori per yh
              ty[0]=yhigh; tx[0]=(yhigh-q)/m; tr[0]=(sqrt( pow(tx[0]-xr,2) + pow(ty[0]-yr,2)));
              //valori per xl
              tx[1]=xlow; ty[1]=m*tx[1]+q; tr[1]=(sqrt( pow(tx[1]-xr,2) + pow(ty[1]-yr,2)));
           }

        }
     else if (  (modangle>=180) && (modangle<=270) )
        {
         // posso toccare yl o xl
         if (modangle==180) {tx[0]=xlow; ty[0]=yr; tr[0]=xr-xlow; tr[1]=tr[0]+1; //usato x escludere
            }
         else
           {
             // faccio i calcoli per yl e xl
              m= tan(angle*PI/180); // angoli in rad
              q= yr-m*xr; // la q
              // valori per yl
              ty[0]=ylow; tx[0]=(ty[0]-q)/m; tr[0]=(sqrt( pow(tx[0]-xr,2) + pow(ty[0]-yr,2)));
              //valori per xl
              tx[1]=xlow; ty[1]=m*tx[1]+q; tr[1]=(sqrt( pow(tx[1]-xr,2) + pow(ty[1]-yr,2)));
           }

        }
     else // tra 270 compreso e 360
        {
           // posso toccare yl o xh
         if (modangle==270) {tx[0]=xr; ty[0]=ylow; tr[0]=yr-ylow; tr[1]=tr[0]+1; //usato x escludere
            }
         else
           {
             // faccio i calcoli per yl e xh
              m= tan(angle*PI/180); // angoli in rad
              q= yr-m*xr; // la q
              // valori per yl
              ty[0]=ylow; tx[0]=(ty[0]-q)/m; tr[0]=(sqrt( pow(tx[0]-xr,2) + pow(ty[0]-yr,2)));
              //valori per xh
              tx[1]=xhigh; ty[1]=m*tx[1]+q; tr[1]=(sqrt( pow(tx[1]-xr,2) + pow(ty[1]-yr,2)));
           }
        }
    
    //fine possibilità
    //confronto raggio e metto nei vettori il minimo
    if (tr[0]<tr[1])
     {
      xpt.push_back(tx[0]);
      ypt.push_back(ty[0]);
     }
    else
     {
      xpt.push_back(tx[1]);
      ypt.push_back(ty[1]);
     }

    }//for
cout << "size" << ypt.size() << xpt.size() << endl;
}
