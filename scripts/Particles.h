#include "Fits.h"
#include "Cuts.h"

#include <Rtypes.h>

#include <limits>
#include <string>

class Particle {
public:
    std::string name;
    std::string name_formatted;
    
    Double_t mass_pdg;
    Double_t life_pdg;

    // Variables from myTree
    std::string mass;
    std::string life;

    // Variables for plotting
    Double_t mass_min;
    Double_t mass_max;
    Double_t life_min;
    Double_t life_max;
    Double_t (*mass_fit_model)(Double_t*, Double_t*);
    Double_t (*life_fit_model)(Double_t*, Double_t*);

    const char* invariant_mass_label;

protected:
    Particle(std::string n) : name(n) {}

public:
    virtual ~Particle() {}

    // Delete copy constructor and assignment operator
    Particle(const Particle&) = delete;
    Particle& operator=(const Particle&) = delete;
};

class Multiquark : public Particle {
public:
    TCut default_cut;
    EColor fill_color;
    EColor line_color;

    std::string output_filename;
    std::string LER_filename; // LER: Low-energy resonance
    std::string HER_filename; // HER: High-energy 

    Double_t (*LER_mass_fit_model)(Double_t*, Double_t*);
    Double_t (*HER_mass_fit_model)(Double_t*, Double_t*);

    float LER_mass_min;
    float LER_mass_max;
    float HER_mass_min;
    float HER_mass_max;

protected:
    Multiquark(std::string name) : Particle(name) {}
    virtual ~Multiquark() {}
};

class Kaon : public Particle {
private:
    Kaon() : Particle("Kaon") {
        this->name_formatted = "K^{0}_{s}";

        this->mass_pdg = 497.611;
        this->life_pdg = 8.954e-11;

        this->mass = "KMass";
        this->life = "KLife";

        // MeV
        this->mass_min = 300;
        this->mass_max = 650;

        // s
        this->life_min = 0.2e-9;
        this->life_max = 1e-9;

        this->mass_fit_model = Fits::KMassFit;
        this->life_fit_model = Fits::lifetime_fit_2exp; 

        this->invariant_mass_label = "m_{#pi^{+}#pi^{-}} [MeV]";
    }

public:
    static Kaon& getInstance() {
        static Kaon instance;
        return instance;
    }
};

class Lambda : public Particle {
private:
    Lambda() : Particle("Lambda") {
        this->name_formatted = "#Lambda^{0}";

        this->mass_pdg = 1115.683;
        this->life_pdg = 2.632e-10;

        this->mass = "LMass";
        this->life = "LLife";

        // MeV
        this->mass_min = 1080;
        this->mass_max = 1300;

        // s
        this->life_min = 0.2e-9;
        this->life_max = 2e-9;

        this->mass_fit_model = Fits::LMassFitBreitWigner;
        this->life_fit_model = Fits::lifetime_fit_2exp; 

        this->invariant_mass_label = "m_{p^{+}#pi^{-}} [MeV]";
    }

public: 
    static Lambda& getInstance() {
        static Lambda instance;
        return instance;
    }
};

class Tetraquark : public Multiquark {
private:
    Tetraquark() : Multiquark("Tetraquark") {
        this->default_cut = Cuts::TetraquarkCut;
        this->fill_color = kTeal;
        this->line_color = static_cast<EColor>(kRed);

        // MeV
        this->LER_mass_min = 1300;
        this->LER_mass_max = 2700;
        this->HER_mass_min = 2900;
        this->HER_mass_max = 8000;

        this->output_filename = "KKInvMass.png";
        this->LER_filename = "KKInvMassLowEnergy.png";
        this->HER_filename = "KKInvMassHighEnergy";

        this->name_formatted = "K^{0}_{s}K^{0}_{s}";

        this->mass_pdg = std::numeric_limits<double>::quiet_NaN();

        this->mass = "recMassKK";
        this->life = "recLifeKK";

        this->mass_min = 0;
        this->mass_max = 10000;

        this->life_min = 0;
        this->life_max = std::numeric_limits<double>::quiet_NaN();

        this->LER_mass_fit_model = Fits::GaussPlus3rdOrderPoly;
        this->HER_mass_fit_model = Fits::exp_quadratic;
        this->life_fit_model = nullptr;

        this->invariant_mass_label = "m_{K^{0}_{s}K^{0}_{s}} [MeV]";
    }

public:
    static Tetraquark& getInstance() {
        static Tetraquark instance;
        return instance;
    }
};

class Pentaquark : public Multiquark{
private:
    Pentaquark() : Multiquark("Pentaquark") {
        this->default_cut = Cuts::PentaquarkCut;
        this->fill_color = kCyan;
        this->line_color = static_cast<EColor>(kCyan + 4);

        this->LER_mass_min = 1800;
        this->LER_mass_max = 3100;
        this->HER_mass_min = 2900;
        this->HER_mass_max = 8000;

        this->output_filename = "KLInvMass.png";
        this->LER_filename = "KLInvMassLowEnergy.png";
        this->HER_filename = "KLInvMassHighEnergy";

        this->name_formatted = "K^{0}_{s}#Lambda^{0}";

        this->mass_pdg = std::numeric_limits<double>::quiet_NaN();

        this->mass = "recMassKL";
        this->life = "recLifeKL";

        this->mass_min = 0;
        this->mass_max = 10000;

        this->life_min = 0;
        this->life_max = std::numeric_limits<double>::quiet_NaN();

        this->LER_mass_fit_model = Fits::GaussPlus3rdOrderPoly;
        this->HER_mass_fit_model = Fits::exp_quadratic;
        this->life_fit_model = nullptr;

        this->invariant_mass_label = "m_{K^{0}_{s}#Lambda^{0}} [MeV]";

    }

public: 
    static Pentaquark& getInstance() {
        static Pentaquark instance;
        return instance;
    }
};

class Hexaquark: public Multiquark{
private:
    Hexaquark() : Multiquark("Pentaquark") {
        this->default_cut = Cuts::HexaquarkCut;
        this->fill_color = static_cast<EColor>(kGreen - 6);
        this->line_color = static_cast<EColor>(kGreen - 8);

        this->LER_mass_min = 1800;
        this->LER_mass_max = 3300;
        this->HER_mass_min = 3200;
        this->HER_mass_max = 8000;

        this->output_filename = "LLInvMass.png";
        this->LER_filename = "LLInvMassLowEnergy.png";
        this->HER_filename = "LLInvMassHighEnergy";

        this->name_formatted = "#Lambda^{0}#Lambda^{0}";

        this->mass_pdg = std::numeric_limits<double>::quiet_NaN();

        this->mass = "recMassLL";
        this->life = "recLifeLL";

        this->mass_min = 0;
        this->mass_max = 10000;

        this->life_min = 0;
        this->life_max = std::numeric_limits<double>::quiet_NaN();

        this->LER_mass_fit_model = Fits::GaussPlus3rdOrderPoly; // Subject to change
        this->HER_mass_fit_model = Fits::exp_quadratic; // Subject to change
        this->life_fit_model = nullptr;

        this->invariant_mass_label = "m_{#Lambda^{0}#Lambda^{0}} [MeV]";
    }

public:
    static Hexaquark& getInstance() {
        static Hexaquark instance;
        return instance;
    }
};
