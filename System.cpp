#include "Header.h"

using namespace std;
// {{{ Public Function
System::System(int* Array, int sizeX, int sizeY)
{
  Lx=sizeX;
  Ly=sizeY;
  K1=1.;
  K2=1.;
  Kvol=1.;
  eps=0.;
  int size(Lx*Ly);
  //Make a copy of the system array to be sure the Python interface doesn't mess with the pointer.
  CurrentState.resize(size);
  nodes.resize(g_Nkey());
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
}
System::~System()
{
  for(auto& it : springs){delete it.second;}
  springs.clear();
  for(auto& it : springs3){delete it.second;}
  springs3.clear();
  for(auto& it : nodes[0]){delete it.second;}
  nodes.clear();
  for(auto& it : sites){delete it.second;}
  sites.clear();
  delete cg;
}
void System::UpdateEnergy(int *Array, int SizeX, int SizeY)
{
  // Make the difference between this array and the "CurrentState" array to locate what changed.
  // We then delete/Re-create the sites/nodes/springs of this location.
  if(SizeX != Lx | SizeY != Ly)
    {cout<<"Error int the size of the different array"<<endl;}
  // look at all the site position that changed
  vector<int> RemovedSite;
  for(int i=0; i<SizeX*SizeY;i++)
    {
      if(Array[i]-CurrentState[i] < 0){RemovedSite.push_back(i);}
    }
  //delete We delete all the sites/spring/spring3
  for(auto& it : springs){delete it.second;}
  springs.clear();
  for(auto& it : springs3){delete it.second;}
  springs3.clear();
  for(auto& it : sites){delete it.second;}
  sites.clear();
  //Only the nodes are deleted selectively
  for(auto& it : RemovedSite)
    {DeleteNodeSpring(it);}
  for(int i = 0 ; i <SizeX*SizeY ; i++)
    {CurrentState[i]=Array[i];}

  MakeSites();
  MakeNodes();
  MakeSprings();
  MakeSpring3();
  ComputeEnergy();
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
	sites[i]=new Site(i%Lx,i/Lx);
      }
    }
  }
}
void System::MakeNodes()
{
  // look at every sites
  for(auto& it : sites){
    // pick each index that has to be created for this site
    for(auto& index :it.second->g_nodes_to_create()){
      // Look at the map if we can find this node
      try{nodes[index].at({it.second->g_I(),it.second->g_J()});}
      // if not we create one
      catch(const std::out_of_range& oor){
	Node* node=new Node(index,it.second->g_I(),it.second->g_J());
	//arrange the new node in all the containers
	for(int n=0;n<node->g_I().size();n++){
	  nodes[n][{node->g_I()[n],node->g_J()[n]}]=node;
	}
      }      
    }    
  }
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
      if(nodes[it2.first][{it.second->g_I(),it.second->g_J()}]
	 >nodes[it2.second][{it.second->g_I(),it.second->g_J()}])
	{
	  N1=nodes[it2.first][{it.second->g_I(),it.second->g_J()}];
	  N2=nodes[it2.second][{it.second->g_I(),it.second->g_J()}];
	}
      else
	{
	  N1=nodes[it2.second][{it.second->g_I(),it.second->g_J()}];
	  N2=nodes[it2.first][{it.second->g_I(),it.second->g_J()}];	  
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
    for(auto& it2 : Index){
      springs3[it.first]= new Spring3(nodes[it2[0]][{i,j}]
				     ,nodes[it2[1]][{i,j}]
				     ,nodes[it2[2]][{i,j}]
				     ,getKvol(it2[0],it2[1],it2[2],this)
				     ,getA0(it2[0],it2[1],it2[2],this));
    }
  }
}

void System::ComputeEnergy()
{
  cg->RemakeSprings(springs);
  vector<Node*> nodetovect;
  for(auto& it: nodes[0]){
    nodetovect.push_back(it.second);
  }
  cg->RemakeDoF(nodetovect);
  cg->RemakeSpring3(springs3);
  cg->Evolv();
  cg->ActualizeNodePosition(nodetovect);
  Energy=cg->GetEnergy();
  //for(auto& it: nodes[0]){cout<<it.second->g_X()<<" "<<it.second->g_Y()<<endl;}
}

void System::OutputSpring(char* filename)
{
  ofstream Out;
  Out.open(filename, ofstream::out | ofstream::trunc);
  for(auto& it: springs)
    {
      Out<<it.second->g_N1()->g_X()<<" "<<it.second->g_N1()->g_Y()<<" "
	 <<it.second->g_N2()->g_X()<<" "<<it.second->g_N2()->g_Y()<<" "
	 <<it.second->g_K()<<" "<<it.second->g_L0()<<endl;
    }
  Out.close();
}

void System::OutputSite(char* filename)
{
  ofstream Out;
  Out.open(filename, ofstream::out | ofstream::trunc);
  for(auto& it: sites)
    {
      vector<int> Index(it.second->g_nodes_to_create());
      int i(it.second->g_I()),j(it.second->g_J());
      for(auto& ind:Index)
	{
	  Out<<nodes[ind][{i,j}]->g_X()<<" "<<nodes[ind][{i,j}]->g_Y()<<" ";
	}
      Out<<"\n";
    }
  Out.close();
}

void System::DeleteNodeSpring(int SiteIndex)
{
  //Check all the node of the given site
  for(int k=0;k<g_Nkey();k++)
    {      
      int i(SiteIndex%Ly),j(SiteIndex/Ly);
      //Build the vector of all the node we need to erase      
      vector<vector<int>> allIndex(get_all(i,j,k));
      //Try to delete the given node if not already done
      try{delete nodes[k][{i,j}];}
      catch(int e){}
      //Remove this node from every container
      for(int n=0;n<allIndex.size();n++)
	{nodes[n].erase({allIndex[n][0],allIndex[n][1]});}      
    }
}
// }}}
