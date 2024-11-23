/*
 * TruthEval.h
 *
 *  Created on: Oct 29, 2017
 *      Author: yuhw@nmsu.edu
 */

#ifndef _H_TruthTrack_H_
#define _H_TruthTrack_H_

#include <TObject.h>

class TruthTrack : public TObject{
public:
	TruthTrack () :
		pid(0),
		vx(-9999), vy(-9999), vz(-9999), t(-9999),
		px(-9999), py(-9999), pz(-9999), e(-9999),
		pt(-9999), p(-9999), eta(-9999), mass(-9999),
		parentid(-9999),
		det_id(-9999),
		edep_coil(-9999),
		path_coil(-9999),
		edep_wire(-9999),
		path_wire(-9999)
		{}
	int pid;
	float vx, vy, vz, t;
	float px, py, pz, e;
	float pt, p, eta, mass;

	int parentid;
	int det_id; /// 0 : Target, 1: Coil, 9999: other
	float edep_coil;
	float path_coil;
	float edep_wire;
	float path_wire;

	ClassDef(TruthTrack, 1)
};

#endif
