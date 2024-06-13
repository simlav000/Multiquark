// This script attempts to simplify the tree-building section of KsStudy.cxx

void createBranches(TTree* tree) {
    tree->Branch("RecR", &m_recR,"recR/F");
    tree->Branch("RecR_Err", &m_recRErr,"recRErr/F"); 
    tree->Branch("RecDelR", &m_recDelR,"recDelR/F"); 
    tree->Branch("RecPt", &m_recPt,"recPt/F"); 
    tree->Branch("RecPt_Err", &m_recPtErr,"recPtErr/F"); 
    tree->Branch("RecCosTheta", &m_recCosTheta,"recCosTheta/F"); 
    tree->Branch("RecMass", &m_recMass,"recMass/F"); 
    tree->Branch("RecMassErr", &m_recMassErr,"recMassErr/F"); 
    tree->Branch("RecMassLambda", &m_recMassL,"recMassLambda/F");
    tree->Branch("RecMassErrLambda", &m_recMassErrL,"recMassErrLambda/F");
    tree->Branch("RecMassLambdaBar", &m_recMassLB,"recMassLambdaBar/F");
    tree->Branch("RecMassErrLambdaBar", &m_recMassErrLB,"recMassErrLambdaBar/F");
    tree->Branch("Vz", &m_vz,"recZ/F"); 
    tree->Branch("Vz", &m_recPhi,"recPhi/F"); 
    tree->Branch("Pz", &m_pz,"unitPz/F"); 
    tree->Branch("Fz", &m_fz,"unitFz/F"); 
    tree->Branch("pvZ", &m_PVz,"pvZ/F"); 
    tree->Branch("chi2ndf",&m_chi2ndf,"chi2ndf/F");
    tree->Branch("xSV", &m_xSV , "xSV/F");  // x coord of vertex
    tree->Branch("ySV", &m_ySV , "ySV/F");  // y coord of vertex
    tree->Branch("kLife",&m_kLife,"kLife/F");
    tree->Branch("lLife",&m_lLife,"lLife/F");
}

TTree setupTree() {
    // Ntuple branches, truth --> MC
    TTree* mainTree =  new TTree("Main","Main"); // Tree to store track information

    //Save information for the four tracks that recreate the secondary vertecies. The first and third will be the protons if lambda particles are recreated.
    //Currently don't have any way of distinguishing protons and pions
    mainTree->Branch("TrackPt_1", &m_trackPt_1,"trackPt_1/F");
    mainTree->Branch("TrackEta_1", &m_trackEta_1,"trackEta_1/F");
    mainTree->Branch("TrackPt_2", &m_trackPt_2,"trackPt_2/F"); 
    mainTree->Branch("TrackEta_2", &m_trackEta_2,"trackEta_2/F");
    mainTree->Branch("TrackPt_3", &m_trackPt_3,"trackPt_3/F"); 
    mainTree->Branch("TrackEta_3", &m_trackEta_3,"trackEta_3/F");
    mainTree->Branch("TrackPt_4", &m_trackPt_4,"trackPt_4/F"); 
    mainTree->Branch("TrackEta_4", &m_trackEta_4,"trackEta_4/F");

    myTree->Branch("NumK", &m_numK,"numK/F");
    myTree->Branch("NumL", &m_numL,"numL/F"); //adding no. of Ks and Ls in the tree

    //Saves for multiquark data
    myTree->Branch("RecConvMass", &m_recConvMass,"recConvMass/F");
    myTree->Branch("pplus",&m_pplus,"pplus/F");
    myTree->Branch("pminus",&m_pminus,"pminus/F");
    myTree->Branch("nSCTp", &m_nSCTp, "nSCTp/I" );
    myTree->Branch("nSCTn", &m_nSCTn, "nSCTn/I");
    myTree->Branch("nPixp", &m_nPixp, "nPixp/I");//!
    myTree->Branch("nPixn", &m_nPixn, "nPixn/I");
    myTree->Branch("htp", &m_htp ,"htp/F");
    myTree->Branch("htn", &m_htn , "htn/F");

    // myTree->Branch("numSV", &m_numSV, "numSV/I");  // number of secondary vertices

    myTree->Branch("invMKK", &m_invMKK, "invMKK/F"); // K-K (tetraquark) invariant mass
    myTree->Branch("invMKL", &m_invMKL, "invMKL/F"); // K-L (pentaquark) invariant mass
    myTree->Branch("invMLL", &m_invMLL, "invMLL/F"); // L-L (sextaquark) invariant mass


    TTree* MCTree = new TTree("MC", "MC"); // May not even need this one

    // Truth values for MC data
    MCTree->Branch("TrueR", &m_trueR,"trueR/F");  
    MCTree->Branch("TruePt", &m_truePt,"truePt/F"); 
    MCTree->Branch("TrueEta", &m_trueEta,"trueEta/F");
    MCTree->Branch("TrueCosTheta", &m_trueCosTheta,"trueCosTheta/F"); 

    //save information for first secondary vertex reconstructed. This will be the lambda particle if a pentaquark is reconstructed
    // Make trees for B, KK, BKK, LL, BLL, and KL
    TTree* V0Tree = new TTree("V0", "V0");
    TTree* BTree = new TTree("B", "B");
    TTree* KKTree = new TTree("KK", "KK");
    TTree* BKKTree = new TTree("BKK", "BKK");
    TTree* LLTree = new TTree("LL", "LL");
    TTree* BLLTree = new TTree("BLL", "BLL");
    TTree* KLTree = new TTree("KL", "KL");

    // Fill each tree with the same branches. 
    createBranches(V0Tree);
    createBranches(BTree);
    createBranches(KKTree);
    createBranches(BKKTree);
    createBranches(LLTree);
    createBranches(BLLTree);
    createBranches(KLTree);

    return myTree;
}

