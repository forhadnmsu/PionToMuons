using namespace std;

double ratio_error(
    const double a,
    const double b,
    const double ea,
    const double eb
    ) {
  double r = a/b;
  double er = r*sqrt(
      (ea/a)*(ea/a) + (eb/b)*(eb/b)
      );
  return er;
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
  TH1D *h = h1->Clone(hname);
  int nbin = h->GetNbinsX();
  for(int ibin=1; ibin<=nbin; ++ibin) {
    double a = h1->GetBinContent(ibin);
    double b = h2->GetBinContent(ibin);
    double r = a/b;
    double e = binom_error(a, b);
    h->SetBinContent(ibin, r);
    h->SetBinError(ibin, e);
  }

  return h;
}
namespace {
  int nfiles = 4;
  char* inputs [] = {
    //"eval_gap_0.root",
    "eval_gap_1.root",
    "eval_gap_2.root",
    "eval_gap_5.root",
    "eval_gap_10.root"
  };

  float gap_size[] = { 1, 2, 5, 10};
  float gap_size_error[] = {0, 0, 0, 0, 0, 0, 0, 0};
}

void drawRelAcc() {

  float mean[] = {0, 0, 0, 0, 0, 0, 0, 0};
  float mean_error[] = {0, 0, 0, 0, 0, 0, 0, 0};
  float mean0 = 1;
  float mean_error0 = 0;
  for (int i=0; i<nfiles; ++i) {
    TFile *f = TFile::Open(inputs[i],"read");
    TH1D *h = new TH1D("h","h",3,0,3);
    //T->Project("h","Sum$(gnhodo>=8)>=2");
    T->Project("h","Sum$(ntruhits>0)>=2");
    mean[i] = h->GetMean();
    mean_error[i] = h->GetMeanError();
    if(i==0) {
      mean0 = mean[i];
      mean_error0 = mean_error[i];
    }
    mean[i] = mean[i]/mean0;
    mean_error[i] = mean_error[i]/mean0;
  }

  TCanvas *drawRelAcc_c0 = new TCanvas("drawRelAcc_c0","drawRelAcc_c0");
  drawRelAcc_c0->SetGrid();

  TGraphErrors* gr = new TGraphErrors(nfiles, gap_size, mean, gap_size_error, mean_error);
  gr->SetTitle(";H1 gap size (cm); Drell-Yan Acc.");
  gr->SetMarkerStyle(20);
  gr->Draw("ap");
  gr->GetXaxis()->SetRangeUser(-5,11);

  float data_mean[] = {1, 0.982, 0.954, 0.917};
  float data_gap[]  = {1, 2,     4,     6};
  TGraph* data_gr = new TGraph(4, data_gap, data_mean);
  data_gr->SetMarkerStyle(4);
  data_gr->SetMarkerColor(kRed);
  data_gr->Draw("same,p");
}

void drawAccPhi() {
  TCanvas *c0 = new TCanvas("c0","c0"); c0->SetGrid();
  TCanvas *c1 = new TCanvas("c1","c1"); c1->SetGrid();
  TCanvas *c2 = new TCanvas("c2","c2"); c2->SetGrid();
  TCanvas *c3 = new TCanvas("c3","c3"); c3->SetGrid();

  float mod[] = {0, 0, 0, 0, 0, 0, 0, 0};
  float mod_error[] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (int i=0; i<nfiles; ++i) {
    TFile *f = TFile::Open(inputs[i],"read");
    TH1D *hnum = new TH1D("hnum","hnum",16, -3.14, 3.14);
    TH1D *hden = new TH1D("hden","hden",16, -3.14, 3.14);

    //T->Project("hnum","dimu_gphi","dimu_nrec>=2");
    T->Project("hnum","dimu_gphi","Sum$(gnhodo>=8)>=2");
    T->Project("hden","dimu_gphi");

    TH1D* hrat = getEffHist("hrat", hnum, hden);
    //TH1D* hrat = hden->Clone("hrat");;
    //TH1D* hrat = hnum->Clone("hrat");;

    hnum->SetMarkerStyle(20);
    hden->SetMarkerStyle(20);
    hrat->SetMarkerStyle(20);

    int color = kBlack;

    if(i==0) {

      color = kBlack;

      c0->cd();
      hnum->SetTitle("nDimu-trig. vs. #phi; #phi [rad]; nDimu-trig.");
      hnum->SetMarkerColor(color);
      hnum->SetLineColor(color);
      hnum->SetMinimum(0);
      hnum->Draw("e");

      c1->cd();
      hden->SetTitle("nDimu-gen. vs. #phi; #phi [rad]; nDimu-gen.");
      hden->SetMarkerColor(color);
      hden->SetLineColor(color);
      hden->SetMinimum(0);
      hden->Draw("e");

      c2->cd();
      hrat->SetTitle("Acc. vs. #phi; #phi [rad]; Acc.");
      hrat->SetMarkerColor(color);
      hrat->SetLineColor(color);
      hrat->SetMinimum(0);
      hrat->SetMaximum(0.1);
      hrat->Draw("e");
      //hrat->SetStats(0);
      TF1 *cos = new TF1("cos","[0]*cos(2*x)+[1]");
      cos->SetLineColor(color);
      hrat->Fit("cos");
      mod[i] = cos->GetParameter(0);
      mod_error[i] = cos->GetParError(0);
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
      TF1 *cos = new TF1("cos","[0]*cos(2*x)+[1]");
      cos->SetLineColor(color);
      hrat->Fit("cos");
      mod[i] = cos->GetParameter(0);
      mod_error[i] = cos->GetParError(0);
    }
  }


  c3->cd();
  TGraphErrors* gr = new TGraphErrors(nfiles, gap_size, mod, gap_size_error, mod_error);
  gr->SetTitle(";H1 gap size (cm); Cos(2x) Mod. Str.");
  gr->SetMarkerStyle(20);
  gr->Draw("ap");
  gr->GetXaxis()->SetRangeUser(-5,11);
  gr->GetYaxis()->SetRangeUser(-0.01, 0);

}

void ana() {
  gStyle->SetOptFit();

  drawRelAcc();
  //drawAccPhi();
}
