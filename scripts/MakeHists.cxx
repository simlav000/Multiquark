// Custom includes
#include "Fits.h"
#include "Particles.h"
#include "Utilities.h"

// ROOT includes
#include <TBranch.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>

void HighEnergyResonanceFit(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }

    float mass_min = 2900;
    float mass_max = 8000;
    // HER: High-energy Resonance
    //float mass_min = mq->HER_mass_min;
    //float mass_max = mq->HER_mass_max;

    // Create and draw histogram canvas
    TCanvas *hist_canvas = new TCanvas("hist_canvas", "Histogram Canvas", 1000, 500);
    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);

    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle((mq->invariant_mass_label).c_str());
    hist->GetYaxis()->SetTitle("Counts per bin");

    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");

    TF1 *fit = new TF1("InvMassFit", mq->HER_mass_fit_model, mass_min, mass_max, 4); 
    fit->SetParNames("a", "b", "c", "x-offset");
    fit->SetParameter(3, 3000);
    fit->SetLineColor(mq->line_color);
    fit->SetLineWidth(2);
    fit->SetNpx(1000);

    gStyle->SetErrorX(0);

    hist->Fit(fit);


    hist->Draw();
    fit->Draw("SAME");

    hist_canvas->SaveAs((mq->HER_filename).c_str());


    TCanvas *residuals_canvas = new TCanvas("Significance plot", "Histogram Canvas", 1000, 500);
    TH1F *residuals = new TH1F("hist", "Significance", num_bins, mass_min, mass_max);

    residuals->SetLineColor(kBlack);
    residuals->GetXaxis()->SetTitle((mq->invariant_mass_label).c_str());
    residuals->GetYaxis()->SetTitle("Counts per bin");

    int count = 0;
    for (int i = 1; i <= hist->GetNbinsX(); i++) {
        count++;

        std::vector<double> x, y;
        for (int j = 1; j <= hist->GetNbinsX(); j++) {
            if (i == j) continue;
            x.push_back(hist->GetBinCenter(j));
            y.push_back(hist->GetBinContent(j));
        }

        TGraph graph(x.size(), &x[0], &y[0]);

        TF1 *bkg_fit = new TF1("Background fit", mq->HER_mass_fit_model, mass_min, mass_max, 3);
        bkg_fit->SetParameter(3, 3000);

        int fit_status = graph.Fit(bkg_fit, "Q");
        if (fit_status != 0) {
            std::cerr << "Fit failed for bin " << i << std::endl;
            delete bkg_fit;
            continue; 
        }

        double bin_value = hist->GetBinContent(i);
        double bin_error = hist->GetBinError(i);
        std::cout << "Bin error: " << bin_error << std::endl;
        double fit_value = fit->Eval(hist->GetBinCenter(i));       

        double residual = bin_value - fit_value;
        double significance = residual / bin_error;
        residuals->SetBinContent(i, significance);
    }
    std::cout << "count: " << count << std::endl;

    residuals->Draw();
    
    std::string fname = "Significance" + std::to_string(num_bins) + "bins.png";
    residuals_canvas->SaveAs(fname.c_str());
}

