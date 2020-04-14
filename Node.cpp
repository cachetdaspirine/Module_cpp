#include "Header.h"

using namespace std;

Node::Node(){}

Node::Node(int k, int i, int j,double eps)
{
  map<int,vector<int>> all(get_all(i,j,k));
  for(auto& it : all)
    {
      I[it.first] = it.second[0];
      J[it.first] = it.second[1];
    }
  SetInitialPosition(X,Y,k,i,j,eps);
  //cout<<"index creator "<<i<<" "<<j<<" "<<k<<endl;
  //for(auto& it : all){cout<<it.first<<" "<<it.second[0]<<" "<<it.second[1]<<endl;}
  //cout<<this<<endl;
  //cout<<endl;
}
Node::~Node(){}//cout<<"delete "<<this<<endl;}

map<int,int> Node::g_I() const{return I;}
map<int,int> Node::g_J() const{return J;}

int Node::g_IX() const{return IX;}
int Node::g_IY() const{return IY;}

double Node::g_X() const{return X;}
double Node::g_Y() const{return Y;}

void Node::set_X(double x){X=x;}
void Node::set_Y(double y){Y=y;}

void Node::set_IX(int ix){IX=ix;}
void Node::set_IY(int iy){IY=iy;}
