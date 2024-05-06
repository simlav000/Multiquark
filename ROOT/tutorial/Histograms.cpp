// Frequency distributions in ROOT are handled by a set of classes derived from
// the histogram class TH1, in our case, TH1F. The letter F stands for float.

// Here we define a function which has two parameters [0] and [1] which 
// may be set later.
TF1 efunc("efunc", "exp([0] + [1]*x)", 0.,5.0);

// Parameters are set using SetParameter(n, val) where n is the parameter number
// (in this case [0] or [1]) and val is the desired value.
efunc.SetParameter(0,1);
efunc.SetParameter(1,-1);

TH1F h("h", "example histogram", 100, 0., 5.);
for (int i=0; i<1000; i++) {h.Fill(efunc.GetRandom());}
h.Draw();

// Note: This is not a valid macro. These commands must be input directly into 
// the ROOT terminal.

// The class TH1F does not contain a convenient input format from plain text files.
// The following lines of C++ code do the job. One number per line stored in 
// the text file "expo.dat" is read in via an input stream and filled in the 
// histogram until the end of file is reached.

TH1F h("H", "example2", 100, 0.0, 5.0);
ifstream inp; double x;
inp.open("expo.dat");
while (inp >> x) {h.Fill(x);}
// Here, >> is an extraction operator. It is specifically overloaded in the
// context of ifstream objects. It is used to write data from the file 
// associated with the ifstream object inp into the variable x. This operation
// iterates over all data entries and assigns them to x one at a time until
// there are none left.
// This is a standard method of reading data files in C++.

h.Draw();
inp.close();
