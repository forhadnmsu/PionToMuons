#ifndef _ANA_EFF_HODO__H_
#define _ANA_EFF_HODO__H_
#include <fstream>
#include <fun4all/SubsysReco.h>
class TFile;
class TH1;

class AnaEffHodo: public SubsysReco {
  unsigned int n_evt_all;
  unsigned int n_evt_trig;
  unsigned int n_evt_nhit;

  std::ofstream ofs;
  TFile* f_out;
  TH1* h1_eff_all;
  TH1* h1_eff_ok;
  TH1* h1_nhit;
  TH1* h1_ele;
  TH1* h1_time;

 public:
  AnaEffHodo();
  virtual ~AnaEffHodo() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
};

#endif /* _ANA_EFF_HODO__H_ */
