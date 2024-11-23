/** Fun4MainDaq.C:  Fun4all macro to decode the MainDAQ data.
 * 
 * To run this macro on a local computer, you need copy Coda file and also
 *  mapping files.  You can use the following commands;
     RUN=28700
     DIR_LOCAL=/data/e906
     
     mkdir -p $DIR_LOCAL/runs
     RUN6=$(printf '%06i' $RUN)
     scp -p  e906-gat6.fnal.gov:/data3/data/mainDAQ/run_$RUN6.dat $DIR_LOCAL
     scp -pr e906-gat6.fnal.gov:/data2/production/runs/run_$RUN6  $DIR_LOCAL/runs
 */
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <g4main/PHG4Reco.h>
#include "G4_SensitiveDetectors.C"
#include "G4_Target.C"
R__LOAD_LIBRARY(libinterface_main)
R__LOAD_LIBRARY(libonlmonserver)
R__LOAD_LIBRARY(libdecoder_maindaq)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libktracker)
#endif

int Fun4MainDaq(const int nevent = 0, const int run = 24172)
{
  gSystem->Load("libdecoder_maindaq.so");
  gSystem->Load("libonlmonserver.so");
  gSystem->Load("libg4detectors");
  gSystem->Load("libktracker.so");

  const char* dir_in  = "/data3/data/mainDAQ";
  const char* dir_out = ".";
  const bool is_online = true; // false;

  ostringstream oss;
  oss << setfill('0') 
      << dir_in << "/run_" << setw(6) << run << ".dat";
  string fn_in = oss.str();
  oss.str("");
  oss << dir_out << "/run_" << setw(6) << run << ".root";
  string fn_out = oss.str();

  OnlMonServer* se = OnlMonServer::instance();
  //se->Verbosity(1);

  const double FMAGSTR = -1.044;//-1.054;
  const double KMAGSTR = -1.025;//-0.951;

  recoConsts *rc = recoConsts::instance();
  rc->set_DoubleFlag("FMAGSTR", FMAGSTR);
  rc->set_DoubleFlag("KMAGSTR", KMAGSTR);
  rc->Print();

  JobOptsSvc *jobopt_svc = JobOptsSvc::instance();
  jobopt_svc->init("e906_data.opts");

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

  se->registerSubsystem(new CalibInTime());
  se->registerSubsystem(new CalibXT());

  // trakcing module
  gSystem->Load("libktracker.so");
  KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
  //ktracker->Verbosity(99);
  ktracker->set_enable_event_reducer(true);
  ktracker->set_DS_level(0);
  se->registerSubsystem(ktracker);


  Fun4AllEVIOInputManager *in = new Fun4AllEVIOInputManager("MainDaq");
  in->Verbosity(1);
  in->EventSamplingFactor(10);
  //if (is_online) in->PretendSpillInterval(20);

  in->DirParam("/seaquest/production/runs");
  in->fileopen(fn_in);
  se->registerInputManager(in);

  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", fn_out);
  se->registerOutputManager(out);

  se->registerSubsystem(new DbUpRun());
  se->registerSubsystem(new DbUpSpill());
  se->registerSubsystem(new CalibInTime());
  se->registerSubsystem(new CalibXT());

  if (is_online) { // Register the online-monitoring clients
    se->StartServer();
    se->registerSubsystem(new OnlMonMainDaq());
    se->registerSubsystem(new OnlMonTrigSig());
    se->registerSubsystem(new OnlMonV1495(OnlMonV1495::H1X, 1));
    se->registerSubsystem(new OnlMonV1495(OnlMonV1495::H2X, 1));
    se->registerSubsystem(new OnlMonV1495(OnlMonV1495::H3X, 1));
    se->registerSubsystem(new OnlMonV1495(OnlMonV1495::H4X, 1));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H1X));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H2X));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H3X));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H4X));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H1Y));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H2Y));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H4Y1));
    se->registerSubsystem(new OnlMonHodo (OnlMonHodo::H4Y2));
    se->registerSubsystem(new OnlMonCham (OnlMonCham::D0));
    se->registerSubsystem(new OnlMonCham (OnlMonCham::D1));
    se->registerSubsystem(new OnlMonCham (OnlMonCham::D2));
    se->registerSubsystem(new OnlMonCham (OnlMonCham::D3p));
    se->registerSubsystem(new OnlMonCham (OnlMonCham::D3m));
    se->registerSubsystem(new OnlMonProp (OnlMonProp::P1));
    se->registerSubsystem(new OnlMonProp (OnlMonProp::P2));
    se->registerSubsystem(new OnlMonReco ());
  }

  se->run(nevent);
  se->End();
  
  delete se;
  cout << "Fun4MainDaq Done!" << endl;
  return 0;
}
