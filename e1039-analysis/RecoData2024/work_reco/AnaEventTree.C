R__LOAD_LIBRARY(RecoData2024)

void AnaEventTree(const char* dir_data_base="data/reco")
{
  TChain* tree = new TChain("tree");

  ifstream ifs("list_run_spill.txt");
  int run_id, spill_id;
  while (ifs >> run_id >> spill_id) {
    ostringstream oss;
    oss << setfill('0') << dir_data_base << "/run_" << setw(6) << run_id
        << "/spill_" << setw(9) << spill_id << "/out/output.root";
    if (gSystem->AccessPathName(oss.str().c_str())) continue;
    tree->Add(oss.str().c_str());
  }
  ifs.close();
  
  AnaDimuon::AnalyzeTree(tree);
  
  exit(0);
}
