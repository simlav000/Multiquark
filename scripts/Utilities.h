#ifndef UTILITIES_H
#define UTILITIES_H
#include <Rtypes.h>
#include <TMath.h>

#include <string>
#include <iomanip>
#include <iostream>

inline void printSignalArea(Double_t A, Double_t sigma) {
    Double_t sqrt_two_pi = sqrt(TMath::TwoPi());
    Double_t AreaEstimate = sqrt_two_pi * A * sigma;
    std::cout << "Area: " << AreaEstimate << std::endl;
}

inline std::string FillHist(std::string var_name, std::string hist_name) {
    std::string formatted = var_name + ">>" + hist_name;
    return formatted;
}

// Utility to convert inline float to string with specific precision
inline std::string floatToString(float v, int p, bool exp_format=false) {
    std::stringstream ss;
    if (!exp_format) {
        ss << std::fixed << std::setprecision(p) << v;
    } else {
        ss << std::setprecision(p) << v;
    }
    return ss.str();
}

#endif // UTILITIES_H
