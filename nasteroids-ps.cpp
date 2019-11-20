#include <iostream>
#include <cstring>
#include <random>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <omp.h>
#include <chrono>
#include "nasteroids-par.h"
using namespace std;
using namespace std::chrono;


int main(int argc, char **argv){
  using clk = chrono::high_resolution_clock;
  auto t1 = clk::now();

  //double t1=omp_get_wtime();


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
  auto t2 = clk::now();
  auto diff = duration_cast<seconds>(t2-t1);
  cout << "Tiempo de ejecución: " << diff.count() << "seconds" << '\n';




  /*double t2=omp_get_wtime();

  double diff= t2-t1;


  cout << "Tiempo de ejecución: " << setprecision(9) << diff << "seconds" << '\n';*/


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
  ofstream fs("init_conf.txt");
  fs << num_asteroides << " " << num_iteraciones << " " << num_planetas << " " << semilla << "\n";
  fs << fixed;
  for (int i = 0; i < num_asteroides;i++){
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
  double dist = 0.0;
  double pendiente = 0.0;
  double angulo = 0.0;
  int contador_iteraciones = 0;
  /*fuerza fuerzaAsteroideAsteroide[num_asteroides][num_asteroides];
  fuerza fuerzaAsteroidePlaneta[num_asteroides][num_planetas];*/
  fuerza **fuerzaAsteroideAsteroide;
  fuerzaAsteroideAsteroide = new fuerza*[num_asteroides];

  for(int i=0; i<num_asteroides;i++) {
    fuerzaAsteroideAsteroide[i] = new fuerza[num_asteroides];
  }

  fuerza **fuerzaAsteroidePlaneta;
  fuerzaAsteroidePlaneta = new fuerza*[num_asteroides];

  for(int i=0; i<num_asteroides;i++) {
    fuerzaAsteroidePlaneta[i] = new fuerza[num_planetas];
  }

  while (contador_iteraciones< iteraciones){

    for(int i = 0; i < num_asteroides; i++){

      for(int j = i+1; j < num_asteroides; j++){
          dist = distancia(asteroides[i].posX, asteroides[i].posY, asteroides[j].posX, asteroides[j].posY);
        if(dist > 5.0){
          pendiente = pendient(asteroides[i].posX,asteroides[i].posY, asteroides[j].posX,asteroides[j].posY);
          angulo = calcAngulo(pendiente);
          fuerzasEntreAsteroides(fuerzaAsteroideAsteroide,i,j,angulo,dist,asteroides);
        }
        else{
          reboteEntreAsteroides(i,j,fuerzaAsteroideAsteroide, asteroides[i],asteroides[j]);
        }
      }

      for(int h = 0; h<num_planetas; h++){
        dist = distancia(asteroides[i].posX, asteroides[i].posY, planetas[h].posX, planetas[h].posY);
        pendiente = pendient(asteroides[i].posX,asteroides[i].posY, planetas[h].posX,planetas[h].posY);
        angulo = calcAngulo(pendiente);
        fuerzasEntreAsteroidePlaneta(i,h,asteroides[i], planetas[h],dist,angulo,fuerzaAsteroidePlaneta);
      }
    }
    double sumatorioFuerzasX = 0.0;
    double sumatorioFuerzasY = 0.0;

    for (int i = 0; i < num_asteroides; i++){
      sumatorioFuerzasX = 0.0;
      sumatorioFuerzasY = 0.0;
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
      NewPosition(i,asteroides,sumatorioFuerzasX,sumatorioFuerzasY);
      ReboteGrind(i,asteroides);
     }
     contador_iteraciones++;
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
   ofstream fs("out2.txt");
   fs << fixed;
   for (int i = 0; i < num_asteroides; i++){
     fs << setprecision(3) << asteroides[i].posX << " " << setprecision(3) << asteroides[i].posY << " " << setprecision(3) << asteroides[i].velX << " " << setprecision(3) << asteroides[i].velY << " " << setprecision(3) << asteroides[i].masa << "\n";
   }
 }
 double pendient(double aPosX, double aPosY,double bPosX, double bPosY){
   double pendiente = 0.0;
   double aux = 0.0;
   pendiente = aPosY - bPosY;
   aux = aPosX- bPosX;
   pendiente = pendiente / aux;
   if(pendiente > 1.0){
     pendiente = 1.0;
   }else if(pendiente < -1.0){
     pendiente = -1.0;
   }
   return pendiente;
 }
 double calcAngulo(double pendiente){
   double angulo = 0.0;
   angulo = atan(pendiente);
   return angulo;
 }
 void fuerzasEntreAsteroides(fuerza **fuerzaAsteroideAsteroide,int i, int j, double angulo, double dist,asteroide *asteroides){
   double fuerzas = 0.0;
   double aux = 0.0;
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
 void reboteEntreAsteroides(int i, int j, fuerza **fuerzaAsteroideAsteroide, asteroide a, asteroide b){
   fuerzaAsteroideAsteroide[i][j].fuerzaX=0.0;
   fuerzaAsteroideAsteroide[i][j].fuerzaY=0.0;
   fuerzaAsteroideAsteroide[j][i].fuerzaX=0.0;
   fuerzaAsteroideAsteroide[j][i].fuerzaY=0.0;
   double aux = 0.0;
   double auxVelocidad = 0.0;
   aux = a.velX;
   auxVelocidad = a.velY;
   a.velX = b.velX;
   a.velY = b.velY;
   b.velX = aux;
   b.velY = auxVelocidad;
 }
 void fuerzasEntreAsteroidePlaneta(int i, int h,asteroide a, planeta b, double distancia, double angulo, fuerza **fuerzaAsteroidePlaneta){
   double fuerzas = 0.0;
   double aux = 0.0;

   fuerzas = GBL_GRAVITY * a.masa *  b.masa;
   aux = distancia * distancia;
   fuerzas = fuerzas / aux;
   fuerzaAsteroidePlaneta[i][h].fuerzaX = fuerzas * cos(angulo);
   fuerzaAsteroidePlaneta[i][h].fuerzaY = fuerzas * sin(angulo);
 }
 void NewPosition(int i, asteroide *asteroides,double sumatorioFuerzasX, double sumatorioFuerzasY){
   double aux =0.0;
   double aceleracionX = 0.0;
   double aceleracionY = 0.0;
   double velocidadX = 0.0;
   double velocidadY = 0.0;
   aux = 1/asteroides[i].masa;
   aceleracionX = aux*sumatorioFuerzasX;
   aceleracionY = aux*sumatorioFuerzasY;
   velocidadX = aceleracionX * GBL_ITIME;
   asteroides[i].velX = asteroides[i].velX + velocidadX;
   velocidadY = aceleracionY * GBL_ITIME;
   asteroides[i].velY = asteroides[i].velY + velocidadY;
   aux = asteroides[i].velX * GBL_ITIME;
   aux = aux + asteroides[i].posX;
   asteroides[i].posX = aux;
   aux = asteroides[i].velY * GBL_ITIME;
   aux = aux + asteroides[i].posY;
   asteroides[i].posY = aux;
 }
 void ReboteGrind(int i, asteroide *asteroides){
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