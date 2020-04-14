#include "Header.h"

using namespace std;
//This function return a list of pair of index that should make a spring
//becarefull each spring has to be made only once per site!
vector< pair<int,int> > GetSpringAdjacency(int i, int j){  
  vector<pair<int,int>> Res;
  if((i+j)%2==0){
    Res.push_back({1,3});
    Res.push_back({1,5});
    Res.push_back({5,3});
  }
  else{
    Res.push_back({0,2});
    Res.push_back({0,4});
    Res.push_back({2,4});
  }
  return Res;
}
vector<vector<int>> GetSpring3Adjacency(int i, int j){
  vector<vector<int>> Res;
  if((i+j)%2==0){
    Res.push_back({1,3,5});
  }
  else{
    Res.push_back({0,2,4});
  }
  return Res;
}
double getK(int index1, int index2,System* system){
  return system->K1; // in the C function that create system we'll find a way to adjust K
}
double getKvol(int index1, int index2, int index3,System* system){return system->Kvol;}
double getA0(int index1, int index2, int index3,System* system){return 1;;}
double getL0(int index1, int index2,System* system){
  return 1+system->eps;
}

				  
