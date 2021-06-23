////////////////////////////////////////////////////////////////////////////////
//   BT2017RunAct.cc for beamtest2017 of JEDI                                 //
//                                                                            //
//   Definitions of BT2017RunAct class's member functions. Details of user    //
// actions are here.                                                          //
//                                                                            //
//                    - 21. Nov. 2017. Hoyong Jeong (hyjeong@hep.korea.ac.kr) //
////////////////////////////////////////////////////////////////////////////////

#include <ctime>
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4Event.hh"

#include "BT2017RunAct.hh"
#include "BT2017DetMap.hh"

//////////////////////////////////////////////////
//   Constructor                                //
//////////////////////////////////////////////////
BT2017RunAct::BT2017RunAct(BT2017ConMan* CM)
	: G4UserRunAction(), m_CM(CM)
{
	// Initialize detector setup configuration
	m_DetSet = m_CM -> GetDetSet();
	BT2017DetMap* DM = new BT2017DetMap(); 

	// Create analysis manager
	// The choice of analysis technology is done via selection of a namespace
	// in BT2017Ana.hh
	AM = G4AnalysisManager::Instance();
	G4cout << "Using " << AM -> GetType() << G4endl;

	// Default settings
	// I don't like chatterbox...
	AM -> SetVerboseLevel(0);
	// Creating ntuple
	AM -> CreateNtuple("hits", "eDep");
	AM -> CreateNtupleIColumn("eventID");         // Column ID =  0

	// generator events
	AM -> CreateNtupleDColumn("Particle");
	AM -> CreateNtupleDColumn("Kinetic_Energy");
	AM -> CreateNtupleDColumn("theta_lab");
	AM -> CreateNtupleDColumn("phi_lab");
	AM -> CreateNtupleDColumn("position_X");
	AM -> CreateNtupleDColumn("position_Y");

	for ( int i = 0; i < 120; i++ )
		if ( m_DetSet[i] ) AM -> CreateNtupleDColumn(DM -> GetDetNameFromDetID(i));

	for(G4int i = 0; i < 4; i++)
		AM -> CreateNtupleDColumn("kE_" + DM->GetdENameFromdEID(i));

	for(G4int i = 0; i < 28; i++)
		AM -> CreateNtupleDColumn("kE_" + DM->GetTrNameFromTrID(i));

	AM -> CreateNtupleDColumn("tg_theta");

	AM -> FinishNtuple();

	delete DM;
}

//////////////////////////////////////////////////
//   Destructor                                 //
//////////////////////////////////////////////////
BT2017RunAct::~BT2017RunAct()
{
	delete G4AnalysisManager::Instance();
}

//////////////////////////////////////////////////
//   Begin of run action                        //
//////////////////////////////////////////////////
void BT2017RunAct::BeginOfRunAction(const G4Run* /*run*/)
{
	// All actions defined here will be excuted at the beginning of every run.
	// What is a run? You may type "/run/beamOn [someNumber]".
	// Whenever you do this, "one run" runs.

	// Inform the runManager to save random number seed
	//G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	// Open an output file
	// The default file name is set in pEDMRunAct::pEDMRunAct(),
	// it can be overwritten in a macro

        printf("*******************************************************************\n");   
	G4String fileName = "";//getenv("HOME");
	fileName += /*"/JEDI/simulation/*/"output/";
   
    // set beam
    fileName += m_CM->GetParName();
    
    // set target
	if ( m_CM -> GetUseTar() ) {
		std::cout << "target is ON " << std::endl;
		fileName += m_CM -> GetTarMat();
	}else 
		fileName += "Empty";

	// Get kinetic energy to include it to file name
    G4double dKE = m_CM -> GetBeamKE();
    G4int iKE = dKE / MeV;

    fileName += "-";
    fileName += std::to_string(iKE);
    fileName += "MeV";

    std::cout << fileName << std::endl;
	AM -> OpenFile(fileName);
    printf("*******************************************************************\n");   
}

//////////////////////////////////////////////////
//   End of run action                          //
//////////////////////////////////////////////////
void BT2017RunAct::EndOfRunAction(const G4Run* /*run*/)
{
	// You must save. Otherwise, file will be just empty...
	AM -> Write();
	// You must close the file. Otherwise, file will be crahsed...
 	AM -> CloseFile();
}
