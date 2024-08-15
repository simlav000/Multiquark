// Custom includes
#include "Fits.h"
#include "Particles.h"
#include "Utilities.h"

#include <stdexcept>

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
#include <TLine.h>
#include <TStyle.h>
#include <TTree.h>
#include <TPaveStats.h>

void HighEnergyResonanceFit(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }

    // HER: High-energy Resonance
    float mass_min = mq->HER_mass_min;
    float mass_max = mq->HER_mass_max;

    // Create and draw histogram canvas
    TCanvas *hist_canvas = new TCanvas("hist_canvas", "Histogram Canvas", 1000, 500);
    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);

    double binSize = hist->GetBinWidth(1);

    gStyle->SetOptStat("e"); 

    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle(mq->invariant_mass_label);
    hist->GetYaxis()->SetTitle("Counts per bin");
    
    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");
    hist_canvas->Update();

    // Adding bin size to stats box
    TPaveStats* stats = (TPaveStats*)hist->GetListOfFunctions()->FindObject("stats");
    stats->SetName("custom stats box");
    TList* listOfLines = stats->GetListOfLines();
    listOfLines->Print();
    TLatex* binSizeLatex = new TLatex(0, 0, Form("Bin Size = %6.2f", binSize));
    binSizeLatex->SetTextFont(stats->GetTextFont());
    binSizeLatex->SetTextSize(stats->GetTextSize());
    listOfLines->Add(binSizeLatex);
    hist->SetStats(0);
    hist_canvas->Modified();
    

    TF1 *fit = new TF1("InvMassFit", mq->HER_mass_fit_model, mass_min, mass_max, 4); 

    fit->SetParNames("a", "b", "c", "x-offset");
    fit->SetParameter(0, -2.56385e-08);
    fit->SetParameter(1, 0.00078);
    fit->SetParameter(2, -10.4492);
    fit->SetParameter(3, 1452);
    fit->SetLineColor(mq->line_color);
    fit->SetLineWidth(2);
    fit->SetNpx(1000);

    gStyle->SetErrorX(0);

    // Try fitting and catch any potential exceptions
    try {
        hist->Fit(fit);
    } catch (const std::exception& e) {
        std::cerr << "Exception during fit: " << e.what() << std::endl;
        return;
    }

    hist->Draw();
    fit->Draw("SAME");

    hist_canvas->SaveAs((mq->HER_filename + std::to_string(num_bins)  + "Bins.png").c_str());


    TCanvas *significance_canvas = new TCanvas("Significance plot", "Histogram Canvas", 1000, 500);
    TH1F *significance = new TH1F("hist", "Significance", num_bins, mass_min, mass_max);

    significance->SetLineColor(kBlack);
    significance->GetXaxis()->SetTitle(mq->invariant_mass_label);
    significance->GetYaxis()->SetTitle("Signal Significance (#sigma)");



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

        TF1 *bkg_fit = new TF1("Background fit", mq->HER_mass_fit_model, mass_min, mass_max, 4);
        bkg_fit->SetParameter(0, -1.91227e-08);
        bkg_fit->SetParameter(1, 0.00079);
        bkg_fit->SetParameter(2, -9.59125);
        bkg_fit->SetParameter(3, 569);

        int fit_status = graph.Fit(bkg_fit, "Q");
        if (fit_status != 0) {
            std::cerr << "Fit failed for bin " << i << std::endl;
            delete bkg_fit;
            continue; 
        }

        double bin_value = hist->GetBinContent(i);
        double bin_error = hist->GetBinError(i);
        double fit_value = bkg_fit->Eval(hist->GetBinCenter(i));       

        double residual = bin_value - fit_value;
        double significance_value = residual / bin_error;
        significance->SetBinContent(i, significance_value);
    }

    significance->Draw();
    
    std::string sig_fname = mq->HER_filename + "Significance" + std::to_string(num_bins) + "Bins.png";
    significance_canvas->SaveAs(sig_fname.c_str());

    // Plotting significance projection onto y-axis as a histogram to see how 
    // significance is distributed
    TCanvas *projection_canvas = new TCanvas("hist_canvas", "Histogram Canvas", 1000, 500);
    std::string projection_hist_title = mq->name_formatted + " Significance Distribution";

    // This is the only thing stopping me from building this histogram in the previous loop :(
    Double_t sig_min = significance->GetMinimum();
    Double_t sig_max = significance->GetMaximum();
    Double_t sig_nbins = TMath::Ceil(TMath::Log2(significance->GetNbinsX()) + 1); // Sturge's formula

    TH1F *projection_hist = new TH1F("hist", projection_hist_title.c_str(), sig_nbins, sig_min, sig_max);

    for (int i = 0; i < significance->GetNbinsX(); i++) {
        double significance_value = significance->GetBinContent(i);
        projection_hist->Fill(significance_value);
    }

    projection_hist->GetXaxis()->SetName("Significance (#sigma)");
    projection_hist->GetYaxis()->SetName("Bin count");

    gStyle->SetOptStat("mre"); // m: mean, r: rms, e: num entries
    projection_hist->Draw();

    std::string proj_fname = mq->HER_filename + "SigDistribution" + std::to_string(num_bins) + "Bins.png";
    projection_canvas->SaveAs(proj_fname.c_str());
}

