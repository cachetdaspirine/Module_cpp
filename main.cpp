#include "Header.h"
int main(int argc, char* argv[])
{
  int array[5*5]={1,0,0,0,0,
		  0,0,0,0,0,
		  0,0,0,0,0,
		  0,0,0,0,0,
		  0,0,0,0,1};
  System* system=new System(array,5,5);
  delete system;
  return 0;
}
