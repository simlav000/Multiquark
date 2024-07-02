// Custom includes
#include "Cuts.h"
#include "Particles.h"
#include "RtypesCore.h"

// ROOT includes
#include <TBranch.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMath.h>
#include <TStyle.h>
#include <TTree.h>

// C++ includes
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

std::string FillHist(std::string var_name, std::string hist_name) {
    std::string formatted = var_name + ">>" + hist_name;
    return formatted;
}

void printSignalArea(Double_t A, Double_t sigma) {
    Double_t sqrt_two_pi = sqrt(TMath::TwoPi());
    Double_t AreaEstimate = sqrt_two_pi * A * sigma;
    std::cout << "Area: " << AreaEstimate << std::endl;
}

void MakeKLMassHist(TTree* myTree) {

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    TH2F *hist = new TH2F("hist_KLMass", "K_{s} VS #Lambda mass", 50, 250, 1000, 50, 1000, 1600);

    gStyle->SetPalette(89); //kMint
    hist->SetStats(false);

    // Fill the histogram with data from the TTree
    myTree->Draw("LMass:KMass >> hist_KLMass", cut_on_KcosTheta_3D);

    // Set axis titles
    hist->GetYaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]");

    // Draw the histogram with a color map
    hist->Draw("COLZ");

    // Save the canvas as a PNG file
    canvas->SaveAs("KLMass2.png");

    // Clean up
    delete hist;
    delete canvas;
}


