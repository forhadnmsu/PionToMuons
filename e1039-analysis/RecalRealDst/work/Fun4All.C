//#include <TSystem.h>
R__LOAD_LIBRARY(libcalibrator)
R__LOAD_LIBRARY(libRecalRealDst)

int Fun4All(const int run_id, const int spill_id, const std::string dst_in, const std::string sraw_out)
{
  //recoConsts* rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER", run_id);
  //rc->Print();

  Fun4AllServer* se = Fun4AllServer::instance();
  se->Verbosity(0);
  se->setRun(run_id);

  CalibDriftDist* cal_dd = new CalibDriftDist();
  se->registerSubsystem(cal_dd);

  Fun4AllInputManager* in = new Fun4AllDstInputManager("DSTIN");
  in->Verbosity(0);
  in->fileopen(dst_in.c_str());
  se->registerInputManager(in);

  Fun4AllSRawEventOutputManager* out = new Fun4AllSRawEventOutputManager("SRAWOUT");
  out->SetFileName(sraw_out);
  se->registerOutputManager(out);

  se->run();
  se->End();
  delete se;
  //gSystem->Exit(0);
  return 0;
}
