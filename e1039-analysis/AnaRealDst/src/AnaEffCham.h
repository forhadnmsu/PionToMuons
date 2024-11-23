#ifndef _ANA_EFF_CHAM__H_
#define _ANA_EFF_CHAM__H_
#include <fstream>
#include <fun4all/SubsysReco.h>
class TFile;
class TH1;

class AnaEffCham: public SubsysReco {
 public:
  typedef enum { D0, D1, D2, D3p, D3m } ChamType_t;
  static const int N_PL = 6;

 private:
  ChamType_t m_type;
  int m_pl0;
  std::vector<int>         list_pl_id;
  std::vector<std::string> list_pl_name;
  std::vector<int>         list_pl_n_ele;

  unsigned int n_evt_all;
  unsigned int n_evt_trig;
  unsigned int n_evt_nhit;

  std::ofstream ofs;
  TFile* f_out;
  TH1* h1_eff_all;
  TH1* h1_eff_ok;
  TH1* h1_nhit[99];
  TH1* h1_ele [99];
  TH1* h1_time[99];

 public:
  AnaEffCham(const ChamType_t type);
  virtual ~AnaEffCham() {;}
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);
};

#endif /* _ANA_EFF_CHAM__H_ */
