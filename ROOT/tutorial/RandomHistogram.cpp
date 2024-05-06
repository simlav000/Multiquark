#include <iostream>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>

void RandomHistogram() {
    TF1 efunc("efunc", "exp([0] + [1]*x)",0.,5.);
    efunc.SetParameter(0,1);
    efunc.SetParameter(1,-1);
    TH1F h("h", "random histogram", 1000, 0., 5.);
    for (int i = 0; i < 1000; i++) {
        h.Fill(efunc.GetRandom());
    }
    TCanvas *canvas = new TCanvas("canvas", "canvas", 800, 600);
    h.Draw();
    canvas->Draw();
}
