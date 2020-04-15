#include "Header.h"

using namespace std;

Site::Site(int i, int j,map<int,int> dimension)
{
  dim=dimension;
  I=i;
  J=j;
  Ineigh=ISiteAdjacency(I,J);
  Jneigh=JSiteAdjacency(I,J);
}

vector<int> Site::g_nodes_to_create() const
{
  return g_nodes_from_site(I,J);
}
vector<int> Site::g_nodes_to_output() const
{
  return g_nodes_from_site_output(I,J);
}

int Site::g_I() const{return I;}
int Site::g_J() const{return J;}
int Site::g_dim(int const index)const {return dim.at(index);}
