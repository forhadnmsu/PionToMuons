/** Fun4DST.C:  Fun4all macro to run full reconstruction chain from DST files. 
 */
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <TSystem.h>

#include "G4_SensitiveDetectors.C"
#include "G4_Target.C"

R__LOAD_LIBRARY(libinterface_main)
R__LOAD_LIBRARY(libfun4all)
R__LOAD_LIBRARY(libdecoder_maindaq)
R__LOAD_LIBRARY(libg4testbench)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libg4eval)
R__LOAD_LIBRARY(libevt_filter)
R__LOAD_LIBRARY(libktracker)
R__LOAD_LIBRARY(libonlmonserver)
#endif

int Fun4DST(
const int nevent = 0,
const int run = 195
)
{
  gSystem->Load("libinterface_main.so");
  gSystem->Load("libfun4all");
  gSystem->Load("libdecoder_maindaq");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4eval");
  gSystem->Load("libktracker.so");
  gSystem->Load("libonlmonserver.so");

  const char* dst_dir = "./"; // "/data2/e1039/dst";
  const char* out_dir = "./";

  ostringstream oss;
  oss << setfill('0') 
      << dst_dir << "/run_" << setw(6) << run << "_spin.root";
  string in_file = oss.str();
  oss.str("");
  oss << out_dir << "/run_" << setw(6) << run << ".root";
  string out_dst = oss.str();

  Fun4AllServer* se = Fun4AllServer::instance();
  se->Verbosity(0);

  const double FMAGSTR = -1.044;//-1.054;
  const double KMAGSTR = -1.025;//-0.951;

  recoConsts *rc = recoConsts::instance();
  rc->set_DoubleFlag("FMAGSTR", FMAGSTR);
  rc->set_DoubleFlag("KMAGSTR", KMAGSTR);
  rc->Print();

  JobOptsSvc *jobopt_svc = JobOptsSvc::instance();
  jobopt_svc->init("run7_data.opts");

  GeomSvc::UseDbSvc(true);
  GeomSvc *geom_svc = GeomSvc::instance();

  // Fun4All G4 module
  PHG4Reco *g4Reco = new PHG4Reco();
  //PHG4Reco::G4Seed(123);
  //g4Reco->set_field(5.);
  g4Reco->set_field_map(
      jobopt_svc->m_fMagFile+" "+
      jobopt_svc->m_kMagFile+" "+
      Form("%f",FMAGSTR) + " " +
      Form("%f",KMAGSTR) + " " +
      "5.0",
      PHFieldConfig::RegionalConst);
  // size of the world - every detector has to fit in here
  g4Reco->SetWorldSizeX(1000);
  g4Reco->SetWorldSizeY(1000);
  g4Reco->SetWorldSizeZ(5000);
  // shape of our world - it is a tube
  g4Reco->SetWorldShape("G4BOX");
  // this is what our world is filled with
  g4Reco->SetWorldMaterial("G4_AIR"); //G4_Galactic, G4_AIR
  // Geant4 Physics list to use
  g4Reco->SetPhysicsList("FTFP_BERT");

  // insensitive elements of the spectrometer
  PHG4E1039InsensSubsystem* insens = new PHG4E1039InsensSubsystem("Insens");
  g4Reco->registerSubsystem(insens);

  // collimator, targer and shielding between target and FMag
  gROOT->LoadMacro("G4_Target.C");
  SetupTarget(g4Reco);

  // sensitive elements of the spectrometer
  gROOT->LoadMacro("G4_SensitiveDetectors.C");
  SetupSensitiveDetectors(g4Reco);

  se->registerSubsystem(g4Reco);

  // calib: in time
  CalibInTime* cali_intime = new CalibInTime();
  se->registerSubsystem(cali_intime);

  // calib: TDC to drift time
  CalibXT* cali_xt = new CalibXT();
  se->registerSubsystem(cali_xt);

  // Event Filter
  EvtFilter *evt_filter = new EvtFilter();
  se->registerSubsystem(evt_filter);

  // trakcing module
  KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
  ktracker->Verbosity(10);
  ktracker->set_enable_event_reducer(true);
  ktracker->set_DS_level(0);
  se->registerSubsystem(ktracker);

  // input manager for DST file
  Fun4AllInputManager *in = new Fun4AllDstInputManager("RealDst");
  in->fileopen(in_file);
  se->registerInputManager(in);


  // output manager for DST files
  //Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", out_dst);
  //se->registerOutputManager(out);

  se->run(nevent);

  PHGeomUtility::ExportGeomtry(se->topNode(),"geom.root");
  
  // finish job - close and save output files
  se->End();
  se->PrintTimer();
  std::cout << "All done" << std::endl;
  
  // cleanup - delete the server and exit
  delete se;
  return 0;
}
