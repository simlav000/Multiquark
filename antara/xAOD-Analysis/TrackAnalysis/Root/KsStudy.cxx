#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <TrackAnalysis/KsStudy.h>

#include "TrackAnalysis/TrackTruthHelper.h"
#include "TrackAnalysis/Selection.h"

#include "xAODRootAccess/TStore.h"
#include <xAODEventInfo/EventInfo.h>

// this is needed to distribute the algorithm to the workers
ClassImp(KsStudy)




KsStudy :: KsStudy ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
  m_eventCounter= 0; //keeps track of # events

  //m_triggerChain = "HLT_mb_mbts_L1MBTS_1";    // Depending on the noise and rate, the low mu trigger will be either
  m_GRLFilePath = "TrackAnalysis/data15_13TeV.periodB1_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"; // GoodRunsList
  //  m_GRLFilePath = "TrackAnalysis/data16_13TeV.periodAllYear_DetStatus-v89-pro21-01_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"; 
  // m_GRLFilePath = "TrackAnalysis/data16_13TeV.periodAFP_DetStatus-v84-pro20-16_DQDefects-00-02-04_PHYS_StandardModel_MinimuBias2010_Ignore_BS.xml";
//m_useTrigger = false;
  m_useGRL = true;
  m_brokenTrkLinks = true; //this shouldn't be a problem anymore


}



EL::StatusCode KsStudy :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.


  job.useXAOD ();

  // let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init( "KsStudy" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  // Ntuple branches, truth --> MC
  myTree =  new TTree("KsValidation","KsValidation"); //Create KsValidation tree and branches below

  //Save information for the four tracks that recreate the secondary vertecies. The first and third will be the protons if lambda particles are recreated.
  //Currently don't have any way of distinguishing protons and pions
  myTree->Branch("TrackPt_1", &m_trackPt_1,"trackPt_1/F"); 
  myTree->Branch("TrackEta_1", &m_trackEta_1,"trackEta_1/F");
  myTree->Branch("TrackPt_2", &m_trackPt_2,"trackPt_2/F"); 
  myTree->Branch("TrackEta_2", &m_trackEta_2,"trackEta_2/F");
  myTree->Branch("TrackPt_3", &m_trackPt_3,"trackPt_3/F"); 
  myTree->Branch("TrackEta_3", &m_trackEta_3,"trackEta_3/F");
  myTree->Branch("TrackPt_4", &m_trackPt_4,"trackPt_4/F"); 
  myTree->Branch("TrackEta_4", &m_trackEta_4,"trackEta_4/F");
