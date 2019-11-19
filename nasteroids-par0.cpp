#include <iostream>
#include <cstring>
#include <random>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <omp.h>
#include <chrono>
#include "nasteroids-seq.h"
#define NT 4
using namespace std;
using namespace std::chrono;
void calcFuerza(int i,asteroide *asteroides,planeta *planetas,fuerza **fuerzaAsteroideAsteroide,fuerza **fuerzaAsteroidePlaneta,int num_asteroides,int num_planetas);
 void calcNewPosVel(int i,asteroide *asteroides,fuerza  **fuerzaAsteroideAsteroide,fuerza **fuerzaAsteroidePlaneta,int num_asteroides,int num_planetas);
int main(int argc, char **argv){
  /*using clk = chrono::high_resolution_clock;
  auto t1 = clk::now();*/

  double t1=omp_get_wtime();


    if(comprobarParametros(argc, argv) == -1){
        cout << "nasteroids-seq: Wrong arguments.\nCorrect use:\nnasteroids-seq num_asteroides num_iteraciones num_planetas semilla\n";
    }
    const int num_asteroides = atoi(argv[1]);
    const int num_iteraciones = atoi(argv[2]);
    const int num_planetas = atoi(argv[3]);
    int semilla = atoi(argv[4]);
    planeta *planetas = new planeta[num_planetas];
    asteroide *asteroides = new asteroide[num_asteroides];

    bigBang(num_planetas,planetas,num_asteroides,asteroides,semilla, num_iteraciones);
    movimiento(asteroides, num_asteroides, planetas, num_planetas, num_iteraciones);
    salida(asteroides, num_asteroides);
    delete [] asteroides;
    delete [] planetas;




  double t2=omp_get_wtime();

  double diff= t2-t1;


  cout << "Tiempo de ejecución: " << setprecision(9) << diff << "seconds" << '\n';


}
int comprobarParametros(int argc, char **argv){

  int comparacion = strlen(argv[0]);
  int j = 0;
  if(argc != 5){
    return -1;
  }
  else{
    for(int i = 1; i < argc; i++){
      if (*argv[i] < 0 || *argv[4] == 0) {
        return -1;
      }
      comparacion = strlen(argv[i]);
      while (j < comparacion){
        if(isdigit(argv[i][j]) == 0){
          return -1;
        }
        j++;
      }
    }
  }
  return 1;
}
  void bigBang(int num_planetas, planeta *planetas, int num_asteroides, asteroide *asteroides, int semilla, int num_iteraciones){
  default_random_engine re{semilla};
  uniform_real_distribution<double> xdist{0.0,nextafter(GBL_WIDTH,numeric_limits<double>::max())};
  uniform_real_distribution<double> ydist{0.0,nextafter(GBL_HEIGHT,numeric_limits<double>::max())};
  normal_distribution<double> mdist{GBL_MASA, GBL_SDM};
  ofstream fs("init_conf1.txt");
  fs << num_asteroides << " " << num_iteraciones << " " << num_planetas << " " << semilla << "\n";
  fs << fixed;


  for (int i = 0; i < num_asteroides;i++){
    //std::cout << "Iteracion: " << i <<  "Hilo: "<<  omp_get_thread_num()<<'\n';
    asteroides[i].posX = xdist(re);
    asteroides[i].posY = ydist(re);
    asteroides[i].masa = mdist(re);
    asteroides[i].velX = 0.0;
    asteroides[i].velY = 0.0;
    fs << setprecision(3) << asteroides[i].posX << " " << setprecision(3) << asteroides[i].posY << " "<< setprecision(3) << asteroides[i].masa << " "<< "\n";
  }

  int modulo = 0;
  for (int i = 0; i < num_planetas;i++){
    modulo = i%4;
    if (modulo == 0){
      planetas[i].posX = 0.0;
      planetas[i].posY = ydist(re);
      planetas[i].masa = mdist(re)*10;
      fs << setprecision(3) << planetas[i].posX << " "<< setprecision(3) << planetas[i].posY << " " << setprecision(3) << planetas[i].masa << "\n";
    }
    else if (modulo == 1){
      planetas[i].posX = xdist(re);
      planetas[i].posY = 0.0;
      planetas[i].masa = mdist(re)*10;
      fs << setprecision(3) << planetas[i].posX << " "<< setprecision(3) << planetas[i].posY << " " << setprecision(3) << planetas[i].masa << "\n";
    }
    else if (modulo == 2){
      planetas[i].posX = 200.0;
      planetas[i].posY = ydist(re);
      planetas[i].masa = mdist(re)*10;
      fs << setprecision(3) << planetas[i].posX << " "<< setprecision(3) << planetas[i].posY << " " << setprecision(3) << planetas[i].masa << "\n";
    }
    else {
      planetas[i].posX = xdist(re);
      planetas[i].posY = 200.0;
      planetas[i].masa = mdist(re)*10;
      fs << setprecision(3) << planetas[i].posX << " "<< setprecision(3) << planetas[i].posY << " " << setprecision(3) << planetas[i].masa << "\n";
    }
  }
}

 void movimiento(asteroide *asteroides, int num_asteroides, planeta *planetas, int num_planetas, int iteraciones){




  int contador_iteraciones = 0;
  /*fuerza fuerzaAsteroideAsteroide[num_asteroides][num_asteroides];
  fuerza fuerzaAsteroidePlaneta[num_asteroides][num_planetas];*/
  fuerza **fuerzaAsteroideAsteroide;
  fuerzaAsteroideAsteroide = new fuerza*[num_asteroides];
  fuerza **fuerzaAsteroidePlaneta;
  fuerzaAsteroidePlaneta = new fuerza*[num_asteroides];


      for(int i=0; i<num_asteroides;i++) {
        fuerzaAsteroideAsteroide[i] = new fuerza[num_asteroides];
      }


      for(int i=0; i<num_asteroides;i++) {
        fuerzaAsteroidePlaneta[i] = new fuerza[num_planetas];
      }



  for(contador_iteraciones= 0; contador_iteraciones < iteraciones; contador_iteraciones++){
    #pragma omp parallel
    #pragma omp for schedule(dynamic)
    for(int i = 0; i < num_asteroides; i++){

      calcFuerza(i,asteroides,planetas,fuerzaAsteroideAsteroide,fuerzaAsteroidePlaneta,num_asteroides,num_planetas);

    }

    #pragma omp for schedule(dynamic)
    for (int i = 0; i < num_asteroides; i++){

        calcNewPosVel(i,asteroides,fuerzaAsteroideAsteroide,fuerzaAsteroidePlaneta,num_asteroides,num_planetas);
     }
     //contador_iteraciones++;
  }
  for (int i = 0; i < num_asteroides; i++) {
        delete [] fuerzaAsteroideAsteroide[i];
    }
  delete [] fuerzaAsteroideAsteroide;

  for (int i = 0; i < num_asteroides; i++) {
        delete [] fuerzaAsteroidePlaneta[i];
    }
  delete [] fuerzaAsteroidePlaneta;

 }

