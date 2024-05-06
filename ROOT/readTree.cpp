#include <TFile.h>
#include <TTree.h>

void readTree(const char* filename) {
    TFile file(filename, "READ");
    if (!file.IsOpen()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    // Get the TTree object from the file
    TTree* tree = dynamic_case<TTree*>(file.Get("KsValidation"));
    if (!tree) {
        std::cerr << "Error: Tree cannot be found" << std::endl;
        return;
    }

    
