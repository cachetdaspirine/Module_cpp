#include "Header.h"


extern "C"
{
  void* CreateSystem(int* array, int LX, int LY,double epsilon,double Kmain,double Kcoupling,double KVOL)
  {
    return new(std::nothrow) System(array,LX,LY,epsilon,Kmain,Kcoupling,KVOL);;
  }
  void DeleteSystem(void* ptr)
  {
    System* system = reinterpret_cast<System* >(ptr);
    delete system;
  }
  void UpdateSystemEnergy(void* ptr,int* array, int LX, int LY)
  {
    try
      {
    	System* system = reinterpret_cast<System* >(ptr);
	system->UpdateEnergy(array,LX,LY,true);
      }
    catch(int e){cout<<"Error "<<e<<"\n";}
  }
  void UpdateSystemState(void* ptr,int* array, int LX, int LY)
  {
    try
      {
    	System* system = reinterpret_cast<System* >(ptr);
	system->UpdateEnergy(array,LX,LY,false);
      }
    catch(int e){cout<<"Error "<<e<<"\n";}
  }
  void SetElasticConstant(double epsilon,double Kmain, double Kcoupling,double KVOL, void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System* >(ptr);
	system->eps=epsilon;
	system->K1=Kmain;
	system->K2=Kcoupling;
	system->Kvol=KVOL;
      }
    catch(int e){cout<<"Error "<<e<<"\n";}
  }
  double GetSystemEnergy(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->get_Energy();
      }
    catch(int e)
      {cout<<"Error "<<e<<"\n";}
  }
  void OutputSystemSite(void* ptr, const char* filename)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	system->OutputSite(filename);
      }
    catch(int e){cout<<"error : "<<e<<"\n";}
  }
  void OutputSystemSpring(void* ptr, const char* filename)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	system->OutputSpring(filename);
      }
    catch(int e){cout<<"error : "<<e<<"\n";}
  }
  void SetNodesPosition(void* ptr, double* NewX, double* NewY, int* NewIndex, int NewSize)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	system->SetXYIndex(NewX,NewY,NewIndex,NewSize);
      }
    catch(int e){cout<<"Error : "<<e<<"\n";}
  }
  double* get_X(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->g_X();
      }
    catch(int e){cout<<"Error : "<<e<<"\n";}
  }
  double* get_Y(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->g_Y();
      }
    catch(int e){cout<<"Error : "<<e<<"\n";}
  }
  int* get_Index(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->g_Index();
      }
    catch(int e){cout<<"Error : "<<e<<"\n";}
  }
  int g_Size(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->g_size();	
      }
    catch(int e){cout<<"Error : "<<e<<"\n";}
  }
}
