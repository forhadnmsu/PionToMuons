/*
 * TruthEval.h
 *
 *  Created on: Oct 29, 2017
 *      Author: yuhw@nmsu.edu
 */

#ifndef _H_TruthEval_H_
#define _H_TruthEval_H_

// Fun4All includes
#include <fun4all/SubsysReco.h>

// STL includes
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <map>

class PHG4TruthInfoContainer;
class PHG4HitContainer;

class TFile;
class TTree;

class TClonesArray;

class TruthEval: public SubsysReco {

public:

	TruthEval(const std::string &name = "TruthEval", const std::string &out = "eval.root");
	virtual ~TruthEval() {
	}

	int Init(PHCompositeNode *topNode);
	int InitRun(PHCompositeNode *topNode);
	int process_event(PHCompositeNode *topNode);
	int End(PHCompositeNode *topNode);

	int InitEvalTree();
	int ResetEvalVars();

	std::string _out_name;

	float target_angle; // deg
	float target_r;
	float target_l;
	float target_z;

	float coil_in_r;
	float coil_ot_r;
	float coil_min_y_0;
	float coil_max_y_0;
	float coil_min_y_1;
	float coil_max_y_1;


private:

	int GetNodes(PHCompositeNode *topNode);

	int _event;


	PHG4TruthInfoContainer *_g4truth_container;

	PHG4HitContainer *_g4hit_coil;

	TTree *_tout;
	TClonesArray *_tca_truthtracks;

	TTree *_tout1;
	float _total_edep;
};


#endif /* _H_TruthEval_H_ */
