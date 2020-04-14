#include "Header.h"
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
                                                                                                                                                                              
           /\    ____________________
          /  \   \   0    /\   1    /
         / 2  \   \ i-1  /  \ i+1  /
        /  i   \   \,j  /    \,j  /
       / ,j+1   \   \  / i,j  \  /
      /__________\   \/________\/
     /\          /\   \   2    /
    /  \   i,j  /  \   \  i   /
   /i-1 \      / 1  \   \,j-1/
  / ,j   \    / i+1  \   \  /
 /   0    \  /   ,j   \   \/
/__________\/__________\

*/

std::vector<int> ISiteAdjacency(int i,int j){
  bool up;
  if((i+j)%2==0){up=false;}
  else{up=true;}
  std::vector<int> Res;
  Res.resize(3);
  if(up){
    Res[0] = i-1;
    Res[1] = i+1;
    Res[2] = i;
  }
  else{
    Res[0] = i-1;
    Res[1] = i+1;
    Res[2] = i;
  }
  return Res;
}
std::vector<int> JSiteAdjacency(int i,int j){
  bool up;
  if((i+j)%2==0){up=false;}
  else{up=true;}
  std::vector<int> Res;
  Res.resize(3);
  if(up){
    Res[0] = j;
    Res[1] = j;
    Res[2] = j-1;
  }
  else{
    Res[0] = j;
    Res[1] = j;
    Res[2] = j+1;
  }
  return Res;
}

std::vector<int> g_nodes_from_site(const int i, const int j)
{
  std::vector<int> Res;
  Res.resize(3);
  if((i+j)%2==0)
    {
      Res[0] = 1;
      Res[1] = 3;
      Res[2] = 5;
    }
  else
    {
      Res[0] = 0;
      Res[1] = 2;
      Res[2] = 4;
    }
  return Res;
}
