
#include <iostream>

using namespace std;

int Fun4PatternDB(
    const int nEvents = 1,
    const int nmu = 1,
    const double target_coil_pos_z = -300
    )
{
  const bool do_collimator = false;
  const bool do_target = false;
  const double target_l = 7.9; //cm
  const double target_z = (7.9-target_l)/2.; //cm
  const int use_g4steps = 1;

  const bool gen_gun = false;
  const bool gen_pythia8 = false;
  const bool gen_particle = true;
  const bool gen_scan = false;

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");

  JobOptsSvc *jobopt_svc = JobOptsSvc::instance();
  jobopt_svc->init("default.opts");

  GeomSvc *geom_svc = GeomSvc::instance();

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(100);

  // particle gun
  if(gen_gun) {
    PHG4ParticleGun *gun = new PHG4ParticleGun("GUN");
    gun->set_name("mu+");
    gun->set_vtx(0, 0, target_coil_pos_z);
    gun->set_mom(3, 0, 40);
    se->registerSubsystem(gun);
  }

  if(gen_pythia8) {
    gSystem->Load("libPHPythia8.so");

    PHPythia8 *pythia8 = new PHPythia8();
    pythia8->set_config_file("phpythia8_DY.cfg");
    pythia8->set_vertex_distribution_mean(0, 0, target_coil_pos_z, 0);
    se->registerSubsystem(pythia8);

    pythia8->set_trigger_AND();

    PHPy8ParticleTrigger* trigger_mup = new PHPy8ParticleTrigger();
    trigger_mup->AddParticles("-13");
    //trigger_mup->SetPxHighLow(7, 0.5);
    //trigger_mup->SetPyHighLow(6, -6);
    trigger_mup->SetPzHighLow(120, 10);
    pythia8->register_trigger(trigger_mup);

    PHPy8ParticleTrigger* trigger_mum = new PHPy8ParticleTrigger();
    trigger_mum->AddParticles("13");
    //trigger_mum->SetPxHighLow(-0.5, -7);
    //trigger_mum->SetPyHighLow(6, -6);
    trigger_mum->SetPzHighLow(120, 10);
    pythia8->register_trigger(trigger_mum);

    HepMCNodeReader *hr = new HepMCNodeReader();
    hr->set_particle_filter_on(true);
    hr->insert_particle_filter_pid(13);
    hr->insert_particle_filter_pid(-13);
    se->registerSubsystem(hr);
  }

  if(gen_scan) {
    PHG4PSScanGenerator *gen = new PHG4PSScanGenerator("MUP");
    gen->add_particles("mu+", nmu);  // mu+,e+,proton,pi+,Upsilon
    gen->set_vertex_distribution_function(PHG4PSScanGenerator::Uniform,
        PHG4PSScanGenerator::Uniform,
        PHG4PSScanGenerator::Uniform);
    gen->set_vertex_distribution_mean(0.0, 0.0, target_coil_pos_z);
    gen->set_vertex_distribution_width(0.0, 0.0, 0.0);
    gen->set_vertex_size_function(PHG4PSScanGenerator::Uniform);
    gen->set_vertex_size_parameters(0.0, 0.0);

    //gen->set_pxpypz_range(1, 4, -1, 1, 30, 60);
    //gen->set_pxpypz_range(1, 4, -1, 1, 50, 51);
    gen->set_pxpypz_range(1, 1.6, 0.7, 1, 50, 51);
		gen->set_px_step(0.3);
		gen->set_py_step(0.3);
		gen->set_pz_step(3);

    gen->Verbosity(0);
    se->registerSubsystem(gen);
  }

  if(gen_particle) {
    PHG4SimpleEventGenerator *gen2 = new PHG4SimpleEventGenerator("MUP");
    gen2->add_particles("mu+", nmu);  // mu+,e+,proton,pi+,Upsilon
    gen2->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform);
    gen2->set_vertex_distribution_mean(0.0, 0.0, target_coil_pos_z);
    gen2->set_vertex_distribution_width(0.0, 0.0, 0.0);
    gen2->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
    gen2->set_vertex_size_parameters(0.0, 0.0);

    gen2->set_pxpypz_range(0,6, -6,6, 10,100);
    //gen2->set_pxpypz_range(1,4, -1,1, 30,60);

    //gen2->set_vertex_distribution_mean(10.4, 19.8, 614);
    //gen2->set_pxpypz_range(-1.28,-1.28, 0.79,0.79, 27.1,27.1);

    //gen2->set_vertex_distribution_mean(23.1, 8.5, 614);
    //gen2->set_pxpypz_range(-0.68,-0.68, 0.36,0.36, 28.4,28.4);

    gen2->Verbosity(0);
    se->registerSubsystem(gen2);
  }

  // Fun4All G4 module
  PHG4Reco *g4Reco = new PHG4Reco();
  //g4Reco->G4Seed(123);
  //g4Reco->set_field(5.);
  g4Reco->set_field_map(
      jobopt_svc->m_fMagFile+" "+
      jobopt_svc->m_kMagFile+" "+
			"1.0 1.0 0.0",
      4);
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

  PHG4E1039InsensSubsystem* insens = new PHG4E1039InsensSubsystem("Insens");
  g4Reco->registerSubsystem(insens);

  gROOT->LoadMacro("G4_Target.C");
  SetupTarget(g4Reco, do_collimator, do_target, target_coil_pos_z, target_l, target_z, use_g4steps);

  gROOT->LoadMacro("G4_SensitiveDetectors.C");
  SetupSensitiveDetectors(g4Reco);

  se->registerSubsystem(g4Reco);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  DPDigitizer *digitizer = new DPDigitizer("DPDigitizer", 0);
  se->registerSubsystem(digitizer);

  gSystem->Load("libktracker.so");
  KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
  //ktracker->Verbosity(10);
  //ktracker->set_DS_level(0);
  se->registerSubsystem(ktracker);

  ///////////////////////////////////////////
  // Output
  ///////////////////////////////////////////

  // save a comprehensive  evaluation file
  PHG4DSTReader *reader = new PHG4DSTReader(
      string("DSTReader.root"));
  reader->set_save_particle(true);
  reader->set_load_all_particle(false);
  reader->set_load_active_particle(true);
  reader->set_save_vertex(true);
  //reader->AddNode("Coil");
  //reader->AddNode("Target");
  //reader->AddNode("Collimator");
  reader->AddNode("C1X");
  reader->AddNode("C2X");
  se->registerSubsystem(reader);

  // input - we need a dummy to drive the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
  se->registerInputManager(in);

  //Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "DST.root");
  //se->registerOutputManager(out);

  // a quick evaluator to inspect on hit/particle/tower level

  if (nEvents > 0)
  {
    se->run(nEvents);

    //PHGeomUtility::ExportGeomtry(se->topNode(),"geom.root");

    // finish job - close and save output files
    se->End();
    se->PrintTimer();
    std::cout << "All done" << std::endl;

    // cleanup - delete the server and exit
    delete se;
    gSystem->Exit(0);
  }
  return;
}

PHG4ParticleGun *get_gun(const char *name = "PGUN")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4ParticleGun *pgun = (PHG4ParticleGun *) se->getSubsysReco(name);
  return pgun;
}
