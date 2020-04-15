#ifndef Site_h
#define Site_h

class Site
{
 public:
  Site(int i, int j,std::map<int,int> dimension);
  int g_I() const;
  int g_J() const;
  int g_dim(int const index) const;
  std::vector<int> g_nodes_to_create() const;
  std::vector<int> g_nodes_to_output() const;
 private:
  int I,J;
  std::vector<int> Ineigh;
  std::vector<int> Jneigh;
  std::map<int,int> dim;
  
  
  
};
#endif
