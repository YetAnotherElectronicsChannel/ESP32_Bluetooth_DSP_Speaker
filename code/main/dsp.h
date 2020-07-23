

struct iir_filt {
   float in_z1;
   float in_z2;
   float out_z1;
   float out_z2;
   float a0[4];
   float a1[4];
   float a2[4];
   float b1[4];
   float b2[4];  
};


//bass-speaker, 45Hz Hipass
static struct iir_filt conf_45_hp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 0.9875820178250215, 0.9937716134506484 , 0.9954766638878052, 0.9958434200204267 },
	.a1 = { -1.975164035650043, -1.9875432269012967, -1.9909533277756104,  -1.9916868400408534 },
	.a2 = { 0.9875820178250215, 0.9937716134506484, 0.9954766638878052, 0.9958434200204267 },
	.b1 = { -1.975009826344679, -1.9875044344654942, -1.9909328674920315, -1.9916695631391037 },
	.b2 = { 0.9753182449554073, 0.9875820193370991, 0.9909737880591895, 0.991704116942603 },
	
}; 	


//bass-speaker, 2500Hz Lowpass
static struct iir_filt conf_2k5_lp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 0.13993146179027674, 0.044278036805267616, 0.025175362450974036, 0.021620113635254866 },
	.a1 = { 0.2798629235805535, 0.08855607361053523, 0.05035072490194807, 0.04324022727050973 },
	.a2 = { 0.13993146179027674, 0.044278036805267616, 0.025175362450974036, 0.021620113635254866 } ,
	.b1 = { -0.699698900564656, -1.3228374096880198, -1.50365042037159, -1.5430779694435248 },
	.b2 = { 0.259424747725763, 0.4999495569090904, 0.6043518701754859, 	0.6295584239845442}
	
}; 	

//bass-speaker, eq 60Hz +5db, q=2.0, bass fun factor :-)
static struct iir_filt conf_60_eq = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 1.0045571784556593, 1.0022854380342285, 1.00165972312044, 1.0015251377673262 },
	.a1 = { -1.9877372051598552, -1.9939885650528055, -1.995661969908465, -1.9960191810845285 },
	.a2 = { 0.9837319156746053, 0.991841509165585, 0.994075168233407, 0.9945556071485654 },
	.b1 = { -1.9877372051598552, -1.9939885650528055, -1.995661969908465, -1.9960191810845285 },
	.b2 = { 0.9882890941302647, 0.9941269471998133, 0.9957348913538471, 0.9960807449158914 }
	
}; 	


//tweeter 2800 Hz Hipass
static struct iir_filt conf_2k8_hp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = {  0.44599764558093963, 0.6764097852300075, 0.753716633131342, 0.7713299340241907},
	.a1 = { -0.8919952911618793, -1.352819570460015, -1.507433266262684, -1.5426598680483814},
	.a2 = { 0.44599764558093963, 0.6764097852300075, 0.753716633131342, 0.7713299340241907},
	.b1 = { -0.5570289325445305, -1.2452156906579934, -1.4458299168752424, -1.489668635259956},
	.b2 = { 0.2269616497792281, 0.4604234502620365, 0.5690366156501254, 0.595651100836807}
	
}; 		
uint8_t fs;

static float process_iir (float inSampleF, struct iir_filt * config) {
	float outSampleF =
	(* config).a0[fs] * inSampleF
	+ (* config).a1[fs] * (* config).in_z1
	+ (* config).a2[fs] * (* config).in_z2
	- (* config).b1[fs] * (* config).out_z1
	- (* config).b2[fs] * (* config).out_z2;
	(* config).in_z2 = (* config).in_z1;
	(* config).in_z1 = inSampleF;
	(* config).out_z2 = (* config).out_z1;
	(* config).out_z1 = outSampleF;
	return outSampleF;
}

static void process_data (uint8_t * data, size_t item_size) {
	
	int16_t * samples = (int16_t *) data;
	int16_t * outsample = (int16_t *) data;
	
	for (int i=0; i<item_size; i=i+4) {
		//restore input samples and make monosum
		float insample = (float) *samples;
		samples++;
		insample += *samples;
		samples++;
		//monosum now available in insample
		
		//process bass speaker
		float lowsample = process_iir(insample, &conf_45_hp);
		lowsample = process_iir(lowsample, &conf_2k5_lp);
		lowsample = process_iir(lowsample, &conf_60_eq);
		//process tweeter
		float highsample = process_iir(insample, &conf_2k8_hp);
		
		
		//restore two outputsamples lowsample & highsample to outputbuffer
		*outsample = (int16_t) lowsample;
		outsample++;
		*outsample = (int16_t) highsample;
		outsample++;		
	
	}

}

static void set_sample_rate (uint8_t samplerate) {
	fs=samplerate;
}

