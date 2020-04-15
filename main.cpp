#include "Header.h"
int main(int argc, char* argv[])
{
  int array[5*5]={0,0,0,0,0,
		  0,0,0,0,0,
		  0,1,1,1,0,
		  0,0,1,0,0,
                  0,0,0,0,0};
  System* system=new System(array,5,5,0.1,1.,1.,1.);
  cout<<system->get_Energy()<<endl;
  int array2[5*5]={0,0,0,0,0,
		  0,0,1,0,0,
		  0,1,1,1,0,
		  0,0,0,0,0,
		  0,0,0,0,0};
  system->UpdateEnergy(array2,5,5,true);
  cout<<system->get_Energy()<<endl;
  delete system;
  return 0;
}
