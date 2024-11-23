float phimin = 10000;
float phimax = -10000;
float x0min = 100000;
float x0max = -100000;
float lamda0min = 100000;
float lamda0max = -100000;

void plot_matscan_worker(
    const char *datfile = "matscan.dat",
    const double theta_req = -90,
    TGraph* &gr,
    TGraph* &grl
    )
{
  FILE *f = fopen(datfile,"r");
  if (!f)
  {
    cout << "file " << datfile << " cannot be opened" << endl;
    return;
  }
  float theta;
  float phi;
  float path;
  float x0;
  float lamda0;
  vector<float> phivec;
  vector<float> x0vec;
  vector<float> lamda0vec;

  while(fscanf(f,"%f %f %f %f %f",&theta,&phi,&path,&x0,&lamda0) != EOF)
  {
    //cout << "theta: " << theta
    //  << ", phi: " << phi
    //  << ", path: " << path
    //  << ", x0: " << x0
    //  << ", lamda0: " << lamda0
    //  << endl;

    if(theta_req>0 && fabs(theta - theta_req)>0.0001) continue;

    if (phi > phimax)
    {
      phimax = phi;
    }
    if (phi < phimin)
    {
      phimin = phi;
    }
    if (x0 > x0max)
    {
      x0max = x0;
    }
    if (x0 < x0min)
    {
      x0min = x0;
    }
    if (lamda0 > lamda0max)
    {
      lamda0max = lamda0;
    }
    if (lamda0 < lamda0min)
    {
      lamda0min = lamda0;
    }
    phivec.push_back(phi);
    x0vec.push_back(x0);
    lamda0vec.push_back(lamda0);
  }
  float *phiarr = new float[phivec.size()];
  float *x0arr = new float[x0vec.size()];
  float *lamda0arr = new float[lamda0vec.size()];
  for (int i=0; i<phivec.size(); i++)
  {
    phiarr[i] = phivec[i];
    x0arr[i] = x0vec[i];
    lamda0arr[i] = lamda0vec[i];
  }

  //TCanvas *c = new TCanvas("c1","material scan");
  //c->Divide(1,2);
  //c->cd(1);
  //TH2 *h2 = new TH2F("hx2","x0 vs phi",2,phimin-1,phimax+1,2,0,x0max+0.1*x0max);
  //h2->SetStats(kFALSE);
  //h2->Draw();
  //TGraph *gr = new TGraph(phivec.size(),phiarr,x0arr);
  //gr->SetMarkerStyle(21);
  //gr->SetMarkerSize(0.1);
  //gr->Draw("LP");
  //c->cd(2);  
  //TH2 *hl2 = new TH2F("hlamda2","lamda0 vs phi",2,phimin-1,phimax+1,2,0,lamda0max+lamda0max/10.);
  //hl2->SetStats(kFALSE);
  //hl2->Draw();
  //TGraph *grl = new TGraph(phivec.size(),phiarr,lamda0arr);
  //grl->SetMarkerStyle(21);
  //grl->SetMarkerSize(0.1);
  //grl->Draw("LP");

  gr = new TGraph(phivec.size(),phiarr,x0arr);
  gr->SetMarkerStyle(21);
  gr->SetMarkerSize(0.1);
  gr->SetLineWidth(2);

  grl = new TGraph(phivec.size(),phiarr,lamda0arr);
  grl->SetMarkerStyle(21);
  grl->SetMarkerSize(0.1);
  grl->SetLineWidth(2);

  fclose(f);
  return;
}

void plot_matscan(
    const char* datfile = "matscan.dat"
    ){

#define ntheta 6
  double thetas[ntheta] = {90, 80, 70, 60, 50, 40};
  TGraph *gr[ntheta];
  TGraph *grl[ntheta];

  for(int i=0;i<ntheta;i++)
  {
    plot_matscan_worker(datfile, thetas[i], gr[i], grl[i]);
  }

  TCanvas *c = new TCanvas("c1","material scan");
  c->Divide(1,2);
  c->cd(1)->SetGrid();
  TH2 *h2 = new TH2F("hx2","x0 vs phi",2,phimin-1,phimax+1,2,0,x0max+0.1*x0max);
  h2->SetStats(kFALSE);
  h2->Draw();
  c->cd(2)->SetGrid();
  TH2 *hl2 = new TH2F("hlamda2","lamda0 vs phi",2,phimin-1,phimax+1,2,0,lamda0max+lamda0max/10.);
  hl2->SetStats(kFALSE);
  hl2->Draw();

  for(int i=0;i<ntheta;i++)
  {
    c->cd(1);
    gr[i]->SetLineColor(i+1);
    gr[i]->Draw("LP");
    c->cd(2);
    grl[i]->SetLineColor(i+1);
    grl[i]->Draw("LP");
  }


  return;
}
