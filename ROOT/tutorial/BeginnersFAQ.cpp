// In the official ROOT documentation, you find special data types replacing
// the normal ones, for example Double_t, Float_t or Int_t in place of double,
// float and int. Using the ROOT types makes it easier to port code between
// platforms (64/32 bit) or operating systems (windows/linux), as these types 
// are mapped to suitable ones in the ROOT header files. If you want adaptive
// code of this type, use the ROOT type declarations. However, usually you do
// not need such adaptive code, and you can safely use the standard C type
// declarations for your private code.
//
// The behaviour of a ROOT session can be tailored with the options in the 
// .rootrc file. Examples of the tunable parameters are the ones related to the
// operating and window system, to the fonts to be used, and the location of 
// start-up files.
//
// The .rootrc file defines the location of two rather important files inspected
// at startup: rootalias.C and rootlogon.C. They can contain code that needs to
// be loaded and executed at ROOT startup. rootalias.C is only loaded and best
// used to define some often used functions. rootlogon.C contains code that 
// will be executed at startup: this file is extremely useful for example to 
// pre-load a custom style for the plots created in ROOT. This is done most
// easily by creating a new TStyle object with your preferred settings, as 
// described by the class reference guide, and then use the command

gROOT->SetStyle("MyStyleName");

// to make this new style definition the default one. Another relevant file is
// rootlogoff.C which is called when a session is terminated.

// ROOT Global Pointers.
// All global pointers in ROOT begin with a small "g". Some were already 
// introduced (like gROOT), but here is more info:
//
//  - gROOT: This variable is the entry point to the ROOT system. Technically,
//  it is an instance of the TROOT class. Using the gROOT pointer, one has 
//  access to basically every objected created in a ROOT-based program. The
//  TROOT object is essentially a container of several lists pointing to the
//  main ROOT objects.
//
//  - gStyle: By default, ROOT created a default style that can be accessed via
//  the gStyle pointer. This class includes functions to set some of the 
//  following object attributes:
//      - Canvas
//      - Pad
//      - Histogram axis
//      - Lines
//      - Fill areas
//      - Text
//      - Markers
//      - Functions
//      - Histogram statistics and titles
//      - etc...
//
//  - gSystem: An instance of a base class definiting a generic interface to the
//  underlying Operating System, in our case TUnixSystem.
//
//  - gInterpreter: The entry point for the ROOT interpreter. Technically an 
//  abstraction level over a singleton instance of TCling.
