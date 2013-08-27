//
// ph_vs_tof_bang.cpp
//  
//
//  Created by Josh Brown on 5/9/2013.
//

/* 

to run this file open root in the directry it and the root files are in and type

.x  ph_vs_tof_bang.cpp

to store the histograms in the outfile listed below type

outfile->Write()

*/


{
    //Opens run#####.root from the directory the script is in
    TFile *f = new TFile("aligned_singles.root");
    //creates a pointer TCSP that points to the TCSP tree produced by the analyzer
    TTree* TCSP = (TTree*)f->Get("aligned_events");
 
    //creating a file to store all histograms
	TFile *outfile = new TFile("ph_vs_tof.root", "RECREATE");

    //Declares and initializes histograms of floats used to accumulate variables of interest
	// ("name","title",number of bins, min bin, max bin)
	TH2F *ptrTOF_t_vs_PH = new TH2F("Energy_v_Height", "Pulse Height vs Time",600, 0,600,4096,0,4096);
	TH1F *TOF_ns = new TH1F("TOF_ns", "Counts vs TOF",25000, -10,5000); 
        TH1F *ptrPH = new TH1F("Pulse_Height", "Counts vs Pulse Height",1024, 0,4096);


   


//this section defines the structures that are stored in the tree
//defining structures for the branches
typedef struct {
    UInt_t MidasEvent;
    UInt_t ADC_ev;
    UInt_t ADC_00;
    UInt_t ADC_01;
    UInt_t ADC_02;

} RAW_ADC;

typedef struct {

    UInt_t MidasEvent;
    UInt_t TDC_ev;
    UInt_t TDC_00_0;
    UInt_t TDC_01_0;
    UInt_t TDC_02_0;
    UInt_t TDC_03_0;
    UInt_t TDC_04_0;
    UInt_t TDC_05_0;
//    UInt_t TDC_06;
//    uint32_t TDC_07;
//    uint32_t TDC_08;
//    uint32_t TDC_09;
//    uint32_t TDC_10;
//    uint32_t TDC_11;
//    uint32_t TDC_12;
//    uint32_t TDC_13;
//    uint32_t TDC_14;
    UInt_t TDC_15_0;
 
 
 
    UInt_t TDC_00_1;
    UInt_t TDC_01_1;
    UInt_t TDC_02_1;
    UInt_t TDC_03_1;
    UInt_t TDC_04_1;
    UInt_t TDC_05_1;
//    UInt_t TDC_06;
//    UInt_t TDC_07;
//    UInt_t TDC_08;
//    UInt_t TDC_09;
//    UInt_t TDC_10;
//    UInt_t TDC_11;
//    UInt_t TDC_12;
//    UInt_t TDC_13;
//    UInt_t TDC_14;
    UInt_t TDC_15_1;



    UInt_t TDC_00_2;
    UInt_t TDC_01_2;
    UInt_t TDC_02_2;
    UInt_t TDC_03_2;
    UInt_t TDC_04_2;
    UInt_t TDC_05_2;
//    UInt_t TDC_06;
//    UInt_t TDC_07;
//    UInt_t TDC_08;
//    UInt_t TDC_09;
//    UInt_t TDC_10;
//    UInt_t TDC_11;
//    UInt_t TDC_12;
//    UInt_t TDC_13;
//    UInt_t TDC_14;
    UInt_t TDC_15_2;


    UInt_t n_0_max;
    UInt_t n_1_max; 
    UInt_t n_2_max;
    UInt_t n_3_max;
    UInt_t n_4_max;
    UInt_t n_5_max;
    UInt_t n_15_max;	
//    UInt_t TDC_15_1;
//    UInt_t TDC_15_2;




} RAW_TDC;
//allocating variables

    Double_t TOF;
    Double_t V; 
    Double_t beta;
    Double_t gamma;
    Double_t E;
    Double_t Mn=939.5653451; 
    Double_t FP=2050;
    Double_t C=2.979245800e10;
    Double_t Channel_dif_0,Channel_dif_1,Channel_dif_2;
    TRandom3 rnd;

    //creating an objects of type RAW_ADC and RAW_TDC for storing variables retrieved from the tree
	RAW_TDC tdc;
	RAW_ADC adc;
    
    //gets the branch of interest from the tree and makes it so GetEntries populates event
    TBranch *ptr_tdc = TCSP->GetBranch("TDC_aligned");
    ptr_tdc->SetAddress(&tdc);
    TBranch *ptr_adc = TCSP->GetBranch("ADC_aligned");
    ptr_adc->SetAddress(&adc);


    //gets the length of the tree used for the loop
    ULong64_t n_Tree = TCSP->GetEntries();
           cout << n_Tree << endl;
    //loop constant is "has read j entries"

    for (ULong64_t j=0;j<2443523;j++) {

        //populates the event structure with the j-1 entry of the tree
        TCSP->GetEntry(j);
	//fills the pulse height histogram
	ptrPH->Fill(adc.ADC_00);

		
	//Calulates positive difference between the RF and the scintilator hit and stores it 
	//in the variable Channel_dif 
	//if(tdc.TDC_02<4e9){
//	if(tdc.TDC_ev<4e9&&adc.ADC_00<4090){	
		if(tdc.n_2_max=1){
		if(tdc.TDC_15_0<4e9&&tdc.TDC_02_0<4e9){
			Channel_dif_0= ((tdc.TDC_02_0+15000)-tdc.TDC_15_0);
					}
		if(tdc.TDC_15_1<4e9&&tdc.TDC_02_0<4e9){
			Channel_dif_1= ((tdc.TDC_02_0+15000)-tdc.TDC_15_1);
					}
		if(tdc.TDC_15_2<4e9&&tdc.TDC_02_0<4e9){
			Channel_dif_2= ((tdc.TDC_02_0+15000)-tdc.TDC_15_2);
				}
				}
//			}
	//Fills the TOF histogram with the rf spectrum with the gamma 
	//peak centered at 25.6 ns
	TOF_ns->Fill((Channel_dif_0+rnd.Rndm())*0.195);
	TOF_ns->Fill((Channel_dif_1+rnd.Rndm())*0.195);
	TOF_ns->Fill((Channel_dif_2+rnd.Rndm())*0.195);

		
	//Fills the PH_vs_TOF 2d histogram with the gamma at 25.6 ns
	ptrTOF_t_vs_PH->Fill(((Channel_dif_0+rnd.Rndm())*0.195-2400),adc.ADC_00);
	ptrTOF_t_vs_PH->Fill(((Channel_dif_1+rnd.Rndm())*0.195-2400),adc.ADC_00);
	ptrTOF_t_vs_PH->Fill(((Channel_dif_2+rnd.Rndm())*0.195-2400),adc.ADC_00);



		//Event counter for the loop
		if(j % 1000==0) {
                cout << "\revents left= " << n_Tree - j;
                cout.flush();
            			}	
    
  					}//end of loop histograms filled with j events
	

  
}