void MakeMassHist(Particle* p, TTree* myTree, int num_bins, TCut Cut1, TCut Cut2, TCut Cut3) {

    // MeV
    float mass_min = p->mass_min;
    float mass_max = p->mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = p->name_formatted + " Invariant Mass";

    TH1F* hist1 = new TH1F("hist_InvMass_Cut1", hist_name.c_str() , num_bins, mass_min, mass_max);
    TH1F* hist2 = new TH1F("hist_InvMass_Cut2", "", num_bins, mass_min, mass_max);
    TH1F* hist3 = new TH1F("hist_InvMass_Cut3", "", num_bins, mass_min, mass_max);

    hist1->SetFillColor(kViolet + 6);
    hist1->SetLineColor(kBlack);
    hist1->GetXaxis()->SetTitle((p->invariant_mass_label).c_str());
    hist1->GetYaxis()->SetTitle("Counts per bin");
    hist1->SetMinimum(0);   // Needed to see signal after cuts
    hist1->SetStats(false); // Get rid of stats box

    hist2->SetFillColor(kOrange + 10);
    hist2->SetLineColor(kBlack);

    hist3->SetFillColor(kSpring - 2);
    hist3->SetLineColor(kBlack);

    myTree->Draw(FillHist(p->mass, "hist_InvMass_Cut1").c_str(), Cut1, "hist"); 
    myTree->Draw(FillHist(p->mass, "hist_InvMass_Cut2").c_str(), Cut2, "hist same"); 
    myTree->Draw(FillHist(p->mass, "hist_InvMass_Cut3").c_str(), Cut3, "hist same"); 

    // Everything is more or less general except for the fitting section 
    // You'll have to edit this part depending on your fits and particles
    TF1 *Cut1Fit = new TF1("Cut1Fit", KMassFit, 1090, 1150, 6);
    TF1 *Cut2Fit = new TF1("Cut2Fit", KMassFit, 1090, 1150, 6);
    TF1 *Cut3Fit = new TF1("Cut3Fit", KMassFit, 1090, 1150, 6);

    Cut1Fit->SetParNames("A_1", "mu_1", "sigma_1", "c_1", "b_1", "a_1"); 
    Cut1Fit->SetParLimits(0, 0, 600000); // A > 0
    Cut1Fit->SetParameter(1, p->mass_pdg);
    Cut1Fit->SetParLimits(2, 0, 10000);    // sigma > 0
    Cut1Fit->SetLineColor(kMagenta + 4);
    Cut1Fit->SetLineWidth(2);

    Cut2Fit->SetParNames("A_2", "mu_2", "sigma_2", "c_2", "b_2", "a_2"); 
    Cut2Fit->SetParameter(0, 2400);
    Cut2Fit->SetParLimits(0, 0, 600000);
    Cut2Fit->SetParameter(1, p->mass_pdg);
    Cut2Fit->SetParLimits(1, 0, 2000);
    Cut2Fit->SetParameter(2, 3);
    Cut2Fit->SetParLimits(2, 1, 10000);
    Cut2Fit->SetLineColor(kOrange);
    Cut2Fit->SetLineWidth(2);

    Cut3Fit->SetParNames("A_3", "mu_3", "sigma_3", "c_3", "b_3", "a_3"); 
    Cut3Fit->SetParLimits(0, 0, 600000);
    Cut3Fit->SetParameter(1, p->mass_pdg);
    Cut3Fit->SetParLimits(2, 0, 100);
    Cut3Fit->SetLineColor(kGreen + 4);
    Cut3Fit->SetLineWidth(2);

    hist1->Fit("Cut1Fit", "R");
    hist2->Fit("Cut2Fit", "R");
    hist3->Fit("Cut3Fit", "R");

    Cut1Fit->SetNpx(1000);
    Cut3Fit->SetNpx(1000);
    Cut2Fit->SetNpx(1000);

    Double_t A_1     = Cut1Fit->GetParameter(0);
    Double_t sigma_1 = Cut1Fit->GetParameter(2);

    Double_t A_2     = Cut2Fit->GetParameter(0);
    Double_t sigma_2 = Cut2Fit->GetParameter(2);

    Double_t A_3     = Cut3Fit->GetParameter(0);
    Double_t sigma_3 = Cut3Fit->GetParameter(2);

    printSignalArea(A_1, sigma_1);
    printSignalArea(A_2, sigma_2);
    printSignalArea(A_3, sigma_3);

    Cut1Fit->Draw("SAME");
    Cut3Fit->Draw("SAME");
    Cut2Fit->Draw("SAME");


    // Absolute legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, Cut1.GetName(), "f");
    legend->AddEntry(Cut1Fit, "Full distribution fit", "l"); 
    legend->AddEntry(hist2, Cut2.GetName(), "f");
    legend->AddEntry(Cut2Fit, "Cosine cut fit", "l"); 
    legend->AddEntry(hist3, Cut3.GetName(), "f");
    legend->AddEntry(Cut3Fit, "All cuts fit", "l"); 
    legend->Draw();

    canvas->SaveAs("KMass.png");

    delete hist1;
    delete hist2;
    delete hist3;
    delete Cut1Fit;
    delete Cut2Fit;
    delete Cut3Fit;
    delete legend;
    delete canvas;
}