void LowEnergyResonanceFit(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }
        
    // LER: Low-energy Resonance
    //float mass_min = 1280;
    //float mass_max = 2600;
    float mass_min = mq->LER_mass_min;
    float mass_max = mq->LER_mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);

    hist->SetFillColor(mq->fill_color);
    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle(mq->invariant_mass_label);
    hist->GetYaxis()->SetTitle("Counts per bin");

    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");

    // Set bin errors, assuming bin count is Poisson distributed
    for (int i = 1; i <= hist->GetNbinsX(); i++) {
        double n = hist->GetBinContent(i);
        double error = sqrt(n);
        hist->SetBinError(i, error);
    }
    
    gStyle->SetErrorX(0);

    TF1 *fit = new TF1("InvMassFit", Fits::TwoGaussPlus3rdOrderPoly, mass_min, mass_max, 10);
    
    fit->SetParNames("A1", "mu1", "sigma1",
                     "A2", "mu2", "sigma2");
                     //"A3", "mu3", "sigma3");
    //fit->SetParLimits(0, 10, 1000); // A > 0
    //fit->SetParameter(1, 1320);
    //fit->SetParLimits(1, 1315, 1325);
    //fit->SetParameter(2, 30);
    //fit->SetParLimits(2, 5, 15);
    fit->SetParLimits(0, 0, 1000);
    fit->SetParameter(1, 1524);
    fit->SetParLimits(1, 1400, 1590);
    fit->SetParameter(2, 30);
    fit->SetParLimits(2, 15, 45);
    fit->SetParLimits(3, 0, 1000);
    fit->SetParameter(4, 1710);
    fit->SetParLimits(4, 1600, 1800);
    fit->SetParameter(5, 28.57);
    fit->SetParLimits(5, 5, 35);
    fit->SetParameter(6, -1.20568e-06);
    fit->SetParameter(7, 0.00165074);
    fit->SetParameter(8, 5.5785);
    fit->SetParameter(9, 527.087);
    fit->SetLineColor(mq->line_color);
    fit->SetLineWidth(2);
    fit->SetNpx(1000);

    hist->Fit("InvMassFit", "R");

    Double_t A1 = fit->GetParameter(0);
    Double_t mu1 = fit->GetParameter(1);
    Double_t mu_err1 = fit->GetParError(1);
    Double_t sigma1 = fit->GetParameter(2);
    Double_t sigma_err1 = fit->GetParError(2);
    Double_t A2 = fit->GetParameter(3);
    Double_t mu2 = fit->GetParameter(4);
    Double_t mu_err2 = fit->GetParError(4);
    Double_t sigma2 = fit->GetParameter(5);
    Double_t sigma_err2 = fit->GetParError(5);
    //Double_t A3 = fit->GetParameter(6);
    //Double_t mu3 = fit->GetParameter(7);
    //Double_t mu_err3 = fit->GetParError(7);
    //Double_t sigma3 = fit->GetParameter(8);
    //Double_t sigma_err3 = fit->GetParError(8);
    Double_t a = fit->GetParameter(6);
    Double_t b = fit->GetParameter(7);
    Double_t c = fit->GetParameter(8);
    Double_t d = fit->GetParameter(9);
    
    // Use parameters of fit to plot individual contributions
    int n_pts = 10000;
    Double_t m[n_pts];
    Double_t y_signal[n_pts];
    Double_t y_background[n_pts];

    double dm = (mass_max - mass_min) / (n_pts -  1);

    for (int i = 0; i < n_pts; i++) {
        m[i] = mass_min + dm * i;
        y_signal[i] = A1*TMath::Gaus(m[i], mu1, sigma1) + A2*TMath::Gaus(m[i], mu2, sigma2); // + A3*TMath::Gaus(m[i], mu3, sigma3);
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

    TLatex fitMean1;
    TLatex fitSigma1;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    fitMean1.SetNDC(); 
    fitSigma1.SetNDC(); 
    fitMean1.SetTextSize(0.03);
    fitSigma1.SetTextSize(0.03);
    std::string mean_result1 = "#mu_{1} = " + floatToString(mu1, 2) + " #pm " + floatToString(mu_err1, 2) + " MeV";
    std::string sigma_result1 = "#sigma_{1} = " + floatToString(sigma1, 2) + " #pm " + floatToString(sigma_err2, 2) + " MeV";
    fitMean1.DrawLatex(0.45, 0.45, mean_result1.c_str()); 
    fitMean1.DrawLatex(0.45, 0.40, sigma_result1.c_str()); 

    TLatex fitMean2;
    TLatex fitSigma2;
    fitMean2.SetNDC(); 
    fitSigma2.SetNDC(); 
    fitMean2.SetTextSize(0.03);
    fitSigma2.SetTextSize(0.03);
    std::string mean_result2 = "#mu_{2} = " + floatToString(mu2, 2) + " #pm " + floatToString(mu_err2, 2) + " MeV";
    std::string sigma_result2 = "#sigma_{2} = " + floatToString(sigma2, 2) + " #pm " + floatToString(sigma_err2, 2) + " MeV";
    fitMean2.DrawLatex(0.45, 0.35, mean_result2.c_str()); 
    fitMean2.DrawLatex(0.45, 0.30, sigma_result2.c_str()); 

    //TLatex fitMean3;
    //TLatex fitSigma3;
    //fitMean3.SetNDC(); 
    //fitSigma3.SetNDC(); 
    //fitMean3.SetTextSize(0.03);
    //fitSigma3.SetTextSize(0.03);
    //std::string mean_result3 = "#mu_{3} = " + floatToString(mu3, 2) + " #pm " + floatToString(mu_err3, 2) + " MeV";
    //std::string sigma_result3 = "#sigma_{3} = " + floatToString(sigma3, 2) + " #pm " + floatToString(sigma_err3, 2) + " MeV";
    //fitMean3.DrawLatex(0.45, 0.25, mean_result3.c_str()); 
    //fitMean3.DrawLatex(0.45, 0.20, sigma_result3.c_str()); 

    canvas->SaveAs(mq->LER_filename.c_str());
}

