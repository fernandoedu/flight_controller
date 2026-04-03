#ifndef __LOWPASS_FILTER_H__
#define __LOWPASS_FILTER_H__

typedef enum {
    LPF_TYPE_BESSEL,
    LPF_TYPE_BUTTERWORTH,
    LPF_TYPE_CHEBYSHEV
}Lowpassfilter_Type;

typedef struct {    
	float b0, b1, b2;  // Numerator coefficients
    float a1, a2;      // Denominator coefficients
    float x1, x2;      // Input delays
    float y1, y2;      // Output delays
    Lowpassfilter_Type type;      
}Lowpassfilter_InitTypeDef;

typedef struct
{
	float x;
	float y;
	float z;
}Lowpassfilter_Data;

typedef struct
{
    Lowpassfilter_InitTypeDef   init;
	Lowpassfilter_Data          filteredData;
}Lowpassfilter_Handle;

void LowpassFilter_Init(Lowpassfilter_Handle *filt, Lowpassfilter_Type type, float cutoffFreqHz, float sampleTimeS);
void LowpassFilter_Update(Lowpassfilter_Handle *filt, Lowpassfilter_Data input);
Lowpassfilter_Data Lowpassfilter_GetFilteredData();

#endif
