#ifndef _TREE_DATA__H_
#define _TREE_DATA__H_
#include <vector>
#include <TROOT.h>
#include <TLorentzVector.h>

struct EventData {
  short run_id;
  int   spill_id;
  int   event_id;
  short fpga_bits;
  short nim_bits;
  short D1;
  short D2;
  short D3p;
  short D3m;

  EventData();
  virtual ~EventData() {;}
  ClassDef(EventData, 1);
};

struct HitData {
  short  det_id;
  short  ele_id;
  double tdc_time;
  double drift_dist;

  HitData();
  virtual ~HitData() {;}
  ClassDef(HitData, 1);
};

typedef std::vector<HitData> HitList;

#endif // _TREE_DATA__H_