void MakeInvMassHist(Particle* p, TTree* PVTree, int num_bins) {
    Multiquark* mq = dynamic_cast<Multiquark*>(p);
    if (!mq) {
        throw std::runtime_error("This function is for multiquarks ONLY!");
    }
        
    float mass_min = 2300; //mq->mass_min;
    float mass_max = mq->mass_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = mq->name_formatted + " Invariant Mass Distribution";
    TH1F *hist = new TH1F("hist", hist_name.c_str(), num_bins, mass_min, mass_max);


    PVTree->Draw(FillHist(mq->mass, "hist").c_str(), mq->default_cut, "hist");

    hist->SetFillColor(mq->fill_color);
    hist->SetLineColor(kBlack);
    hist->GetXaxis()->SetTitle(mq->invariant_mass_label);
    hist->GetYaxis()->SetTitle("Counts per bin");
    
    canvas->SaveAs(mq->output_filename.c_str());

    delete hist;
    delete canvas;
}

void MakeKLMassHist(TTree* V0Tree) {
    int num_bins_x = 300;
    int num_bins_y = 300;

    int x_low = 280;
    int x_high = 650;

    int y_low = 1080;
    int y_high = 1600;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    canvas->SetRightMargin(0.15);

    TH2F *hist = new TH2F("hist_KLMass", "#Lambda^{0} vs. K_{s}^{0} Invariant Mass", num_bins_x, x_low, x_high, num_bins_y, y_low, y_high);

    gStyle->SetPalette(kRainBow); // https://root.cern.ch/doc/master/classTColor.html
    hist->SetStats(false);

    // Fill the histogram with data from the TTree
    V0Tree->Draw("LMass:KMass >> hist_KLMass",
                 Cuts::cut_on_KcosTheta_3D && Cuts::cut_on_KpT &&
                 Cuts::cut_on_KDeltaRxy &&
                 Cuts::cut_on_LpT && Cuts::cut_on_LDeltaRxy
                 ); 

    // Draw the histogram with a color map
    hist->Draw("COLZ");

    // Set axis titles
    hist->GetXaxis()->SetTitle("m_{#pi^{+}#pi^{-}} [MeV]");
    hist->GetYaxis()->SetTitle("m_{p^{+}#pi^{-}} [MeV]");

    TLine *vertical_line = new TLine(475, 0, 475, 1600);
    vertical_line->SetLineColor(kRed);
    vertical_line->SetLineWidth(2);
    vertical_line->Draw("SAME");

    TLine *horizontal_line = new TLine(0, 1125, 900, 1125);
    horizontal_line->SetLineColor(kRed);
    horizontal_line->SetLineWidth(2);
    horizontal_line->Draw("SAME");
    
    // Save the canvas as a PNG file
    canvas->SaveAs("LMassVsKMass.png");
}

