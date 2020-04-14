#include "Header.h"
/*
This file contain the structure of how the nodes are arranged.
The different function need to be adapted if we want to change
the structure of the particles.
 */
//------------------------------------------------------------
// First : we give a certain node and the function returns all
// The different index this node can have
//------------------------------------------------------------
/*
________                            _____                  
___  __ \______________ ____      _____(_)_______ _______ _
__  / / /__  ___/_  __ `/__ | /| / /__  / __  __ \__  __ `/
_  /_/ / _  /    / /_/ / __ |/ |/ / _  /  _  / / /_  /_/ / 
/_____/  /_/     \__,_/  ____/|__/  /_/   /_/ /_/ _\__, /  
                                                  /____/

       ________       _____ ______               
______ ___  __/       __  /____  /_ _____        
_  __ \__  /_         _  __/__  __ \_  _ \       
/ /_/ /_  __/         / /_  _  / / //  __/       
\____/ /_/            \__/  /_/ /_/ \___/        
                                             

        _____                        _____                       
__________  /_____________  ___________  /_____  _______________ 
__  ___/_  __/__  ___/_  / / /_  ___/_  __/_  / / /__  ___/_  _ \
_(__  ) / /_  _  /    / /_/ / / /__  / /_  / /_/ / _  /    /  __/
/____/  \__/  /_/     \__,_/  \___/  \__/  \__,_/  /_/     \___/ 
                                                                                                                                                                              
      ____________                      0
     /\          /\                    /\
    /  \ i,j+1,3/  \                  /  \
   /i-1 \      /    \                /    \
  /j+1,4 \    / i+1  \              / i,j  \    (i+j)%2==1
 /        \  /,j+1,2  \            /        \
/__________\/__________\         2/__________\4
\          /\   i+1    / 
 \ i-1    /  \  ,j,1  /          1____________5
  \,j,5  /    \      /            \          /
   \    /      \    /              \  i,j   /
    \  /  i,j,0 \  /                \      /    (i+j)%2==0
     \/__________\/                  \    /
                                      \  /
                                       \/
                                        3
__________                 ______                        _____ _____                             
___  ____/____  __________ ___  /______ ________ ______ ___  /____(_)______ _______        _____ 
__  __/   __  |/_/___  __ \__  / _  __ `/__  __ \_  __ `/_  __/__  / _  __ \__  __ \       ___(_)
_  /___   __>  <  __  /_/ /_  /  / /_/ / _  / / // /_/ / / /_  _  /  / /_/ /_  / / /       ___   
/_____/   /_/|_|  _  .___/ /_/   \__,_/  /_/ /_/ \__,_/  \__/  /_/   \____/ /_/ /_/        _(_)  
                  /_/                                                                            
a site i,j has 3 nodes => 6 labeling depending  if the site is up or down.
Eache node belong to 6 sites, the next function  gives all the indices for
each node position. i,j referrs to the site position and k is the position
of the node with respect to a given site i,j.
*/

std::vector<std::vector<int>> get_all(int i, int j, int k){
  // If you have a Site i,j and ask for all the adress of its node k this is what
  // this function gives you.
  std::vector<std::vector<int>> Res;
  Res.resize(6);
  for(int n=0;n<6;n++){Res[n].resize(6);}
  if(k==0){
    Res[0] = {i,j};
    Res[1] = {i+1,j+1};
    Res[2] = {i+1,j+1};
    Res[3] = {i,j+1};
    Res[4] = {i-1,j+1};
    Res[5] = {i-1,j};
  }
  else if(k==5){
    Res[0] = {i+1,j};
    Res[1] = {i+2,j};
    Res[2] = {i+2,j+1};
    Res[3] = {i+1,j+1};
    Res[4] = {i,j+1};
    Res[5] = {i,j};
  }
  else if(k==4){
    Res[0] = {i+1,j-1};
    Res[1] = {i+2,j-1};
    Res[2] = {i+2,j};
    Res[3] = {i+1,j};
    Res[4] = {i,j};
    Res[5] = {i,j-1};
  }
  else if(k==3){
    Res[0] = {i,j-1};
    Res[1] = {i+1,j-1};
    Res[2] = {i+1,j};
    Res[3] = {i,j};
    Res[4] = {i-1,j};
    Res[5] = {i-1,j-1};
  }
  else if(k==1){
    Res[0] = {i-1,j,0};
    Res[1] = {i,j,0};
    Res[2] = {i,j+1,2};
    Res[3] = {i-1,j+1,3};
    Res[4] = {i-2,j+1,4};
    Res[5] = {i-2,j,5};
  }
  else if(k==2){
    Res[0] = {i-1,j-1};
    Res[1] = {i,j-1};
    Res[2] = {i,j};
    Res[3] = {i-1,j};
    Res[4] = {i-2,j};
    Res[5] = {i-2,j-1};
  }
  return Res;
};
int g_Nkey(){return 6;}
void SetInitialPosition(double& X, double& Y, int NodeIndex, int SiteI, int SiteJ){
  X = SiteI*0.5;
    Y = 0.2887 + SiteJ * 0.866;
    if((SiteI+SiteJ)%2==1){//up
      if (NodeIndex == 0) {
        Y += 0.5774;
      } else if (NodeIndex == 4) {
        X += 0.5;
        Y -= 0.2887;
      } else if(NodeIndex ==2){
        X -= 0.5;
        Y -= 0.2887;
      }
    }
    else{// down
      if (NodeIndex==3) {
        Y -= 0.5774;
      } else if (NodeIndex == 1) {
        X -= 0.5;
        Y += 0.2887;
      } else if(NodeIndex == 5){
        X += 0.5;
        Y += 0.2887;
      }
    }
};

