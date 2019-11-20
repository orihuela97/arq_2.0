const float GBL_GRAVITY = 6.674E-5;
const float GBL_ITIME = 0.1;
const double GBL_MIN_DIST = 5.0;
const int GBL_WIDTH = 200;
const int GBL_HEIGHT = 200;
const int GBL_MASA = 1000;
const int GBL_SDM = 50;



struct planeta{
  double posX;
  double posY;
  double masa;
};
struct asteroide{
  double posX;
  double posY;
  double masa;
  double velX;
  double velY;
};
struct fuerza{
  double fuerzaX;
  double fuerzaY;
};
 double pendient(double aPosX, double aPosY,double bPosX, double bPosY);
double calcAngulo(double pendiente);
void ReboteGrind(int i, asteroide *asteroides);

 void NewPosition(int i, asteroide *asteroides,double sumatorioFuerzasX, double sumatorioFuerzasY);
void fuerzasEntreAsteroides(fuerza **fuerzaAsteroideAsteroide,int i, int j, double angulo, double dist, asteroide *asteroides);
int comprobarParametros(int argc, char **argv);
void fuerzasEntreAsteroidePlaneta(int i, int h,asteroide a, planeta b, double distancia, double angulo,fuerza **fuerzaAsteroidePlaneta);
void reboteEntreAsteroides(int i, int j, fuerza **fuerzaAsteroideAsteroide, asteroide a, asteroide b);
 void fuerzasEntreAsteroidePlaneta(asteroide a, planeta b, double distancia, double angulo, fuerza **fuerzaAsteroidePlaneta);
void bigBang(int num_planetas, planeta *planetas, int num_asteroides, asteroide *asteroides, int seed, int num_iteraciones);
void movimiento(asteroide *asteroides, int num_asteroides, planeta *planetas, int num_planetas, int num_iteraciones);
double distancia(double aX, double aY, double bX, double bY);
void salida(asteroide *asteroides, int num_asteroides);
