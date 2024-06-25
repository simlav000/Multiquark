#include "Cuts.h"
#include "Fits.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TLegend.h>
#include <TStyle.h>
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
    // TODO: In Cuts.h, we make a cut on distance (Rxy). Figure out if this 
    // is sqrt(dx^2 + dy^2) or sqrt(dx^2 + dy^2 + dz^2) and what they mean
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

    // TODO: Figure out if this should be m_{p^{+}#pi^{-}}
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
    legend->AddEntry(hist2, entry, "f");
    legend->AddEntry(hist3, "All cuts", "f");
    legend->Draw();

    canvas->SaveAs("LMass.png");

    delete hist1;
    delete hist2;
    delete legend;
    delete canvas;
}

void MakeKLifeHist(TTree* myTree) {
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    TH1F* hist1 = new TH1F("hist_KLife", "K^{0}_{s} Lifetime", 100, 0, 1e-9);

    myTree->Draw("KLife>>hist_KLife", "", "hist"); 


    // Format: TF1("name", fit_func, lowlim, highlim, nparams)
    TF1 *KLifeFit = new TF1("KLifeFit", lifetime_fit_2exp, 0, 1e-09, 5);


    // Set Parameter Names
    KLifeFit->SetParName(0, "C0");
    KLifeFit->SetParName(1, "t_b");
    KLifeFit->SetParName(2, "Cb");
    KLifeFit->SetParName(3, "t_s");
    KLifeFit->SetParName(4, "Cs");

    KLifeFit->SetParameter(0, 16382);
    KLifeFit->SetParameter(1, 2.5e-10); // background lifetime PDG
    KLifeFit->SetParameter(3, 8.965e-11); // Kshort lifetime PDG
    KLifeFit->SetParameter(4, 500000);

    
    //KLifeFit->SetParLimits(0, 0, 1e6); // C0
    KLifeFit->SetParLimits(1, 1e-10, 1e-08); // t_b
    //KLifeFit->SetParLimits(2, 0, 1e6); // Cb
    KLifeFit->SetParLimits(3, 7e-11, 1e-10); // t_s 
    //KLifeFit->SetParLimits(4, 0, 1e6); // Cs

    KLifeFit->SetLineColor(kBlue);
    KLifeFit->SetLineWidth(2);

    hist1->Fit("KLifeFit", "R");

    KLifeFit->Draw("SAME");

    canvas->SaveAs("KLife.png");

    delete KLifeFit;
    delete canvas;
    delete hist1;

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