MultiQuark
  //Truth values for MC data
  myTree->Branch("TrueR", &m_trueR,"trueR/F");  
  myTree->Branch("TruePt", &m_truePt,"truePt/F"); 
  myTree->Branch("TrueEta", &m_trueEta,"trueEta/F");
  myTree->Branch("TrueCosTheta", &m_trueCosTheta,"trueCosTheta/F"); 

  //save information for first secondary vertex reconstructed. This will be the lambda particle if a pentaquark is reconstructed
  myTree->Branch("RecR", &m_recR,"recR/F"); 
  myTree->Branch("RecR_Err", &m_recRErr,"recRErr/F"); 
  myTree->Branch("RecDelR", &m_recDelR,"recDelR/F"); 
  myTree->Branch("RecPt", &m_recPt,"recPt/F"); 
  myTree->Branch("RecPt_Err", &m_recPtErr,"recPtErr/F"); 
  myTree->Branch("RecCosTheta", &m_recCosTheta,"recCosTheta/F"); 
  myTree->Branch("RecMass", &m_recMass,"recMass/F"); 
  myTree->Branch("RecMassErr", &m_recMassErr,"recMassErr/F"); 
  myTree->Branch("RecMassLambda", &m_recMassL,"recMassLambda/F");
  myTree->Branch("RecMassErrLambda", &m_recMassErrL,"recMassErrLambda/F");
  myTree->Branch("RecMassLambdaBar", &m_recMassLB,"recMassLambdaBar/F");
  myTree->Branch("RecMassErrLambdaBar", &m_recMassErrLB,"recMassErrLambdaBar/F");
  myTree->Branch("Vz", &m_vz,"recZ/F"); 
  myTree->Branch("Vz", &m_recPhi,"recPhi/F"); 
  myTree->Branch("Pz", &m_pz,"unitPz/F"); 
  myTree->Branch("Fz", &m_fz,"unitFz/F"); 
  myTree->Branch("pvZ", &m_PVz,"pvZ/F"); 
  myTree->Branch("chi2ndf",&m_chi2ndf,"chi2ndf/F");
  myTree->Branch("xSV", &m_xSV , "xSV/F");  // x coord of vertex
  myTree->Branch("ySV", &m_ySV , "ySV/F");  // y coord of vertex
  myTree->Branch("kLife",&m_kLife,"kLife/F");
  myTree->Branch("lLife",&m_lLife,"lLife/F");

  myTree->Branch("NumK", &m_numK,"numK/F");
  myTree->Branch("NumL", &m_numL,"numL/F"); //adding no. of Ks and Ls in the tree

  //Save information for second secondary vertex if a multiquark is reconstructed.
  //If no mutliquark reconstructed then this will be left null and only the first vertex will be 
  //saved so that lifetimes and masses of kaons and lambdas can be reconstructed properly
  /*myTree->Branch("RecR_B", &m_recR_B,"recR_B/F"); 
  myTree->Branch("RecR_Err_B", &m_recRErr_B,"recRErr_B/F"); 
  myTree->Branch("RecDelR_B", &m_recDelR_B,"recDelR_B/F"); 
  myTree->Branch("RecPt_B", &m_recPt_B,"recPt_B/F"); 
  myTree->Branch("RecPt_Err_B", &m_recPtErr_B,"recPtErr_B/F"); 
  myTree->Branch("RecCosTheta_B", &m_recCosTheta_B,"recCosTheta_B/F"); 
  myTree->Branch("RecMass_B", &m_recMass_B,"recMass_B/F"); 
  myTree->Branch("RecMassErr_B", &m_recMassErr_B,"recMassErr_B/F"); 
  myTree->Branch("RecMassLambda_B", &m_recMassL_B,"recMassLambda_B/F");
  myTree->Branch("RecMassErrLambda_B", &m_recMassErrL_B,"recMassErrLambda_B/F");
  myTree->Branch("RecMassLambdaBar_B", &m_recMassLB_B,"recMassLambdaBar_B/F");
  myTree->Branch("RecMassErrLambdaBar_B", &m_recMassErrLB_B,"recMassErrLambdaBar_B/F");
  myTree->Branch("Vz_B", &m_vz_B,"recZ_B/F"); 
  myTree->Branch("Vz_B", &m_recPhi_B,"recPhi_B/F"); 
  myTree->Branch("Pz_B", &m_pz_B,"unitPz_B/F"); 
  myTree->Branch("Fz_B", &m_fz_B,"unitFz_B/F"); 
  myTree->Branch("pvZ_B", &m_PVz_B,"pvZ_B/F"); 
  myTree->Branch("chi2ndf_B",&m_chi2ndf_B,"chi2ndf_B/F");
  myTree->Branch("xSV_B", &m_xSV_B , "xSV_B/F");  // x coord of vertex
  myTree->Branch("ySV_B", &m_ySV_B , "ySV_B/F");  // y coord of vertex
  myTree->Branch("kLife_B",&m_kLife_B,"kLife_B/F");
  myTree->Branch("lLife_B",&m_lLife_B,"lLife_B/F");*/

  //trying different branch for specifically KK
  myTree->Branch("RecRKK", &m_recRKK,"recRKK/F"); 
  myTree->Branch("RecR_ErrKK", &m_recRErrKK,"recRErrKK/F"); 
  myTree->Branch("RecDelRKK", &m_recDelRKK,"recDelRKK/F"); 
  myTree->Branch("RecPtKK", &m_recPtKK,"recPtKK/F"); 
  myTree->Branch("RecPt_ErrKK", &m_recPtErrKK,"recPtErrKK/F"); 
  myTree->Branch("RecCosThetaKK", &m_recCosThetaKK,"recCosThetaKK/F"); 
  myTree->Branch("RecMassKK", &m_recMassKK,"recMassKK/F"); 
  myTree->Branch("RecMassErrKK", &m_recMassErrKK,"recMassErrKK/F"); 
  myTree->Branch("RecMassLambdaKK", &m_recMassLKK,"recMassLambdaKK/F");
  myTree->Branch("RecMassErrLambdaKK", &m_recMassErrLKK,"recMassErrLambdaKK/F");
  myTree->Branch("RecMassLambdaBarKK", &m_recMassLBKK,"recMassLambdaBarKK/F");
  myTree->Branch("RecMassErrLambdaBarKK", &m_recMassErrLBKK,"recMassErrLambdaBarKK/F");
  myTree->Branch("VzKK", &m_vzKK,"recZKK/F"); 
  myTree->Branch("VzKK", &m_recPhiKK,"recPhiKK/F"); 
  myTree->Branch("PzKK", &m_pzKK,"unitPzKK/F"); 
  myTree->Branch("FzKK", &m_fzKK,"unitFzKK/F"); 
  myTree->Branch("pvZKK", &m_PVzKK,"pvZKK/F"); 
  myTree->Branch("chi2ndfKK",&m_chi2ndfKK,"chi2ndfKK/F");
  myTree->Branch("xSVKK", &m_xSVKK , "xSVKK/F");  // x coord of vertex
  myTree->Branch("ySVKK", &m_ySVKK , "ySVKK/F");  // y coord of vertex
  myTree->Branch("kLifeKK",&m_kLifeKK,"kLifeKK/F");
  myTree->Branch("lLifeKK",&m_lLifeKK,"lLifeKK/F");

  myTree->Branch("RecR_BKK", &m_recR_BKK,"recR_BKK/F"); 
  myTree->Branch("RecR_Err_BKK", &m_recRErr_BKK,"recRErr_BKK/F"); 
  myTree->Branch("RecDelR_BKK", &m_recDelR_BKK,"recDelR_BKK/F"); 
  myTree->Branch("RecPt_BKK", &m_recPt_BKK,"recPt_BKK/F"); 
  myTree->Branch("RecPt_Err_BKK", &m_recPtErr_BKK,"recPtErr_BKK/F"); 
  myTree->Branch("RecCosTheta_BKK", &m_recCosTheta_BKK,"recCosTheta_BKK/F"); 
  myTree->Branch("RecMass_BKK", &m_recMass_BKK,"recMass_BKK/F"); 
  myTree->Branch("RecMassErr_BKK", &m_recMassErr_BKK,"recMassErr_BKK/F"); 
  myTree->Branch("RecMassLambda_BKK", &m_recMassL_BKK,"recMassLambda_BKK/F");
  myTree->Branch("RecMassErrLambda_BKK", &m_recMassErrL_BKK,"recMassErrLambda_BKK/F");
  myTree->Branch("RecMassLambdaBar_BKK", &m_recMassLB_BKK,"recMassLambdaBar_BKK/F");
  myTree->Branch("RecMassErrLambdaBar_BKK", &m_recMassErrLB_BKK,"recMassErrLambdaBar_BKK/F");
  myTree->Branch("Vz_BKK", &m_vz_BKK,"recZ_BKK/F"); 
  myTree->Branch("Vz_BKK", &m_recPhi_BKK,"recPhi_BKK/F"); 
  myTree->Branch("Pz_BKK", &m_pz_BKK,"unitPz_BKK/F"); 
  myTree->Branch("Fz_BKK", &m_fz_BKK,"unitFz_BKK/F"); 
  myTree->Branch("pvZ_BKK", &m_PVz_BKK,"pvZ_BKK/F"); 
  myTree->Branch("chi2ndf_BKK",&m_chi2ndf_BKK,"chi2ndf_BKK/F");
  myTree->Branch("xSV_BKK", &m_xSV_BKK , "xSV_BKK/F");  // x coord of vertex
  myTree->Branch("ySV_BKK", &m_ySV_BKK , "ySV_BKK/F");  // y coord of vertex
  myTree->Branch("kLife_BKK",&m_kLife_BKK,"kLife_BKK/F");
  myTree->Branch("lLife_BKK",&m_lLife_BKK,"lLife_BKK/F");

  myTree->Branch("RecRLL", &m_recRLL,"recRLL/F"); 
  myTree->Branch("RecR_ErrLL", &m_recRErrLL,"recRErrLL/F"); 
  myTree->Branch("RecDelRLL", &m_recDelRLL,"recDelRLL/F"); 
  myTree->Branch("RecPtLL", &m_recPtLL,"recPtLL/F"); 
  myTree->Branch("RecPt_ErrLL", &m_recPtErrLL,"recPtErrLL/F"); 
  myTree->Branch("RecCosThetaLL", &m_recCosThetaLL,"recCosThetaLL/F"); 
  myTree->Branch("RecMassLL", &m_recMassLL,"recMassLL/F"); 
  myTree->Branch("RecMassErrLL", &m_recMassErrLL,"recMassErrLL/F"); 
  myTree->Branch("RecMassLambdaLL", &m_recMassLLL,"recMassLambdaLL/F");
  myTree->Branch("RecMassErrLambdaLL", &m_recMassErrLLL,"recMassErrLambdaLL/F");
  myTree->Branch("RecMassLambdaBarLL", &m_recMassLBLL,"recMassLambdaBarLL/F");
  myTree->Branch("RecMassErrLambdaBarLL", &m_recMassErrLBLL,"recMassErrLambdaBarLL/F");
  myTree->Branch("VzLL", &m_vzLL,"recZLL/F"); 
  myTree->Branch("VzLL", &m_recPhiLL,"recPhiLL/F"); 
  myTree->Branch("PzLL", &m_pzLL,"unitPzLL/F"); 
  myTree->Branch("FzLL", &m_fzLL,"unitFzLL/F"); 
  myTree->Branch("pvZLL", &m_PVzLL,"pvZLL/F"); 
  myTree->Branch("chi2ndfLL",&m_chi2ndfLL,"chi2ndfLL/F");
  myTree->Branch("xSVLL", &m_xSVLL , "xSVLL/F");  // x coord of vertex
  myTree->Branch("ySVLL", &m_ySVLL , "ySVLL/F");  // y coord of vertex
  myTree->Branch("kLifeLL",&m_kLifeLL,"kLifeLL/F");
  myTree->Branch("lLifeLL",&m_lLifeLL,"lLifeLL/F");

  myTree->Branch("RecR_BLL", &m_recR_BLL,"recR_BLL/F"); 
  myTree->Branch("RecR_Err_BLL", &m_recRErr_BLL,"recRErr_BLL/F"); 
  myTree->Branch("RecDelR_BLL", &m_recDelR_BLL,"recDelR_BLL/F"); 
  myTree->Branch("RecPt_BLL", &m_recPt_BLL,"recPt_BLL/F"); 
  myTree->Branch("RecPt_Err_BLL", &m_recPtErr_BLL,"recPtErr_BLL/F"); 
  myTree->Branch("RecCosTheta_BLL", &m_recCosTheta_BLL,"recCosTheta_BLL/F"); 
  myTree->Branch("RecMass_BLL", &m_recMass_BLL,"recMass_BLL/F"); 
  myTree->Branch("RecMassErr_BLL", &m_recMassErr_BLL,"recMassErr_BLL/F"); 
  myTree->Branch("RecMassLambda_BLL", &m_recMassL_BLL,"recMassLambda_BLL/F");
  myTree->Branch("RecMassErrLambda_BLL", &m_recMassErrL_BLL,"recMassErrLambda_BLL/F");
  myTree->Branch("RecMassLambdaBar_BLL", &m_recMassLB_BLL,"recMassLambdaBar_BLL/F");
  myTree->Branch("RecMassErrLambdaBar_BLL", &m_recMassErrLB_BLL,"recMassErrLambdaBar_BLL/F");
  myTree->Branch("Vz_BLL", &m_vz_BLL,"recZ_BLL/F"); 
  myTree->Branch("Vz_BLL", &m_recPhi_BLL,"recPhi_BLL/F"); 
  myTree->Branch("Pz_BLL", &m_pz_BLL,"unitPz_BLL/F"); 
  myTree->Branch("Fz_BLL", &m_fz_BLL,"unitFz_BLL/F"); 
  myTree->Branch("pvZ_BLL", &m_PVz_BLL,"pvZ_BLL/F"); 
  myTree->Branch("chi2ndf_BLL",&m_chi2ndf_BLL,"chi2ndf_BLL/F");
  myTree->Branch("xSV_BLL", &m_xSV_BLL , "xSV_BLL/F");  // x coord of vertex
  myTree->Branch("ySV_BLL", &m_ySV_BLL , "ySV_BLL/F");  // y coord of vertex
  myTree->Branch("kLife_BLL",&m_kLife_BLL,"kLife_BLL/F");
  myTree->Branch("lLife_BLL",&m_lLife_BLL,"lLife_BLL/F");

  myTree->Branch("RecRKL", &m_recRKL,"recRKL/F"); 
  myTree->Branch("RecR_ErrKL", &m_recRErrKL,"recRErrKL/F"); 
  myTree->Branch("RecDelRKL", &m_recDelRKL,"recDelRKL/F"); 
  myTree->Branch("RecPtKL", &m_recPtKL,"recPtKL/F"); 
  myTree->Branch("RecPt_ErrKL", &m_recPtErrKL,"recPtErrKL/F"); 
  myTree->Branch("RecCosThetaKL", &m_recCosThetaKL,"recCosThetaKL/F"); 
  myTree->Branch("RecMassKL", &m_recMassKL,"recMassKL/F"); 
  myTree->Branch("RecMassErrKL", &m_recMassErrKL,"recMassErrKL/F"); 
  myTree->Branch("RecMassLambdaKL", &m_recMassLKL,"recMassLambdaKL/F");
  myTree->Branch("RecMassErrLambdaKL", &m_recMassErrLKL,"recMassErrLambdaKL/F");
  myTree->Branch("RecMassLambdaBarKL", &m_recMassLBKL,"recMassLambdaBarKL/F");
  myTree->Branch("RecMassErrLambdaBarKL", &m_recMassErrLBKL,"recMassErrLambdaBarKL/F");
  myTree->Branch("VzKL", &m_vzKL,"recZKL/F"); 
  myTree->Branch("VzKL", &m_recPhiKL,"recPhiKL/F"); 
  myTree->Branch("PzKL", &m_pzKL,"unitPzKL/F"); 
  myTree->Branch("FzKL", &m_fzKL,"unitFzKL/F"); 
  myTree->Branch("pvZKL", &m_PVzKL,"pvZKL/F"); 
  myTree->Branch("chi2ndfKL",&m_chi2ndfKL,"chi2ndfKL/F");
  myTree->Branch("xSVKL", &m_xSVKL , "xSVKL/F");  // x coord of vertex
  myTree->Branch("ySVKL", &m_ySVKL , "ySVKL/F");  // y coord of vertex
  myTree->Branch("kLifeKL",&m_kLifeKL,"kLifeKL/F");
  myTree->Branch("lLifeKL",&m_lLifeKL,"lLifeKL/F");

  myTree->Branch("RecR_BKL", &m_recR_BKL,"recR_BKL/F"); 
  myTree->Branch("RecR_Err_BKL", &m_recRErr_BKL,"recRErr_BKL/F"); 
  myTree->Branch("RecDelR_BKL", &m_recDelR_BKL,"recDelR_BKL/F"); 
  myTree->Branch("RecPt_BKL", &m_recPt_BKL,"recPt_BKL/F"); 
  myTree->Branch("RecPt_Err_BKL", &m_recPtErr_BKL,"recPtErr_BKL/F"); 
  myTree->Branch("RecCosTheta_BKL", &m_recCosTheta_BKL,"recCosTheta_BKL/F"); 
  myTree->Branch("RecMass_BKL", &m_recMass_BKL,"recMass_BKL/F"); 
  myTree->Branch("RecMassErr_BKL", &m_recMassErr_BKL,"recMassErr_BKL/F"); 
  myTree->Branch("RecMassLambda_BKL", &m_recMassL_BKL,"recMassLambda_BKL/F");
  myTree->Branch("RecMassErrLambda_BKL", &m_recMassErrL_BKL,"recMassErrLambda_BKL/F");
  myTree->Branch("RecMassLambdaBar_BKL", &m_recMassLB_BKL,"recMassLambdaBar_BKL/F");
  myTree->Branch("RecMassErrLambdaBar_BKL", &m_recMassErrLB_BKL,"recMassErrLambdaBar_BKL/F");
  myTree->Branch("Vz_BKL", &m_vz_BKL,"recZ_BKL/F"); 
  myTree->Branch("Vz_BKL", &m_recPhi_BKL,"recPhi_BKL/F"); 
  myTree->Branch("Pz_BKL", &m_pz_BKL,"unitPz_BKL/F"); 
  myTree->Branch("Fz_BKL", &m_fz_BKL,"unitFz_BKL/F"); 
  myTree->Branch("pvZ_BKL", &m_PVz_BKL,"pvZ_BKL/F"); 
  myTree->Branch("chi2ndf_BKL",&m_chi2ndf_BKL,"chi2ndf_BKL/F");
  myTree->Branch("xSV_BKL", &m_xSV_BKL , "xSV_BKL/F");  // x coord of vertex
  myTree->Branch("ySV_BKL", &m_ySV_BKL , "ySV_BKL/F");  // y coord of vertex
  myTree->Branch("kLife_BKL",&m_kLife_BKL,"kLife_BKL/F");
  myTree->Branch("lLife_BKL",&m_lLife_BKL,"lLife_BKL/F");

  //Saves for multiquark data
  myTree->Branch("RecConvMass", &m_recConvMass,"recConvMass/F");
  myTree->Branch("pplus",&m_pplus,"pplus/F");
  myTree->Branch("pminus",&m_pminus,"pminus/F");
  myTree->Branch("nSCTp", &m_nSCTp, "nSCTp/I" );
  myTree->Branch("nSCTn", &m_nSCTn, "nSCTn/I");
  myTree->Branch("nPixp", &m_nPixp, "nPixp/I");//!
  myTree->Branch("nPixn", &m_nPixn, "nPixn/I");
  myTree->Branch("htp", &m_htp ,"htp/F");
  myTree->Branch("htn", &m_htn , "htn/F");

 // myTree->Branch("numSV", &m_numSV, "numSV/I");  // number of secondary vertices

  myTree->Branch("invMKK", &m_invMKK, "invMKK/F"); // K-K (tetraquark) invariant mass
  myTree->Branch("invMKL", &m_invMKL, "invMKL/F"); // K-L (pentaquark) invariant mass
  myTree->Branch("invMLL", &m_invMLL, "invMLL/F"); // L-L (sextaquark) invariant mass


  wk()->addOutput (myTree); // wk()->addoutput(__) delivers result of an algorithm to the outside world

  m_recoPosvsReco =  new TH2D("RecoPossibleVsRecod",";Both Tracks; Vertex;",11,-0.5,10.5,11,-0.5,10.5);
  wk()->addOutput (m_recoPosvsReco);
  
  h_recoPosandReco =  new TH1D("RecoPossibleAndRecod","Events",3,0,3);
  h_recoPosandReco->SetCanExtend(TH1::kAllAxes); // For making the hist axes extendable
  wk()->addOutput (h_recoPosandReco);
 
  // the following gets filled into labelled bins 
  // (adds 1 to a bin each time an event passes the cut indivated by the label)
  h_cuts = new TH1D("Cuts","Events per cut",3,0,3);
  h_cuts->SetCanExtend(TH1::kAllAxes); // extendable axes
  wk()->addOutput (h_cuts);

  h_ksCandidates = new TH1D("KsCandidatesCuts","Vertices per cut",3,0,3);
  h_ksCandidates->SetCanExtend(TH1::kAllAxes); // extendable axes
  wk()->addOutput(h_ksCandidates);

  h_numSVs = new TH1I("NumSVs","Number of SVs per Event",31,-0.5,30.5);
  //h_numSVs->SetCanExtend(TH1::kAllAxes);
  wk()->addOutput(h_numSVs); 

  h_numKs = new TH1I("NumKs","Number of Ks per Event",31,-0.5,30.5);
  wk()->addOutput(h_numKs);

  h_numLs = new TH1I("NumLs","Number of Ls per Event",31,-0.5,30.5);
  wk()->addOutput(h_numLs);


  h_numKcuts = new TH1I ("Kaon cuts", "Number of Ks after each cut", 10,0,10);
  wk()->addOutput(h_numKcuts);

  h_numLcuts = new TH1I ("Lambda cuts", "Number of Ls after each cut", 10,0,10);
  wk()->addOutput(h_numLcuts);
 
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  m_KKcheck = 0;
  m_LLcheck = 0;
  m_KLevent = 0;
  //m_KLevent1 = 0;
  m_KLcheck = 0;
  m_KL00 = 0;
  m_KL01 = 0;
  m_KL02 = 0;
  m_KL03 = 0;
  m_KL04 = 0;
  m_KL05 = 0;
  m_KL06 = 0;
  m_KL07 = 0;
  m_KL08 = 0;
  m_KL09 = 0;
  m_KL10 = 0;
  m_KL11 = 0;
  m_0KcountKL04 = 0;
  m_1KcountKL04 = 0;
  m_2KcountKL04 = 0;
  m_3KcountKL04 = 0;
  m_4KcountKL04 = 0;
  m_5KcountKL04 = 0;
  m_6KcountKL04 = 0;
  m_7KcountKL04 = 0;
  m_8KcountKL04 = 0;
  m_9KcountKL04 = 0;
  m_10KcountKL04 = 0;

  xAOD::TEvent xaodEvent(xAOD::TEvent::kAthenaAccess);
  m_event = wk()->xaodEvent();
  
  // as a check, let's see the number of events in our xAOD
  //Info("initialize()", "Number of events = %lli", m_event->getEntries() ); // print long long int
  // GoodRunsList setup
  if (m_useGRL) {
    m_grl = new GoodRunsListSelectionTool("GoodRunsListSelectionTool");

    std::string fullGRLFilePath = PathResolverFindCalibFile(m_GRLFilePath);
    std::vector<std::string> vecStringGRL;
    vecStringGRL.push_back(fullGRLFilePath.c_str());

    m_grl->setProperty( "GoodRunsListVec", vecStringGRL);
    m_grl->setProperty("PassThrough", false); // if true (default) will ignore result of GRL and will just pass all events

    if (m_grl->initialize().isFailure()) {
      Error("initialize()","Failed to initialize trigger decision tool");
      return EL::StatusCode::FAILURE;
    }
    Info("initialize()", "GRL file : %s", m_GRLFilePath.c_str());
  }

  return EL::StatusCode::SUCCESS;
}


