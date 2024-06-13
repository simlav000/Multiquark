#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TObjArray.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

void MakeHists() {
    // Open the ROOT file
    TFile *file = TFile::Open("/home/simlav000/McGill/Multiquark/ROOT/data/dataset.root");
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

    // Set the branch address for KMass
    Float_t KMass, LMass, CosTheta;
    myTree->SetBranchAddress("KMass", &KMass);
    myTree->SetBranchAddress("LMass", &LMass);
    myTree->SetBranchAddress("CosTheta", &CosTheta);

    // Create a histograms
    //TH1F *KMassHistogram = new TH1F("hist_KMass", "K_{s} mass", 50, 0, 1000);
    //KMassHistogram->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    //KMassHistogram->GetYaxis()->SetTitle("Candidates");

    TH2D *KLMassHistogram = new TH2D("hist_KLMass", "K_{s} VS #Lambda mass", 50, 0, 1000, 50, 0, 1000);
    KLMassHistogram->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    KLMassHistogram->GetYaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]");
    
    // Temporarily
    std::ofstream outFile("KLMass.csv");

    // Loop over entries in the TTree and fill the histograms
    Long64_t numEntries = myTree->GetEntries();
    for (Long64_t i = 0; i < numEntries; ++i) {
        myTree->GetEntry(i);
        std::cout << KMass << LMass << std::endl;
        KLMassHistogram->AddBinContent(KMass, LMass);
        outFile << KMass << "," << LMass << std::endl;
        
    } 

    outFile.close();

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
