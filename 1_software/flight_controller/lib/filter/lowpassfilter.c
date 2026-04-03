#include "lowpassfilter.h"

#include <math.h>

#define PI 3.1415926f

static Lowpassfilter_Data filterOutput;

void LowpassFilter_Init(Lowpassfilter_Handle *filt, Lowpassfilter_Type type, float cutoffFreqHz, float sampleFreqHz) 
{
	filt->init.type = type;

	float w0 = 2.0f * PI * cutoffFreqHz / sampleFreqHz;
	float sin_w0 = sinf(w0);
	float cos_w0 = cosf(w0);

	if(LPF_TYPE_BESSEL == filt->init.type)
	{
		// K is tan(w0/2) calculated via sin/cos
        double K = sin_w0 / (1.0 + cos_w0);
        double K2 = K * K;

		// Denominator normalization factor for Bessel s^2 + 3s + 3
        double D = 3.0 + (3.0 * 1.7320f * K) + 3.0 * K2;

    	filt->init.b0 = 3.0 * K2 / D;
    	filt->init.b1 = 2.0 * filt->init.b0;
    	filt->init.b2 = filt->init.b0;
    	filt->init.a1 = (6.0 * K2 - 6.0) / D;
    	filt->init.a2 = (3.0 - (3.0 * 1.7320f * K) + 3.0 * K2) / D;
	}
	else if(LPF_TYPE_BUTTERWORTH == filt->init.type)
	{
    	float Q = 0.7071f; // Butterworth Q factor 1/sqrt(2)
    	float alpha = sin_w0 / (2.0f * Q);
    
	    float cos_w0 = cos_w0;
    	float a0 = 1.0f + alpha;
    
    	filt->init.b0 = (1.0f - cos_w0) / (2.0f * a0);
    	filt->init.b1 = (1.0f - cos_w0) / a0;
    	filt->init.b2 = filt->init.b0;
    	filt->init.a1 = -2.0f * cos_w0 / a0;
    	filt->init.a2 = (1.0f - alpha) / a0;
	}
	else if(LPF_TYPE_CHEBYSHEV == filt->init.type)
	{
		float rippleDb = 0.5;
		// ripple factor (epsilon)
        float eps = sqrtf(powf(10.0, rippleDb / 10.0) - 1.0);
        // intermediate value for pole placement
        float v0 = (1.0 / 2.0) * asinhf(1.0 / eps);

		// Chebyshev specific damping/alpha components
		float sinh_v0 = sinhf(v0);
		float cosh_v0 = coshf(v0);

		// K is tan(w0/2)
        float K = sin_w0 / (1.0 + cos_w0);
        float K2 = K * K;
        
        // Denominator normalization factor
        float D = (K2 + 2.0 * K * sinh_v0 * sinf(PI / 4.0) + (sinh_v0 * sinh_v0 + powf(cosf(PI / 4.0), 2)));
        
        // Digital coefficients (Direct Form I)
        // Note: For Chebyshev, b0 is scaled to ensure 0dB gain at DC or peak
        float scale = (rippleDb > 0) ? powf(10.0, -rippleDb / 20.0) : 1.0;
        
        filt->init.b0 = K2 / D;
        filt->init.b1 = 2.0 * filt->init.b0;
        filt->init.b2 = filt->init.b0;
        
        filt->init.a1 = 2.0 * (K2 - (sinh_v0 * sinh_v0 + powf(cosf(PI / 4.0), 2))) / D;
        filt->init.a2 = (K2 - 2.0 * K * sinh_v0 * sinf(PI / 4.0) + (sinh_v0 * sinh_v0 + powf(cosf(PI / 4.0), 2))) / D;
	}

	
    // Reset states
    filt->init.x1 = filt->init.x2 = 0.0f;
    filt->init.y1 = filt->init.y2 = 0.0f;
}

void LowpassFilter_Update(Lowpassfilter_Handle *filt, Lowpassfilter_Data input) 
{

	float output = filt->init.b0 * inp + filt->init.b1 * filt->init.x1 + filt->init.b2 * filt->init.x2
                   - filt->init.a1 * filt->init.y1 - filt->init.a2 * filt->init.y2;
    
    // Update delays
    filt->init.x2 = filt->init.x1;
    filt->init.x1 = inp;
    filt->init.y2 = filt->init.y1;
    filt->init.y1 = output;
}

Lowpassfilter_Data Lowpassfilter_GetFilteredData(void)
{
	return {0.0, 0.0, 0.0};
}
