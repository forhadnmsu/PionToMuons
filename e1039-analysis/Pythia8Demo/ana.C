using namespace std;
void ana() {
  TFile *f = TFile::Open("DSTReader.root");
  TTree *T = (TTree*) f->Get("T");

  TH1D *h1_mass = new TH1D("h1_mass","Di-muon inv. mass; m_{#mu^{+}#mu^{-}} [GeV]; Events/100MeV",100, 0, 10);

  int n_PHG4Particle;
  TClonesArray *particles;

  T->SetBranchAddress("n_PHG4Particle", &n_PHG4Particle);
  T->SetBranchAddress("PHG4Particle", &particles);

  for(int ientry=0; ientry<T->GetEntries(); ++ientry) {
    T->GetEntry(ientry);
    //cout<< ientry << ": " << n_PHG4Particle << endl;
    //particles->Print();
    int mup_idx = -999;
    int mum_idx = -999;
    for(int ipar=0; ipar<n_PHG4Particle; ++ipar) {
      PHG4Particle* particle = (PHG4Particle*) particles->At(ipar);
      if(particle->get_pid()==13) mum_idx = ipar;
      if(particle->get_pid()==-13) mup_idx = ipar;
      //cout
      //  << particle->get_track_id() << ": "
      //  << particle->get_parent_id() << ": "
      //  << particle->get_name() << ": " 
      //  << particle->get_pid()<< endl;
    }

    if(mup_idx > 0 && mum_idx > 0) {
      TLorentzVector mup;
      PHG4Particle* particle = (PHG4Particle*) particles->At(mup_idx);
      mup.SetPxPyPzE(
          particle->get_px(),
          particle->get_py(),
          particle->get_pz(),
          particle->get_e()
          );
      TLorentzVector mum;
      particle = (PHG4Particle*) particles->At(mum_idx);
      mum.SetPxPyPzE(
          particle->get_px(),
          particle->get_py(),
          particle->get_pz(),
          particle->get_e()
          );
      TLorentzVector mass = mup + mum;
      h1_mass->Fill(mass.Mag());
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1");
  c1->SetLogy();

  h1_mass->SetLineColor(kBlack);
  h1_mass->SetMarkerStyle(20);
  h1_mass->Draw("e");
}
