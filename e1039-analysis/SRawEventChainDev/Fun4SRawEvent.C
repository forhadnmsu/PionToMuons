/** Fun4SRawEvent.C:  Fun4all macro to run full reconstruction chain from SRawEvent files. 
 * 
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

int Fun4SRawEvent(const int nevent = 0, const int run = 28700)
{
  const char* fn_in  = "/data3/analysis/production/R009/02/86/digit_028692_009.root";
  const char* fn_out = "./SRawEvent-DST.root";

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");
  gSystem->Load("libktracker.so");

  const double FMAGSTR = -1.054;
  const double KMAGSTR = -0.951;

  recoConsts *rc = recoConsts::instance();
  rc->set_DoubleFlag("FMAGSTR", FMAGSTR);
  rc->set_DoubleFlag("KMAGSTR", KMAGSTR);
  rc->Print();

  Fun4AllServer* se = 0;
  se = Fun4AllServer::instance();
  se->Verbosity(100);

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

  // trakcing module
  gSystem->Load("libktracker.so");
  KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
  ktracker->Verbosity(0);
  ktracker->set_enable_event_reducer(true);
  ktracker->set_DS_level(2);
  ktracker->set_pattern_db_name(gSystem->ExpandPathName("$E1039_RESOURCE/dsearch/v1/pattern.root"));
  PatternDBUtil::LooseMode(false);
  se->registerSubsystem(ktracker);

  Fun4AllSRawEventInputManager *in = new Fun4AllSRawEventInputManager("SRawEventIM");
  in->Verbosity(0);
  in->set_tree_name("save");
  in->set_branch_name("rawEvent");
  in->fileopen(fn_in);
  se->registerInputManager(in);

  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", fn_out);
  se->registerOutputManager(out);

  se->run(nevent);
  se->End();

  std::cout << KMAGSTR << std::endl;

  delete se;
  cout << "Fun4SRawEvent Done!" << endl;

  gSystem->Exit(0);
  return 0;
}
