// ************************************************
// Histograms_1D_mod20240219_flex.cxx:
// - General purpose simple plotting code, all arguments has to be manually input
//
// Histograms_1D_mod20240502.cxx:
// - K/L plotting code that is meant for use on full samples later
// - Uses TCuts
// - TODO: Area under gaus and 2nd poly to be added, see @todo [TBD]
// - Adding cuts in the form of TCuts [K: TBD; L: TBD; LB: TBD]

// - Save separate presets (like plot options) for various plots:
//      - Data - before vs after cuts
//      - Data vs MC after cuts
//************************************************

#include <string>
//#include "/Users/Cole/Documents/startingResearchResources/20190521_kos_project_OneDrive_1_5-21-2019/Smoother/smoother.cxx"

// Fit Functions
double kfit(const double *x, const double *par){  // Kaon Fit: Voight + 2nd degree polynomial
    return par[0]*TMath::Gaus(x[0], par[1], par[2]) + par[3] + par[4]*x[0] + par[5]*x[0]*x[0];  // Gaussian + 2nd order polynomial
}

    
    
void tut_Histograms_1D(){
    
    int nthreads = 12;
    ROOT::EnableImplicitMT(nthreads);

    // For smaller files (use TFile and TTree)
    // TFile *f = TFile::Open("/Users/Cole/Documents/research/2020/data/fullRun/newOut0/hist-sample.root");//path of MC input file
   // TFile *f = TFile::Open("/home/steben/projects/rrg-steven/steben/MultiquarkSearch/newtril/run/dataoct26/user.kosulliv.00267358.physics_MinBias.recon.AOD.r10170_tid13005594_00.11_hist.210140925/*.root"); // path of data input file
    // TTree *t = (TTree*)f->Get("KsValidation");

    // For larger files (use TChain)
    TChain *t1 = new TChain("KsValidation");

    //get files from eos
    t1->Add("/home/simon515lavoie/multiquark/ROOT/hist-sample.root");

    // Create canvas
    TCanvas *c1 = new TCanvas("nc1", "nc1");
    c1->SetCanvasSize(1000,600);	  // approx size of screen

    gStyle->SetOptFit(1111);    // Configs which fit statistics to print out. See https://root.cern.ch/root/htmldoc/guides/users-guide/FittingHistograms.html#result-of-the-fit
    gStyle->SetOptStat("nemruoi");      // Configs which histogram statistics to print out. See https://root.cern.ch/doc/master/classTPaveStats.html


    //Values
    int numBins = 200;
    float lowLim = 400; // K: 400, L/LB: 1100
    float highLim = 600; // K: 600, L/LB: 1160


    //Load data into histograms
    TH1F *hist1 = new TH1F("nhist1","K_{s} mass", numBins, lowLim, highLim);
    TH1F *hist2 = new TH1F("nhist2","K_{s} mass", numBins, lowLim, highLim);

    hist1->Sumw2();
    //hist2->Sumw2();

    t1->Draw("RecMass>>nhist1", "");
    //t1->Draw("RecMass>>nhist2", K_candidate_cuts);

    //t->Draw("recPhi>>nhist1", cut_on_Kmass_signal_low && cut_on_Kmass_signal_high &&"RecCosTheta_3D > 0.9998 && RecDist_3D > 3 && RecPt > 300 && RecMassLambda > 1125");

    hist1->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]"); //m_{#pi^{+}#pi^{-}} [MeV]
    //hist1->GetXaxis()->SetTitle("d [mm]");
    hist1->GetYaxis()->SetTitle("Candidates / MeV");
    hist1->SetMinimum(0); // Otherwise hist2 doesn't show up

    //hist1->SetMinimum(0.1);     // For log scale
    //hist2->SetMinimum(0.1);     // For log scale
    //c1->SetLogy();            // For log scale
    
    //hist1->SetLineColor(kRed);
    //hist2->SetLineColor(kOrange);

    hist1->SetLineColor(kBlack);
    //hist2->SetLineColor(kBlue);

    hist1->SetLineWidth(2);
    //hist2->SetLineWidth(2);

    //hist1->SetFillColor(kRed-2);
    //hist2->SetFillColor(kOrange-2);

    // COLORS/COLOURS: https://root.cern.ch/doc/master/Rtypes_8h_source.html#l00066 
            
    // Plot Histogram and Update Canvas (for drawing lines later)
    hist1->Draw("HIST");
    //hist2->Draw("HISTsame");

    // Begin fitting
    TF1 *fitFunc = new TF1("fitFunc", kfit, lowLim, highLim, 6);

    // Set initial parameters
    fitFunc->SetParameter(0, 0);
    fitFunc->SetParameter(1, 400);
    fitFunc->SetParameter(2, 20);
    fitFunc->SetParameter(3, 0);
    fitFunc->SetParameter(4, 0);
    fitFunc->SetParameter(5, 0);

    // Perform the fit
    hist1->Fit("fitFunc", "R"); // "R" means in the specified range

    // Fitting the function atop the histogram
    fitFunc->SetLineColor(kBlue);
    fitFunc->Draw("SAME");

    // Draw Legend
    TLegend *legend = new TLegend(.60,.55,.93, .70);  
    legend->AddEntry(hist1,"No cuts (from one sample)", "f");
    legend->AddEntry(hist2,"V0 tracks pT > 100, |eta| < 2.5 (from one sample)", "f");

    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();
    c1->Update();
    c1->SaveAs("~/multiquark/ROOT/test.png");

}
