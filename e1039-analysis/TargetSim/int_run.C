void int_run() {
	gROOT->ProcessLine(".x Fun4All_Test.C\(-1\)");
	//gROOT->ProcessLine(".x Fun4All_G4_E1039_R2.C\(-1\)");
	gROOT->ProcessLine(".L DisplayOn.C");
	PHG4Reco* g4 = DisplayOn();
	g4->ApplyCommand("/vis/modeling/trajectories/generic-0/default/setAuxPtsVisible true");
	g4->ApplyCommand("/vis/viewer/set/background white");
	g4->ApplyCommand("/vis/scene/add/axes 0 0 0 14. cm");
	g4->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 270 90");
	g4->ApplyCommand("/vis/viewer/zoom 0.5");
	g4->ApplyCommand("/tracking/verbose 100");
	Fun4AllServer *se = Fun4AllServer::instance();
	se->run(1);
	se->End();
}
