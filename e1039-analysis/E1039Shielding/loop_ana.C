#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TLorentzVector.h>

#include <iostream>

using namespace std;

namespace {
  int nfiles = 1;
  const char* inputs [] = {
    "trk_eval.root"
  };

  float gap_size[] = {0, 1, 2, 5, 10};
  float gap_size_error[] = {0, 0, 0, 0, 0, 0, 0, 0};
}

double binom_error(
    const double a,
    const double b
    ){
  double r=a/b;
  return sqrt(r*(1-r)/b);
}

TH1D * getEffHist(
    const char* hname,
    const TH1D* h1,
    const TH1D* h2
    ) {
  TH1D *h = (TH1D*)h1->Clone(hname);
  int nbin = h->GetNbinsX();
  for(int ibin=1; ibin<=nbin; ++ibin) {
    double a = h1->GetBinContent(ibin);
    double b = h2->GetBinContent(ibin);

		if(b <= 0) {
			h->SetBinContent(ibin, 0);
			h->SetBinError(ibin, 0);
			continue;
		}

		double r = a/b;
		double e = binom_error(a, b);
		h->SetBinContent(ibin, r);
		h->SetBinError(ibin, e);
	}

	return h;
}

void drawAccMass() {
	TCanvas *c0 = new TCanvas("c0","c0"); c0->SetGrid();
	TCanvas *c1 = new TCanvas("c1","c1"); c1->SetGrid();
	TCanvas *c2 = new TCanvas("c2","c2"); c2->SetGrid();
	TCanvas *c3 = new TCanvas("c3","c3"); c3->SetGrid();

	float mod[] = {0, 0, 0, 0, 0, 0, 0, 0};
	float mod_error[] = {0, 0, 0, 0, 0, 0, 0, 0};
	TH1D* hrelrat0 = NULL;

	for (int i=0; i<nfiles; ++i) {
		float xbins[] = {4, 5, 7, 10, 20};
		TH1D *hnum = new TH1D("hnum","hnum",4, xbins);
		TH1D *hden = new TH1D("hden","hden",4, xbins);

		//
		// Loop a TTree in a file
		//

		TFile *f = TFile::Open(inputs[i],"read");
		TTree *T = (TTree*) f->Get("T");

#define MAX_PARTICLES 100
		int gnhodo[MAX_PARTICLES];
		float gpx[MAX_PARTICLES], gpy[MAX_PARTICLES], gpz[MAX_PARTICLES];
		int n_particles;

		T->SetBranchAddress("gnhodo", &gnhodo);
		T->SetBranchAddress("gpx", &gpx);
		T->SetBranchAddress("gpy", &gpy);
		T->SetBranchAddress("gpz", &gpz);
		T->SetBranchAddress("n_particles", &n_particles);

		float mu_mass = .105658;
		for(int ientry=0; ientry<T->GetEntries(); ++ientry) {
			T->GetEntry(ientry);

			if(n_particles!=2) { cout << "n_particles!=2 @ " << ientry  << endl; continue;}

			TLorentzVector mu0, mu1;
			mu0.SetXYZM(gpx[0], gpy[0], gpz[0], mu_mass);
			mu1.SetXYZM(gpx[1], gpy[1], gpz[1], mu_mass);

			TLorentzVector dimu = mu0+mu1;
			float dimu_gmass = dimu.M();

			hden->Fill(dimu_gmass);

			if(gnhodo[0]>=8 &&  gnhodo[1]>=8)
				hnum->Fill(dimu_gmass);
		}

		// End of loop

		TH1D* hrat = getEffHist("hrat", hnum, hden);
		TH1D* hrelrat = (TH1D*) hrat->Clone("hrelrat");

		hnum->SetMarkerStyle(20);
		hden->SetMarkerStyle(20);
		hrat->SetMarkerStyle(20);
		hrelrat->SetMarkerStyle(20);

		int color = kBlack;

		if(i==0) {

			color = kBlack;

			c0->cd();
			c0->SetLogy();
			hnum->SetTitle("nDimu-trig. vs. M_{#mu#mu}; M_{#mu#mu} [GeV/c^{2}]; nDimu-trig.");
			hnum->SetMarkerColor(color);
			hnum->SetLineColor(color);
			hnum->SetMinimum(1);
			hnum->Draw("e");
			hnum->SetStats(0);

			c1->cd();
			c1->SetLogy();
			hden->SetTitle("nDimu-gen. vs. M_{#mu#mu}; M_{#mu#mu} [GeV/c^{2}]; nDimu-gen.");
			hden->SetMarkerColor(color);
			hden->SetLineColor(color);
			hden->SetMinimum(1);
			hden->Draw("e");
			hden->SetStats(0);

			c2->cd();
			hrat->SetTitle("Acc. vs. M_{#mu#mu}; M_{#mu#mu} [GeV/c^{2}]; Acc.");
			hrat->SetMarkerColor(color);
			hrat->SetLineColor(color);
			hrat->SetMinimum(0.);
			hrat->SetMaximum(1.1);
			hrat->Draw("e");
			hrat->SetStats(0);

			hrelrat0 = (TH1D*) hrelrat->Clone("hrelrat0");
		}
		else {
			color = i+1;

			c0->cd();
			hnum->SetMarkerColor(color);
			hnum->SetLineColor(color);
			hnum->Draw("esame");

			c1->cd();
			hden->SetMarkerColor(color);
			hden->SetLineColor(color);
			hden->Draw("esame");

			c2->cd();
			hrat->SetMarkerColor(color);
			hrat->SetLineColor(color);
			hrat->Draw("esame");

			c3->cd();
			for(int ibin=1;ibin<=hrelrat->GetNbinsX();++ibin ){
				double relrat0_val = hrelrat0->GetBinContent(ibin);
				if(relrat0_val<=0) {
					hrelrat->SetBinContent(ibin,0);
					hrelrat->SetBinError(ibin,0);
					continue;
				}
				hrelrat->SetBinContent(ibin,
						hrelrat->GetBinContent(ibin)/relrat0_val);
				hrelrat->SetBinError(ibin,
						hrelrat->GetBinError(ibin)/relrat0_val);
			}
			hrelrat->SetTitle("Rel. Acc. vs. M_{#mu#mu}; M_{#mu#mu} [GeV/c^{2}]; Rel. Acc.");
			hrelrat->SetMarkerColor(color);
			hrelrat->SetLineColor(color);
			hrelrat->SetStats(0);
			if(i==1)
				hrelrat->Draw("e");
			else
				hrelrat->Draw("esame");
		}
	}
}

void loop_ana() {
	gStyle->SetOptFit();

	drawAccMass();
}
