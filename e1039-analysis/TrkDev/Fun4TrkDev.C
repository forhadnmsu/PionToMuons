#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <phool/recoConsts.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4ParticleGeneratorBase.h>
#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4SimpleEventGenerator.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/HepMCNodeReader.h>
#include <g4main/PHG4TruthSubsystem.h>
#include <g4detectors/PHG4DetectorSubsystem.h>
#include <g4detectors/DPDigitizer.h>
//#include <phpythia6/PHPythia6.h>
#include <phpythia8/PHPythia8.h>
//#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <g4eval/PHG4DSTReader.h>
#include <jobopts_svc/JobOptsSvc.h>
#include <geom_svc/GeomSvc.h>
#include <module_example/TrkEval.h>

#include <TSystem.h>

#include "G4_SensitiveDetectors.C"
#include "G4_Target.C"

R__LOAD_LIBRARY(libfun4all)
R__LOAD_LIBRARY(libg4detectors)
R__LOAD_LIBRARY(libg4testbench)
R__LOAD_LIBRARY(libg4eval)
R__LOAD_LIBRARY(libktracker)
R__LOAD_LIBRARY(libPHPythia8)
//R__LOAD_LIBRARY(libembedding)
#endif

using namespace std;

int Fun4TrkDev(
    const int nEvents = 1,
  	const int nmu = 1,
    const double x0_shift = 0.0 //cm 
    )
{
  const double target_coil_pos_z = -300;
	int embedding_opt = 0;

  const bool do_collimator = false;
  const bool do_target = false;
  const double target_l = 7.9; //cm
  const double target_z = (7.9-target_l)/2.; //cm
  const int use_g4steps = 1;

  const bool gen_gun = false;
  const bool gen_pythia8 = false;
  const bool gen_test = false;
  const bool gen_particle = true;

  gSystem->Load("libfun4all");
  gSystem->Load("libg4detectors");
  gSystem->Load("libg4testbench");
  gSystem->Load("libg4eval");
	gSystem->Load("libktracker.so");

  JobOptsSvc *jobopt_svc = JobOptsSvc::instance();
  jobopt_svc->init("default.opts");

  GeomSvc *geom_svc = GeomSvc::instance();
  std::cout << "D2X::X0: " << geom_svc->getDetectorX0("D2X") << std::endl;
  geom_svc->setDetectorX0("D2X", geom_svc->getDetectorX0("D2X")+x0_shift);
  std::cout << "D2X::X0: " << geom_svc->getDetectorX0("D2X") << std::endl;

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
    PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator("MUP");
		//gen->set_seed(123);
    gen->add_particles("mu+", nmu);  // mu+,e+,proton,pi+,Upsilon
    gen->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform);
    gen->set_vertex_distribution_mean(0.0, 0.0, target_coil_pos_z);
    gen->set_vertex_distribution_width(0.0, 0.0, 0.0);
    gen->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
    gen->set_vertex_size_parameters(0.0, 0.0);

    //gen->set_phi_range(-1.0 * TMath::Pi(), 1.0 * TMath::Pi());
    //gen->set_eta_range(2, 4);

    gen->set_pxpypz_range(-0.2,-0.2, 0.4,0.4, 51,51);
    gen->set_vertex_distribution_mean(25, 8, 615);

    //gen->set_pxpypz_range(3,3, 0.5,0.5, 60,60);
    //gen->set_pxpypz_range(1,4, -1,1, 30,60);
    //gen->set_pxpypz_range(0,6, -6,6, 10,100);

    gen->Verbosity(0);
    se->registerSubsystem(gen);
  }

  if(gen_particle) {
    PHG4SimpleEventGenerator *gen2 = new PHG4SimpleEventGenerator("MUM");
    gen2->add_particles("mu-", nmu);  // mu+,e+,proton,pi+,Upsilon
    gen2->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform,
        PHG4SimpleEventGenerator::Uniform);
    gen2->set_vertex_distribution_mean(0.0, 0.0, target_coil_pos_z);
    gen2->set_vertex_distribution_width(0.0, 0.0, 0.0);
    gen2->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
    gen2->set_vertex_size_parameters(0.0, 0.0);

    //gen2->set_phi_range(-1.0 * TMath::Pi(), 1.0 * TMath::Pi());
    //gen2->set_eta_range(2, 4);
    gen2->set_pxpypz_range(-4, -1, -1, 1, 30, 60);
    gen2->Verbosity(0);
    //se->registerSubsystem(gen2);
  }

  // Fun4All G4 module
  PHG4Reco *g4Reco = new PHG4Reco();
  //PHG4Reco::G4Seed(123);
  //g4Reco->set_field(5.);
  g4Reco->set_field_map(
      jobopt_svc->m_fMagFile+" "+
      jobopt_svc->m_kMagFile+" "+
      "1.0 1.0 0.0",
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
  //digitizer->Verbosity(10);
  se->registerSubsystem(digitizer);

	gSystem->Load("libembedding.so");

#if ROOT_VERSION_CODE < ROOT_VERSION(6,00,0)
	if(embedding_opt == 1) {
		SRawEventEmbed *embed = new SRawEventEmbed("SRawEventEmbed");
		embed->set_in_name("digit_016070_R007.root");
		embed->set_in_tree_name("save");
		embed->set_trigger_bit((1<<0));
		//embed->set_in_name("random_run3a_1.root");
		//embed->set_in_tree_name("mb");
		//embed->set_trigger_bit((1<<7));
		embed->Verbosity(100);
		se->registerSubsystem(embed);
	} else if(embedding_opt == 2) {
		RndmEmbed *embed = new RndmEmbed("RndmEmbed");

		//high
		//double noise_rate_1 = 0.25;
		//double noise_rate_2 = 0.15;
		//double noise_rate_3 = 0.12;

		//medium
		double noise_rate_1 = 0.12;
		double noise_rate_2 = 0.10;
		double noise_rate_3 = 0.08;

		embed->set_noise_rate("D1X",   noise_rate_1);
		embed->set_noise_rate("D1U",   noise_rate_1);
		embed->set_noise_rate("D1V",   noise_rate_1);
		embed->set_noise_rate("D2Xp",  noise_rate_2);
		embed->set_noise_rate("D2U",   noise_rate_2);
		embed->set_noise_rate("D2V",   noise_rate_2);
		embed->set_noise_rate("D3pXp", noise_rate_3);
		embed->set_noise_rate("D3pUp", noise_rate_3);
		embed->set_noise_rate("D3pVp", noise_rate_3);
		embed->set_noise_rate("D3mXp", noise_rate_3);
		embed->set_noise_rate("D3mUp", noise_rate_3);
		embed->set_noise_rate("D3mVp", noise_rate_3);

		embed->print_noise_rate();

		se->registerSubsystem(embed);
	}
#endif


	gSystem->Load("libktracker.so");
	KalmanFastTrackingWrapper *ktracker = new KalmanFastTrackingWrapper();
	ktracker->Verbosity(10);
	ktracker->set_DS_level(0);
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
	ana->AddNode("C1X");
	ana->AddNode("C2X");
	se->registerSubsystem(ana);

	// input - we need a dummy to drive the event loop
	Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
	se->registerInputManager(in);

	Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "DST.root");
	se->registerOutputManager(out);

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
	return 0;
}

PHG4ParticleGun *get_gun(const char *name = "PGUN")
{
	Fun4AllServer *se = Fun4AllServer::instance();
	PHG4ParticleGun *pgun = (PHG4ParticleGun *) se->getSubsysReco(name);
	return pgun;
}
