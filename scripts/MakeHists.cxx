#include "Rtypes.h"
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>

TH2F* MakeKLMassHist() {
    TH2F *hist = new TH2F("hist_KLMass", "K_{s} VS #Lambda mass", 50, 0, 1000, 50, 1000, 1600);
    hist->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    hist->GetYaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]");

    return hist;
}

TH1F* MakeKMassHist() {
    TH1F *hist= new TH1F("hist_KMass", "K_{s} mass", 50, 0, 1000);
    hist->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    hist->GetYaxis()->SetTitle("Candidates");

    return hist;
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

    // Declare desired branches 
    // See m_varNames in Multiquark.h for list of available branches
    Float_t KMass, LMass, CosTheta, Pt;
    myTree->SetBranchAddress("KMass", &KMass);
    myTree->SetBranchAddress("LMass", &LMass);
    myTree->SetBranchAddress("CosTheta", &CosTheta);

    // Create a histograms
    TH1F* KMassHistogram = MakeKMassHist();
    TH2F* KLMassHistogram = MakeKLMassHist();
    
    // Temporarily
    //std::ofstream outFile("KLMass.csv");

    // Loop over entries in the TTree and fill the histograms
    Long64_t numEntries = myTree->GetEntries();
    for (Long64_t i = 0; i < numEntries; ++i) {
        myTree->GetEntry(i);
        KLMassHistogram->Fill(KMass, LMass);
        //outFile << KMass << "," << LMass << std::endl;
        
    } 

    //outFile.close();

    // Create a canvas and draw the histogram
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    // COLZ draws the histogram with a color map
    KLMassHistogram->Draw("COLZ");

    // Save the histogram as an image file (optional)
    canvas->SaveAs("KLMass_histogram.png");

    // Clean up
    delete canvas;
    delete KLMassHistogram;

    // Close the ROOT file
    file->Close();
    delete file;
}


