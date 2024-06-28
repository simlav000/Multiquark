#ifndef CUTS_H
#define CUTS_H

#include <TCut.h>

// ##########################
// # Cuts common to all V0s #
// ##########################
extern float V0chi2_high;
extern TCut cut_on_V0chi2;
extern TCut no_cut;

// ##########################
// #     Cuts for Kaons     #
// ##########################

// mm
extern float KDeltaRxy_low;
extern float KDeltaRxy_high;

// MeV
extern float KpT_low;
extern float K_LmassBand1_low;
extern float K_LmassBand1_high;
extern float K_LmassBand2_low;

extern float KcosTheta_3D_low;

extern TCut cut_on_KDeltaRxy;
extern TCut cut_on_KpT;
extern TCut cut_on_KcosTheta_3D;
extern TCut cut_on_K_LmassBand1;
extern TCut cut_on_K_LmassBand2;
extern TCut cut_on_K_Lmass;

// Kmass signal region: PDG(497.611) +- 20
extern float Kmass_PDG;
extern float Kmass_signal_low;    // PDG -20
extern float Kmass_signal_high;    // PDG +20

extern TCut cut_on_Kmass_signal;

extern TCut KCut2;
extern TCut KCut3;
extern TCut cut_on_KcosTheta_and_pT;
extern TCut K_candidate_cuts; // for plotting K mass distribution
extern TCut K_signal_cuts; // for getting kaons in mass signal region

// ##########################
// #    Cuts for Lambdas    #
// ##########################
extern float k;

// All cuts except mass signal cuts are common to both L and LB
extern float LDeltaRxy_low;
extern float LDeltaRxy_high;
extern float LpT_low;
extern float LcosTheta_3D_low;
extern float L_KmassBand1_low;
extern float L_KmassBand1_high;
extern float L_KmassBand2_low;


extern TCut cut_on_LDeltaRxy;
extern TCut cut_on_LpT;
extern TCut cut_on_LcosTheta_3D;
extern TCut cut_on_L_KmassBand1;
extern TCut cut_on_L_KmassBand2;
extern TCut cut_on_L_Kmass;

// Lmass signal region: PDG(1115.683) +- 7
extern float Lmass_PDG;
extern float Lmass_signal_low;    // PDG -7  
extern float Lmass_signal_high;    // PDG +7

extern TCut cut_on_Lmass_signal;   
extern TCut cut_on_LBmass_signal;


extern TCut L_LB_candidate_cuts; // for plotting L or LB mass distribution
extern TCut L_signal_cuts; // for getting lambdas in mass signal region
extern TCut LB_signal_cuts; // for getting lambdabars in mass signal region

extern std::string floatToString(float v, int p);
extern void SetCutNames();

#endif // CUTS_H
