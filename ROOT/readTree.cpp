#include "RtypesCore.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>

void readTree() {
    const char *filename = "/home/simon515lavoie/McGill/Multiquark/ROOT/data/AOD.13005594._001460.pool.root";

    TFile file(filename, "READ");
    if (!file.IsOpen()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    // Get the TTree 
    TTree *CollectionTree = dynamic_cast<TTree*>(file.Get("CollectionTree"));
    if (!CollectionTree) {
        std::cerr << "Error: Tree can't be found" << std::endl;
        file.Close();
        return;
    }

    TLeaf *leaf = CollectionTree->GetLeaf("V0UnconstrVerticesAuxDyn.Kshort_mass");
    if (!leaf) {
        std::cerr << "Error: Can't find leaf" << std::endl;
        file.Close();
        return;
    }

    file.Close();
}
