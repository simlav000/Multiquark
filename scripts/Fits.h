#include <TMath.h>

// Fit Functions
double kfit(const double *x, const double *par){  // Kaon Fit: Voight + 2nd degree polynomial
    //return par[0]*TMath::Voigt(x[0]-par[1], par[2], par[3], 4) + par[4] + par[5]*x[0] + par[6]*x[0]*x[0];   // Voight + 2nd degree polynomial 
    //return par[0]*TMath::Gaus(x[0], par[1], par[2]) + par[3] + par[4]*x[0];  // Gaussian + linear
    return par[0]*TMath::Gaus(x[0], par[1], par[2]) + par[3] + par[4]*x[0] + par[5]*x[0]*x[0];  // Gaussian + 2nd order polynomial
}

double lfit(const double *x, const double *par){ // Lambda Fit: BreitWigner + Exponential + const
    return par[0] + TMath::Exp(par[1]+par[2]*x[0]) + par[3]*TMath::BreitWigner(x[0],par[4],par[5]);
}

double integrate_poly2 (double p0, double p1, double p2, double low, double up){
    return p0*(up - low) + p1*(up*up - low*low)/2 + p2*(up*up*up - low*low*low)/3;
}

double lifetime_fit_1exp(const double *x, const double *par){ // Lifetime Fit: Const + 2 or 3 exponential terms representing kaon, lambda and bkg resp.
    return par[0] + par[1]*TMath::Exp(-1 * x[0]/par[2]);
}

double lifetime_fit_2exp(const double *x, const double *par){ // Lifetime Fit: Const + 2 or 3 exponential terms representing kaon, lambda and bkg resp.
    return par[0] + par[1]*TMath::Exp(-1 * x[0]/par[2]) + par[3]*TMath::Exp(-1 * x[0]/par[4]);
}

double lifetime_fit_3exp(const double *x, const double *par){ // Lifetime Fit: Const + 2 or 3 exponential terms representing kaon, lambda and bkg resp.
    return par[0] + par[1]*TMath::Exp(-1 * x[0]/par[2]) + par[3]*TMath::Exp(-1 * x[0]/par[4]) + par[5]*TMath::Exp(-1 * x[0]/par[6]);
}
