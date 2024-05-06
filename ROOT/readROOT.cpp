void readROOT(const char* filename) {
    TFile in_file(filename);
    if (!file.IsOpen()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

}