void SimpleHistogram(TTree* V0Tree) {
    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    TH1F *hist = new TH1F("hist", "name", 100, 300, 900);
    V0Tree->Draw("KMass >> hist");
    canvas->SaveAs("Simple.png");
}

void MakeDistanceCutHist(Particle* p,  TTree* V0Tree) {
    int x_low = p->mass_min;
    int x_high = p->mass_max;

    int y_low = 0;
    int y_high = 100;

    int num_bins_x = 100;
    int num_bins_y = 100;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    canvas->SetRightMargin(0.15);

    std::string plot_title = "Distance versus " + p->name_formatted + " Invariant Mass";
    TH2F *hist = new TH2F("dVsInvMass", plot_title.c_str(), 
                          num_bins_x, x_low, x_high,
                          num_bins_y, y_low, y_high);

    gStyle->SetPalette(kRainBow); // https://root.cern.ch/doc/master/classTColor.html
    hist->SetStats(false);

    // Fill the histogram with data from the TTree
    std::string fill_keys = "DeltaR:" + p->mass;
    V0Tree->Draw(FillHist(fill_keys, "dVsInvMass").c_str(), Cuts::cut_on_KcosTheta_3D);  // y:x -> y vs x

    // Set axis titles
    hist->GetXaxis()->SetTitle(p->invariant_mass_label);
    hist->GetYaxis()->SetTitle("Distance from Primary Vertex [mm]");

    // Draw the histogram with a color map
    hist->Draw("COLZ");

    // Save the canvas as a PNG file
    std::string fname = p->name + "_d_vs_InvMass.png";
    canvas->SaveAs(fname.c_str());

    // Clean up
    delete hist;
    delete canvas;
}

