//This is a macro which loops over a root file produced by 
//the offline analyzer. It populates a histogram with all of
//the hits that occur on channel 15 during a BANG experiment.
//It then calculates photon and neutron contribution based on
//the freshly populated histogram and displays these results to
//the console along with the photons to neutrons ratio

//The histogram loop was developed by Josh Brown with the neutron and
//photon calculations developed by Luis Morales 

{
TFile *f = new TFile("run00230.root");

typedef struct {
    UInt_t TDC_ev;
    UInt_t TDC_00;
    UInt_t TDC_01;
    UInt_t TDC_02;
    UInt_t TDC_03;
    UInt_t TDC_04;
    UInt_t TDC_05;
    UInt_t TDC_15_0;
    UInt_t TDC_15_1;
    UInt_t TDC_15_2;
} RAW_TDC;

typedef struct {
    UInt_t ADC_ev;
    UInt_t ADC_00;
    UInt_t ADC_01;
} RAW_ADC;

    
   	RAW_TDC tdc;
	RAW_ADC adc;

    TH1F *ToF = new TH1F("ToF", "Time of Flight", 250000, 0, 250000);


    TTree* TCSP = (TTree*)f->Get("raw_TCSP");

    TBranch *ptr_tdc = TCSP->GetBranch("TDC");
    ptr_tdc->SetAddress(&tdc);
    TBranch *ptr_adc = TCSP->GetBranch("ADC");
    ptr_adc->SetAddress(&adc);


	ULong64_t n_Tree = TCSP->GetEntries();


    for (ULong64_t j=0;j<n_Tree;j++) {


        TCSP->GetEntry(j);

        if (tdc.TDC_02<4e9 && tdc.TDC_15_0<4e9 ){
        ToF->Fill(tdc.TDC_02+10000-tdc.TDC_15_0);
        ToF->Fill(tdc.TDC_02+10000-tdc.TDC_15_1);
    }



}

    Double_t total_int;
    Double_t first_peak_integral;
    Double_t second_peak_integral;
    Double_t gamma_integral;


    ToF->GetXaxis()->SetRange(7460, 7550);
    Int_t start = ToF->GetMaximumBin();
    ToF->GetXaxis()->SetRange(8060, 8160);
    Int_t finish = ToF->GetMaximumBin();
    ToF->GetXaxis()->SetRange(0, 250000);

    Double_t total_int = (ToF->Integral(start, finish));

    TSpectrum *s = new TSpectrum();
    s->Background(ToF, 23);
    ToF->Add(ToF_background,-1);
    ToF->Draw()


    TF1 *f1 = new TF1("f1", "gaus", start-55, start+55);
    ToF->Fit("f1", "R");
    Double_t first_peak_integral = (f1->Integral(start-55, start+55))/2;

    cout<<"first_peak_integral = "<<first_peak_integral<<endl;


    TF1 *f2 = new TF1("f2", "gaus", finish-55, finish+55);
    ToF->Fit("f2", "R");
    Double_t second_peak_integral = (f2->Integral(finish-55, finish+55))/2;



    Double_t gamma_integral = (first_peak_integral + second_peak_integral);


    cout<<"gamma peak1 = "<<first_peak_integral<<endl;
    cout<<"gamma peak2 = "<<second_peak_integral<<endl;
    cout<<"gamma integral = "<<gamma_integral<<endl;
    cout<<"neutron integral = "<<(total_int-gamma_integral)<<endl;
    cout<<"gamma vs neutron ratio = "<<(gamma_integral/(total_int-gamma_integral))<<endl;
    cout<<"statistical error = "<<sqrt((sqrt(gamma_integral)/gamma_integral)**2+(sqrt((total_int-gamma_integral))/(total_int-gamma_integral))**2)<<endl;


//    TFile *spreadsheet = NULL;
//    TFile *spreadsheet = fopen("ToF_Data_Spreadsheet.txt", "w+");
//    if(spreadsheet == NULL){
//        exit(0);
//    }
//    else{


    //}


//    struct ToF_Info {
//        string "run230";
//        stringgamma_integral;
//        neutron_integral;
//        gamma_vs_neutron_ratio;
//        statistical_error;
//    }


//                                       ToF_Ratio_Data(ToF_Info)




}
