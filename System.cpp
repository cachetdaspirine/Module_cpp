#include "Header.h"

using namespace std;
System::System(int* Array, int sizeX, int sizeY,double epsilon,double Kmain,double Kcoupling,double KVOL)
{
  // {{{ constructor

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
  cg=new CG(K1,eps,K2,Kvol,sites.size());
  //OutputSpring();
  // Compute the Energy of the system
  DEBUG_IF(true){cout<<"Compute the Energy"<<endl;}
  ComputeEnergy();

  // }}}
}
System::System(const System& old_system)
{
  // {{{ Copy constructor

  Lx=old_system.Lx;
  Ly=old_system.Ly;
  K1=old_system.K1;
  K2=old_system.K2;
  Kvol=old_system.Kvol;
  eps=old_system.eps;
  int size(Lx*Ly);
  CurrentState.resize(size);
  //memcpy(CurrentState,old_system.CurrentState,size);
  CurrentState=old_system.CurrentState;
  for(auto& it : old_system.sites){sites[it.first]=new Site(*(it.second));}
  for(auto& it : old_system.nodes){for(auto& it2 : it.second){
      try{nodes[it.first].at(it2.first);}
      catch(const std::out_of_range& oor){
	Node* node=new Node(*(it2.second));
      for(auto& it3 : node->g_I()){
	nodes[it3.first][{node->g_I()[it3.first],node->g_J()[it3.first],it2.second->g_dim()}]=node;
      }
      }
    }}
  MakeSprings();
  MakeSpring3();
  cg=new CG(K1,eps,K2,Kvol,sites.size());


  Energy=old_system.Energy;

  // }}}
}
System::~System()
{
  // {{{ Destructor
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
  DEBUG_IF(true){cout<<"Deletion completed"<<endl;}
  // }}}
}
void System::UpdateEnergy(int *Array, int SizeX, int SizeY)
{
  // {{{ Update the Energy accordingly to the new state

  // Make the difference between this array and the "CurrentState" array to locate what changed.
  // We then delete/Re-create the sites/nodes/springs of this location.
  if(SizeX != Lx | SizeY != Ly)
    {cout<<"Error int the size of the different array"<<endl;}
  // look at all the site position that changed
  vector<int> RemovedSite,AddedSite;
  for(int i=0; i<SizeX*SizeY;i++)
    {
      if(Array[i]-CurrentState[i] < 0){RemovedSite.push_back(i);}
      if(CurrentState[i]-Array[i] < 0){AddedSite.push_back(i);}
    }
  for(int i = 0 ; i <SizeX*SizeY ; i++)
    {CurrentState[i]=Array[i];}
  //Only the nodes are deleted selectively
  //------------------------------------------------------------------------------
  //------------------------------------------------------------------------------
  /*
  for(auto& SiteIt : RemovedSite)
    {
      int i(SiteIt%SizeX),j(SiteIt/SizeX);
      cout<<"Removed Site : "<<i<<" "<<j<<endl;
      vector<int> NodesInd(g_nodes_from_site(i,j));
      for(auto& k : NodesInd){
	map<int,vector<int>> NodeVect(get_all(i,j,k));
	for(auto& SiteNodesIt : NodeVect)
	  {
	    DeleteAllNode(SiteNodesIt.second[0],SiteNodesIt.second[1]);
	  }
      }
    }
  for(auto& SiteIt : AddedSite)
    {
      int i(SiteIt%SizeX),j(SiteIt/SizeX);
      cout<<"Added Site : "<<i<<" "<<j<<endl;
      vector<int> NodesInd(g_nodes_from_site(i,j));
      for(auto& k : NodesInd){
	map<int,vector<int>> NodeVect(get_all(i,j,k));
	for(auto& SiteNodesIt : NodeVect)
	  {
	    DeleteAllNode(SiteNodesIt.second[0],SiteNodesIt.second[1]);
	  }
      }
    }
  */
  //for(auto& it: sites){DeleteAllNode(it.second->g_I(), it.second->g_J());}
  //for(auto& it : nodes){cout<<it.second.size()<<endl;}
  //------------------------------------------------------------------------------
  //------------------------------------------------------------------------------
  // Save the nodes index:
    DEBUG_IF(true){cout<<"Save Nodes Position"<<endl;}
  map<tuple<int,int,int,int>,double> SaveX;
  map<tuple<int,int,int,int>,double> SaveY;
  for(auto& it : nodes[0])
    {
      SaveX[{0,get<0>(it.first),get<1>(it.first),get<2>(it.first)}]=it.second->g_X();
      SaveY[{0,get<0>(it.first),get<1>(it.first),get<2>(it.first)}]=it.second->g_Y();
    }
  for(auto& it : nodes[6])
    {
      SaveX[{6,get<0>(it.first),get<1>(it.first),get<2>(it.first)}]=it.second->g_X();
      SaveY[{6,get<0>(it.first),get<1>(it.first),get<2>(it.first)}]=it.second->g_Y();
    }
  //delete We delete all the sites/spring/spring3
  DEBUG_IF(true){cout<<"delete spring"<<endl;}
  for(auto& it : springs){delete it.second;}
  springs.clear();
  DEBUG_IF(true){cout<<"delete spring3"<<endl;}
  for(auto& it : springs3){delete it.second;}
  springs3.clear();
  //for(auto& it : sites){delete it.second;}
  //sites.clear();
  DEBUG_IF(true){cout<<"Actualize sites"<<endl;}
  ActualizeSites(RemovedSite, AddedSite);
  DEBUG_IF(true){cout<<"delete nodes"<<endl;}
  //for(auto& it: nodes){for(auto& it2 : it.second){delete it2.second;}}
  for(auto& it:nodes[0]){delete it.second;}
  for(auto& it:nodes[6]){delete it.second;}
  nodes.clear();
  DEBUG_IF(true){cout<<"nodes all deleted"<<endl;}
  //--------------------------------
  //MakeSites();
  MakeNodes();
  MakeSprings();
  MakeSpring3();
  for(auto& it : SaveX)
    {
      try{nodes[get<0>(it.first)].at({get<1>(it.first),get<2>(it.first),get<3>(it.first)});
	  nodes[get<0>(it.first)][{get<1>(it.first),get<2>(it.first),get<3>(it.first)}]->set_X(it.second);}
      catch(std::out_of_range oor){}
    }
  for(auto& it : SaveY)
    {
      try{nodes[get<0>(it.first)].at({get<1>(it.first),get<2>(it.first),get<3>(it.first)});
	  nodes[get<0>(it.first)][{get<1>(it.first),get<2>(it.first),get<3>(it.first)}]->set_Y(it.second);}
      catch(std::out_of_range oor){}
      }
  DEBUG_IF(true){cout<<"rebuild nodes position"<<endl;}
  ComputeEnergy();

  // }}}
}
void System::ComputeEnergy()
{
  bool Re(false);
 Evolv:
  vector<Node*> nodetovect;
  for(auto& it: nodes[0]){
    nodetovect.push_back(it.second);
  }
  for(auto& it: nodes[6]){
    nodetovect.push_back(it.second);
    }
  cg->RemakeDoF(nodetovect);
  cg->RemakeSprings(springs);
  cg->RemakeSpring3(springs3);
  cg->Evolv();
  cg->ActualizeNodePosition(nodetovect);
  cg->ActualizeGPosition(sites,nodes);
  Energy=cg->GetEnergy();
  if(Re){return;}
  if(cg->CheckStability())
    {
      ResetNodePosition(0.001);
      Re=true;
      goto Evolv;
    }
}
double System::Get_BulkEnergy(){
  //ResetNodePosition(eps);
  for(auto& site : sites){
    double I(site.second->g_I());
    double J(site.second->g_J());
    std::vector<int> nodes_index(g_nodes_from_site(I,J));
    for(auto& n_index : nodes_index){
      nodes[n_index][{I,J,site.second->g_dim(n_index)}]->ResetPosition(n_index,0.);
    }
  }
  vector<Node*> nodetovect;
  for(auto& it: nodes[0]){
    nodetovect.push_back(it.second);
  }
  for(auto& it: nodes[6]){
    nodetovect.push_back(it.second);
    }
  cg->RemakeDoF(nodetovect);
  cg->RemakeSprings(springs);
  cg->RemakeSpring3(springs3);
  return cg->ComputeEnergy();
}
double System::get_Energy() const {return cg->ComputeEnergy();}

