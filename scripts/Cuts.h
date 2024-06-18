#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TCut.h>

// ##########################
// # Cuts common to all V0s #
// ##########################

float V0chi2_high = 15;
TCut cut_on_V0chi2 = Form("chi2ndf < %f", V0chi2_high);

// ##########################
// #     Cuts for Kaons     #
// ##########################

// mm
float KDeltaRxy_low = 4.0f;
float KDeltaRxy_high = 450.0f;

// MeV
float KpT_low = 300;
float K_LmassBand1_low = 0;
float K_LmassBand1_high = 1105;
float K_LmassBand2_low = 1125;

float KcosTheta_3D_low = 0.9990;

TCut cut_on_KDeltaRxy    = Form("DeltaR > %f && DeltaR < %f", KDeltaRxy_low, KDeltaRxy_high);
TCut cut_on_KpT          = Form("Pt > %f", KpT_low);
TCut cut_on_KcosTheta_3D = Form("CosTheta > %f", KcosTheta_3D_low);
TCut cut_on_K_LmassBand1 = Form("LMass > %f && LMass < %f", K_LmassBand1_low, K_LmassBand1_high);
TCut cut_on_K_LmassBand2 = Form("LMass > %f", K_LmassBand2_low);
TCut cut_on_K_Lmass      = cut_on_K_LmassBand1 || cut_on_K_LmassBand2;

// Kmass signal region: PDG(497.611) +- 20
float Kmass_signal_low  = 477.611;    // PDG -20
float Kmass_signal_high = 517.611;    // PDG +20

TCut cut_on_Kmass_signal = Form("KMass > %f && KMass < %f", Kmass_signal_low, Kmass_signal_high);

TCut K_candidate_cuts = cut_on_V0chi2 && cut_on_KDeltaRxy && cut_on_KpT && cut_on_KcosTheta_3D && cut_on_K_Lmass; // for plotting K mass distribution
TCut K_signal_cuts    = K_candidate_cuts && cut_on_Kmass_signal; // for getting kaons in mass signal region

