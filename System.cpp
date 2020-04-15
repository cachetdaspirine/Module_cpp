#include "Header.h"

using namespace std;
// {{{ Public Function
System::System(int* Array, int sizeX, int sizeY,double epsilon,double Kmain,double Kcoupling,double KVOL)
{
  X=NULL;
  Y=NULL;
  Index=NULL;
  IndexSize=0;
  Lx=sizeX;
  Ly=sizeY;
  K1=Kmain;
  K2=Kcoupling;
  Kvol=KVOL;
  eps=epsilon;
  int size(Lx*Ly);
  //Make a copy of the system array to be sure the Python interface doesn't mess with the pointer.
  CurrentState.resize(size);
  DEBUG_IF(true){cout<<"copy the array"<<endl;}
  for(int i = 0 ; i < size ; i++)
    {
      CurrentState[i]=Array[i];
    }

  // Make the sites from the map of 0/1.
  DEBUG_IF(true){cout<<"Make the Sites"<<endl;}
  MakeSites();

  // Make the nodes from the sites we have.
  DEBUG_IF(true){cout<<"Make the nodes"<<endl;}
  MakeNodes();

  // Make the springs from the nodes we have
  DEBUG_IF(true){cout<<"Make the springs"<<endl;}
  MakeSprings();
  MakeSpring3();
  
  // Build the CG
  DEBUG_IF(true){cout<<"Build the CG"<<endl;}  
  cg=new CG();

  // Compute the Energy of the system
  DEBUG_IF(true){cout<<"Compute the Energy"<<endl;}
  ComputeEnergy();
  SaveNodesPosition();
}
System::~System()
{
  DEBUG_IF(true){cout<<"delete the springs"<<endl;}
  for(auto& it: springs){delete (it.second);}
  springs.clear();
  DEBUG_IF(true){cout<<"delete the springs3"<<endl;}
  for(auto& it : springs3){delete it.second;}
  springs3.clear();
  DEBUG_IF(true){cout<<"delete the inner nodes"<<endl;}
  for(auto& it : nodes[0]){/*cout<<"delete "<<it.first.first<<" "<<it.first.second<<endl;*/delete it.second;}
  DEBUG_IF(true){cout<<"delete the outter nodes"<<endl;}
  for(auto& it : nodes[6]){/*cout<<"delete "<<it.first.first<<" "<<it.first.second<<endl;*/delete it.second;}
  nodes.clear();
  DEBUG_IF(true){cout<<"delete the inner Sites"<<endl;}
  for(auto& it : sites){delete it.second;}
  sites.clear();
  DEBUG_IF(true){cout<<"delete the conjugate gradient"<<endl;}
  delete cg;
  delete X;
  delete Y;
  delete Index;
  DEBUG_IF(true){cout<<"Deletion completed"<<endl;}  
}
void System::UpdateEnergy(int *Array, int SizeX, int SizeY,bool Evolv)
{
  DEBUG_IF(true){cout<<"Save Nodes Position"<<endl;}
  SaveNodesPosition();
  // Make the difference between this array and the "CurrentState" array to locate what changed.
  // We then delete/Re-create the sites/nodes/springs of this location.
  if(SizeX != Lx | SizeY != Ly)
    {cout<<"Error int the size of the different array"<<endl;}
  // look at all the site position that changed
  // vector<int> RemovedSite,AddedSite;
  // for(int i=0; i<SizeX*SizeY;i++)
  //   {
  //     if(Array[i]-CurrentState[i] < 0){RemovedSite.push_back(i);}
  //     if(CurrentState[i]-Array[i] < 0){AddedSite.push_back(i);}
  //   }
  //delete We delete all the sites/spring/spring3
  for(auto& it : springs){delete it.second;}
  springs.clear();
  for(auto& it : springs3){delete it.second;}
  springs3.clear();
  for(auto& it : sites){delete it.second;}
  sites.clear();
  //Only the nodes are deleted selectively
  /*
  for(auto& it : RemovedSite)
    {
      int i(it%SizeX),j(it/SizeY);
      DeleteNodeSpring(it);
      vector<int> IN(ISiteAdjacency(i,j)),JN(JSiteAdjacency(i,j));
      for(int n=0;n<IN.size();n++){DeleteNodeSpring(IN[n]+JN[n]*SizeX);}
    }
  for(auto& it : AddedSite)
    {
      int i(it%SizeX),j(it/SizeY);
      vector<int> IN(ISiteAdjacency(i,j)),JN(JSiteAdjacency(i,j));
      for(int n=0;n<IN.size();n++){DeleteNodeSpring(IN[n]+JN[n]*SizeX);}
    }
  */
  //Delete all the nodes...
  for(auto& it : nodes[0]){delete it.second;}
  DEBUG_IF(true){cout<<"delete the outter nodes"<<endl;}
  for(auto& it : nodes[6]){delete it.second;}
  nodes.clear();
  //--------------------------------
  for(int i = 0 ; i <SizeX*SizeY ; i++)
    {CurrentState[i]=Array[i];}
  MakeSites();
  MakeNodes();
  MakeSprings();
  MakeSpring3();
  DEBUG_IF(true){cout<<"rebuild nodes position"<<endl;}
  RebuildNodesPosition();
  if(Evolv){ComputeEnergy();}
}
double System::get_Energy() const {return Energy;}
// }}}

