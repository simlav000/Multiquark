#ifndef FITS_H
#define FITS_H

#include <TMath.h>

namespace Fits {
    // Fit Functions
    inline Double_t KMassFit(Double_t *x, Double_t *par) {
        return par[0]*TMath::Gaus(x[0], par[1], par[2]) + par[3] + par[4]*x[0] + par[5]*x[0]*x[0];  // Gaussian + 2nd order polynomial
    }
    
    inline Double_t GaussPlus3rdOrderPoly(Double_t *x, Double_t *par) {
        return par[0]*TMath::Gaus(x[0], par[1], par[2]) + par[3]*x[0]*x[0]*x[0] + par[4]*x[0]*x[0] + par[5]*x[0] +  par[6];  // Gaussian + 2nd order polynomial
    }

    inline Double_t ThirdOrderPoly(Double_t *x, Double_t *par) {
        return par[0]*x[0]*x[0]*x[0] + par[1]*x[0]*x[0] + par[2]*x[0] + par[3];
    }
    
    
    inline Double_t LMassFitBreitWigner(Double_t *x, Double_t *par) { // Lambda Fit: BreitWigner + Exponential + const
        return par[0]*TMath::BreitWigner(x[0],par[1],par[2]) + par[3]*TMath::Exp(par[4]*x[0]) + par[5];
    }
    
    
    inline Double_t integrate_poly2 (Double_t p0, Double_t p1, Double_t p2, Double_t low, Double_t up) {
        return p0*(up - low) + p1*(up*up - low*low)/2 + p2*(up*up*up - low*low*low)/3;
    }
    
    
    inline Double_t lifetime_fit_2exp(Double_t *x, Double_t *par) { // Lifetime Fit: + 2 or 3 exponential terms representing kaon, lambda and bkg resp.
        return par[0] + par[1]*TMath::Exp(-1 * x[0]/par[2]) + par[3]*TMath::Exp(-1 * x[0]/par[4]);
    }
    
    
    inline Double_t lifetime_fit_3exp(Double_t *x, Double_t *par) { // Lifetime Fit: + 2 or 3 exponential terms representing kaon, lambda and bkg resp.
        return par[0] + par[1]*TMath::Exp(-1 * x[0]/par[2]) + par[3]*TMath::Exp(-1 * x[0]/par[4]) + par[5]*TMath::Exp(-1 * x[0]/par[6]);
    }
    
    
    inline Double_t exp_quadratic(Double_t *x, Double_t *par) {
        return TMath::Exp(-(par[0]*pow(x[0] - par[3], 2) + par[1]*(x[0] - par[3]) + par[2]));
    }
}
#endif // FITS_H
