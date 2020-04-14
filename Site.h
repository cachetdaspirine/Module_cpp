#ifndef Site_h
#define Site_h

class Site
{
 public:
  Site(int i, int j);
  int g_I() const;
  int g_J() const;
  std::vector<int> g_nodes_to_create() const;
 private:
  int I,J;
  std::vector<int> Ineigh;
  std::vector<int> Jneigh;
  
};
#endif
