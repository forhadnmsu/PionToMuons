#include "TreeData.h"
using namespace std;

EventData::EventData()
  : run_id   (0)
  , spill_id (0)
  , event_id (0)
  , fpga_bits(0)
  , nim_bits (0)
  , D1       (0)
  , D2       (0)
  , D3p      (0)
  , D3m      (0)
{
  ;
}

DimuonData::DimuonData() 
  : road_pos(0)
  , road_neg(0)
  , pos_top(false)
  , pos_bot(false)
  , neg_top(false)
  , neg_bot(false)
  , pos(0,0,0)
  , mom(0,0,0,0)
  , n_hits_pos(0)
  , chisq_pos (0)
  , chisq_target_pos  (0)
  , chisq_dump_pos    (0)
  , chisq_upstream_pos(0)
  , pos_pos   (0,0,0)
  , mom_pos   (0,0,0,0)
  , pos_target_pos(0,0,0)
  , pos_dump_pos  (0,0,0)
  , n_hits_neg(0)
  , chisq_neg (0)
  , chisq_target_neg  (0)
  , chisq_dump_neg    (0)
  , chisq_upstream_neg(0)
  , pos_neg   (0,0,0)
  , mom_neg   (0,0,0,0)
  , pos_target_neg(0,0,0)
  , pos_dump_neg  (0,0,0)
  , mom_target(0,0,0,0)
  , mom_dump  (0,0,0,0)
{
  ;
}