EL::StatusCode KsStudy :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  // print every 2000 events, so we know where we are:
  if( (m_eventCounter % 1000) ==0 )
  Info("execute()", "Event number = %i", m_eventCounter );
  m_eventCounter++;
  h_cuts->Fill(ALL_EVENTS,1); // Fill "All Events" bin with another event

  //  if (m_eventCounter != 6758)
  // return EL::StatusCode::SUCCESS;
  //----------------------------
  // Event information
  //---------------------------
  const xAOD::EventInfo* eventInfo = 0;  // EventInfo contains stuff like tracks, date, etc.
  if( ! m_event->retrieve( eventInfo, "EventInfo").isSuccess() ){
    Error("execute()", "Failed to retrieve event info collection. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  // Info("eventNumber = %llu", eventInfo->eventNumber());

  // check if the event is data or MC
  // (many tools are applied either to data or MC)
  bool isMC = false;
  // check if the event is MC
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){ //check if event is a simulation
    isMC = true; // if so, mark is as MC
  }

  //---------------------------
  // Apply GoodRunsList
  //---------------------------
  if ( m_useGRL && !isMC && !m_grl->passRunLB(*eventInfo))
    return EL::StatusCode::SUCCESS;

  h_cuts->Fill(GRL_CUT,1);


  //---------------------------
  // Primary Vertex
  //---------------------------
  const xAOD::VertexContainer* vertices = 0;
  if ( !m_event->retrieve( vertices, "PrimaryVertices" ).isSuccess() ){ // retrieve arguments: container type, container key (from SG). vertices now contains PrimaryVertices
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
    return EL::StatusCode::FAILURE;
  }


  /*
 * the following are taken from StoreGate (SG) and mostly filled in InDetV0Finder and declared in header
 * file. These accessors take the values associated to the unconstr. V0 container. (Details in 
 * InDetV0Finder.cxx and V0Tools.cxx). Essentially, in the V0 container, each entry has an associated
 * value for the invariant masses of K, L, LB. All three were calculated for each entry by assuming 
 * the vertex was each of those particles (one at a time). Cuts are to be done later to decide which 
 * particle each of the entries actually is.
 */

  static  SG::AuxElement::ConstAccessor< float > sumPTacc( "sumPt2" );
  static  SG::AuxElement::ConstAccessor< float > massacc( "Kshort_mass" );
  static  SG::AuxElement::ConstAccessor< float > massErracc( "Kshort_massError" );
  static  SG::AuxElement::ConstAccessor< float > massaccL( "Lambda_mass" );
  static  SG::AuxElement::ConstAccessor< float > massErraccL( "Lambda_massError" );
  static  SG::AuxElement::ConstAccessor< float > massaccLB( "Lambdabar_mass" );
  static  SG::AuxElement::ConstAccessor< float > massErraccLB( "Lambdabar_massError" );
  static  SG::AuxElement::ConstAccessor< float > ptErracc( "pTError" );
  static  SG::AuxElement::ConstAccessor< float > rErracc( "RxyError" );
  static  SG::AuxElement::ConstAccessor< float > pxacc( "px" );
  static  SG::AuxElement::ConstAccessor< float > pyacc( "py" );
  static  SG::AuxElement::ConstAccessor< float > pzacc( "pz" );


// for each vertex in the PrimaryVertices container, check and see if it's a pileup or a primary vertex
  bool hasPrimaryVertex = false;
  bool hasPileUpVertex = false;
  const xAOD::Vertex* primaryVertex = 0;
  for( auto vertex: *vertices ) {                           //****Why 4 track links. What track links are counted in this
    if( vertex->vertexType() == xAOD::VxType::PileUp and
        vertex->trackParticleLinks().size() >= 4 ){ //trackParticleLinks = vector of trks used to make particle
      //to be considered a vertex, it should have at least 4 tracks, otherwise it could be something else mistaken for a vertex
      hasPileUpVertex = true;
    }
    if( vertex->vertexType() == xAOD::VxType::PriVtx ) {
      primaryVertex = vertex; // assign it as the primary vertex (the vertex of interest)
      hasPrimaryVertex = true;
    }
  }

  if( !hasPrimaryVertex )
    return EL::StatusCode::SUCCESS;

  h_cuts->Fill(PRIMARY_VERTEX_CUT,1); // add 1 to "Has Primary Vertex" bin

  if ( hasPileUpVertex )
    return EL::StatusCode::SUCCESS;

  h_cuts->Fill(PILEUP_VERTEX_CUT,1); // add 1 to "No pileup vertex" bin
  
  // get track container of interest
  const xAOD::TrackParticleContainer* recoTracks = 0;
  if ( !m_event->retrieve( recoTracks, "InDetTrackParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
    Error("execute()", "Failed to retrieve Reconstructed Track container. Exiting." );
    return EL::StatusCode::FAILURE;
  }

  Selection selection_noChi2;
  selection_noChi2.defaultSelection(); // applies default cuts (see cut.h and selection.h)
                                       // some of the default cuts: chi2=0.01, pT=500, eta=2.5, etc...
  selection_noChi2.replace(new PtCut(100.)); // replace default pT cut to 100MeV
  selection_noChi2.remove("Chi2Cut"); // remove the cut on chi2

  TrackTruthHelpers tth(100,3.5); // pT=100, eta=3.5, see TrackTruthHelper.h/.cxx
  
  const xAOD::VertexContainer*  ksCandidates = 0;
  if ( !m_event->retrieve( ksCandidates, "V0UnconstrVertices" ).isSuccess() ){ // retrieve arguments: container type, container key. Fill with V0 vertices (all, not just Ks), stored as pairs (2 tracks).
    Error("execute()", "Failed to retrieve V0UnconstrVertice container. Exiting." );
    return EL::StatusCode::FAILURE;
  }
  VertexContainerShallow v0_sc_pair = xAOD::shallowCopyContainer( *ksCandidates ); 
  const auto v0_sc = std::unique_ptr<xAOD::VertexContainer>(v0_sc_pair.first);
  const auto v0_sc_aux = std::unique_ptr<xAOD::ShallowAuxContainer>(v0_sc_pair.second);
  
  std::map<int, int> ksTracks; // initializing map (<key,value>), container and pointers
  std::vector<const xAOD::TruthParticle*> ksTruths;
  const xAOD::TruthParticleContainer* truthParticles = 0;
  std::unique_ptr<xAOD::TruthParticleContainer> truth_sc;
  std::unique_ptr<xAOD::ShallowAuxContainer> truth_sc_aux;
  int actualReco = 0;

/*****************************************************************************************************/
   if (isMC) { //MC ONLY
    if ( !m_event->retrieve( truthParticles, "TruthParticles" ).isSuccess() ){ // retrieve arguments: container type, container key
      Error("execute()", "Failed to retrieve Truth Particle container. Exiting." );
      return EL::StatusCode::FAILURE;
    }
    TruthParticleContainerShallow truth_sc_pair = xAOD::shallowCopyContainer( *truthParticles );
    truth_sc.reset(truth_sc_pair.first); // truth_sc now points to truth_sc_pair.first 
    truth_sc_aux.reset(truth_sc_pair.second); // truth_sc_aux now points to truth_sc_pair.second
    
    //Find all Ks's in the truth record and add them to ksTruths vector
    for( auto truth : *truthParticles){   // for each particle in the truth container
      if(truth->pdgId() == 310){  // Pdg for Kshort is 310
        ksTracks[ truth->barcode() ] = 0 ; // set value at key (truth-> barcode) in map to 0
        ksTruths.push_back(truth);  // add to vector of kaons at the truth/generator level
      }
    }
    
    // Loop over the truth particles shadow copy and set selection decoration flags
    for (xAOD::TruthParticle* truth : *truth_sc) { //truth_sc points to truth_sc_pair.first
      // Is K0 or Lamba 
      const StrangeParticle_t strange = checkStrangeHadron(truth); // Apply truth PID on strange & decay
		                     // for truth, returns kRejected, kK0s, kLambda0 or kLambdaBar0
      truth->auxdata<int>("STRANGE_CLASSIFY") = (int)strange;
   
       
      // For each non-rejected truth particle, find the best matching candidate from MC (v0_sc container)
      if (strange != kRejected) { // If passes truth preselection, try and match to a reco vtx (via mass difference and deltaR)
        xAOD::Vertex* bestCadidate = MatchStrangeToRecoVertex(truth, strange, v0_sc.get());
 				// .get() returns pointer to managed objet (v0_sc_pair.first)
        truth->auxdata<xAOD::Vertex*>("RECO_VERTEX_LINK") = bestCadidate;
      }
    }
  }

/*****************************************************************************************************/
//BOTH DATA AND MC
  
std::vector<xAOD::Vertex*> SVs; // Temporarily save each SV
std::vector<xAOD::Vertex*> KSVs;
std::vector<xAOD::Vertex*> LSVs;
std::vector<int> VType; // kaon = 1, lambda = 2, neither = 0

  bool hasSecVertex = false;
  bool only2K = true; // to use Method 1 (better method)
  bool Kcalculated = false;
  bool Lcalculated = false;
  bool calculated = false;
  bool Kcalculated1 = false;
  bool Lcalculated1 = false;
  bool calculated1 = false; 
  int counter = 0;    
  int Kcounter = 0;
  int Lcounter = 0; 
  bool passKCuts;
  bool passLCuts;
  //  int Lcounter1 = 0;
  //int Kcounter1 = 0;
  int flag = 2;
  //int KKflag = 0;
  //int KLflag = 0;
  //int LLflag = 0;
  //  int K_cos = 0;
  //int K_masslow = 0;

  //final varibles for cuts
  const float IGNORE = -999;

  const float STORE = -100;

  const float COS_THETA_CUT = 0.9998;//0.9998
  const float K_MASS_LOW = 300;//400
  const float K_MASS_HIGH = 700;//600
  const float K_MASS_LOW2 = 340; //this was present in 0303v1, v2, not present in v3
  const float K_MASS_HIGH2 = 480;//475, 500 in v3, 480 in v1
  const float L_MASS_LOW = 900;//1000
  const float L_MASS_LOW2 = 1125;//1125 in 0303v1, 1000 in v3 (and in v2 I guess)
  const float L_MASS_HIGH = 1300;//1200
  const float K_3D_DIST = 3;//4
  const float L_3D_DIST = 13;//17
  const float K_PT = 300;//400
  const float L_PT = 400;//500
  const float C = 299792458;
  const float K_MASS_PDG = 497.648;
  const float L_MASS_PDG = 1115.683;

  const float PT_CUT = 70;//100
  const float ETA_CUT = 3;//2.5
 
  // For multiquark mass method 1 below: Count number of kaons to know when there are only 2 
  if (only2K){
    for ( auto secvtx: *v0_sc ) {
        float px_sVtx  =  pxacc( *secvtx ) ;  //MeV
        float py_sVtx = pyacc( *secvtx );  //MeV
        
        float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
        
        float deltaX = (secvtx -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
        float deltaY = (secvtx -> y() - primaryVertex -> y());
        float deltaZ = (secvtx -> z() - primaryVertex -> z());

        float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
        float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
        
        float pt =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
        float cosTheta = myCosTheta;
        float mass    = massacc( *secvtx );
        float massL    = massaccL( *secvtx );
        float delR = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
        
        // Check if vertex passes Kaon cuts
        if (cosTheta > COS_THETA_CUT && mass > K_MASS_LOW && mass < K_MASS_HIGH && massL > L_MASS_LOW2 && delR > K_3D_DIST && pt > K_PT){ 
            Kcounter++;
	}
        if (cosTheta > COS_THETA_CUT && massL > L_MASS_LOW && massL < L_MASS_HIGH && mass > K_MASS_LOW2 && mass < K_MASS_HIGH2 && delR > L_3D_DIST && pt > L_PT){
            Lcounter++;
	}
	//std::cout << "Kcounter inside loop = " << Kcounter << std::endl;
	//std::cout << "Lcounter inside loop = " << Lcounter << std::endl;
	//std::cout << "Vtx = " << secvtx << std::endl;
    }
  }
  // Fill histogram of counts
  //  std::cout << "Kcounter = " << Kcounter << std::endl;
  //std::cout << "Lcounter = " << Lcounter << std::endl;
  
  if (Kcounter + Lcounter >= 2){
    flag = 0;
  }
  //    std::cout << "flag = " << flag << std::endl;
  

  if (Kcounter == 1 && Lcounter == 1){
    // std::cout << "K = " << Kcounter << ", L = " << Lcounter << std::endl;
    m_KLevent++;
    //Info("K=1, L=1", "Event number = %i", m_eventCounter );
  }
  if (Kcounter + Lcounter == 0)
    m_KL00++;
  if (Kcounter + Lcounter == 1)
    m_KL01++;
  if (Kcounter + Lcounter == 10){
    m_KL10++;
    /*    if (Kcounter == 0)
      m_0KcountKL04++;
    if (Kcounter == 1)
      m_1KcountKL04++;
    if (Kcounter == 2)
      m_2KcountKL04++;
    if (Kcounter == 3)
      m_3KcountKL04++;
    if (Kcounter == 4)
      m_4KcountKL04++;
    if (Kcounter == 5)
      m_5KcountKL04++;
    if (Kcounter == 6)
      m_6KcountKL04++;
    if (Kcounter == 7)
      m_7KcountKL04++;
    if (Kcounter == 8)
      m_8KcountKL04++;
    if (Kcounter == 9)
      m_9KcountKL04++;
    if (Kcounter == 10)
    m_10KcountKL04++;*/
      }
  if (Kcounter + Lcounter == 2){
    m_KL02++;
    /*if (Kcounter == 0)                                                                                                                                                                                    
      m_0KcountKL04++;                                                                                                                                                                                    
    if (Kcounter == 1)                                                                                                                                                                                   
      m_1KcountKL04++;                                                                                                                                                                                    
    if (Kcounter == 2)    
    m_2KcountKL04++;  */
  }
  if (Kcounter + Lcounter == 3)
    m_KL03++;
  if (Kcounter + Lcounter == 4)
    m_KL04++;
  if (Kcounter + Lcounter == 5)
    m_KL05++;
  if (Kcounter + Lcounter == 6)
    m_KL06++;
  if (Kcounter + Lcounter == 7)
    m_KL07++;
  if (Kcounter + Lcounter == 8)
    m_KL08++;
  if (Kcounter + Lcounter == 9)
    m_KL09++;
  if (Kcounter + Lcounter >= 11)
    m_KL11++;


  h_numKs->Fill(Kcounter);
  h_numLs->Fill(Lcounter);
  
  int secvcheck = 0;
  int KKcheck = 0;
  int LLcheck = 0;
  int KLcheck = 0;
  int KKcheck1 = 0;
  int LLcheck1 = 0;
  int KLcheck1 = 0;
  int line = 0;

      
  // Obtain parameters for each SV 
  std::map<int, const xAOD::TruthParticle*> ksTruthMatches; // ksTruths with matches
  for( auto secvtx: *v0_sc ) { // for each SV in v0_sc (points to v0_sc_first)

    h_ksCandidates->Fill(KS_CANDIDATES,1);
    hasSecVertex = true;
    passKCuts = true;
    passLCuts = true;
  
    // Only use SV with 2 track particles that ecah have pT > 100 MeV and eta < 2.5
    if(secvtx->nTrackParticles() == 2 
        && secvtx->trackParticle(0)->pt()>PT_CUT && secvtx->trackParticle(1)->pt()>PT_CUT
        && secvtx->trackParticle(0)->eta()<ETA_CUT && secvtx->trackParticle(1)->eta()<ETA_CUT){  
        
      secvcheck++;
      line++;

        if (secvtx->nTrackParticles() == 2) counter++; // counts # of SV with 2 tracks
        // Note: because of the way the container was originally filled (by pairs), there are no 3 or 4 track SV (i think)
    
        if(secvtx->nTrackParticles() != 2)  // Ignore if vertex doesn't have exactly 2 tracks
            continue;	

        // The following code sets values in the tree branches (for each vertex in v0_sc, data & MC)
        // For the accessors, the values are calculated in InDetV0Finder.cxx

        m_chi2ndf = secvtx->chiSquared()/secvtx->numberDoF();
        
        float px_sVtx = pxacc( *secvtx );  //MeV
        float py_sVtx = pyacc( *secvtx );  //MeV
        
        float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
        
        float deltaX = (secvtx -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
        float deltaY = (secvtx -> y() - primaryVertex -> y());
        float deltaZ = (secvtx -> z() - primaryVertex -> z());

        float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
        float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
        // cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
        
        m_recDelR = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
        
        m_PVz = primaryVertex->z(); // Z component of PV
        
        m_pz  = pzacc( *secvtx  ); // Z component of SV's momentum
        m_pz /=  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx + m_pz*m_pz ); // Pz = pz/|p|  (SV)
        
        m_recR = sqrt(secvtx->x()*secvtx->x() + secvtx->y()*secvtx->y());
        m_recPhi = atan2( secvtx->y(), secvtx->x() ); // phi = arctan(y_sv/x_sv)
        m_vz  =  secvtx->z(); // z component of SV

        m_recPt =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
        m_recCosTheta = myCosTheta;
        m_recMass    = massacc( *secvtx );
        m_recMassErr = massErracc( *secvtx  );
        m_recPtErr   = ptErracc( *secvtx  );
        m_recRErr    = rErracc( *secvtx  );

        m_recMassL    = massaccL( *secvtx );
        m_recMassErrL = massErraccL( *secvtx  );
        m_recMassLB    = massaccLB( *secvtx );
        m_recMassErrLB = massErraccLB( *secvtx  );

        m_recPhi = atan2( secvtx->y(), secvtx->x() );
        m_fz     = deltaZ/m_recDelR;
        
        // x y coordinates of the SV
        m_xSV = secvtx->x();
        m_ySV = secvtx->y();

        m_kLife = m_recDelR/1000/((sqrt(m_pz*m_pz*m_recPt*m_recPt/(1-m_pz*m_pz)+m_recPt*m_recPt)/sqrt((m_pz*m_pz*m_recPt*m_recPt/(1-m_pz*m_pz)+m_recPt*m_recPt) + K_MASS_PDG*K_MASS_PDG))*C);
        m_lLife = m_recDelR/1000/((sqrt(m_pz*m_pz*m_recPt*m_recPt/(1-m_pz*m_pz)+m_recPt*m_recPt)/sqrt((m_pz*m_pz*m_recPt*m_recPt/(1-m_pz*m_pz)+m_recPt*m_recPt) + L_MASS_PDG*L_MASS_PDG))*C);

	m_numK = Kcounter;
	m_numL = Lcounter;

	//std::cout << "Kcounter = " << Kcounter << std::endl;

	h_numKcuts -> Fill(NO_CUTS_K, 1);      
	if (m_recCosTheta > COS_THETA_CUT){
	  h_numKcuts -> Fill(COS_CUT_K,1);
	  if (m_recMass > K_MASS_LOW){
	    h_numKcuts -> Fill(KMASS_LOW_CUT_K,1);
	    if (m_recMass < K_MASS_HIGH){
	      h_numKcuts -> Fill(KMASS_HIGH_CUT_K,1);
		if (m_recMassL > L_MASS_LOW2){
		  h_numKcuts -> Fill(LMASS_LOW_CUT_K,1);
		  if (m_recDelR > K_3D_DIST){
		    h_numKcuts -> Fill(DELR_CUT_K,1);
		    if (m_recPt > K_PT){
		    h_numKcuts -> Fill(PT_CUT_K,1);
		    }
		  }
		}
	    }
	  }
	  }  //cutflow for Kcuts

	h_numLcuts -> Fill(NO_CUTS_L, 1);
	if (m_recCosTheta > COS_THETA_CUT){
          h_numLcuts -> Fill(COS_CUT_L,1);
          if (m_recMassL > L_MASS_LOW){
            h_numLcuts -> Fill(LMASS_LOW_CUT_L,1);
            if (m_recMassL < L_MASS_HIGH){
              h_numLcuts -> Fill(LMASS_HIGH_CUT_L,1);
	      if (m_recMass > K_MASS_LOW2){
		h_numLcuts -> Fill(KMASS_LOW_CUT_L,1);
		if (m_recMass < K_MASS_HIGH2){
		  h_numLcuts -> Fill(KMASS_HIGH_CUT_L,1);
		  if (m_recDelR > L_3D_DIST){
		  h_numLcuts -> Fill(DELR_CUT_L,1);
		    if (m_recPt > L_PT){
                    h_numLcuts -> Fill(PT_CUT_L,1);
		    }
		  }
		}
	      }
	    }
	  } 
	} //cutflow for Lcuts  

    // MULTIQUARK INVARIANT MASS
    // Method 1: Calculate multiquark mass if there are exactly 2 K, 2 L, or 1 K, 1 L in the event
	//if (m_recCosTheta > COS_THETA_CUT && m_recMass > K_MASS_LOW && m_recMass < K_MASS_HIGH && m_recMassL > L_MASS_LOW2 && m_recDelR > K_3D_DIST && m_recPt > K_PT) 
	  //m_recMass_B = m_recMass;
	  //else m_recMass_B = -1;
	  //Kcounter1++;
	//std::cout << "Kcounter1 = " << Kcounter1 << std::endl;
	
	//if (m_recCosTheta > COS_THETA_CUT && m_recMassL > L_MASS_LOW && m_recMassL < L_MASS_HIGH && m_recMass > K_MASS_LOW2 && m_recMass < K_MASS_HIGH2 && m_recDelR > L_3D_DIST && m_recPt > L_PT)
	//m_recMassL_B = m_recMassL;
	//else m_recMassL_B = -1;
	  //Lcounter1++;
	//std::cout << "Lcounter1 = " << Lcounter1 << ", line = " << line << std::endl;
       
    if (only2K && flag == 0){
      if (Kcounter==2){ // If there are exactly 2 kaons
	if (m_recCosTheta > COS_THETA_CUT && m_recMass > K_MASS_LOW && m_recMass < K_MASS_HIGH && m_recMassL > L_MASS_LOW2 && m_recDelR > K_3D_DIST && m_recPt > K_PT)
                       KSVs.push_back(secvtx);
      }
      if (Lcounter==2){ // If there are exactly 2 lambdas
	if (m_recCosTheta > COS_THETA_CUT && m_recMassL > L_MASS_LOW && m_recMassL < L_MASS_HIGH && m_recMass > K_MASS_LOW2 && m_recMass < K_MASS_HIGH2 && m_recDelR > L_3D_DIST && m_recPt > L_PT)
                       LSVs.push_back(secvtx);
      }
      if (Lcounter==1 && Kcounter==1){ // If there is exactly 1 kaon and 1 lambda
  		  if (m_recCosTheta > COS_THETA_CUT && m_recMass > K_MASS_LOW && m_recMass < K_MASS_HIGH && m_recMassL > L_MASS_LOW2 && m_recDelR > K_3D_DIST && m_recPt > K_PT){
                       SVs.push_back(secvtx);
		       //std::cout << "It's a K"<<std::endl;
		       //std::cout << "SVs.at(0) = " << SVs.at(0) << std::endl;
  		  }else if (m_recCosTheta > COS_THETA_CUT && m_recMassL > L_MASS_LOW && m_recMassL < L_MASS_HIGH && m_recMass > K_MASS_LOW2 && m_recMass < K_MASS_HIGH2 && m_recDelR > L_3D_DIST && m_recPt > L_PT){
                       if(!SVs.empty()){
			 //std::cout << "first was K, but it will be moved behind"<<std::endl;
                          xAOD::Vertex* kaonTemp = SVs.back();
			  //std::cout << "kaonTemp = "<<kaonTemp.size()<< std::endl;
                          SVs.pop_back();
			  //std::cout << "SVs.size = "<<SVs.size()<< std::endl;
                          SVs.push_back(secvtx);
			  //std::cout << "SVs.size = "<<SVs.size()<< std::endl;
			  //std::cout << "SVs.at(0) after L is inserted = " << SVs.at(0) << std::endl;
                          SVs.push_back(kaonTemp);
			  //std::cout << "SVs.size = "<<SVs.size()<< std::endl;
			  //std::cout << "SVs.at(1) after L is inserted = " << SVs.at(1) << std::endl;
			  //if(SVs.size()==2)
			  //std::cout << "woo" << std::endl;
                       }else{
                          SVs.push_back(secvtx);
			  //std::cout << "First is L" << std::endl;
                       }
        }
      }
      
      if (KSVs.size()==2)
	KKcheck1++;
      
      if (LSVs.size()==2)
	LLcheck1++;
      
      if (SVs.size()==2)
	KLcheck1++;
  	
      // If 2 kaons are found
       if (KSVs.size()==2 && !Kcalculated){

        //The following code calculates and sets the second secondary vertex information which will be a kaon

	//int Kcounter_cos = 0;
	
	//if (m_recCosTheta > COS_THETA_CUT)
	//Kcounter_cos ++;


	m_chi2ndfKK = KSVs.at(1)->chiSquared()/KSVs.at(1)->numberDoF();
            
       	float px_sVtx = pxacc( *KSVs.at(1) );  //MeV
	float py_sVtx = pyacc( *KSVs.at(1) );  //MeV
            
	float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
            
	float deltaX = (KSVs.at(1) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
	float deltaY = (KSVs.at(1) -> y() - primaryVertex -> y());
	float deltaZ = (KSVs.at(1) -> z() - primaryVertex -> z());

	float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
	float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
	// cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
            
	m_recDelRKK = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
            
	m_PVzKK = primaryVertex->z(); // Z component of PV
            
	m_pzKK  = pzacc( *KSVs.at(1)  ); // Z component of SV's momentum
	m_pzKK /=  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx + m_pzKK*m_pzKK ); // Pz = pz/|p|  (SV)
            
	m_recRKK = sqrt(KSVs.at(1)->x()*KSVs.at(1)->x() + KSVs.at(1)->y()*KSVs.at(1)->y());
	m_recPhiKK = atan2( KSVs.at(1)->y(), KSVs.at(1)->x() ); // phi = arctan(y_sv/x_sv)
	m_vzKK  =  KSVs.at(1)->z(); // z component of SV

	m_recPtKK =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
	m_recCosThetaKK = myCosTheta;
	m_recMassKK    = massacc( *KSVs.at(1) );
	m_recMassErrKK = massErracc( *KSVs.at(1)  );
	m_recPtErrKK   = ptErracc( *KSVs.at(1)  );
	m_recRErrKK    = rErracc( *KSVs.at(1)  );

	m_recMassLKK    = massaccL( *KSVs.at(1) );
	m_recMassErrLKK = massErraccL( *KSVs.at(1)  );
	m_recMassLBKK    = massaccLB( *KSVs.at(1) );
	m_recMassErrLBKK = massErraccLB( *KSVs.at(1)  );

	m_recPhiKK = atan2( KSVs.at(1)->y(), KSVs.at(1)->x() );
	m_fzKK     = deltaZ/m_recDelRKK;
            
	// x y coordinates of the SV
	m_xSVKK = KSVs.at(1)->x();
	m_ySVKK = KSVs.at(1)->y();

	m_kLifeKK = m_recDelRKK/1000/((sqrt(m_pzKK*m_pzKK*m_recPtKK*m_recPtKK/(1-m_pzKK*m_pzKK)+m_recPtKK*m_recPtKK)/sqrt((m_pzKK*m_pzKK*m_recPtKK*m_recPtKK/(1-m_pzKK*m_pzKK)+m_recPtKK*m_recPtKK) + K_MASS_PDG*K_MASS_PDG))*C);
	m_lLifeKK = m_recDelRKK/1000/((sqrt(m_pzKK*m_pzKK*m_recPtKK*m_recPtKK/(1-m_pzKK*m_pzKK)+m_recPtKK*m_recPtKK)/sqrt((m_pzKK*m_pzKK*m_recPtKK*m_recPtKK/(1-m_pzKK*m_pzKK)+m_recPtKK*m_recPtKK) + L_MASS_PDG*L_MASS_PDG))*C);

	/*if (m_recCosTheta > COS_THETA_CUT){
          h_numKcuts -> Fill(COS_CUT_K,1);
          if (m_recMass > K_MASS_LOW){
            h_numKcuts -> Fill(MASSLOW_CUT_K,1);
            if (m_recMass < K_MASS_HIGH){
              h_numKcuts -> Fill(MASSHIGH_CUT_K,1);
	      if (m_recMassL > L_MASS_LOW2){
		h_numKcuts -> Fill(LMASSLOW_CUT_K,1);
		if (m_recDelR > K_3D_DIST){
		  h_numKcuts -> Fill(DELR_CUT_K,1);
		  if (m_recPt > K_PT){
                    h_numKcuts -> Fill(PT_CUT_K,1);
		  }
		}
	      }
            }
          }
        }*/  //cutflow for Kcuts, which will give same number of vertices in each bin   

        m_chi2ndf_BKK = KSVs.at(0)->chiSquared()/KSVs.at(0)->numberDoF();
        
        float px_sVtxKK = pxacc( *KSVs.at(0) );  //MeV
        float py_sVtxKK = pyacc( *KSVs.at(0) );  //MeV
        
        float magPVtxKK =  sqrt(py_sVtxKK*py_sVtxKK + px_sVtxKK*px_sVtxKK); // |Pvertex| = sqrt(px^2 + py^2)
        
        float deltaXKK = (KSVs.at(0) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
        float deltaYKK = (KSVs.at(0) -> y() - primaryVertex -> y());
        float deltaZKK = (KSVs.at(0) -> z() - primaryVertex -> z());

        float magdeltaRKK = sqrt(deltaXKK*deltaXKK +deltaYKK*deltaYKK); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
        float myCosThetaKK = (px_sVtxKK*deltaXKK + py_sVtxKK*deltaYKK)/(magPVtxKK*magdeltaRKK); // From dot product
        // cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
        
        m_recDelR_BKK = sqrt( deltaXKK*deltaXKK +deltaYKK*deltaYKK + deltaZKK * deltaZKK ); // 3D distance
        
        m_PVz_BKK = primaryVertex->z(); // Z component of PV
        
        m_pz_BKK  = pzacc( *KSVs.at(0)  ); // Z component of SV's momentum
        m_pz_BKK /=  sqrt(py_sVtxKK*py_sVtxKK + px_sVtxKK*px_sVtxKK + m_pz_BKK*m_pz_BKK ); // Pz = pz/|p|  (SV)
        
        m_recR_BKK = sqrt(KSVs.at(0)->x()*KSVs.at(0)->x() + KSVs.at(0)->y()*KSVs.at(0)->y());
        m_recPhi_BKK = atan2( KSVs.at(0)->y(), KSVs.at(0)->x() ); // phi = arctan(y_sv/x_sv)
        m_vz_BKK  =  KSVs.at(0)->z(); // z component of SV

        m_recPt_BKK =  sqrt( px_sVtxKK*px_sVtxKK + py_sVtxKK*py_sVtxKK); 
        m_recCosTheta_BKK = myCosThetaKK;
        m_recMass_BKK    = massacc( *KSVs.at(0) );
        m_recMassErr_BKK = massErracc( *KSVs.at(0)  );
        m_recPtErr_BKK   = ptErracc( *KSVs.at(0)  );
        m_recRErr_BKK    = rErracc( *KSVs.at(0)  );

        m_recMassL_BKK    = massaccL( *KSVs.at(0) );
        m_recMassErrL_BKK = massErraccL( *KSVs.at(0)  );
        m_recMassLB_BKK    = massaccLB( *KSVs.at(0) );
        m_recMassErrLB_BKK = massErraccLB( *KSVs.at(0)  );

        m_recPhi_BKK = atan2( KSVs.at(0)->y(), KSVs.at(0)->x() );
        m_fz_BKK     = deltaZKK/m_recDelR_BKK;
        
        // x y coordinates of the SV
        m_xSV_BKK = KSVs.at(0)->x();
        m_ySV_BKK = KSVs.at(0)->y();

        m_kLife_BKK = m_recDelR_BKK/1000/((sqrt(m_pz_BKK*m_pz_BKK*m_recPt_BKK*m_recPt_BKK/(1-m_pz_BKK*m_pz_BKK)+m_recPt_BKK*m_recPt_BKK)/sqrt((m_pz_BKK*m_pz_BKK*m_recPt_BKK*m_recPt_BKK/(1-m_pz_BKK*m_pz_BKK)+m_recPt_BKK*m_recPt_BKK) + K_MASS_PDG*K_MASS_PDG))*C);
        m_lLife_BKK = m_recDelR_BKK/1000/((sqrt(m_pz_BKK*m_pz_BKK*m_recPt_BKK*m_recPt_BKK/(1-m_pz_BKK*m_pz_BKK)+m_recPt_BKK*m_recPt_BKK)/sqrt((m_pz_BKK*m_pz_BKK*m_recPt_BKK*m_recPt_BKK/(1-m_pz_BKK*m_pz_BKK)+m_recPt_BKK*m_recPt_BKK) + L_MASS_PDG*L_MASS_PDG))*C);

        xAOD::TrackParticle::FourMom_t mom1 = KSVs.at(0)->trackParticle(0)->p4() + KSVs.at(0)->trackParticle(1)->p4(); // 1st particle's momentum
        xAOD::TrackParticle::FourMom_t mom2 = KSVs.at(1)->trackParticle(0)->p4() + KSVs.at(1)->trackParticle(1)->p4(); // 2nd particle's momentum
  
        float magP4 = (mom1 + mom2).M();  // invariant mass = magnitude of the total 4-momentum 
        m_invMKK = magP4;
        Kcalculated = true;
	m_KKcheck++;
	KKcheck++;
	//KKflag = 2;

	//std::cout << "MKK calculated" << std::endl;

        //save the track particles pt and eta 
        m_trackPt_1 = KSVs.at(0)->trackParticle(0)->pt();
        m_trackEta_1 = KSVs.at(0)->trackParticle(0)->eta();
        m_trackPt_2 = KSVs.at(0)->trackParticle(1)->pt();
        m_trackEta_2 = KSVs.at(0)->trackParticle(1)->eta();
        m_trackPt_3 = KSVs.at(1)->trackParticle(0)->pt();
        m_trackEta_3 = KSVs.at(1)->trackParticle(0)->eta();
        m_trackPt_4 = KSVs.at(1)->trackParticle(1)->pt();
        m_trackEta_4 = KSVs.at(1)->trackParticle(1)->eta();

      }

       else if (KSVs.size()==1 && !Kcalculated1){
	 m_invMKK = STORE;
	 //KKflag = 1;
	 Kcalculated1 = true;
	 //std::cout << "1st K STORED" << std::endl;
       }

      else{ 
	/*m_recDelRKK = IGNORE;  //I have only added those variables I am working with, but all other should also be added                                                                                  
	m_recDelR_BKK = IGNORE;
	m_recCosThetaKK = IGNORE;
	m_recCosTheta_BKK = IGNORE;
	m_recPtKK = IGNORE;
	m_recPt_BKK = IGNORE;
	m_recMassKK = IGNORE;
	m_recMass_BKK = IGNORE;
	m_recMassLKK = IGNORE;
        m_recMassL_BKK = IGNORE;
	m_pzKK = IGNORE;
	m_pz_BKK = IGNORE;*/
	m_invMKK = IGNORE;
	//KKflag = 3;
	//std::cout << "MKK ignore" << std::endl;
      }
  	    
      // If 2 lambdas are found
      if (LSVs.size()==2 && !Lcalculated){

        // The following code calculates and sets the second secondary vertex information which will be a lambda

	m_chi2ndfLL = LSVs.at(1)->chiSquared()/LSVs.at(1)->numberDoF();
            
	float px_sVtx = pxacc( *LSVs.at(1) );  //MeV
	float py_sVtx = pyacc( *LSVs.at(1) );  //MeV
            
	float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
            
	float deltaX = (LSVs.at(1) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
	float deltaY = (LSVs.at(1) -> y() - primaryVertex -> y());
	float deltaZ = (LSVs.at(1) -> z() - primaryVertex -> z());

	float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
	float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
	// cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
            
	m_recDelRLL = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
            
	m_PVzLL = primaryVertex->z(); // Z component of PV
            
	m_pzLL  = pzacc( *LSVs.at(1)  ); // Z component of SV's momentum
	m_pzLL /=  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx + m_pzLL*m_pzLL ); // Pz = pz/|p|  (SV)
            
	m_recRLL = sqrt(LSVs.at(1)->x()*LSVs.at(1)->x() + LSVs.at(1)->y()*LSVs.at(1)->y());
	m_recPhiLL = atan2( LSVs.at(1)->y(), LSVs.at(1)->x() ); // phi = arctan(y_sv/x_sv)
	m_vzLL  =  LSVs.at(1)->z(); // z component of SV

	m_recPtLL =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
	m_recCosThetaLL = myCosTheta;
	m_recMassLL    = massacc( *LSVs.at(1) );
	m_recMassErrLL = massErracc( *LSVs.at(1)  );
	m_recPtErrLL   = ptErracc( *LSVs.at(1)  );
	m_recRErrLL    = rErracc( *LSVs.at(1)  );

	m_recMassLLL    = massaccL( *LSVs.at(1) );
	m_recMassErrLLL = massErraccL( *LSVs.at(1)  );
	m_recMassLBLL    = massaccLB( *LSVs.at(1) );
	m_recMassErrLBLL = massErraccLB( *LSVs.at(1)  );

	m_recPhiLL = atan2( LSVs.at(1)->y(), LSVs.at(1)->x() );
	m_fzLL     = deltaZ/m_recDelRLL;
            
	// x y coordinates of the SV
	m_xSVLL = LSVs.at(1)->x();
	m_ySVLL = LSVs.at(1)->y();

	m_kLifeLL = m_recDelRLL/1000/((sqrt(m_pzLL*m_pzLL*m_recPtLL*m_recPtLL/(1-m_pzLL*m_pzLL)+m_recPtLL*m_recPtLL)/sqrt((m_pzLL*m_pzLL*m_recPtLL*m_recPtLL/(1-m_pzLL*m_pzLL)+m_recPtLL*m_recPtLL) + K_MASS_PDG*K_MASS_PDG))*C);
	m_lLifeLL = m_recDelRLL/1000/((sqrt(m_pzLL*m_pzLL*m_recPtLL*m_recPtLL/(1-m_pzLL*m_pzLL)+m_recPtLL*m_recPtLL)/sqrt((m_pzLL*m_pzLL*m_recPtLL*m_recPtLL/(1-m_pzLL*m_pzLL)+m_recPtLL*m_recPtLL) + L_MASS_PDG*L_MASS_PDG))*C);


        m_chi2ndf_BLL = LSVs.at(0)->chiSquared()/LSVs.at(0)->numberDoF();
        
        float px_sVtxLL = pxacc( *LSVs.at(0) );  //MeV
        float py_sVtxLL = pyacc( *LSVs.at(0) );  //MeV
        
        float magPVtxLL =  sqrt(py_sVtxLL*py_sVtxLL + px_sVtxLL*px_sVtxLL); // |Pvertex| = sqrt(px^2 + py^2)
        
        float deltaXLL = (LSVs.at(0) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
        float deltaYLL = (LSVs.at(0) -> y() - primaryVertex -> y());
        float deltaZLL = (LSVs.at(0) -> z() - primaryVertex -> z());

        float magdeltaRLL = sqrt(deltaXLL*deltaXLL +deltaYLL*deltaYLL); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
        float myCosThetaLL = (px_sVtxLL*deltaXLL + py_sVtxLL*deltaYLL)/(magPVtxLL*magdeltaRLL); // From dot product
        // cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
        
        m_recDelR_BLL = sqrt( deltaXLL*deltaXLL +deltaYLL*deltaYLL + deltaZLL * deltaZLL ); // 3D distance
        
        m_PVz_BLL = primaryVertex->z(); // Z component of PV
        
        m_pz_BLL  = pzacc( *LSVs.at(0)  ); // Z component of SV's momentum
        m_pz_BLL /=  sqrt(py_sVtxLL*py_sVtxLL + px_sVtxLL*px_sVtxLL + m_pz_BLL*m_pz_BLL ); // Pz = pz/|p|  (SV)
        
        m_recR_BLL = sqrt(LSVs.at(0)->x()*LSVs.at(0)->x() + LSVs.at(0)->y()*LSVs.at(0)->y());
        m_recPhi_BLL = atan2( LSVs.at(0)->y(), LSVs.at(0)->x() ); // phi = arctan(y_sv/x_sv)
        m_vz_BLL  =  LSVs.at(0)->z(); // z component of SV

        m_recPt_BLL =  sqrt( px_sVtxLL*px_sVtxLL + py_sVtxLL*py_sVtxLL); 
        m_recCosTheta_BLL = myCosThetaLL;
        m_recMass_BLL    = massacc( *LSVs.at(0) );
        m_recMassErr_BLL = massErracc( *LSVs.at(0)  );
        m_recPtErr_BLL   = ptErracc( *LSVs.at(0)  );
        m_recRErr_BLL    = rErracc( *LSVs.at(0)  );

        m_recMassL_BLL    = massaccL( *LSVs.at(0) );
        m_recMassErrL_BLL = massErraccL( *LSVs.at(0)  );
        m_recMassLB_BLL    = massaccLB( *LSVs.at(0) );
        m_recMassErrLB_BLL = massErraccLB( *LSVs.at(0)  );

        m_recPhi_BLL = atan2( LSVs.at(0)->y(), LSVs.at(0)->x() );
        m_fz_BLL     = deltaZLL/m_recDelR_BLL;
        
        // x y coordinates of the SV
        m_xSV_BLL = LSVs.at(0)->x();
        m_ySV_BLL = LSVs.at(0)->y();

        m_kLife_BLL = m_recDelR_BLL/1000/((sqrt(m_pz_BLL*m_pz_BLL*m_recPt_BLL*m_recPt_BLL/(1-m_pz_BLL*m_pz_BLL)+m_recPt_BLL*m_recPt_BLL)/sqrt((m_pz_BLL*m_pz_BLL*m_recPt_BLL*m_recPt_BLL/(1-m_pz_BLL*m_pz_BLL)+m_recPt_BLL*m_recPt_BLL) + K_MASS_PDG*K_MASS_PDG))*C);
        m_lLife_BLL = m_recDelR_BLL/1000/((sqrt(m_pz_BLL*m_pz_BLL*m_recPt_BLL*m_recPt_BLL/(1-m_pz_BLL*m_pz_BLL)+m_recPt_BLL*m_recPt_BLL)/sqrt((m_pz_BLL*m_pz_BLL*m_recPt_BLL*m_recPt_BLL/(1-m_pz_BLL*m_pz_BLL)+m_recPt_BLL*m_recPt_BLL) + L_MASS_PDG*L_MASS_PDG))*C);


        xAOD::TrackParticle::FourMom_t mom1 = LSVs.at(0)->trackParticle(0)->p4() + LSVs.at(0)->trackParticle(1)->p4(); // 1st particle's momentum
        xAOD::TrackParticle::FourMom_t mom2 = LSVs.at(1)->trackParticle(0)->p4() + LSVs.at(1)->trackParticle(1)->p4(); // 2nd particle's momentum
  
        float magP4 = (mom1 + mom2).M();  // invariant mass = magnitude of the total 4-momentum 
        m_invMLL = magP4;
        Lcalculated = true;
	m_LLcheck++;
	LLcheck++;
	//LLflag = 2;

	//std::cout << "MLL calculated" << std::endl;
        //save the track particles pt and eta 
        m_trackPt_1 = LSVs.at(0)->trackParticle(0)->pt();
        m_trackEta_1 = LSVs.at(0)->trackParticle(0)->eta();
        m_trackPt_2 = LSVs.at(0)->trackParticle(1)->pt();
        m_trackEta_2 = LSVs.at(0)->trackParticle(1)->eta();
        m_trackPt_3 = LSVs.at(1)->trackParticle(0)->pt();
        m_trackEta_3 = LSVs.at(1)->trackParticle(0)->eta();
        m_trackPt_4 = LSVs.at(1)->trackParticle(1)->pt();
        m_trackEta_4 = LSVs.at(1)->trackParticle(1)->eta();
      }
      else if(LSVs.size()==1 && !Lcalculated1){
	/*m_recDelRLL = IGNORE;  //I have only added those variables I am working with, but all other should also be added                                                                                  
        m_recDelR_BLL = IGNORE;
        m_recCosThetaLL = IGNORE;
        m_recCosTheta_BLL = IGNORE;
        m_recPtLL = IGNORE;
        m_recPt_BLL = IGNORE;
        m_recMassLL = IGNORE;
        m_recMass_BLL = IGNORE;
	m_recMassLLL = IGNORE;
        m_recMassL_BLL = IGNORE;
	m_pzLL = IGNORE;
        m_pz_BLL = IGNORE;*/
	m_invMLL = STORE;
	//LLflag = 1;
	Lcalculated1 = true;
	//std::cout << "1st L STORED" << std::endl;
      }

      else{ 
	/*m_recDelRLL = IGNORE;  //I have only added those variables I am working with, but all other should also be added                                                                                  
        m_recDelR_BLL = IGNORE;
        m_recCosThetaLL = IGNORE;
        m_recCosTheta_BLL = IGNORE;
        m_recPtLL = IGNORE;
        m_recPt_BLL = IGNORE;
        m_recMassLL = IGNORE;
        m_recMass_BLL = IGNORE;
	m_recMassLLL = IGNORE;
        m_recMassL_BLL = IGNORE;
	m_pzLL = IGNORE;
        m_pz_BLL = IGNORE;*/
	m_invMLL = IGNORE;
	//LLflag = 3;
	//std::cout << "MLL ignore" << std::endl;
      }
	    // If 1 lambda and 1 kaon are found

      if (SVs.size()==2 && !calculated){

          //Check to see if current SV is a kaon 
          // if (m_recCosTheta > COS_THETA_CUT && m_recMass > K_MASS_LOW && m_recMass < K_MASS_HIGH && m_recMassL > L_MASS_LOW2 && m_recDelR > K_3D_DIST && m_recPt > K_PT){

            // The following code calculates and sets the normal secondary vertex information which will be a lambda

            m_chi2ndfKL = SVs.at(0)->chiSquared()/SVs.at(0)->numberDoF();
            
            float px_sVtx = pxacc( *SVs.at(0) );  //MeV
            float py_sVtx = pyacc( *SVs.at(0) );  //MeV
            
            float magPVtx =  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx); // |Pvertex| = sqrt(px^2 + py^2)
            
            float deltaX = (SVs.at(0) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
            float deltaY = (SVs.at(0) -> y() - primaryVertex -> y());
            float deltaZ = (SVs.at(0) -> z() - primaryVertex -> z());

            float magdeltaR = sqrt(deltaX*deltaX +deltaY*deltaY); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
            float myCosTheta = (px_sVtx*deltaX + py_sVtx*deltaY)/(magPVtx*magdeltaR); // From dot product
            // cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
            
            m_recDelRKL = sqrt( deltaX*deltaX +deltaY*deltaY + deltaZ * deltaZ ); // 3D distance
            
            m_PVzKL = primaryVertex->z(); // Z component of PV
            
            m_pzKL  = pzacc( *SVs.at(0)  ); // Z component of SV's momentum
            m_pzKL /=  sqrt(py_sVtx*py_sVtx + px_sVtx*px_sVtx + m_pzKL*m_pzKL ); // Pz = pz/|p|  (SV)
            
            m_recRKL = sqrt(SVs.at(0)->x()*SVs.at(0)->x() + SVs.at(0)->y()*SVs.at(0)->y());
            m_recPhiKL = atan2( SVs.at(0)->y(), SVs.at(0)->x() ); // phi = arctan(y_sv/x_sv)
            m_vzKL  =  SVs.at(0)->z(); // z component of SV

            m_recPtKL =  sqrt( px_sVtx*px_sVtx + py_sVtx*py_sVtx); 
            m_recCosThetaKL = myCosTheta;
            m_recMassKL    = massacc( *SVs.at(0) );
            m_recMassErrKL = massErracc( *SVs.at(0)  );
            m_recPtErrKL   = ptErracc( *SVs.at(0)  );
            m_recRErrKL    = rErracc( *SVs.at(0)  );

            m_recMassLKL    = massaccL( *SVs.at(0) );
            m_recMassErrLKL = massErraccL( *SVs.at(0)  );
            m_recMassLBKL    = massaccLB( *SVs.at(0) );
            m_recMassErrLBKL = massErraccLB( *SVs.at(0)  );

            m_recPhiKL = atan2( SVs.at(0)->y(), SVs.at(0)->x() );
            m_fzKL     = deltaZ/m_recDelRKL;
            
            // x y coordinates of the SV
            m_xSVKL = SVs.at(0)->x();
            m_ySVKL = SVs.at(0)->y();

            m_kLifeKL = m_recDelRKL/1000/((sqrt(m_pzKL*m_pzKL*m_recPtKL*m_recPtKL/(1-m_pzKL*m_pzKL)+m_recPtKL*m_recPtKL)/sqrt((m_pzKL*m_pzKL*m_recPtKL*m_recPtKL/(1-m_pzKL*m_pzKL)+m_recPtKL*m_recPtKL) + K_MASS_PDG*K_MASS_PDG))*C);
            m_lLifeKL = m_recDelRKL/1000/((sqrt(m_pzKL*m_pzKL*m_recPtKL*m_recPtKL/(1-m_pzKL*m_pzKL)+m_recPtKL*m_recPtKL)/sqrt((m_pzKL*m_pzKL*m_recPtKL*m_recPtKL/(1-m_pzKL*m_pzKL)+m_recPtKL*m_recPtKL) + L_MASS_PDG*L_MASS_PDG))*C);

            //second vertex //change all LSV to SV
            m_chi2ndf_BKL = SVs.at(1)->chiSquared()/SVs.at(1)->numberDoF();
            
            float px_sVtxKL = pxacc( *SVs.at(1) );  //MeV
            float py_sVtxKL = pyacc( *SVs.at(1) );  //MeV
            
            float magPVtxKL =  sqrt(py_sVtxKL*py_sVtxKL + px_sVtxKL*px_sVtxKL); // |Pvertex| = sqrt(px^2 + py^2)
            
            float deltaXKL = (SVs.at(1) -> x() - primaryVertex -> x()); // deltaX = x_sv - x_pv
            float deltaYKL = (SVs.at(1) -> y() - primaryVertex -> y());
            float deltaZKL = (SVs.at(1) -> z() - primaryVertex -> z());

            float magdeltaRKL = sqrt(deltaXKL*deltaXKL +deltaYKL*deltaYKL); // Distance |R| = sqrt(deltaX^2 + deltaY^2)      
            float myCosThetaKL = (px_sVtxKL*deltaXKL + py_sVtxKL*deltaYKL)/(magPVtxKL*magdeltaRKL); // From dot product
            // cos(theta) = (Px*dx + Py*dy)/(|P|*|R|)
            
            m_recDelR_BKL = sqrt( deltaXKL*deltaXKL +deltaYKL*deltaYKL + deltaZKL * deltaZKL ); // 3D distance
            
            m_PVz_BKL = primaryVertex->z(); // Z component of PV
            
            m_pz_BKL  = pzacc( *SVs.at(1)  ); // Z component of SV's momentum
            m_pz_BKL /=  sqrt(py_sVtxKL*py_sVtxKL + px_sVtxKL*px_sVtxKL + m_pz_BKL*m_pz_BKL ); // Pz = pz/|p|  (SV)
            
            m_recR_BKL = sqrt(SVs.at(1)->x()*SVs.at(1)->x() + SVs.at(1)->y()*SVs.at(1)->y());
            m_recPhi_BKL = atan2( SVs.at(1)->y(), SVs.at(1)->x() ); // phi = arctan(y_sv/x_sv)
            m_vz_BKL  =  SVs.at(1)->z(); // z component of SV

            m_recPt_BKL =  sqrt( px_sVtxKL*px_sVtxKL + py_sVtxKL*py_sVtxKL); 
            m_recCosTheta_BKL = myCosThetaKL;
            m_recMass_BKL    = massacc( *SVs.at(1) );
            m_recMassErr_BKL = massErracc( *SVs.at(1)  );
            m_recPtErr_BKL   = ptErracc( *SVs.at(1)  );
            m_recRErr_BKL    = rErracc( *SVs.at(1)  );

            m_recMassL_BKL    = massaccL( *SVs.at(1) );
            m_recMassErrL_BKL = massErraccL( *SVs.at(1)  );
            m_recMassLB_BKL    = massaccLB( *SVs.at(1) );
            m_recMassErrLB_BKL = massErraccLB( *SVs.at(1)  );

            m_recPhi_BKL = atan2( SVs.at(1)->y(), SVs.at(1)->x() );
            m_fz_BKL     = deltaZKL/m_recDelR_BKL;
            
            // x y coordinates of the SV
            m_xSV_BKL = SVs.at(1)->x();
            m_ySV_BKL = SVs.at(1)->y();

            m_kLife_BKL = m_recDelR_BKL/1000/((sqrt(m_pz_BKL*m_pz_BKL*m_recPt_BKL*m_recPt_BKL/(1-m_pz_BKL*m_pz_BKL)+m_recPt_BKL*m_recPt_BKL)/sqrt((m_pz_BKL*m_pz_BKL*m_recPt_BKL*m_recPt_BKL/(1-m_pz_BKL*m_pz_BKL)+m_recPt_BKL*m_recPt_BKL) + K_MASS_PDG*K_MASS_PDG))*C);
            m_lLife_BKL = m_recDelR_BKL/1000/((sqrt(m_pz_BKL*m_pz_BKL*m_recPt_BKL*m_recPt_BKL/(1-m_pz_BKL*m_pz_BKL)+m_recPt_BKL*m_recPt_BKL)/sqrt((m_pz_BKL*m_pz_BKL*m_recPt_BKL*m_recPt_BKL/(1-m_pz_BKL*m_pz_BKL)+m_recPt_BKL*m_recPt_BKL) + L_MASS_PDG*L_MASS_PDG))*C);
            

        	xAOD::TrackParticle::FourMom_t mom1 = SVs.at(0)->trackParticle(0)->p4() + SVs.at(0)->trackParticle(1)->p4(); // 1st particle's momentum
        	xAOD::TrackParticle::FourMom_t mom2 = SVs.at(1)->trackParticle(0)->p4() + SVs.at(1)->trackParticle(1)->p4(); // 2nd particle's momentum

        	float magP4 = (mom1 + mom2).M();  // invariant mass = magnitude of the total 4-momentum 
        	m_invMKL = magP4;
        	calculated = true;
		m_KLcheck++;
		KLcheck++;
		//KLflag = 2;

		//std::cout << "MKL calculated" << std::endl;
          //save the track particles pt and eta 
          m_trackPt_1 = SVs.at(0)->trackParticle(0)->pt();
          m_trackEta_1 = SVs.at(0)->trackParticle(0)->eta();
          m_trackPt_2 = SVs.at(0)->trackParticle(1)->pt();
          m_trackEta_2 = SVs.at(0)->trackParticle(1)->eta();
          m_trackPt_3 = SVs.at(1)->trackParticle(0)->pt();
          m_trackEta_3 = SVs.at(1)->trackParticle(0)->eta();
          m_trackPt_4 = SVs.at(1)->trackParticle(1)->pt();
          m_trackEta_4 = SVs.at(1)->trackParticle(1)->eta();
	    }
      else if(SVs.size()==1 && !calculated1){
	/*m_recDelRKL = IGNORE;  //I have only added those variables I am working with, but all other should also be added                                                                                  
        m_recDelR_BKL = IGNORE;
        m_recCosThetaKL = IGNORE;
        m_recCosTheta_BKL = IGNORE;
        m_recPtKL = IGNORE;
        m_recPt_BKL = IGNORE;
        m_recMassKL = IGNORE;
        m_recMass_BKL = IGNORE;
	m_recMassLKL = IGNORE;
	m_recMassL_BKL = IGNORE;
	m_pzKL = IGNORE;
        m_pz_BKL = IGNORE;*/
	m_invMKL = STORE;
	//KLflag = 1;
	calculated1 = true;
	//std::cout << "1st vtx STORED" << std::endl;
      }
      else{
	/*m_recDelRKL = IGNORE;  //I have only added those variables I am working with, but all other should also be added                                                                                  
        m_recDelR_BKL = IGNORE;
        m_recCosThetaKL = IGNORE;
        m_recCosTheta_BKL = IGNORE;
        m_recPtKL = IGNORE;
        m_recPt_BKL = IGNORE;
        m_recMassKL = IGNORE;
        m_recMass_BKL = IGNORE;
        m_recMassLKL = IGNORE;
        m_recMassL_BKL = IGNORE;
	m_pzKL = IGNORE;
        m_pz_BKL = IGNORE;*/
	m_invMKL = IGNORE;
	//KLflag = 3;
	//std::cout << "MKL ignore" << std::endl;
      }
     // end of method 1

    // Method 2 (bad method): Calculates multiquark mass if first two SVs are both Ks, both Ls or 1 L and 1 K         
    }else{
      SVs.push_back(secvtx); // Add SV to vector
            
      // Check if vertex passes Kaon cuts
       if (m_recCosTheta < COS_THETA_CUT || m_recMass < K_MASS_LOW || m_recMass > K_MASS_HIGH || m_recMassL < L_MASS_LOW2 || m_recDelR < K_3D_DIST || m_recPt < K_PT)
               passKCuts = false;
            
      // Check if vertex passes Lambda cuts
      if (m_recCosTheta < COS_THETA_CUT || m_recMassL < L_MASS_LOW || m_recMassL > L_MASS_HIGH || m_recMass > K_MASS_HIGH2 || m_recDelR < L_3D_DIST || m_recPt < L_PT) 
                passLCuts = false;

      if (passKCuts == true){
            VType.push_back(1); //Kaon
            Kcounter++;
      }
      else if (passLCuts == true) VType.push_back(2); //Lambda
      else VType.push_back(0); //Neither
        

        
      if (counter == 2 && VType.at(0) != 0 && VType.at(1) != 0) { // When there is a second event, and the particles aren't of type "neither"
        xAOD::TrackParticle::FourMom_t mom1 = SVs.at(0)->trackParticle(0)->p4() + SVs.at(0)->trackParticle(1)->p4(); // 1st particle's momentum
        xAOD::TrackParticle::FourMom_t mom2 = SVs.at(1)->trackParticle(0)->p4() + SVs.at(1)->trackParticle(1)->p4(); // 2nd particle's momentum

        float magP4 = (mom1 + mom2).M();  // invariant mass = magnitude of the total 4-momentum 
            
        if (VType.at(0) == 1 && VType.at(1) == 1) { // if both SVs are kaons, fill tetraquark mass
            m_invMKK = magP4;    
            m_invMKL = IGNORE;
            m_invMLL = IGNORE;
        }
        else if (VType.at(0) == 1 && VType.at(1) == 2){    // if one SV is a kaon and the other a lambda, fill pentaquark mass
            m_invMKL = magP4;
            m_invMKK = IGNORE;
            m_invMLL = IGNORE;
        }
        else if (VType.at(0) == 2 && VType.at(1) == 1) {   // if one SV is a kaon and the other a lambda, fill pentaquark mass
            m_invMKL = magP4;
            m_invMKK = IGNORE;
            m_invMLL = IGNORE;
        }
        else if (VType.at(0) == 2 && VType.at(1) == 2) {   // if both SVs are lambdas, fill sextaquark mass
            m_invMLL = magP4;
            m_invMKK = IGNORE;
            m_invMKL = IGNORE;
        }
        else {  // if none of the above, set to IGNORE (ignored)
            m_invMKK = IGNORE;
            m_invMKL = IGNORE;
            m_invMLL = IGNORE;
        }
                
      } else {  // if one of the SVs is neither a kaon or lambda, set to IGNORE (ignored)
          m_invMKK = IGNORE;
          m_invMKL = IGNORE;
          m_invMLL = IGNORE;
      }
    }  // End of Method 2

  
      /*****************************************************************************************************/
        int tracksFromKs(0);  // # tracks from K
        const xAOD::TruthParticle *truthMotherKs = 0;


        if( isMC ) {  // MC ONLY
        std::cout << "Checking track paticle in MC: " << secvtx->trackParticle(0) << std::endl;

        if( !m_brokenTrkLinks ) {  
            for(unsigned int i =0; i< secvtx->nTrackParticles(); ++i ){  // for each track from the SV
                auto track  = secvtx->trackParticle( i );
                
                auto truthTrk = tth.truthParticle( track );
                if (!truthTrk)
                    continue;
                
                if (!truthTrk->hasProdVtx()) // ProdVtx = production vertex (where K was formed), skip if none
                    continue;


                //for each incoming particle, take the ith one and if it's a kaon, assign it to truthMotherKs
                for(unsigned int i =0; i< truthTrk->prodVtx()->nIncomingParticles(); ++i ){ //for each incoming particle
                    auto truthMother  = truthTrk->prodVtx()->incomingParticle( i ); // ith incoming particle     
                    if(truthMother->pdgId() == 310){  // If Ks was mother 
                        truthMotherKs = truthMother; 
                        ++tracksFromKs;  // add track coming from K
                        break;
                    }
                } 
            }
        } else {
            auto truthVtx = truthVertex(secvtx);
            if ( truthVtx == 0) { //if not truthvertex, assign these values so they're ignored
                    m_trueR = IGNORE;
                    m_truePt = IGNORE;
                    m_trueEta = IGNORE;
                    m_trueCosTheta = IGNORE;
                    break;
            }
            if ( !truthVtx->hasDecayVtx() ) { // if no decay vertex
                    m_trueR = IGNORE;
                    m_truePt = IGNORE;
                    m_trueEta = IGNORE;
                    m_trueCosTheta = IGNORE;
                    break;
            }

            // If vertex is K or Lambda and has 2 daughter particles, set truth values
            if ( (truthVtx->absPdgId() == 310 || truthVtx->absPdgId() == 3122)  
                && truthVtx->decayVtx()->nOutgoingParticles() == 2 ){  
        
                // Set values in branches for each truth vertex
                m_trueR = truthVtx->decayVtx()->perp(); // perp = transverse distance from beam
                m_truePt = truthVtx->pt();
                m_trueEta = truthVtx->eta();
                m_trueCosTheta = 1;
                ++actualReco;
                h_recoPosandReco->Fill("ActualReco", 1);

            } else {
                m_trueR = IGNORE;
                m_truePt = IGNORE;
                m_trueEta = IGNORE;
                m_trueCosTheta = IGNORE;
            }
        }
        /*
        }
        
        for (auto truth : ksTruths) {
            if ( ksTruthMatches.count(truth->barcode()) ) continue;
            if ( !truth->hasDecayVtx() ) continue;
            if ( !(truth->decayVtx()->nOutgoingParticles() == 2) ) continue;
            float truthR = truth->decayVtx()->perp();
            if ( abs(m_recR - truthR) < 0.1 ) {
            ksTruthMatches[ truth->barcode() ] = truth;
            m_trueR = truthR;
            m_truePt =  truth->pt();
            m_trueCosTheta = 1;
            ++actualReco;
            h_recoPosandReco->Fill("ActualReco", 1);
            } else {
            m_trueR = IGNORE;
            m_truePt = IGNORE;
            m_trueCosTheta = IGNORE;
            }
        }*/
        }
    
    /*****************************************************************************************************/

        if (!m_brokenTrkLinks || !isMC ) { // FOR DATA
        
        /*
            const std::vector< ElementLink< xAOD::TrackParticleContainer > > v0Tracks = secvtx->trackParticleLinks();

            const xAOD::TrackParticle* track1 = *v0Tracks.at(0);
            const xAOD::TrackParticle* track2 = *v0Tracks.at(1);

            std::cout << "track1 : " << track1 << std::endl;
            if (track1 == nullptr || track2 == nullptr) {
        //      m_logger << ERROR << "TruthMatch: V0 Rejected due missing track links " << (v0Track0 == nullptr) << " " << (v0Track1 == nullptr) << SLogger::endmsg;
            std::cout << "Null tracks. " << std::endl;
            continue;
            }
        */

            // in vertex_v1.cxx, the method trackParticle(int i) returns 0 if not accessible
            auto track1  = secvtx->trackParticle( 0 ); // particle from 1st track (assumes 2 tracks)
            auto track2  = secvtx->trackParticle( 1 ); // particle from 2nd track
            //if (track1 != 0 ) std::cout << "HECK YES" << std::endl;



            // Calculate conversion photon energy
            float px1 = (track1->p4()).Px();

            float px2 = (track2->p4()).Px();
            float py1 = (track1->p4()).Py();
            float py2 = (track2->p4()).Py();
            float pz1 = (track1->p4()).Pz();
            float pz2 = (track2->p4()).Pz();
            float e1sq = 0.511*0.511 + px1*px1 + py1*py1 + pz1*pz1; // mass e- = 0.511
            float e2sq = 0.511*0.511 + px2*px2 + py2*py2 + pz2*pz2; // e^2 = m^2 + px^2 + py^2 + pz^2
            float e1 = (e1sq > 0.) ? sqrt(e1sq) : 0.;
            float e2 = (e2sq > 0.) ? sqrt(e2sq) : 0.;
            
            // Add energies and momentum to reconstruct mass
            float E = e1+e2;
            float px = px1 + px2;
            float py = py1 + py2;
            float pz = pz1 + pz2;
        
            // Reconstructed mass: m = sqrt(E^2 - px^2 - py^2 - pz^2)
            float msq = E*E-px*px-py*py-pz*pz;
            m_recConvMass = (msq > 0.) ? sqrt(msq) : 0.;
            

            if(track1->charge() > 0){ // If track1 is positive (pi+)
                m_pplus = 1./track1->qOverP();
                m_pminus = 1./track2->qOverP();

                uint8_t tmp = 0;
                // Positive track 1
                track1->summaryValue(tmp, xAOD::numberOfPixelHits);
                m_nPixp = tmp;      
                track1->summaryValue(tmp, xAOD::numberOfSCTHits);
                m_nSCTp = tmp;
                track1->summaryValue(m_htp, xAOD::eProbabilityHT);

                // Negative track 2
                track2->summaryValue(tmp, xAOD::numberOfPixelHits);
                m_nPixn = tmp;
                track2->summaryValue(tmp, xAOD::numberOfSCTHits);
                m_nSCTn = tmp;
                track2->summaryValue(m_htn, xAOD::eProbabilityHT);

            }else{  // If track1 is negative
                Error("execute()", "Found a negative track1. Exiting." );
                return EL::StatusCode::FAILURE;
                m_pplus = 1./track2->qOverP();
                m_pminus = 1./track1->qOverP();
                
                uint8_t tmp = 0;
                // Positive track 2
                track2->summaryValue(tmp, xAOD::numberOfPixelHits);
                m_nPixp = tmp;      
                track2->summaryValue(tmp, xAOD::numberOfSCTHits);
                m_nSCTp = tmp;
                track2->summaryValue(m_htp, xAOD::eProbabilityHT);

                // Negative track 1
                track1->summaryValue(tmp, xAOD::numberOfPixelHits);
                m_nPixn = tmp;
                track1->summaryValue(tmp, xAOD::numberOfSCTHits);
                m_nSCTn = tmp;
                track1->summaryValue(m_htn, xAOD::eProbabilityHT);
            } 

            // If there's a decay vertex and 2 tracks from the Kaon, assign truth values (for data)
            if(truthMotherKs && truthMotherKs->hasDecayVtx() && tracksFromKs == 2){
                m_trueR =  truthMotherKs->decayVtx()->perp(); // perp = vertex transverse distance from beam
                m_truePt =  truthMotherKs->pt();
                m_trueCosTheta = 1;//!  // theta = 0
                ++actualReco;
                h_recoPosandReco->Fill("ActualReco", 1);
            } else {
                m_trueR = IGNORE;
                m_truePt = IGNORE;
                m_trueCosTheta = IGNORE;
            }
            
        }

        /*****************************************************************************************************/
        if (m_invMLL == IGNORE && m_invMKL == IGNORE && m_invMKK == IGNORE)
	//if (KKflag == 3 && KLflag == 3 && LLflag ==3)  
	continue;
	
	//std::cout << "Vertex in N tuple" << std::endl;

        myTree->Fill();
    }        
  }//end of loop through secondary verticies

 // m_numSV = counter; // assign counter (# SV) to branch

  //std::cout << "Sec vertices = " << secvcheck << std::endl;
  //  std::cout << "KK initial = " << KKcheck1 << std::endl;
  //  std::cout << "LL initial = " << LLcheck1 <<std::endl;
    // std::cout << "KL initial = " << KLcheck1 <<std::endl;
  //std::cout << "KK = " << KKcheck << std::endl;
  //std::cout << "LL = " << LLcheck <<std::endl;
  //std::cout << "KL = " << KLcheck <<std::endl;

  //std::cout << "Lcounter1 = " << Lcounter1 << std::endl;
  //std::cout << " Kcounter1 = " << Kcounter1 << std::endl;

  h_numSVs->Fill(counter); // Add counter value to the number of SVs histogram
  //h_numKs->Fill(Kcounter);
  
  if( hasSecVertex )
    h_cuts->Fill(HAS_SECVX_CUT,1); // add 1 to "Has sec Vertices" bin
  
  if(!isMC)  
    return EL::StatusCode::SUCCESS;  // exit if data (not MC)

  // loop over the tracks in the container
  // auto type: xAOD::TrackParticleContainer::const_iterator
  for( auto recoTrk : *recoTracks) { //recoTracks filled with "InDetTrackParticles"


    // Make your track selection
    //if( !selection_noChi2.accept(recoTrk, primaryVertex) )
      //continue;
    //std::cout << "Passed selection " << std::endl; 


    auto truth = tth.truthParticle( recoTrk );
    if(truth){
      if (truth->hasProdVtx()){  // if it's a truth particle and has production vertex
        for(unsigned int i =0; i< truth->prodVtx()->nIncomingParticles(); ++i ){ // for each incoming particle
          auto truthMother  = truth->prodVtx()->incomingParticle( i );  // Get the ith incoming particle
          if(truthMother->pdgId() == 310){ // if it's a Kaon
            ksTracks[ truthMother->barcode() ] += 1; // add 1 to map element at key truthMother->barcode
            break;
          }
        }
      }      
    }
  } // end for loop over reconstructed tracks
  
  
  int possibleReco = 0 ;
  for( auto ksTrack: ksTracks ){ // for every element (pair: <key,value>) in KsTracks map
    if(ksTrack.second >  1 )     // .second gives the value (.first gives the key)
      ++possibleReco;            // if more than 1 truth mother kaon, add to possible reco
      h_recoPosandReco->Fill("PossibleReco", 1);
  }

  
  m_recoPosvsReco->Fill(possibleReco,actualReco);
  h_recoPosandReco->Fill("ActualReco", actualReco);
  
  if(possibleReco==0 ||  true)
    return EL::StatusCode::SUCCESS;

  //this was added by cole
  int nKS = 0;

  for( auto truth : *truthParticles){  // for each truth particle
    
    if(truth->pdgId() != 310) continue; // skip if not a kaon
          
    if (!truth->hasDecayVtx())
      continue;

    int nTracks(0);
    for(unsigned int i =0; i< truth->decayVtx()->nOutgoingParticles(); ++i ){ // for each pion (decay product)
      auto truthDaughter  = truth->decayVtx()->outgoingParticle( i ); //daughter=outgoing particle from devay vtx
      if( fabs(truthDaughter->eta()) <2.5 &&  abs(truthDaughter->pdgId()) == 211 ) 
       ++nTracks; // if |eta|<2.5 and pdgID = +/- 211 (+/- pion), increase # of tracks
    } 

    if (nTracks == 2)  // if 2 tracks (2 pion daughters with eta<2.5), add to Ks counter
      ++nKS;
    
    /*
    if( truth->status() != 1 || truth->barcode() > 200e3 ){
      continue;
    } 
    
    if( truth->isStrangeBaryon() &&  truth->decayVtx()){
      std::cout << "Parent " << truth->pdgId() << std::endl;
      for( unsigned int child(0); child < truth->decayVtx()->nOutgoingParticles(); ++child ){
        std::cout << "Child " << child << "  "<< truth->decayVtx()->outgoingParticle(child)->pdgId() << " " << truth->decayVtx()->outgoingParticle(child)->barcode() << std::endl;
      }    
    }*/
  }


  std::cout << "possibleReco "  <<  possibleReco << std::endl;
  std::cout << "Generated "     <<  nKS << std::endl;

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
 
  //std::cout << "KL possible event = " << m_KLevent << std::endl;
  //std::cout << "KL actual event = " << m_KLevent1 << std::endl;
  //std::cout << "KK events = " << m_KKcheck << std::endl;
  //std::cout << "LL events = " << m_LLcheck << std::endl;
  //std::cout << "KL actual events = " << m_KLcheck << std::endl;
  //std::cout << "0 K+L = " << m_KL00 << std::endl;
  //std::cout << "3 K+L = " << m_KL03 << std::endl;
  //std::cout << "2 Kaons + Lambdas = " << m_KL02 << std::endl;
  //std::cout << "1 Kaons + Lambdas = " << m_KL01 << std::endl;
  //std::cout << "10 Kaons + Lambdas = " << m_KL10 << std::endl;
  //std::cout << "0 K, 2 L = " << m_0KcountKL04 << std::endl;
  //std::cout << "1 K, 1 L = " << m_1KcountKL04 << std::endl;
  //std::cout << "2 K, 0 L = " << m_2KcountKL04 << std::endl;
  /*std::cout << "3 K, 7 L = " << m_3KcountKL04 << std::endl;
  std::cout << "4 K, 6 L = " << m_4KcountKL04 << std::endl;
  std::cout << "5 K, 5 L = " << m_5KcountKL04 << std::endl;
  std::cout << "6 K, 4 L = " << m_6KcountKL04 << std::endl;
  std::cout << "7 K, 3 L = " << m_7KcountKL04 << std::endl;
  std::cout << "8 K, 2 L = " << m_8KcountKL04 << std::endl;
  std::cout << "9 K, 1 L = " << m_9KcountKL04 << std::endl;
  std::cout << "10 K = " << m_10KcountKL04 << std::endl;*/
  //std::cout << "4 Kaons + Lambdas = " << m_KL04 << std::endl;
  //std::cout << "5 Kaons + Lambdas = " << m_KL05 << std::endl;
  //std::cout << "6 Kaons + Lambdas = " << m_KL06 << std::endl;
  //std::cout << "7 Kaons + Lambdas = " << m_KL07 << std::endl;
  //std::cout << "8 Kaons + Lambdas = " << m_KL08 << std::endl;
  //std::cout << "9 Kaons + Lambdas = " << m_KL09 << std::endl;
  //std::cout << "10 Kaons + Lambdas = " << m_KL10 << std::endl;
  //std::cout << "11 Kaons + Lambdas = " << m_KL11 << std::endl;

 return EL::StatusCode::SUCCESS;
}



EL::StatusCode KsStudy :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}