void MakePtCutHist(Particle* p, TTree* V0Tree) {
    int x_low = p->mass_min; // 300 
    int x_high = p->mass_max; // 650

    int y_low = 0;
    int y_high = 3000;

    int num_bins_x = 100;
    int num_bins_y = 100;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);
    canvas->SetRightMargin(0.15);
    
    std::string plot_title = "p_{T} vs " + p->name_formatted + " Invariant Mass";
    TH2F *hist = new TH2F("pTMassHist", plot_title.c_str(),
                          num_bins_x, x_low, x_high, 
                          num_bins_y, y_low, y_high);

    gStyle->SetPalette(kRainBow); // https://root.cern.ch/doc/master/classTColor.html
    hist->SetStats(false);

    std::string fill_keys = "p_T:" + p->mass;
    V0Tree->Draw(FillHist(fill_keys, "pTMassHist").c_str(), Cuts::cut_on_KcosTheta_3D);  // y:x -> y vs x

    // Set axis titles
    hist->GetXaxis()->SetTitle(p->invariant_mass_label);
    hist->GetYaxis()->SetTitle("p_{T} [MeV]");

    // Draw the histogram with a color map
    hist->Draw("COLZ");

    // Save the canvas as a PNG file
    std::string fname = p->name + "_pT_vs_InvMass.png";
    canvas->SaveAs(fname.c_str());

    // Clean up
    delete hist;
    delete canvas;
}

void MakeMassHist(Particle* p, TTree* V0Tree, int num_bins, TCut Cut1, TCut Cut2, TCut Cut3) {

    // These defaults are good to get the whole distribution but
    // a custom range can be set to zoom in
    // MeV
    //float mass_min = p->mass_min;
    //float mass_max = p->mass_max;
    float mass_min = 400;
    float mass_max = 650;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_name = p->name_formatted + " Invariant Mass";

    TH1F* hist1 = new TH1F("hist_InvMass_Cut1", hist_name.c_str() , num_bins, mass_min, mass_max);
    TH1F* hist2 = new TH1F("hist_InvMass_Cut2", "", num_bins, mass_min, mass_max);
    TH1F* hist3 = new TH1F("hist_InvMass_Cut3", "", num_bins, mass_min, mass_max);

    hist1->SetFillColor(kViolet + 6);
    hist1->SetLineColor(kBlack);
    hist1->GetXaxis()->SetTitle(p->invariant_mass_label);
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

    TF1 *Cut1Fit = new TF1("Cut1Fit", Fits::KMassFit, mass_min, mass_max, 6);
    TF1 *Cut2Fit = new TF1("Cut2Fit", Fits::KMassFit, mass_min, mass_max, 6);
    TF1 *Cut3Fit = new TF1("Cut3Fit", Fits::LMassFitBreitWigner, mass_min, mass_max, 6);

    Cut1Fit->SetParNames("A_1", "mu_1", "sigma_1", "c_1", "b_1", "a_1"); 
    Cut1Fit->SetParLimits(0, 0, 600000); // A > 0
    Cut1Fit->SetParameter(1, p->mass_pdg); // Mu = PDG mass
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
    Cut3Fit->SetParLimits(2, 0, 25);
    Cut3Fit->SetLineColor(kGreen + 4);
    Cut3Fit->SetLineWidth(2);
    Cut3Fit->SetNpx(1000);
    //-------------------------------------------------------------------------

    hist1->Fit("Cut1Fit", "R");
    hist2->Fit("Cut2Fit", "R");
    hist3->Fit("Cut3Fit", "R");


    Double_t A_1         = Cut1Fit->GetParameter(0);
    Double_t mu_1        = Cut1Fit->GetParameter(1);
    Double_t mu_1_err    = Cut1Fit->GetParError(1);
    Double_t sigma_1     = Cut1Fit->GetParameter(2);
    Double_t sigma_1_err = Cut1Fit->GetParError(2);

    Double_t A_2         = Cut2Fit->GetParameter(0);
    Double_t mu_2        = Cut2Fit->GetParameter(1);
    Double_t mu_2_err    = Cut2Fit->GetParError(1);
    Double_t sigma_2     = Cut2Fit->GetParameter(2);
    Double_t sigma_2_err = Cut2Fit->GetParError(2);

    Double_t A_3         = Cut3Fit->GetParameter(0);
    Double_t mu_3        = Cut3Fit->GetParameter(1);
    Double_t mu_3_err    = Cut3Fit->GetParError(1);
    Double_t sigma_3     = Cut3Fit->GetParameter(2);
    Double_t sigma_3_err = Cut3Fit->GetParError(2);

    printSignalArea(A_1, sigma_1);
    printSignalArea(A_2, sigma_2);
    printSignalArea(A_3, sigma_3);

    //Cut1Fit->Draw("SAME");
    //Cut2Fit->Draw("SAME");
    Cut3Fit->Draw("SAME");


    // Absolute legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hist1, Cut1.GetName(), "f");
    //legend->AddEntry(Cut1Fit, "Full distribution fit", "l"); 
    legend->AddEntry(hist2, Cut2.GetName(), "f");
    //legend->AddEntry(Cut2Fit, "Cosine cut fit", "l"); 
    legend->AddEntry(hist3, Cut3.GetName(), "f");
    legend->AddEntry(Cut3Fit, "All cuts fit", "l"); 
    legend->Draw();


    TLatex fitMean;
    TLatex fitSigma;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    fitMean.SetNDC(); 
    fitSigma.SetNDC(); 
    fitMean.SetTextSize(0.03);
    fitSigma.SetTextSize(0.03);
    std::string mean_result = "#mu = " + floatToString(mu_3, 2) + " #pm " + floatToString(mu_3_err, 2) + " MeV";
    std::string sigma_result = "#sigma = " + floatToString(sigma_3, 2) + " #pm " + floatToString(sigma_3_err, 2) + " MeV";
    fitMean.DrawLatex(0.55, 0.55, mean_result.c_str()); 
    fitSigma.DrawLatex(0.55, 0.50, sigma_result.c_str()); 

    canvas->SaveAs((p->mass + ".png").c_str());

}

