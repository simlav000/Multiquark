#ifndef CUTS_H
#define CUTS_H

#include <TCut.h>

// ##########################
// # Cuts common to all V0s #
// ##########################
inline float V0chi2_high = 15;
inline TCut cut_on_V0chi2 = Form("chi2ndf < %f", V0chi2_high);

// ##########################
// #     Cuts for Kaons     #
// ##########################

// mm
inline float KDeltaRxy_low = 4;
inline float KDeltaRxy_high = 450;

// MeV
inline float KpT_low = 300;
inline float K_LmassBand1_low = 0;
inline float K_LmassBand1_high = 1105;
inline float K_LmassBand2_low = 1125;

inline float KcosTheta_3D_low = 0.9990;

inline TCut cut_on_KDeltaRxy    = Form("DeltaR > %f && DeltaR < %f", KDeltaRxy_low, KDeltaRxy_high);
inline TCut cut_on_KpT          = Form("Pt > %f", KpT_low);
inline TCut cut_on_KcosTheta_3D = Form("CosTheta > %f", KcosTheta_3D_low);
inline TCut cut_on_K_LmassBand1 = Form("LMass > %f && LMass < %f", K_LmassBand1_low, K_LmassBand1_high);
inline TCut cut_on_K_LmassBand2 = Form("LMass > %f", K_LmassBand2_low);
inline TCut cut_on_K_Lmass      = cut_on_K_LmassBand1 || cut_on_K_LmassBand2;

// Kmass signal region: PDG(497.611) +- 20
inline float Kmass_signal_low  = 477.611;    // PDG -20
inline float Kmass_signal_high = 517.611;    // PDG +20

inline TCut cut_on_Kmass_signal = Form("KMass > %f && KMass < %f", Kmass_signal_low, Kmass_signal_high);

inline TCut K_candidate_cuts = cut_on_V0chi2 && cut_on_KDeltaRxy && cut_on_KpT && cut_on_KcosTheta_3D && cut_on_K_Lmass; // for plotting K mass distribution
inline TCut K_signal_cuts    = K_candidate_cuts && cut_on_Kmass_signal; // for getting kaons in mass signal region

// ##########################
// #    Cuts for Lambdas    #
// ##########################
inline float k = 1;

// All cuts except mass signal cuts are common to both L and LB
inline float LDeltaRxy_low = 17;
inline float LDeltaRxy_high = 450;
inline float LpT_low = 500;
inline float LcosTheta_3D_low = 0.9998;
inline float L_KmassBand1_low = 340;
inline float L_KmassBand1_high = 480;
inline float L_KmassBand2_low = 520;


inline TCut cut_on_LDeltaRxy    = Form("DeltaR > %f && DeltaR < %f", LDeltaRxy_low, LDeltaRxy_high);
inline TCut cut_on_LpT          = Form("Pt > %f", LpT_low);
inline TCut cut_on_LcosTheta_3D = Form("CosTheta > %f", LcosTheta_3D_low);
inline TCut cut_on_L_KmassBand1 = Form("KMass > %f && KMass < %f", L_KmassBand1_low, L_KmassBand1_high);
inline TCut cut_on_L_KmassBand2 = Form("KMass > %f", L_KmassBand2_low);
inline TCut cut_on_L_Kmass      = cut_on_L_KmassBand1 || cut_on_L_KmassBand2;

// Lmass signal region: PDG(1115.683) +- 7
inline float Lmass_signal_low   = 1108.683;    // PDG -7  
inline float Lmass_signal_high  = 1122.683;    // PDG +7

inline TCut cut_on_Lmass_signal = Form("LMass > %f && LMass < %f", Lmass_signal_low, Lmass_signal_high);   
inline TCut cut_on_LBmass_signal = Form("LBarMass > %f && LBarMass < %f", Lmass_signal_low, Lmass_signal_high);


inline TCut L_LB_candidate_cuts = cut_on_V0chi2 && cut_on_LDeltaRxy && cut_on_LpT && cut_on_LcosTheta_3D && cut_on_L_Kmass; // for plotting L or LB mass distribution
inline TCut L_signal_cuts    = L_LB_candidate_cuts && cut_on_Lmass_signal; // for getting lambdas in mass signal region
inline TCut LB_signal_cuts   = L_LB_candidate_cuts && cut_on_LBmass_signal; // for getting lambdabars in mass signal region

#endif // CUTS_H