void LowEnergyResonanceFit(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }
        
    // LER: Low-energy Resonance
    float mass_min = mq->LER_mass_min;
    float mass_max = mq->LER_mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);

    hist->SetFillColor(mq->fill_color);
    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle((mq->invariant_mass_label).c_str());
    hist->GetYaxis()->SetTitle("Counts per bin");

    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");

    // Set bin errors, assuming bin count is Poisson distributed
    for (int i = 1; i <= hist->GetNbinsX(); i++) {
        double n = hist->GetBinContent(i);
        double error = sqrt(n);
        hist->SetBinError(i, error);
    }
    
    gStyle->SetErrorX(0);

    TF1 *fit = new TF1("InvMassFit", Fits::GaussPlus3rdOrderPoly, mass_min, mass_max, 7);
    
    fit->SetParNames("A", "mu", "sigma", "a", "b", "c", "d");
    fit->SetParLimits(0, 0, 1000); // A > 0
    fit->SetParameter(1, 1535.19);
    fit->SetParLimits(1, 1400, 1590);
    fit->SetParameter(2, 28.57);
    fit->SetParLimits(2, 15, 45);
    fit->SetLineColor(mq->line_color);
    fit->SetLineWidth(2);
    fit->SetNpx(1000);

    hist->Fit("InvMassFit", "R");

    Double_t A = fit->GetParameter(0);
    Double_t mu = fit->GetParameter(1);
    Double_t mu_err = fit->GetParError(1);
    Double_t sigma = fit->GetParameter(2);
    Double_t sigma_err = fit->GetParError(2);
    Double_t a = fit->GetParameter(3);
    Double_t b = fit->GetParameter(4);
    Double_t c = fit->GetParameter(5);
    Double_t d = fit->GetParameter(6);
    
    // Use parameters of fit to plot individual contributions
    int n_pts = 10000;
    Double_t m[n_pts];
    Double_t y_signal[n_pts];
    Double_t y_background[n_pts];

    double dm = (mass_max - mass_min) / (n_pts -  1);

    for (int i = 0; i < n_pts; i++) {
        m[i] = mass_min + dm * i;
        y_signal[i] = A*TMath::Gaus(m[i], mu, sigma);
        y_background[i] = a*m[i]*m[i]*m[i] + b*m[i]*m[i] + c*m[i] + d;
    }
    
    TGraph *background = new TGraph(n_pts, m, y_background);
    background->SetLineWidth(2);
    background->SetLineStyle(kDashed);
    background->SetLineColor(kBlue);

    TGraph *signal = new TGraph(n_pts, m, y_signal);
    signal->SetLineWidth(2);
    signal->SetLineColor(kOrange);
    // Signal can only be seen if vertical scale starts at 0
    // using hist->SetMinimum(0); but it doesn't look all that good

    hist->Draw("SAME E1");
    fit->Draw("SAME");
    background->Draw("SAME");


    TLegend* legend = new TLegend(0.7, 0.1, 0.9, 0.3); // Bottom right corner
    legend->AddEntry(hist, "Data (Minimum Bias)", "f");
    legend->AddEntry(fit, "Signal + Background", "l");
    legend->AddEntry(background, "Background", "l");
    legend->Draw();

    TLatex fitMean;
    TLatex fitSigma;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    fitMean.SetNDC(); 
    fitSigma.SetNDC(); 
    fitMean.SetTextSize(0.03);
    fitSigma.SetTextSize(0.03);
    std::string mean_result = "#mu = " + floatToString(mu, 2) + " #pm " + floatToString(mu_err, 2) + " MeV";
    std::string sigma_result = "#sigma = " + floatToString(sigma, 2) + " #pm " + floatToString(sigma_err, 2) + " MeV";
    fitMean.DrawLatex(0.35, 0.35, mean_result.c_str()); 
    fitMean.DrawLatex(0.35, 0.30, sigma_result.c_str()); 

    canvas->SaveAs(mq->LER_filename.c_str());

    delete hist;
    delete fit;
    delete background;
    delete signal;
    delete canvas;
    delete legend;
}

void MakeInvMassHist(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }
        
    float mass_min = mq->mass_min;
    float mass_max = mq->mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);


    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");

    hist->SetFillColor(mq->fill_color);
    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle((mq->invariant_mass_label).c_str());
    hist->GetYaxis()->SetTitle("Counts per bin");
    
    canvas->SaveAs(mq->output_filename.c_str());

    delete hist;
    delete canvas;
}

void MakeKLMassHist(TTree* V0Tree) {
    int num_bins_x = 100;
    int num_bins_y = 100;

    int x_low = 250;
    int x_high = 900;

    int y_low = 1000;
    int y_high = 1600;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    canvas->SetRightMargin(0.15);

    TH2F *hist = new TH2F("hist_KLMass", "K_{s} VS #Lambda mass", num_bins_x, x_low, x_high, num_bins_y, y_low, y_high);

    gStyle->SetPalette(kRainBow); //kMint
    hist->SetStats(false);

    // Fill the histogram with data from the TTree
    V0Tree->Draw("LMass:KMass >> hist_KLMass",
                 Cuts::cut_on_KcosTheta_3D && Cuts::cut_on_KpT &&
                 Cuts::cut_on_KDeltaRxy &&
                 Cuts::cut_on_LpT && Cuts::cut_on_LDeltaRxy
                 ); 

    // Set axis titles
    hist->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    hist->GetYaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]");

    // Draw the histogram with a color map
    hist->Draw("COLZ");

    // Save the canvas as a PNG file
    canvas->SaveAs("KLMassCutRp_Tcos.png");

    // Clean up
    delete hist;
    delete canvas;
}


