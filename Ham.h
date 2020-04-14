#ifndef Ham_h
#define Ham_h


struct Ham{
  std::map<std::pair<Node*, Node*>, Spring*> springs;
  std::vector<Spring3*> springs3;
  
  Doub operator() (VecDoub_I &x)
  {
    Doub Nrj(0);
    for(auto& it: springs){
      Nrj+=it.second->ComputeNRJ(x);
    }
    for(auto& it: springs3){
      Nrj+=it->ComputeNRJ(x);
    }
    return Nrj;
  }
  void df(VecDoub_I &x, VecDoub_O &deriv)
  {
    for (int i = 0; i < deriv.size(); i++) {deriv[i] = 0.;}
    for(auto& it : springs){
      it.second->ComputeDerivative(x,deriv);
    }
    for(auto& it: springs3){
      it->ComputeDerivative(x,deriv);
    }
  }
};
#endif
