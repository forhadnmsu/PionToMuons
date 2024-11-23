#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TSystem.h>

void ana(
		const char* in = "DSTReader.root",
		const char* out = "hist.root"
		){

	gSystem->Load("libg4testbench");

	TFile *outf = TFile::Open(out, "recreate");

	TFile *inf = TFile::Open(in, "read");
	TTree *T = (TTree*) inf->Get("T");

	//T->SetAlias("avg_x", "0.5*(G4HIT_Coil.x[][0]+G4HIT_Coil.x[][1])");
	//T->SetAlias("avg_y", "0.5*(G4HIT_Coil.y[][0]+G4HIT_Coil.y[][1])");
	//T->SetAlias("avg_z", "0.5*(G4HIT_Coil.z[][0]+G4HIT_Coil.z[][1])");
	//T->SetAlias("edep", "G4HIT_Coil.edep");

	T->SetAlias("avg_x", "G4HIT_Coil.get_avg_x()");
	T->SetAlias("avg_y", "G4HIT_Coil.get_avg_y()");
	T->SetAlias("avg_z", "G4HIT_Coil.get_avg_z()");
	T->SetAlias("avg_r", "sqrt(avg_x*avg_x+avg_z*avg_z)");
	T->SetAlias("avg_phi", "TMath::ATan2(-avg_x,avg_z)/TMath::Pi()*180");
	T->SetAlias("edep", "G4HIT_Coil.get_edep()");

	TH2D *h2d_ezy = new TH2D("h2d_ezy","edep [GeV]; avg_z [cm]; avg_y [cm]", 50, -25, 25, 50, -25, 25);
	T->Project("h2d_ezy", "avg_y:avg_z", "edep");
	outf->cd();
	h2d_ezy->Write();

	TH2D *h2d_ezy_0 = new TH2D("h2d_ezy_0","edep [GeV], |x|<0.5cm; Z [cm]; Y [cm]", 50, -25, 25, 50, -25, 25);
	T->Project("h2d_ezy_0", "avg_y:avg_z", "edep*(abs(avg_x)<0.5)");
	outf->cd();
	h2d_ezy->Write();

	double ybin[51];
	for(int i=0;i<51;++i) ybin[i] = i - 25.;

	//for(int i=27;i<28;++i) {
	for(int i=0;i<50;++i) {
		const char* hname = Form("h2d_ezx_%02d",i);
		TH2D *h2d_ezx = new TH2D(hname,
				Form("edep [GeV], %02.0f<avg_y<%02.0f cm; avg_z [cm]; avg_x [cm]", ybin[i], ybin[i+1]),
				50, -25, 25, 50, -25, 25);
		T->Project(hname, "avg_x:avg_z" , Form("edep*(avg_y>%f&&avg_y<%f)",ybin[i],ybin[i+1]));
		outf->cd();
		h2d_ezx->Write();
	}

	TH2D *h2d_ephiy = new TH2D("h2d_ephiy","edep [GeV]; #phi [deg.]; avg_y [cm]", 38, -180, 180, 50, -25, 25);
	T->Project("h2d_ephiy", "avg_y:avg_phi", "edep*(avg_r<7)");
	outf->cd();
	h2d_ephiy->Write();

	outf->Close();
}
