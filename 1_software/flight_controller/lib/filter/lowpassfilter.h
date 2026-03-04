#ifndef __LOWPASS_FILTER_H
#define __LOWPASS_FILTER_H

typedef struct {    
	float coeff[2];
	float out[2];

} lowpassfilter;

void lowpassfilter_init(lowpassfilter *filt, float cutoffFreqHz, float sampleTimeS);
float lowpassfilter_update(lowpassfilter *filt, float inp);

#endif
