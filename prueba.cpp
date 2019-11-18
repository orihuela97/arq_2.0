#include <iostream>
#include <omp.h>
using namespace std;

int suma(int s1,int s2);
int mult(int s1,int s2);

int main(){
  int i,a;
  int sum=0;
  int suma2=1;

#pragma omp parallel
{
      #pragma omp for reduction(+:sum) reduction(+:suma2)
      for(i=0;i<5;i++){
        #pragma omp critical
        std::cout << omp_get_thread_num() << '\n';
        sum = sum+i;
        suma2=suma2+i;
      }


}

      std::cout << i << '\n';
      std::cout << sum << '\n';
      std::cout << suma2 << '\n';




  return 0;
}


int suma(int s1,int s2){
  return s1+s2;
}


int mult(int s1,int s2){
  return s1*s2;
}
