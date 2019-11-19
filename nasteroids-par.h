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
double pendient(asteroide a, asteroide b);
double calcAngulo(double pendiente);

int comprobarParametros(int argc, char **argv);
void bigBang(int num_planetas, planeta *planetas, int num_asteroides, asteroide *asteroides, int seed, int num_iteraciones);
void movimiento(asteroide *asteroides, int num_asteroides, planeta *planetas, int num_planetas, int num_iteraciones);
double distancia(double aX, double aY, double bX, double bY);
void salida(asteroide *asteroides, int num_asteroides);