// void fuerzasPlanetasAsteroides(asteroide asteroideA, planeta planetas){

 //}

 double distancia(double aX, double aY, double bX, double bY){
   double dist = 0.0;
   double x = 0.0;
   x = (aX - bX);
   x = x * x;
   double y = 0.0;
   y = (aY - bY);
   y = y * y;
   dist = x + y;
   dist = sqrt(dist);
   return dist;
  }
 void salida(asteroide *asteroides, int num_asteroides){
   ofstream fs("out1.txt");
   fs << fixed;
   for (int i = 0; i < num_asteroides; i++){
     fs << setprecision(3) << asteroides[i].posX << " " << setprecision(3) << asteroides[i].posY << " " << setprecision(3) << asteroides[i].velX << " " << setprecision(3) << asteroides[i].velY << " " << setprecision(3) << asteroides[i].masa << "\n";
   }
 }
 void calcFuerza(int i,asteroide *asteroides,planeta *planetas,fuerza **fuerzaAsteroideAsteroide,fuerza **fuerzaAsteroidePlaneta,int num_asteroides,int num_planetas){
   double dist = 0.0;
   double pendiente = 0.0;
   double aux = 0.0;
   double angulo = 0.0;
   double fuerzas = 0.0;
   double auxVelocidad = 0.0;

   for(int j = i+1; j < num_asteroides; j++){

       dist = distancia(asteroides[i].posX, asteroides[i].posY, asteroides[j].posX, asteroides[j].posY);

     if(dist > 5.0){
       pendiente = asteroides[i].posY - asteroides[j].posY;
       aux = asteroides[i].posX - asteroides[j].posX;
       pendiente = pendiente / aux;
       if(pendiente > 1.0){
         pendiente = 1.0;
       }else if(pendiente < -1.0){
         pendiente = -1.0;
       }
       angulo = atan(pendiente);

       fuerzas = GBL_GRAVITY * asteroides[i].masa * asteroides[j].masa;
       aux = dist * dist;
       fuerzas = fuerzas / aux;

       fuerzaAsteroideAsteroide[i][j].fuerzaX = fuerzas * cos(angulo);
       fuerzaAsteroideAsteroide[j][i].fuerzaX = -(fuerzas * cos(angulo));
       if(fuerzaAsteroideAsteroide[i][j].fuerzaX > 100.0){
         fuerzaAsteroideAsteroide[i][j].fuerzaX = 100.0;
         fuerzaAsteroideAsteroide[j][i].fuerzaX = -100.0;
       }
       if(fuerzaAsteroideAsteroide[j][i].fuerzaX > 100.0){
         fuerzaAsteroideAsteroide[j][i].fuerzaX = -100.0;
         fuerzaAsteroideAsteroide[i][j].fuerzaX = 100.0;
       }

       fuerzaAsteroideAsteroide[i][j].fuerzaY = fuerzas * sin(angulo);
       fuerzaAsteroideAsteroide[j][i].fuerzaY = -(fuerzas * sin(angulo));
       if(fuerzaAsteroideAsteroide[i][j].fuerzaY > 100.0){
         fuerzaAsteroideAsteroide[i][j].fuerzaY = 100.0;
         fuerzaAsteroideAsteroide[j][i].fuerzaY = -100.0;
       }
       if(fuerzaAsteroideAsteroide[j][i].fuerzaY > 100.0){
         fuerzaAsteroideAsteroide[j][i].fuerzaY = -100.0;
         fuerzaAsteroideAsteroide[i][j].fuerzaY = 100.0;
       }
     }
     else{
       fuerzaAsteroideAsteroide[i][j].fuerzaX=0.0;
       fuerzaAsteroideAsteroide[i][j].fuerzaY=0.0;
       fuerzaAsteroideAsteroide[j][i].fuerzaX=0.0;
       fuerzaAsteroideAsteroide[j][i].fuerzaY=0.0;
       aux = asteroides[i].velX;
       auxVelocidad = asteroides[i].velY;
       asteroides[i].velX = asteroides[j].velX;
       asteroides[i].velY = asteroides[j].velY;
       asteroides[j].velX = aux;
       asteroides[j].velY = auxVelocidad;
     }
   }

   for(int h = 0; h<num_planetas; h++){
     dist = distancia(asteroides[i].posX, asteroides[i].posY, planetas[h].posX, planetas[h].posY);
     pendiente = asteroides[i].posY -  planetas[h].posY;
     aux = asteroides[i].posX -  planetas[h].posX;
     pendiente = pendiente / aux;
     if(pendiente > 1.0){
       pendiente = 1.0;
     }else if(pendiente < -1.0){
       pendiente = -1.0;
     }
     angulo = atan(pendiente);
     fuerzas = GBL_GRAVITY * asteroides[i].masa *  planetas[h].masa;
     aux = dist * dist;
     fuerzas = fuerzas / aux;

     fuerzaAsteroidePlaneta[i][h].fuerzaX = fuerzas * cos(angulo);
     fuerzaAsteroidePlaneta[i][h].fuerzaY = fuerzas * sin(angulo);
   }
 }
 void calcNewPosVel(int i,asteroide *asteroides,fuerza  **fuerzaAsteroideAsteroide,fuerza **fuerzaAsteroidePlaneta,int num_asteroides,int num_planetas){
   double aux = 0.0;
   double aceleracionX = 0.0;
   double aceleracionY = 0.0;
   double velocidadX = 0.0;
   double velocidadY = 0.0;
   double sumatorioFuerzasX = 0.0;
   double sumatorioFuerzasY = 0.0;


   for(int j = 0; j < num_asteroides; j++){
     if(i!=j){
       sumatorioFuerzasX += fuerzaAsteroideAsteroide[i][j].fuerzaX;
       sumatorioFuerzasY += fuerzaAsteroideAsteroide[i][j].fuerzaY;
     }

   }
   for(int h = 0; h < num_planetas; h++){
     sumatorioFuerzasX = sumatorioFuerzasX + fuerzaAsteroidePlaneta[i][h].fuerzaX;
     sumatorioFuerzasY = sumatorioFuerzasY + fuerzaAsteroidePlaneta[i][h].fuerzaY;
   }

   aux = 1/asteroides[i].masa;
   aceleracionX = aux*sumatorioFuerzasX;

   aceleracionY = aux*sumatorioFuerzasY;
   velocidadX = aceleracionX * GBL_ITIME;
   //velocidadX = asteroides[i].velX + velocidadX;
   asteroides[i].velX = asteroides[i].velX + velocidadX;
   velocidadY = aceleracionY * GBL_ITIME;
   //velocidadY = asteroides[i].velY + velocidadY;
   asteroides[i].velY = asteroides[i].velY + velocidadY;
   aux = asteroides[i].velX * GBL_ITIME;
   aux = aux + asteroides[i].posX;
   asteroides[i].posX = aux;
   aux = asteroides[i].velY * GBL_ITIME;
   aux = aux + asteroides[i].posY;
   asteroides[i].posY = aux;
   if (asteroides[i].posX <= 0.0){
     asteroides[i].posX = 5.0;
     asteroides[i].velX = asteroides[i].velX * -1.0;
   }
   if (asteroides[i].posY <= 0.0){
     asteroides[i].posY = 5.0;
     asteroides[i].velY = asteroides[i].velY * -1.0;
   }
   if (asteroides[i].posX >= GBL_WIDTH){
     asteroides[i].posX = GBL_WIDTH - 5.0;
     asteroides[i].velX = asteroides[i].velX * -1.0;
   }
   if (asteroides[i].posY >= GBL_HEIGHT){
     asteroides[i].posY = GBL_HEIGHT - 5.0;
     asteroides[i].velY = asteroides[i].velY * -1.0;
   }
 }