void MakeMassHist(Particle* p, TTree* V0Tree, int num_bins, TCut Cut1, TCut Cut2, TCut Cut3) {

    // MeV
    float mass_min = p->mass_min;
    float mass_max = p->mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = p->name_formatted + " Invariant Mass";

    TH1F* hist1 = new TH1F("hist_InvMass_Cut1", hist_name.c_str() , num_bins, mass_min, mass_max);
    TH1F* hist2 = new TH1F("hist_InvMass_Cut2", "", num_bins, mass_min, mass_max);
    TH1F* hist3 = new TH1F("hist_InvMass_Cut3", "", num_bins, mass_min, mass_max);

    hist1->SetFillColor(kViolet + 6);
    hist1->SetLineColor(kBlack);
    hist1->GetXaxis()->SetTitle((p->invariant_mass_label).c_str());
    hist1->GetYaxis()->SetTitle("Counts per bin");
    hist1->SetMinimum(0);   // Needed to see signal after cuts
    hist1->SetStats(false); // Get rid of stats box

    hist2->SetFillColor(kOrange + 10);
    hist2->SetLineColor(kBlack);

    hist3->SetFillColor(kSpring - 2);
    hist3->SetLineColor(kBlack);

    V0Tree->Draw(FillHist(p->mass, "hist_InvMass_Cut1").c_str(), Cut1, "hist"); 
    V0Tree->Draw(FillHist(p->mass, "hist_InvMass_Cut2").c_str(), Cut2, "hist same"); 
    V0Tree->Draw(FillHist(p->mass, "hist_InvMass_Cut3").c_str(), Cut3, "hist same"); 

    //-------------------------------------------------------------------------
    // Everything is more or less general except for the fitting section 
    // You'll have to edit this part depending on your fits and particles

    TF1 *Cut1Fit = new TF1("Cut1Fit", Fits::KMassFit, 1090, 1145, 6);
    TF1 *Cut2Fit = new TF1("Cut2Fit", Fits::KMassFit, 1090, 1150, 6);
    TF1 *Cut3Fit = new TF1("Cut3Fit", Fits::KMassFit, 1090, 1150, 6);

    Cut1Fit->SetParNames("A_1", "mu_1", "sigma_1", "c_1", "b_1", "a_1"); 
    Cut1Fit->SetParLimits(0, 0, 600000); // A > 0
    Cut1Fit->SetParameter(1, p->mass_pdg);
    Cut1Fit->SetParLimits(2, 0, 10000);    // sigma > 0
    Cut1Fit->SetLineColor(kMagenta + 4);
    Cut1Fit->SetLineWidth(2);
    Cut1Fit->SetNpx(1000);

    Cut2Fit->SetParNames("A_2", "mu_2", "sigma_2", "c_2", "b_2", "a_2"); 
    Cut2Fit->SetParameter(0, 2400);
    Cut2Fit->SetParLimits(0, 0, 600000);
    Cut2Fit->SetParameter(1, p->mass_pdg);
    Cut2Fit->SetParLimits(1, 0, 2000);
    Cut2Fit->SetParameter(2, 3);
    Cut2Fit->SetParLimits(2, 1, 10000);
    Cut2Fit->SetLineColor(kOrange);
    Cut2Fit->SetLineWidth(2);
    Cut2Fit->SetNpx(1000);

    Cut3Fit->SetParNames("A_3", "mu_3", "sigma_3", "c_3", "b_3", "a_3"); 
    Cut3Fit->SetParLimits(0, 0, 600000);
    Cut3Fit->SetParameter(1, p->mass_pdg);
    Cut3Fit->SetParLimits(2, 0, 100);
    Cut3Fit->SetLineColor(kGreen + 4);
    Cut3Fit->SetLineWidth(2);
    Cut3Fit->SetNpx(1000);
    //-------------------------------------------------------------------------

    hist1->Fit("Cut1Fit", "R");
    hist2->Fit("Cut2Fit", "R");
    hist3->Fit("Cut3Fit", "R");


    Double_t A_1     = Cut1Fit->GetParameter(0);
    Double_t sigma_1 = Cut1Fit->GetParameter(2);

    Double_t A_2     = Cut2Fit->GetParameter(0);
    Double_t sigma_2 = Cut2Fit->GetParameter(2);

    Double_t A_3     = Cut3Fit->GetParameter(0);
    Double_t sigma_3 = Cut3Fit->GetParameter(2);

    printSignalArea(A_1, sigma_1);
    printSignalArea(A_2, sigma_2);
    printSignalArea(A_3, sigma_3);

    Cut1Fit->Draw("SAME");
    Cut3Fit->Draw("SAME");
    Cut2Fit->Draw("SAME");


    // Absolute legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, Cut1.GetName(), "f");
    legend->AddEntry(Cut1Fit, "Full distribution fit", "l"); 
    legend->AddEntry(hist2, Cut2.GetName(), "f");
    legend->AddEntry(Cut2Fit, "Cosine cut fit", "l"); 
    legend->AddEntry(hist3, Cut3.GetName(), "f");
    legend->AddEntry(Cut3Fit, "All cuts fit", "l"); 
    legend->Draw();

    canvas->SaveAs("KMass.png");

    delete hist1;
    delete hist2;
    delete hist3;
    delete Cut1Fit;
    delete Cut2Fit;
    delete Cut3Fit;
    delete legend;
    delete canvas;
}