void MakeKLifeHist(TTree* myTree) {

    float t_min = 0.025e-9;
    float t_max = 0.22e-9;
    int num_bins = 100;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    TH1F* hist = new TH1F("hist_KLife", "K^{0}_{s} Lifetime", num_bins, t_min, t_max);
    hist->GetXaxis()->SetTitle("Time [s]"); 
    hist->GetYaxis()->SetTitle("Counts per bin");
    hist->SetMinimum(0);
    hist->SetStats(false);

    myTree->Draw("KLife>>hist_KLife", "", "hist"); 

    // Format: TF1("name", fit_func, lowlim, highlim, nparams)
    TF1 *KLifeFit = new TF1("KLifeFit", lifetime_fit_2exp, t_min, t_max, 5);

    // Set Parameter Names
    KLifeFit->SetParName(0, "C_0");  // Constant offset
    KLifeFit->SetParName(1, "C_b");  // Background amplitude
    KLifeFit->SetParName(2, "t_b");  // Background lifetime
    KLifeFit->SetParName(3, "C_K");  // Kaon amplitude
    KLifeFit->SetParName(4, "t_K");  // Kaon lifetime

    // Initial guesses
    KLifeFit->SetParameter(0, 16382);     // C_0
    KLifeFit->SetParameter(2, 2.5e-10);   // background lifetime PDG
    KLifeFit->SetParameter(4, 8.965e-11); // Kshort lifetime PDG

    // Limits on search
    KLifeFit->SetParLimits(1, 0, 1e+10);     // C_b > 0
    KLifeFit->SetParLimits(2, 1e-10, 1e-08); // t_b
    KLifeFit->SetParLimits(3, 0, 1e+10);     // C_K > 0
    KLifeFit->SetParLimits(4, 7e-11, 1e-10); // t_s 

    KLifeFit->SetLineColor(kBlue);
    KLifeFit->SetLineWidth(2);


    hist->Fit("KLifeFit", "R");

    KLifeFit->Draw("SAME");

    double c_0 = KLifeFit->GetParameter(0);
    double c_b = KLifeFit->GetParameter(1);
    double t_b = KLifeFit->GetParameter(2);
    double c_k = KLifeFit->GetParameter(3);
    double t_k = KLifeFit->GetParameter(4);

    // Use parameters of fit to plot individual contributions
    int n_pts = 1000;
    double t[n_pts];
    double y_background[n_pts];
    double y_signal[n_pts];
    double dt = (t_max - t_min) / (n_pts -  1);

    for (int i = 0; i < n_pts; i++) {
        t[i] = t_min + dt * i;
        y_background[i] = c_0 + c_b * TMath::Exp(-t[i]/t_b);
        y_signal[i] = c_k * TMath::Exp(-t[i]/t_k);
    }
    
    TGraph *background = new TGraph(n_pts, t, y_background);
    background->SetLineWidth(2);
    background->SetLineColor(kTeal);
    background->Draw("SAME");

    TGraph *signal = new TGraph(n_pts, t, y_signal);
    signal->SetLineWidth(2);
    signal->SetLineColor(kRed);
    signal->Draw("SAME");
        
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist, "Data", "f"); // f = fill 
    legend->AddEntry(KLifeFit, "Signal + Background", "l"); // l = line
    legend->AddEntry(background, "Background" , "l");
    legend->AddEntry(signal, "Signal", "l");
    legend->Draw();

    TLatex fitModel;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    fitModel.SetNDC(); 
    fitModel.SetTextSize(0.03);
    fitModel.DrawLatex(0.35, 0.7, "C_{0} + C_{b}e^{-t/#tau_{b}} + C_{s}e^{-t/#tau_{s}}");

    // Setting to log scale to easily see exponential region turn linear 
    //gPad->SetLogy();
    //canvas->Update();

    canvas->SaveAs("KLife.png");
    
    delete legend;
    delete KLifeFit;
    delete canvas;
    delete hist;

}

void MakeHists() {
    // Find ROOT file
    std::string home = std::getenv("HOME");
    std::string path = "/McGill/Multiquark/data/";
    std::string data = "dataset.root";
    std::string full = home + path + data;
    const char* name = full.c_str();
    
    // Open the ROOT file
    TFile *file = TFile::Open(name);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file" << std::endl;
        return;
    }

    // Get the TTree
    TTree *myTree = dynamic_cast<TTree*>(file->Get("myTree")); 
    if (!myTree) {
        std::cout << "Error: Could not get TTree" << std::endl;
        file->Close();
        delete file;
        return;
    }

    // See bottom of Cuts.h, associates a name to a cut to be printed in 
    // the histogram legends.
    SetCutNames();

    // See Particles.h, provides default data for kaon and lambda plots
    Kaon k;
    Lambda l;

    MakeMassHist(&l, myTree, 100, cut_on_KcosTheta_3D, KCut2, KCut3);
    //MakeLMassHist(myTree);
    //MakeKLMassHist(myTree);
    //MakeKLifeHist(myTree);
    
    // Clean up
    file->Close();
    delete file;
}


