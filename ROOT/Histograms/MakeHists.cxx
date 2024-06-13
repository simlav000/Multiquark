#include "TFile.h"
#include "TTree.h"
#include "TH1F.h" // Include TH1F for 1D histograms
#include <vector>
#include <map>
#include <iostream>

void MakeHists() {
    const char* fileName = "/home/simon515lavoie/McGill/Multiquark/ROOT/Histograms/KshortMass.root"; // Update with your file name
    const char* treeName = "myTree"; // Update with your TTree name

    // Open ROOT file
    TFile *file = new TFile(fileName, "READ");

    // Check if file opened successfully
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file" << std::endl;
        if (file) file->Close();
        delete file;
        return;
    }

    // Get TTree from the file
    TTree *tree = dynamic_cast<TTree*>(file->Get(treeName));

    // Check if TTree was retrieved successfully
    if (!tree) {
        std::cerr << "Error: Could not retrieve TTree from file" << std::endl;
        file->Close();
        delete file;
        return;
    }

    // Get the list of branches
    TObjArray *branches = tree->GetListOfBranches();

    // Map to store branch data
    std::map<std::string, std::vector<float>> branchDataMap;

    // Loop over branches
    for (Int_t i = 0; i < branches->GetEntries(); ++i) {
        TBranch *branch = dynamic_cast<TBranch*>(branches->At(i));
        if (branch) {
            // Check if the branch is a leaf list (array)
            if (branch->GetListOfLeaves()->GetEntries() > 1) {
                // Get the branch name
                std::string branchName = branch->GetName();

                // Get the number of entries in the branch
                Long64_t numEntries = branch->GetEntries();

                // Create a vector to store the branch data
                std::vector<float> branchData(numEntries);

                // Set branch address to the vector
                branch->SetAddress(&branchData[0]);

                // Loop through entries of the branch and retrieve data
                for (Long64_t j = 0; j < numEntries; ++j) {
                    branch->GetEntry(j);
                }

                // Add the branch data to the map
                branchDataMap[branchName] = branchData;
            }
        }
    }

    // Create a histogram from branchDataMap["recMass"]
    // Assuming branchDataMap["recMass"] exists and has data
    if (branchDataMap.find("recMass") != branchDataMap.end()) {
        std::vector<float>& data = branchDataMap["recMass"];

        // Create a histogram
        TH1F *histogram = new TH1F("histogram_recMass", "Histogram of recMass", 100, 0, 100); // Adjust binning as needed

        // Fill histogram with data
        for (auto value : data) {
            histogram->Fill(value);
        }

        // Optionally, draw the histogram
        TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
        histogram->Draw();
        canvas->Draw();
    } else {
        std::cerr << "Error: Branch 'recMass' not found or has no data" << std::endl;
    }

    // Cleanup
    file->Close();
    delete file;
}