void MakeKLifeHist(TTree* V0Tree) {

    float t_min = 0.025e-9;
    float t_max = 0.22e-9;
    int num_bins = 100;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    TH1F* hist = new TH1F("hist_KLife", "K^{0}_{s} Lifetime", num_bins, t_min, t_max);
    hist->GetXaxis()->SetTitle("Time [s]"); 
    hist->GetYaxis()->SetTitle("Counts per bin");
    hist->SetMinimum(0);
    hist->SetStats(false);

    V0Tree->Draw("KLife>>hist_KLife", "", "hist"); 

    // Format: TF1("name", fit_func, lowlim, highlim, nparams)
    TF1 *KLifeFit = new TF1("KLifeFit", Fits::lifetime_fit_2exp, t_min, t_max, 5);

    // Set Parameter Names
    KLifeFit->SetParName(0, "C_0");  // Constant offset
    KLifeFit->SetParName(1, "C_b");  // Background amplitude
    KLifeFit->SetParName(2, "t_b");  // Background lifetime
    KLifeFit->SetParName(3, "C_K");  // Kaon amplitude
    KLifeFit->SetParName(4, "t_K");  // Kaon lifetime

    // Initial guesses
    KLifeFit->SetParameter(0, 16382);     // C_0
    KLifeFit->SetParameter(2, 2.5e-10);   // background lifetime PDG
    KLifeFit->SetParameter(4, 8.965e-11); // Kshort lifetime PDG

    // Limits on search
    KLifeFit->SetParLimits(1, 0, 1e+10);     // C_b > 0
    KLifeFit->SetParLimits(2, 1e-10, 1e-08); // t_b
    KLifeFit->SetParLimits(3, 0, 1e+10);     // C_K > 0
    KLifeFit->SetParLimits(4, 7e-11, 1e-10); // t_s 

    KLifeFit->SetLineColor(kBlue);
    KLifeFit->SetLineWidth(2);


    hist->Fit("KLifeFit", "R");

    KLifeFit->Draw("SAME");

    double c_0 = KLifeFit->GetParameter(0);
    double c_b = KLifeFit->GetParameter(1);
    double t_b = KLifeFit->GetParameter(2);
    double c_k = KLifeFit->GetParameter(3);
    double t_k = KLifeFit->GetParameter(4);

    // Use parameters of fit to plot individual contributions
    int n_pts = 1000;
    double t[n_pts];
    double y_background[n_pts];
    double y_signal[n_pts];
    double dt = (t_max - t_min) / (n_pts -  1);

    for (int i = 0; i < n_pts; i++) {
        t[i] = t_min + dt * i;
        y_background[i] = c_0 + c_b * TMath::Exp(-t[i]/t_b);
        y_signal[i] = c_k * TMath::Exp(-t[i]/t_k);
    }
    
    TGraph *background = new TGraph(n_pts, t, y_background);
    background->SetLineWidth(2);
    background->SetLineColor(kTeal);
    background->Draw("SAME");

    TGraph *signal = new TGraph(n_pts, t, y_signal);
    signal->SetLineWidth(2);
    signal->SetLineColor(kRed);
    signal->Draw("SAME");
        
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist, "Data", "f"); // f = fill 
    legend->AddEntry(KLifeFit, "Signal + Background", "l"); // l = line
    legend->AddEntry(background, "Background" , "l");
    legend->AddEntry(signal, "Signal", "l");
    legend->Draw();

    TLatex fitModel;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    fitModel.SetNDC(); 
    fitModel.SetTextSize(0.03);
    fitModel.DrawLatex(0.35, 0.7, "C_{0} + C_{b}e^{-t/#tau_{b}} + C_{s}e^{-t/#tau_{s}}");

    TLatex lifetimeResult;
    lifetimeResult.SetNDC();
    lifetimeResult.SetTextSize(0.03);
    std::string lifetimeLabel = "#tau_{s} = " + floatToString(t_k, 3, true);
    lifetimeResult.DrawLatex(0.5, 0.5, lifetimeLabel.c_str());

    // Setting to log scale to easily see exponential region turn linear 
    //gPad->SetLogy();
    //canvas->Update();

    canvas->SaveAs("KLife.png");
    
    delete legend;
    delete KLifeFit;
    delete canvas;
    delete hist;

}

