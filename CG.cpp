#include "Header.h"

using namespace std;

CG::CG(){
  Energy=0;
}

void CG::RemakeDoF(vector<Node*> nodes){
  DoF.resize(2*nodes.size());
  for(int i=0;i<nodes.size();i++){
    DoF[2*i]=nodes[i]->g_X();
    nodes[i]->set_IX(2*i);
    DoF[2*i+1]=nodes[i]->g_Y();
    nodes[i]->set_IY(2*i+1);
  }  
}

void CG::RemakeSprings(std::map<std::pair<Node*, Node*>, Spring*> springs){
  ham.springs=springs;
}

void CG::RemakeSpring3(std::map<std::pair<int,int>,Spring3*> springs3){
  vector<Spring3*> vectsprings3;
  for(auto& it: springs3){vectsprings3.push_back(it.second);}
  ham.springs3=vectsprings3;
}

double CG::GetEnergy(){return Energy;}

void CG::Evolv(){
  Frprmn<Ham> frprmn(ham);
  DoF=frprmn.minimize(DoF);
  Energy=ham(DoF);
}

void CG::ActualizeNodePosition(std::vector<Node*> nodes){
  for(auto& it: nodes){
    it->set_X(DoF[it->g_IX()]);
    it->set_Y(DoF[it->g_IY()]);    
  }
}
