//Time of Flight 2D Histogram Slice Fitting Macro
//->Adjusted from Josh Brown's original code by Luis Morales on 8/20/13<-
//Was adjusted in order to implement a superposed two gaussian fit
//over the respective slices.
//With a robust two gaussian fit we can unpack
//the respective contributions from either gaussian
//and thus unpack the respective ToF spectra.


//here's an idea, use TSpectrum to find the first peak, fit a gaussian to said peak
//then use 3 x the sigma parameter in order to find where the cutoff point should be




{
//creates data structure to store variables from the fit we want to keep
struct num_interest{

Double_t chi,NDF,chi_ndf,mean_1,sigma_1,amp_1,center_1,tof,chi,NDF,mean_2,sigma_2,amp_2,center_2;
};

//creates functions which will later be used to fit the ToF slices
TF1 *high_g = new TF1("high_g","gaus"); //fits higher pulse height region
TF1 *low_g = new TF1("low_g", "gaus"); //fits lower pulse height region
TF1 *super_g = new TF1("super_g","gaus(0)+gaus(3)");  //is the superposition of the previous functions


//ensures that the center of the gaussians are bounded within a reasonable range
//ie, high_g must have a center (where 'center' = parameter '1')
//that is between 300 and 800

high_g->SetParLimits(1,300,800);
low_g->SetParLimits(1, 100, 300);

//declares an object which is of type 'num_interest'
//these value objects will house relevant parameters from each slice
num_interest value;

//creates a tree which will store the two soon to be constructed
//'luminosity variables' branches
TTree *lumos = new TTree("lumos","luminosity variables");

//declares an empty histogram which will be filled throughout the 'for' loop
TH1 *current_slice;

//creates two branches wherein important fit parameters
//and whole histograms are to be stored seperately
lumos->Branch("histograms","TH1",&current_slice);
lumos->Branch("vars",&value,"chi/D:NDF:chi_ndf:mean_1:sigma_1:amp_1:center_1:tof:mean_2:sigma_2:amp_2:center2");


//denotes the range of bins which will have their slices fit
//(this is roughly the entire 2D histogram excluding the two weird edge features)
int i = 255;
int n = 300;


//regions of interest that currently don't work, 255-300, not as much, but still should be looked at-->300-325


//begins loop 
for(i ; i<n ; i++){
	//gets the current slice, ie, the projection in y of the pulse height spectrum
	current_slice = Energy_v_Height->ProjectionY("ToF_Slice",i,i);
	

	//this 'if' statement checks whether the current slice is in
	//the region which is roughly the gamma peak.
	//(where between 314-326 is the "rough gamma region")
	//the 'dividing bin' is the bin in which the contributions of the
	//the low_g gaussian roughly finish, this is chosen to be the
	//mean of the current slice in non-gamma regions, but is chosen
	//as 450 otherwise
	if(i<314|i>326){Double32_t dividing_bin = current_slice->GetMean();
	cout << "Current slice is not close to the gamma region."<<endl;
			}
	//this is executed if 'i' is in the "rough gamma region"
        else{dividing_bin = 450;};

    //TSpectrum, peak finders etc.


	
	//range is adjusted for high_g fit
	current_slice->SetAxisRange(dividing_bin, 4096);
	
	
	//high_g's center and width are set to initial guesses
	high_g->SetParameter(1,400);
	high_g->SetParameter(2,250);

	
	//high_g is fit to our previously selected region of the slice
	current_slice->Fit("high_g","IEMO");


	//range is adjusted for low_g fit
	current_slice->SetAxisRange(100, dividing_bin);

	//low_g's center and width are set to initial guesses
	low_g->SetParameter(1, 150);
	low_g->SetParameter(2, 95);

	//low_g is fit to our previously selected region of the slice
	current_slice->Fit("low_g", "IEMO");

	//constructs an array to contain low_g's and high_g's parameters
	Double_t par[6];
	high_g->GetParameters(&par[0]);
	low_g->GetParameters(&par[3]);

	//sets super_g's parameters to the addition of these two gaussians
	super_g->SetParameters(par);

	//resets the axis zoom
	current_slice->SetAxisRange(100, 4096);

	//fits with the superposed gaussian
	current_slice->Fit("super_g", "IEMO");



	
	//this pulls the final superposed gaussian fit function, and makes
	//a pointer to this function called 'funky'
	TF1 *funky = current_slice->GetFunction("super_g");
	
	//gets the various relevant fit parameters and stores them in our
	//previously created 'value' data structure
	value.chi = funky->GetChisquare();
	value.NDF = funky->GetNDF();
	value.chi_ndf = value.chi/value.NDF;
	value.amp_1 = funky->GetParameter(0);
	value.amp_2 = funky->GetParameter(3);
	value.center_1 = funky->GetParameter(1);
	value.center_2 = funky->GetParameter(4);
	value.sigma_1 = funky->GetParameter(2);
	value.sigma_2 = funky->GetParameter(5);
 	value.tof = i;
//	value.cut = value.center+3*value.sigma;


//Uncomment these lines if you want the parameters for each slice
//printed to the console
//	cout<<" chi = "<<value.chi<<endl;
//	cout<<" NDF = "<<value.NDF<<endl;
//	cout<<" chi_ndf = "<<value.chi_ndf<<endl;
//	cout<<" amp_1 = "<<value.amp_1<<endl;
//	cout<<" center_1 = "<<value.center_1<<endl;
//	cout<<" sigma_1 = "<<value.sigma_1<<endl;
//	cout<<" amp_2 = "<<value.amp_2<<endl;
//	cout<<" center_2 = "<<value.center_2<<endl;
//	cout<<" sigma_2 = "<<value.sigma_2<<endl;
//	cout<<" tof = "<<value.tof<<endl;
//	cout<<" cut = "<<value.cut<<endl;

	//fills previously created tree with the now populated 'value' data stuctures and current histogram
	lumos->Fill();


	//destroys previous slice and final superposed function
	~*current_slice;
	~*funky;
		}
}
