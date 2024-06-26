#include "Cuts.h"
#include "Fits.h"
#include "TMath.h"

#include <TBranch.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TLatex.h>

#include <iostream>
#include <cstdlib>
#include <string>

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

void MakeKMassHist(TTree* myTree) {
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    TH1F* hist1 = new TH1F("hist_KMass", "K^{0}_{s} Invariant Mass", 200, 300, 650);
    TH1F* hist2 = new TH1F("hist_KMass_CosTheta_cut", "K_{s} mass (CosTheta cut)", 200, 300, 650);
    TH1F* hist3 = new TH1F("hist_KMass_AllCuts", "K_{s} mass (All cuts)", 200, 300, 650);

    hist1->SetFillColor(kViolet + 6);
    hist1->SetLineColor(kBlack);
    hist2->SetFillColor(kOrange + 10);
    hist2->SetLineColor(kBlack);
    hist3->SetFillColor(kSpring - 2);
    hist3->SetLineColor(kBlack);


    myTree->Draw("KMass>>hist_KMass", "", "hist"); 
    myTree->Draw("KMass>>hist_KMass_CosTheta_cut", cut_on_KcosTheta_3D, "hist same");
    myTree->Draw("KMass>>hist_KMass_AllCuts", K_signal_cuts, "hist same");

    hist1->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    hist1->GetYaxis()->SetTitle("Counts per bin");

    // Adjust y-axis range
    hist1->SetMinimum(0);

    // Get rid of little stats box
    hist1->SetStats(false);
    hist2->SetStats(false);
    hist3->SetStats(false);

    // Absolute legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, "Full distribution", "f");
    std::string label = "Cos(#theta) > " + std::to_string(KcosTheta_3D_low);
    const char* entry = label.c_str();
    legend->AddEntry(hist2, entry, "f");
    legend->AddEntry(hist3, "All cuts", "f");
    legend->Draw();

    canvas->SaveAs("KMass.png");

    delete hist1;
    delete hist2;
    delete hist3;
    delete legend;
    delete canvas;
}

void MakeLMassHist(TTree* myTree) {
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1080, 600);

    TH1F* hist1 = new TH1F("hist_LMass", "#Lambda Invariant Mass", 200, 1080, 1300);
    TH1F* hist2 = new TH1F("hist_LMass_CosTheta_cut", "#Lambda mass (CosTheta cut)", 200, 1080, 1300);
    TH1F* hist3 = new TH1F("hist_LMass_AllCuts", "#Lambda mass (All cuts)", 200, 1080, 1300);

    //hist->Sumw2();
    hist1->SetFillColor(kViolet + 6);
    hist1->SetLineColor(kBlack);
    hist2->SetFillColor(kOrange + 10);
    hist2->SetLineColor(kBlack);
    hist3->SetFillColor(kSpring - 2);
    hist3->SetLineColor(kBlack);


    myTree->Draw("LMass>>hist_LMass", "", "hist"); 
    myTree->Draw("LMass>>hist_LMass_CosTheta_cut", cut_on_LcosTheta_3D, "hist same");
    myTree->Draw("LMass>>hist_LMass_AllCuts", L_signal_cuts, "hist same");

    hist1->GetXaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]"); 
    hist1->GetYaxis()->SetTitle("Counts per bin");

    // Adjust y-axis range
    hist1->SetMinimum(0);

    // Get rid of little stats box
    hist1->SetStats(false);
    hist2->SetStats(false);
    hist3->SetStats(false);

    // Absolute legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, "Full distribution", "f");
    std::string label = "Cos(#theta) > " + std::to_string(LcosTheta_3D_low);
    const char* entry = label.c_str();
    legend->AddEntry(hist2, entry, "f"); // f = fill
    legend->AddEntry(hist3, "All cuts", "f");
    legend->Draw();

    canvas->SaveAs("LMass.png");

    delete hist1;
    delete hist2;
    delete legend;
    delete canvas;
}

void MakeKLifeHist(TTree* myTree) {

    float t_min = 0;
    float t_max = 0.7e-9;
    int num_bins = 100;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    TH1F* hist = new TH1F("hist_KLife", "K^{0}_{s} Lifetime", num_bins, t_min, t_max);
    hist->GetXaxis()->SetTitle("Time [s]"); 
    hist->GetYaxis()->SetTitle("Counts per bin");
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
        
    // Absolute legend
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

    //MakeKMassHist(myTree);
    //MakeLMassHist(myTree);
    //MakeKLMassHist(myTree);
    MakeKLifeHist(myTree);
    
    // Close the ROOT file
    file->Close();
    delete file;
}