// {{{ Private Function
void System::ResetNodePosition(double EPS)
{
  int count(0);
  for(auto& it : nodes[0]){it.second->ResetPosition(0,EPS);count++;}
  for(auto& it : nodes[6]){it.second->ResetPosition(6,EPS);count++;}
}
void System::MakeSites()
{
  for(int i=0;i<CurrentState.size();i++){
    if(CurrentState[i]==1){
      try{sites.at(i);}
      catch(const std::out_of_range& oor){
	sites[i]=new Site(i%Lx,i/Lx,set_dim(i,CurrentState,Lx,Ly),NULL);
      }
    }
  }
}
void System::ActualizeSites(std::vector<int>& Removed, std::vector<int>& Added)
{
  for(auto& it : Removed){delete sites[it]; sites.erase(it);}
  for(auto& it : Added){
    try
      {
	sites.at(it);
	cout<<"A site already exist here, cannot create a new one"<<endl;
	continue;
      }
    catch(std::out_of_range oor){}
    vector<int> IN(ISiteAdjacency(it%Lx,it/Lx));
    vector<int> JN(JSiteAdjacency(it%Lx,it/Lx));
    for(int n=0; n<IN.size();n++)
      {
	try{sites.at(IN[n]+JN[n]*Lx);}
	catch(std::out_of_range oor){continue;}
	sites[it]=new Site(it%Lx,it/Lx,set_dim(it,CurrentState,Lx,Ly),sites[IN[n]+JN[n]*Lx]);
	goto NEXT;
      }
    sites[it]=new Site(it%Lx,it/Lx,set_dim(it,CurrentState,Lx,Ly),NULL);
  NEXT:
    continue;
  }
  for(auto& it : sites){it.second->RemakeDim(set_dim(it.first,CurrentState,Lx,Ly));}
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
	Node* node=new Node(it.second,index,eps);
	//arrange the new node in all the containers
	for(auto & it2 : node->g_I()){
	  nodes[it2.first][{node->g_I()[it2.first],node->g_J()[it2.first],it.second->g_dim(index)}]=node;
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
// }}}

// {{{ Output function
void System::OutputSpring(const char* filename)
{
  ofstream Out;
  Out.open(filename, ofstream::out | ofstream::trunc);
  for(auto& it: springs)
    {
      //if(it.second->g_L0()==1+eps ){//|| it.second->g_L0()==1-eps){
      if(it.second->g_L0()==1+eps || it.second->g_L0()==1-eps){
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
void System:: g_G(int i, int j, double& Xg, double& Yg){}
bool System::NeighExist(int i, int j, int k)
 {

 }
vector<Node*> System::GetSideNodes(bool TopOrLeft,bool Horizontal)
  { /*
               true, true
                ______
               |      |
   true, false |      | false, false
               |______|
               false, true

    */
   set<Node*> NodeToVect;
 //              i / j / k
   vector<tuple<int,int,int>> Index(Get_Boundary_Nodes(TopOrLeft,Horizontal,Lx,Ly));
   /*for(auto& it : sites){
     cout<<it.first%Lx<<" "<<it.first/Lx<<endl;
   }*/
   for(auto& I : Index) {
       int i(get<0>(I)),j(get<1>(I)),k(get<2>(I));
       /*cout<<i<<" "<<j<<" "<<k<<endl;
       try{
       cout<<sites.at(i+j*Lx)<<endl;}
       catch(int e){cout<<"yep";throw 0;}*/
       NodeToVect.insert(nodes[k][{i,j,sites[i+j*Lx]->g_dim(k)}]);
   }
   vector<Node*> Res(NodeToVect.begin(),NodeToVect.end());
   return Res;
  }
  vector<Node*> System::NodeToVect()
  {
    set<Node*> Res;
    /*if ( Free ){
      for(auto& it : nodes[0]){if(!it.second->g_fix()){Res.insert(it.second);}}
      for(auto& it : nodes[6]){if(!it.second->g_fix()){Res.insert(it.second);}}
    }*/
    //else{
      for(auto& it : nodes[0]){Res.insert(it.second);}
      for(auto& it : nodes[6]){Res.insert(it.second);}
    //}
    vector<Node*> RRes(Res.begin(),Res.end());
    return RRes;
  }
  void System::MoveNodes(double Gx, double Gy)
  {
     for (auto& it : nodes[0]){
       it.second->set_X((1+Gx)*it.second->g_X());
       it.second->set_Y((1+Gy)*it.second->g_Y());
     }
     for (auto& it : nodes[6]){
       it.second->set_X((1+Gx)*it.second->g_X());
       it.second->set_Y((1+Gy)*it.second->g_Y());
     }
     std::vector<Node*> nodetovect(NodeToVect());
     cg->RemakeDoF(nodetovect);
  }

  double System::Get_Extension(int ax)
{
  if(ax == 0){
    vector<Node*> SidesLeft(GetSideNodes(true,false));
    vector<Node*> SidesRight(GetSideNodes(false,false));
    double XLeft(0),XRight(0);
    for(auto& it : SidesLeft){
      XLeft+=it->g_X();
    }
    XLeft=XLeft/SidesLeft.size();
    for(auto& it : SidesRight){
      XRight+=it->g_X();
    }
    XRight = XRight/SidesRight.size();
    return XRight-XLeft;
  }
  if(ax == 1){
    vector<Node*> SidesTop(GetSideNodes(true,true));
    vector<Node*> SidesBot(GetSideNodes(false,true));
    double YTop(0),YBot(0);
    for(auto& it : SidesTop){
      YTop+=it->g_Y();
    }
    for(auto& it : SidesBot){
      YBot+=it->g_Y();
    }
    YTop = YTop / SidesTop.size();
    YBot = YBot / SidesBot.size();
    return YTop-YBot;
  }
}
/*
void System::DeleteAllNode(int i, int j)
{
  try{sites.at(i+Lx*j);}
  catch(std::out_of_range oor){return;}
  //Check all the node of the given site
  //Build the vector of all the node we need to erase
  vector<int> NodeInd(g_nodes_from_site(i,j));
  for(auto& k : NodeInd)
    {
      int dim(sites[i+Lx*j]->g_dim(k));
      map<int,vector<int>> allIndex(get_all(i,j,k));
      //Try to delete the given node if not already done
      try
	{
	  nodes[k].at({i,j,dim});
	  delete nodes[k][{i,j,dim}];
	  //cout<<"Node removed : "<<i<<" "<<j<<" "<<k<<endl;
	}
      catch(std::out_of_range oor){}
      //Remove this node from every container
      for(auto& it : allIndex)
	{nodes[it.first].erase({it.second[0],it.second[1],dim});}
  }
}*/
// }}}
