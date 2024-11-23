
#include <iostream>

using namespace std;

int Fun4E1039Shielding(
    const int nEvents = 1,
    const double hodo_gap = 0.,
    const int do_e1039_shielding = 1,
    const double target_coil_pos_z = -300
    )
{
  const bool do_collimator = true;
  const bool do_target = true;
  const double target_l = 7.9; //cm
  const double target_z = (7.9-target_l)/2.; //cm
  const int use_g4steps = 1;

  const bool gen_gun = false;
  const bool gen_pythia8 = true;
  const bool gen_test = false;
  const bool gen_particle = false;

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");

  JobOptsSvc *jobopt_svc = JobOptsSvc::instance();
  jobopt_svc->init("default.opts");

  GeomSvc *geom_svc = GeomSvc::instance();
  geom_svc->setDetectorY0("H1T", 35.+hodo_gap/2.); //orig. ~  35 cm
  geom_svc->setDetectorY0("H1B", -35.-hodo_gap/2.);//orig. ~ -35 cm

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  // particle gun
  if(gen_gun) {
    PHG4ParticleGun *gun_beam = new PHG4ParticleGun("GUN_beam");
    gun_beam->set_name("proton");//proton
    gun_beam->set_vtx(0, 0, -400); //-363.32 cm
    gun_beam->set_mom(0, 0, 120);
    TF2 *beam_profile = new TF2("beam_profile",
        //"(((x**2+y**2)<=0.81)*exp(-(x**2+y**2)/0.18))+(((x**2+y**2)>0.81&&(x**2+y**2)<=25&&abs(y)<1.)*0.9*exp(-4.5)/(sqrt(x**2+y**2)))",
        "(((x**2+y**2)<=0.81)*exp(-(x**2+y**2)/0.18))+(((x**2+y**2)>0.81&&(x**2+y**2)<=25)*0.9*exp(-4.5)/(sqrt(x**2+y**2)))",
        -5,5,-5,5);
    //gun_beam->set_beam_profile(beam_profile);
    //se->registerSubsystem(gun_beam);
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

  if(gen_test) {
    PHG4ParticleGun *gun_mup = new PHG4ParticleGun("GUN_mup");
    gun_mup->set_name("mu+");
    //gun_mup->set_vtx(30, 0, 500);
    //gun_mup->set_mom(0, 0, 30.);
    gun_mup->set_vtx(0., 0., target_coil_pos_z);
    gun_mup->set_mom(3., 0.2, 40.);
    se->registerSubsystem(gun_mup);

    PHG4ParticleGun *gun_mum = new PHG4ParticleGun("GUN_mum");
    gun_mum->set_name("mu-");
    //gun_mum->set_vtx(-30, 0, 500);
    //gun_mum->set_mom(0., 0., 30.);
    gun_mum->set_vtx(0., 0., target_coil_pos_z);
    gun_mum->set_mom(-3., -0.2, 40.);
    se->registerSubsystem(gun_mum);
  }

  if(gen_particle) {
    // toss low multiplicity dummy events
    PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator();
    gen->add_particles("mu+", 1);  // mu+,e+,proton,pi+,Upsilon

    gen->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform);
    gen->set_vertex_distribution_mean(0.0, 0.0, target_coil_pos_z);
    gen->set_vertex_distribution_width(0.0, 0.0, 0.0);

    gen->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
    gen->set_vertex_size_parameters(0.0, 0.0);

    gen->set_eta_range(2, 4);
    gen->set_phi_range(-1.0 * TMath::Pi(), 1.0 * TMath::Pi());
    //gen->set_pt_range(1.0, 3.0);
    //gen->set_p_range(20, 25);

    //gen->set_pxpypz_range(2, 3.3, -0.5, 0.5, 20, 21);
    //gen->set_pxpypz_range(1, 4, -1, 1, 15, 60);
    gen->set_pxpypz_range(1, 4, -1, 1, 30, 60);

    //gen->Embed(2);
    gen->Verbosity(0);

    se->registerSubsystem(gen);
  }

  // Fun4All G4 module
  PHG4Reco *g4Reco = new PHG4Reco();
	g4Reco->Verbosity(1);
  //g4Reco->G4Seed(123);
  //g4Reco->set_field(5.);
  g4Reco->set_field_map(
      jobopt_svc->m_fMagFile+" "+
      jobopt_svc->m_kMagFile+" "+
			"1.0 1.0 5.0",
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

  se->registerSubsystem(g4Reco);
  
  if(do_e1039_shielding == 1) {
    const double inch = 2.54;

    PHG4SquareTubeSubsystem* shielding = NULL;

    shielding = new PHG4SquareTubeSubsystem("Shielding1",0);
    shielding->set_string_param("hole_type","circle");
    shielding->set_double_param("place_x",0);
    shielding->set_double_param("place_y",0);
    shielding->set_double_param("place_z", (-18*inch/2.-(2.15+11.38+ 36)*inch)); // I have added all the z length and put the z into the center of mass of the block.
    shielding->set_double_param("size_x",250*inch); //the info is not given?
    shielding->set_double_param("size_y",200*inch); //the info is not given?
    shielding->set_double_param("size_z",(18-0.001)*inch);
    shielding->set_double_param("inner_diameter",4*inch);
    shielding->set_string_param("material","G4_CONCRETE");
    g4Reco->registerSubsystem(shielding);

    shielding = new PHG4SquareTubeSubsystem("Shielding2",0);
    shielding->set_string_param("hole_type","circle");
    shielding->set_double_param("place_x",0);
    shielding->set_double_param("place_y",0);
    shielding->set_double_param("place_z",(-36*inch/2.-(2.15+11.38)*inch)); // I have added all the z length and put the z into the center of mass of the block.
    shielding->set_double_param("size_x",250*inch); //the info is not given?
    shielding->set_double_param("size_y",200*inch); //the info is not given?
    shielding->set_double_param("size_z",36*inch);
    shielding->set_double_param("inner_diameter",6*inch);
    shielding->set_string_param("material","G4_CONCRETE");
    g4Reco->registerSubsystem(shielding);

    shielding = new PHG4SquareTubeSubsystem("Shielding3",0);
    shielding->set_double_param("place_x",0);
    shielding->set_double_param("place_y",0);
    shielding->set_double_param("place_z",-11.38*inch/2.);
    shielding->set_double_param("size_x",50*inch);
    shielding->set_double_param("size_y",50*inch);
    shielding->set_double_param("size_z",(11.38-0.001)*inch);
    shielding->set_double_param("inner_size_x",6*inch);
    shielding->set_double_param("inner_size_y",6*inch);
    shielding->set_string_param("material","G4_CONCRETE");
    g4Reco->registerSubsystem(shielding);
  }

  PHG4E1039InsensSubsystem* insens = new PHG4E1039InsensSubsystem("Insens");
  g4Reco->registerSubsystem(insens);

  gROOT->LoadMacro("G4_Target.C");
  SetupTarget(g4Reco, do_collimator, do_target, target_coil_pos_z, target_l, target_z, use_g4steps);

  gROOT->LoadMacro("G4_SensitiveDetectors.C");
  SetupSensitiveDetectors(g4Reco);

  PHG4TruthSubsystem *truth = new PHG4TruthSubsystem();
  g4Reco->registerSubsystem(truth);

  DPDigitizer *digitizer = new DPDigitizer("DPDigitizer", 0);
  se->registerSubsystem(digitizer);

  gSystem->Load("libktracker.so");
  KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
  ktracker->set_geom_file_name("geom.root");
  ktracker->Verbosity(10);
  se->registerSubsystem(ktracker);

  ///////////////////////////////////////////
  // Output
  ///////////////////////////////////////////

  // save a comprehensive  evaluation file
  PHG4DSTReader *ana = new PHG4DSTReader(
      string("DSTReader.root"));
  ana->set_save_particle(true);
  ana->set_load_all_particle(false);
  ana->set_load_active_particle(true);
  ana->set_save_vertex(true);
  //ana->AddNode("Coil");
  //ana->AddNode("Target");
  //ana->AddNode("Collimator");
  //ana->AddNode("C1X");
  //ana->AddNode("C2X");
  se->registerSubsystem(ana);

  // input - we need a dummy to drive the event loop
  Fun4AllInputManager *in = new Fun4AllDummyInputManager("DUMMY");
  se->registerInputManager(in);

  //Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "DST.root");
  //se->registerOutputManager(out);

  // a quick evaluator to inspect on hit/particle/tower level

  if (nEvents > 0)
  {
    se->run(nEvents);

    PHGeomUtility::ExportGeomtry(se->topNode(),"geom.root");

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
