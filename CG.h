#ifndef CG_h
#define CG_h
class CG{
 public:
  CG();
  double GetEnergy();
  void RemakeDoF(std::vector<Node*> nodes);
  void RemakeSprings(std::map<std::pair<Node*, Node*>, Spring*> springs);
  void RemakeSpring3(std::map<int,Spring3*> springs);
  void ActualizeNodePosition(std::vector<Node*> nodes);
  void Evolv();
 private:
  Ham ham;
  VecDoub DoF;
  double Energy;
};
#endif
