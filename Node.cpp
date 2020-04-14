#include "Header.h"

using namespace std;

Node::Node(){}

Node::Node(int k, int i, int j)
{
  vector<vector<int>> all(get_all(i,j,k));
  I.resize(all.size());
  J.resize(all.size());
  for(int n=0; n<all.size(); n++)
    {
      I[n] = all[n][0];
      J[n] = all[n][1];
    }
  SetInitialPosition(X,Y,k,i,j);
}

vector<int> Node::g_I() const{return I;}
vector<int> Node::g_J() const{return J;}

int Node::g_IX() const{return IX;}
int Node::g_IY() const{return IY;}

double Node::g_X() const{return X;}
double Node::g_Y() const{return Y;}

void Node::set_X(double x){X=x;}
void Node::set_Y(double y){Y=y;}

void Node::set_IX(int ix){IX=ix;}
void Node::set_IY(int iy){IY=iy;}