// {{{ Private Function
void System::MakeSites()
{
  for(int i=0;i<CurrentState.size();i++){
    if(CurrentState[i]==1){
      try{sites.at(i);}
      catch(const std::out_of_range& oor){
	sites[i]=new Site(i%Lx,i/Lx,set_dim(i,CurrentState,Lx,Ly));
      }
    }
  }
}
void System::MakeNodes()
{
  // look at every sites
  for(auto& it : sites){
    // pick each index that has to be created for this site
    vector<int> nodes_to_create(it.second->g_nodes_to_create());
    for(auto& index :nodes_to_create){
      // Look at the map if we can find this node
      try{nodes[index].at({it.second->g_I(),it.second->g_J(),it.second->g_dim(index)});}
      // if not we create one
      catch(const std::out_of_range& oor){
	Node* node=new Node(index,it.second->g_I(),it.second->g_J(),it.second->g_dim(index),eps);
	//arrange the new node in all the containers
	for(auto & it2 : node->g_I()){
	  nodes[it2.first][{node->g_I()[it2.first],node->g_J()[it2.first],it.second->g_dim(index)}]=node;
	}
      }      
    } 
  }
  /*  for(auto& it : nodes){
    for(auto& it2 : it.second){
      cout<<it.first<<" "<<get<0>(it2.first)<<" "<<get<1>(it2.first)<<" "<<get<2>(it2.first)<<" "<<it2.second<<endl;
    }
    }*/
}
void System::MakeSprings()
{
  // We build the spring site per site.
  // it is the site that determine if a spring exist or not.

  for(auto & it : sites){
    // for a given site this returns the list of doublet of node index that should make a spring
    vector<pair<int,int>> NodeIndex(GetSpringAdjacency(it.second->g_I(),it.second->g_J()));
    for(auto& it2 : NodeIndex){
      Node* N1;
      Node* N2;
      // Make sure that the N1>N2
      if(nodes[it2.first][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.first)}]
	 >nodes[it2.second][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.second)}])
	{
	  N1=nodes[it2.first][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.first)}];
	  N2=nodes[it2.second][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.second)}];
	}
      else
	{
	  N1=nodes[it2.second][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.second)}];
	  N2=nodes[it2.first][{it.second->g_I(),it.second->g_J(),it.second->g_dim(it2.first)}];	  
	}
      try{springs.at({N1,N2})->Multiplicitypp();}
      catch(const std::out_of_range& oor){
	springs[{N1,N2}]=new Spring(N1,N2
				    ,getK(it2.first,it2.second,this)
				    ,getL0(it2.first,it2.second,this));	
      }      
    }
  }
}
void System::MakeSpring3(){
  for(auto& it: sites){
    int i(it.second->g_I()),j(it.second->g_J());
    vector<vector<int>> Index(GetSpring3Adjacency(i,j));
    for(int n=0;n<Index.size();n++){
      springs3[{it.first,n}]= new Spring3(nodes[Index[n][0]][{i,j,it.second->g_dim(Index[n][0])}]
				     ,nodes[Index[n][1]][{i,j,it.second->g_dim(Index[n][1])}]
					  ,nodes[Index[n][2]][{i,j,it.second->g_dim(Index[n][2])}]
				     ,getKvol(Index[n][0],Index[n][1],Index[n][2],this)
				     ,getA0(Index[n][0],Index[n][1],Index[n][2],this));
    }
  }
}
void System::SaveNodesPosition()
{
  IndexSize=sites.size()*g_Nnodes();  
  delete X;
  delete Y;
  delete Index;
  X = new double[IndexSize];
  Y = new double[IndexSize];
  Index = new int[IndexSize];
  int n=(0);
  for(auto& it : sites){
    vector<int> kvect(g_nodes_from_site(it.second->g_I(),it.second->g_J()));
    for(int k=0;k<kvect.size();k++){
      X[n]=nodes[kvect[k]][{it.second->g_I(),it.second->g_J(),it.second->g_dim(kvect[k])}]->g_X();
      Y[n]=nodes[kvect[k]][{it.second->g_I(),it.second->g_J(),it.second->g_dim(kvect[k])}]->g_Y();
      Index[n]=k+it.second->g_I()*g_Nnodes()+it.second->g_J()*g_Nnodes()*Lx;
      n++;
    }
  }
}
void System::RebuildNodesPosition()
{
  for(int n=0;n<IndexSize;n++){
    //cout<<Index[n]<<" "<<X[n]<<" "<<Y[n]<<endl;
    int j(Index[n]/(Lx*g_Nnodes()));
    int i((Index[n]%(Lx*g_Nnodes()))/g_Nnodes());
    int k((Index[n]%(Lx*g_Nnodes()))%g_Nnodes());
    //cout<<i<<" "<<j<<" "<<k<<endl;
    try{sites.at(i+Lx*j);}
    catch(const std::out_of_range& oor){continue;}
    vector<int> kvect(g_nodes_from_site(sites[i+Lx*j]->g_I(),sites[i+Lx*j]->g_J()));
    //for(auto&  it:kvect){cout<<it<<" ";}cout<<endl;
    nodes[kvect[k]][{i,j,sites[i+j*Lx]->g_dim(kvect[k])}]->set_X(X[n]);
    nodes[kvect[k]][{i,j,sites[i+j*Lx]->g_dim(kvect[k])}]->set_Y(Y[n]);
  }
  /*
  for(auto& it : sites){
    for(int k=0;k<g_Nnodes();k++){
      XNodes[k+it->g_I()*g_Nnodes()+it->g_J()*g_Nnodes()*Lx]=
      nodes[k][{it->g_I(),it->g_J(),it->g_dim(k)}]->g_X();
      YNodes[k+it->g_I()*g_Nnodes()+it->g_J()*g_Nnodes()*Lx]=
	nodes[k][{it->g_I(),it->g_J(),it->g_dim(k)}]->g_Y();
    }
  }
  DEBUG_IF(true){cout<<"Rebuilt successfull"<<endl;}
  */
}
void System::SetXYIndex(double* NewX,double* NewY,int* NewIndex, int NewSize)
{
  delete X;
  delete Y;
  delete Index;
  X=new double[NewSize];
  Y=new double[NewSize];
  Index = new int[NewSize];
  memcpy(X,NewX,NewSize);//sizeof(X)/sizeof(X[0]));
  memcpy(Y,NewY,NewSize);//sizeof(Y)/sizeof(Y[0]));
  memcpy(Index,NewIndex,NewSize);//sizeof(Index)/sizeof(Index[0]));
}
double* System::g_X() const{return X;}
double* System::g_Y() const{return Y;}
int* System::g_Index() const{return Index;}
int System::g_size()const {return IndexSize;}
void System::ComputeEnergy()
{
  vector<Node*> nodetovect;
  for(auto& it: nodes[0]){
    nodetovect.push_back(it.second);
  }
  for(auto& it: nodes[6]){
    nodetovect.push_back(it.second);
    }
  cg->RemakeSprings(springs);
  cg->RemakeDoF(nodetovect);
  cg->RemakeSpring3(springs3);
  cg->Evolv();
  cg->ActualizeNodePosition(nodetovect);
  Energy=cg->GetEnergy();
}

