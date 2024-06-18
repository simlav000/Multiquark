#include "Cuts.h"

#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TCut.h>
#include <TApplication.h>
#include <iostream>
#include <cstdlib>
#include <string>

TH2F* MakeKLMassHist() {
    TH2F *hist = new TH2F("hist_KLMass", "K_{s} VS #Lambda mass", 50, 250, 1000, 50, 1000, 1600);
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
    Float_t KMass, LMass, CosTheta, Pt, DeltaR;
    myTree->SetBranchAddress("KMass", &KMass);
    myTree->SetBranchAddress("LMass", &LMass);
    myTree->SetBranchAddress("CosTheta", &CosTheta);
    myTree->SetBranchAddress("Pt", &Pt);
    myTree->SetBranchAddress("DeltaR", &DeltaR);

    // Create a histograms
    TH1F* KMassHistogram = MakeKMassHist();
    TH2F* KLMassHistogram = MakeKLMassHist();
    
    // Loop over entries in the TTree and fill the histograms
    Long64_t numEntries = myTree->GetEntries();
    for (Long64_t i = 0; i < numEntries; ++i) {
        myTree->GetEntry(i);
        
        //KMassHistogram->Fill(KMass); 

        KMassHistogram->Fill(KMass);
        //KLMassHistogram->Fill(KMass, LMass);
    }

    // Create a split canvas and draw the histogram
    TCanvas *canvas1 = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    
    // Draw KLMassHistogram on first canvas
    KLMassHistogram->Draw("COLZ"); // COLZ draws the histogram with a color map

    // Save the histogram as an image file (optional)
    canvas1->SaveAs("KLMass_histogram.png");

    TCanvas *canvas2 = new TCanvas("canvas", "Histogram Canvas", 800, 600);

    // Draw KMassHistogram
    KMassHistogram->Draw();

    // Save the histogram as an image file (optional)
    canvas2->SaveAs("KMass_histogram.png");

    TCanvas *c3 = new TCanvas("canvas", "Histogram Canvas", 800, 600);
    TH1F* KMassHistogram2 = new TH1F("hist_KMass", "K_{s} mass", 200, 400, 600);
    //KMassHistogram2->Sumw2();
    myTree->Draw("KMass>>hist_KMass"); 

    c3->SaveAs("test.png");


    // Clean up
    delete canvas1;
    delete canvas2;
    delete KLMassHistogram;
    delete KMassHistogram;

    // Close the ROOT file
    file->Close();
    delete file;
}


