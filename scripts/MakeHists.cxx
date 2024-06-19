#include "Cuts.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TLegend.h>
#include <iostream>
#include <cstdlib>
#include <string>

void MakeKLMassHist(TTree* myTree) {
    // Declare desired branches 
    // See m_varNames in Multiquark.h for list of available branches
    Float_t KMass, LMass, CosTheta, Pt, DeltaR;
    myTree->SetBranchAddress("KMass", &KMass);
    myTree->SetBranchAddress("LMass", &LMass);
    myTree->SetBranchAddress("CosTheta", &CosTheta);
    myTree->SetBranchAddress("Pt", &Pt);
    myTree->SetBranchAddress("DeltaR", &DeltaR);
    //
    // Create a 2D histogram
    TH2F *hist = new TH2F("hist_KLMass", "K_{s} VS #Lambda mass", 50, 250, 1000, 50, 1000, 1600);

    // Loop over entries in the TTree and fill the histograms
    Long64_t numEntries = myTree->GetEntries();
    for (Long64_t i = 0; i < numEntries; ++i) {
        myTree->GetEntry(i);
        
        hist->Fill(KMass, LMass);
    }

    // For now this method doesn't work properly
    // Create a canvas
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);


    // Fill the histogram with data from the TTree
    myTree->Draw("KMass:LMass >> hist_KLMass");

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
    std::string label = "Cos(#theta) > " + std::to_string(KcosTheta_3D_low);
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

    MakeKMassHist(myTree);
    MakeLMassHist(myTree);
    
    // Close the ROOT file
    file->Close();
    delete file;
}


