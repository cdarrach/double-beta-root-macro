/**
*  DoubleBetaRootMacro.C
*
*  Run by typing at root command:
* > .L DoubleBetaRootMacro.C
* > DoubleBetaRootMacro()
*
**/

void Test(char *infile)
{
	// Open the file
	TFile *f = new TFile(infile);

	// Access the events tree (always called T)
	TTree *tree = f->Get("T");

	// And the data-structure branch (always called ds)
	RAT::DS::Root *rds = new RAT::DS::Root();
	tree->SetBranchAddress("ds", &rds);
	
	// PMT Properties are in different tree (we only store this info once per run as PMTs don't move)
	TTree *runtree = f->Get("runT");
	RAT::DS::Run *pmtds = new RAT::DS::Run();
	runtree->SetBranchAddress("run", &pmtds);
	runtree->GetEntry();
	RAT::DS::PMTProperties *pmtProp = pmtds->GetPMTProp();

	// How many events are there
	int nEvents = tree->GetEntries();
	// Loop through 10 of them
	for(int i=0; i<10;++i){
		tree->GetEntry(i);
		// Get the MC information (use method of Root class in namespace RAT::DS)
		RAT::DS::MC *rmc = rds->GetMC();
		// What are we interested in from the MC object?
		double dUT = rmc->GetDeltaUT(); 	// time between this event and previous one
		std::cout << "Event " << rmc->GetEventID() << " simulated dUT = " << dUT << " ns" << std::endl;	

		std::cout << rmc->GetMCParticleCount() << " particles, "
			<< rmc->GetNumPE() << " photoelectrons, "
			<< rmc->GetMCHitCount() << " hits, "
			<< rmc->GetTotScintEdep() << " MeV deposited in scintillator " 
			<< std::endl;
		// Now get some info on the first generated particle
		RAT::DS::MCParticle *rmcparticle = rmc->GetMCParticle(0);	
		std::cout  << "PDG code = "<< rmcparticle->GetPDGCode() 
			<< ", x-position = "<< rmcparticle->GetPos().x() 
			<< "mm, y-momentum ="<< rmcparticle->GetMom().y() << " MeV/c "
			<< std::endl;

		// the root data structure stores how many triggered events
		int nevC = rds->GetEVCount();
		std::cout <<"Triggered " << nevC << " events " <<std::endl;
		if(nevC>0){
			// Get the Event information
			RAT::DS::EV *rev = rds->GetEV(0);
			int npmtCal = rev->GetPMTCalCount();
			std::cout << "#Normal Calibrated PMTs = " << npmtCal 
			          << ", Nhits = " << rev->GetNhits()<< std::endl;	  
			// Loop through the PMT hits and output charge and time and position
			for(int ip=0;ip<npmtCal;++ip){
				RAT::DS::PMTCal *pmt = rev->GetPMTCal(ip);
				std::cout << "PMT " << pmt->GetID() << " q = " << pmt->GetCharge()
					<< " t = " << pmt->GetTime() 
					<< " x = " << pmt->GetPos(pmtProp).x() 
					<< " y = " << pmt->GetPos(pmtProp).y() 
					<< " z = " << pmt->GetPos(pmtProp).z() 
					<< std::endl;
			}		
		}		
	}
}