void MakeLifetimeHist(Particle* p, TTree* V0Tree, int num_bins) {

    float t_min = p->life_min;
    float t_max = p->life_max;

    TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 1000, 600);

    std::string hist_title = p->name_formatted + " Lifetime";
    TH1F* hist = new TH1F("hist_Lifetime", hist_title.c_str(), num_bins, t_min, t_max);
    hist->GetXaxis()->SetTitle("Time [s]"); 
    hist->GetYaxis()->SetTitle("Counts per bin");
    hist->SetMinimum(0);
    hist->SetStats(false);

    V0Tree->Draw(FillHist(p->life, "hist_Lifetime").c_str(), "", "hist"); 

    // Format: TF1("name", fit_func, lowlim, highlim, nparams)
    TF1 *LifetimeFit = new TF1("LifetimeFit", Fits::lifetime_fit_2exp, t_min, t_max, 5);

    // Set Parameter Names
    LifetimeFit->SetParName(0, "C_0");  // Constant offset
    LifetimeFit->SetParName(1, "C_b");  // Background amplitude
    LifetimeFit->SetParName(2, "t_b");  // Background lifetime
    LifetimeFit->SetParName(3, "C_s");  // Signal amplitude
    LifetimeFit->SetParName(4, "t_s");  // Signal lifetime

    // Initial guesses
    LifetimeFit->SetParameter(0, 0.002);
    LifetimeFit->SetParameter(1, 397332);
    LifetimeFit->SetParameter(2, 1e-09);
    LifetimeFit->SetParameter(3, 4.6e+06);
    LifetimeFit->SetParameter(4, p->life_pdg); // Kshort lifetime PDG

    // Limits on search
    LifetimeFit->SetParLimits(0, 0, 1e+10);     // C_0 > 0
    LifetimeFit->SetParLimits(1, 0, 1e+10);     // C_b > 0
    LifetimeFit->SetParLimits(2, 1e-12, 1e-07); // t_b
    LifetimeFit->SetParLimits(3, 0, 1e+10);     // C_s > 0
    LifetimeFit->SetParLimits(4, 1e-12, 3e-10); // t_s 

    LifetimeFit->SetLineColor(kBlue);
    LifetimeFit->SetLineWidth(2);


    hist->Fit("LifetimeFit", "R");

    LifetimeFit->Draw("SAME");

    double c_0 = LifetimeFit->GetParameter(0);
    double c_b = LifetimeFit->GetParameter(1);
    double t_b = LifetimeFit->GetParameter(2);
    double c_s = LifetimeFit->GetParameter(3);
    double t_s = LifetimeFit->GetParameter(4);
    double t_s_err = LifetimeFit->GetParError(4);
    double reduced_chisqr = LifetimeFit->GetChisquare() / LifetimeFit->GetNDF();

    // Use parameters of fit to plot individual contributions
    int n_pts = 1000;
    double t[n_pts];
    double y_background[n_pts];
    double y_signal[n_pts];
    double dt = (t_max - t_min) / (n_pts -  1);

    for (int i = 0; i < n_pts; i++) {
        t[i] = t_min + dt * i;
        y_background[i] = c_0 + c_b * TMath::Exp(-t[i]/t_b);
        y_signal[i] = c_s * TMath::Exp(-t[i]/t_s);
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
    legend->AddEntry(LifetimeFit, "Signal + Background", "l"); // l = line
    legend->AddEntry(background, "Background" , "l");
    legend->AddEntry(signal, "Signal", "l");
    legend->Draw();

    //TLatex fitModel;
    // Set normalized device coordinates, makes (x,y) position
    // range between 0 and 1 instead of in the actual data's range
    //fitModel.SetNDC(); 
    //fitModel.SetTextSize(0.03);
    //fitModel.DrawLatex(0.35, 0.7, "C_{0} + C_{b}e^{-t/#tau_{b}} + C_{s}e^{-t/#tau_{s}}");

    TLatex lifetimeResult;
    lifetimeResult.SetNDC();
    lifetimeResult.SetTextSize(0.03);
    std::string lifetimeLabel = "#tau_{s} = " + floatToString(t_s, 3, true) + " #pm " + floatToString(t_s_err, 3, true);
    lifetimeResult.DrawLatex(0.5, 0.5, lifetimeLabel.c_str());

    TLatex chiSquared;
    chiSquared.SetNDC();
    chiSquared.SetTextSize(0.03);
    std::string chiSquaredLabel = "#Chi^{2}_{reduced} = " + floatToString(reduced_chisqr, 3);
    chiSquared.DrawLatex(0.5, 0.45, chiSquaredLabel.c_str());
    // Setting to log scale to easily see exponential region turn linear 
    //gPad->SetLogy();
    //canvas->Update();

    std::string output_fname = p->name + "Lifetime.png";
    canvas->SaveAs(output_fname.c_str());
}

void MakeHists() {
    // Find ROOT file
    std::string home = std::getenv("HOME");
    std::string path = "/McGill/Multiquark/data/";
    std::string data = "datasetHUGE.root";
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

    //MakeMassHist(&k, V0Tree, 500, Cuts::cut_on_KcosTheta_3D, Cuts::KCut2, Cuts::KCut3);

    //MakeLifetimeHist(&k, V0Tree);
    MakeKLMassHist(V0Tree);
    //MakeLifetimeHist(&k, V0Tree, 400);
    //test(&k, V0Tree);
    
    //MakeInvMassHist(&hq, PVTree, 300);
    //MakeInvMassHist(&pq, PVTree, 300);
    //MakeInvMassHist(&hq, PVTree, 200);
    //LowEnergyResonanceFit(&tq, PVTree, 109);
    //HighEnergyResonanceFit(&tq, PVTree, 25);

    //MakeInvMassHist(&tq, PVTree, 80);
    //MakeDistanceCutHist(&l, V0Tree);
    //MakePtCutHist(&l, V0Tree);
}


