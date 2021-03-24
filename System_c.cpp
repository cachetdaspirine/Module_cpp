#include "Header.h"


extern "C"
{
  void* CreateSystem(int* array, int LX, int LY,double epsilon,double Kmain,double Kcoupling,double KVOL)
  {
    return new(std::nothrow) System(array,LX,LY,epsilon,Kmain,Kcoupling,KVOL);;
  }
  double AffineDeformation(void* ptr, double Gx, double Gy)
{
  System* system = reinterpret_cast<System* >(ptr);
  //double Ebefore(system->get_Energy());
  system->MoveNodes(Gx,Gy);
  return system->get_Energy();
}
double Extension(void* ptr, int ax)
{
  System* system = reinterpret_cast<System* >(ptr);
  return system->Get_Extension(ax);
}
  void* CopySystem(void* ptr)
  {
    System* system = reinterpret_cast<System* >(ptr);
    return new(std::nothrow) System(*system);
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
	system->UpdateEnergy(array,LX,LY);
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
  double GetBulkEnergy(void* ptr)
  {
    try
      {
	System* system = reinterpret_cast<System *>(ptr);
	return system->Get_BulkEnergy();
      }
    catch(int e)
      {cout<<"Error "<<e<<"\n";}
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
}