void MakeHists() {
    // Find ROOT file
    std::string home = std::getenv("HOME");
    std::string path = "/McGill/Multiquark/data/";
    std::string data = "DID1234.root";
    std::string full = home + path + data;
    const char* name = full.c_str();
    
    // Open the ROOT file
    TFile *file = TFile::Open(name);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file" << std::endl;
        return;
    }

    bool hasV0 = true;
    bool hasPV = true;

    // Get the secondary vertex Tree (if present)
    TTree *V0Tree = dynamic_cast<TTree*>(file->Get("V0Tree")); 
    if (!V0Tree) {
        std::cout << "Warning: Could not get secondary vertex tree" << std::endl;
        hasV0 = false;
    }

    // Get the primary vertex Tree 
    TTree *PVTree = dynamic_cast<TTree*>(file->Get("PVTree")); 
    if (!PVTree) {
        std::cout << "Warning: Could not get primary vertex tree" << std::endl;
        hasPV = false;
    }

    if (!hasPV && !hasV0) {
        std::cerr << "Error: No data found" << std::endl;
        file->Close();
        delete file;
    }

    // See bottom of Cuts.h, associates a name to a cut to be printed in 
    // the histogram legends.
    Cuts::SetCutNames();

    // See Particles.h, provides default data for kaon and lambda plots
    Kaon& k        = Kaon::getInstance();
    Lambda& l      = Lambda::getInstance();
    Tetraquark& tq = Tetraquark::getInstance();
    Pentaquark& pq = Pentaquark::getInstance();
    Hexaquark& hq  = Hexaquark::getInstance();

    //MakeMassHist(&k, V0Tree, num_bins, Cuts::no_cut, Cuts::cut_on_KcosTheta_3D, Cuts::L_LB_candidate_cuts);
    //MakeKLMassHist(V0Tree);
    //MakeKLifeHist(V0Tree);
    
    //MakeInvMassHist(&tq, PVTree, 300);
    //MakeInvMassHist(&pq, PVTree, 300);
    //MakeInvMassHist(&hq, PVTree, 200);
    //LowEnergyResonanceFit(&tq, PVTree, 75);
    HighEnergyResonanceFit(&tq, PVTree, 100);

    //MakeInvMassHist(&hq, PVTree, 80);
    
    // Clean up
    //file->Close();
    //delete file;
}


