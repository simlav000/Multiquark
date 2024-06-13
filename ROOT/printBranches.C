#include <iostream>
#include <TFile.h>
#include <TTree.h>

void printTreeBranches(const char* filename) {
    // Open the ROOT file
    TFile *file = TFile::Open(filename);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return;
    }

    // Loop over all objects in the file
    TKey *key;
    TIter nextkey(file->GetListOfKeys());
    while ((key = (TKey*)nextkey())) {
        TObject *obj = key->ReadObj();
        if (obj->IsA() == TTree::Class()) {
            // If the object is a TTree, print its branches' names
            TTree *tree = dynamic_cast<TTree*>(obj);
            std::cout << "TTree: " << tree->GetName() << std::endl;
            TObjArray *branches = tree->GetListOfBranches();
            for (Int_t i = 0; i < branches->GetEntries(); ++i) {
                TBranch *branch = dynamic_cast<TBranch*>(branches->At(i));
                if (branch)
                    std::cout << "    " << branch->GetName() << std::endl;
            }
        }
    }

    // Close the ROOT file
    file->Close();
}

void printTreeBranchesMacro(const char* filename) {
    gROOT->SetBatch(kTRUE); // Set batch mode to prevent pop-up windows
    printTreeBranches(filename);
}

#ifndef __CINT__
int main(int argc, char *argv[]) {
    // Check the number of arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.root>" << std::endl;
        return 1;
    }

    // Call the macro function with the filename as an argument
    printTreeBranchesMacro(argv[1]);

    return 0;
}
#endif