void System::OutputSpring(const char* filename)
{
  ofstream Out;
  Out.open(filename, ofstream::out | ofstream::trunc);
  for(auto& it: springs)
    {
      if(it.second->g_L0()==1+eps){
      Out<<it.second->g_N1()->g_X()<<" "<<it.second->g_N1()->g_Y()<<" "
	 <<it.second->g_N2()->g_X()<<" "<<it.second->g_N2()->g_Y()<<" "
	 <<it.second->g_K()<<" "<<it.second->g_L0()<<endl;
      }
    }
  Out.close();
}

void System::OutputSite(const char* filename)
{
  ofstream Out;
  Out.open(filename, ofstream::out | ofstream::trunc);
  for(auto& it: sites)
    {
      vector<int> Index(it.second->g_nodes_to_output());
      int i(it.second->g_I()),j(it.second->g_J());
      for(auto& ind:Index)
	{
	  Out<<nodes[ind][{i,j,it.second->g_dim(ind)}]->g_X()<<" "<<nodes[ind][{i,j,it.second->g_dim(ind)}]->g_Y()<<" ";
	}
      Out<<"\n";
    }
  Out.close();
}

void System::DeleteNodeSpring(int SiteIndex)
{
  if(sites.find(SiteIndex)!=sites.end())
    {
      //Check all the node of the given site
      for(int k=0;k<12;k++)
	{      
	  int i(SiteIndex%Ly),j(SiteIndex/Ly);
	  //Build the vector of all the node we need to erase      
	  map<int,vector<int>> allIndex(get_all(i,j,k));
	  //Try to delete the given node if not already done
	  try{delete nodes[k][{i,j,sites[SiteIndex]->g_dim(k)}];}
	  catch(int e){}
	  //Remove this node from every container
	  for(auto& it : allIndex)
	    {nodes[it.first].erase({it.second[0],it.second[1],sites[SiteIndex]->g_dim(k)});}
	}
    }
}
// }}}
