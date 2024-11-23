void int_run() {
	gROOT->ProcessLine(".x Fun4Test.C\(-1\)");
	gROOT->ProcessLine(".L DisplayOn.C");
	PHG4Reco* g4 = DisplayOn();
	Fun4AllServer *se = Fun4AllServer::instance();
  g4->ApplyCommand("/vis/viewer/set/viewpointThetaPhi 90 91");
	se->run(1);

  g4->ApplyCommand("/vis/viewer/panTo 2000 0 cm");
  g4->ApplyCommand("/vis/viewer/zoom 4");

	//g4->ApplyCommand("/vis/viewer/zoom 0.5");
	//g4->ApplyCommand("/vis/viewer/set/background white");
	//g4->ApplyCommand("/vis/scene/add/axes 0 0 0 14. cm");
	//g4->ApplyCommand("/vis/modeling/trajectories/generic-0/default/setAuxPtsVisible true");
	//g4->ApplyCommand("/tracking/verbose 100");
	se->End();
}
