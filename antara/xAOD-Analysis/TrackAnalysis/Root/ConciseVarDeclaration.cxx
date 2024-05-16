#include <vector>
#include <string>
#include <unordered_map>
#include "TTree.h"

int main() {
    std::vector<std::string> varNames = {
        "recR", "recDelR", "recPt", "recRErr", "recPtErr", 
        "recCosTheta", "recMass", "recMassErr", "recMassL", "recMassErrL", 
        "recMassLB", "recMassErrLB", "chi2ndf", "recPhi", "pz", 
        "fz", "vz", "PVz", "xSV", "ySV", "kLife", "lLife"
    };

    std::unordered_map<std::string, float> varMap;
    for (const auto& name : varNames) {
        varMap[name] = 0.0f;  // Initialize all variables to 0.0f
    }

    std::vector<float*> varPointers;
    for (const auto& name : varNames) {
        varPointers.push_back(&varMap[name]);
    }

    TTree* myTree = new TTree("myTree", "A tree with multiple branches");

    for (size_t i = 0; i < varNames.size(); ++i) {
        std::string branchName = varNames[i];
        std::string leafList = branchName + "/F";
        myTree->Branch(branchName.c_str(), varPointers[i], leafList.c_str());
    }

    // Test: Setting recR to 1
    varMap["recR"] = 2.0;

    myTree->Fill();

    myTree->Print();

    TFile* file = new TFile("myTree.root", "RECREATE");
    myTree->Write();
    file->Close();

    delete myTree;
    delete file;
    
    return 0;
}

