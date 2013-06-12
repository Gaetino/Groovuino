#ifndef LOWPASS_H_
#define LOWPASS_H_
#define FX_SHIFT 8
#define SHIFTED_1 256

class LowPassFilter
{

public:


	LowPassFilter();

	void setCutoffFreq(unsigned char cutoff)
	{
		f = cutoff;
		setFeedbackf((int)cutoff);
	}


	void setResonance(unsigned char resonance)
	{
		q = resonance;
		setFeedbackq((int)resonance);
	}

	inline
	int next(int in)
	{
		buf0+=fxmul(f,  ((in - buf0) + fxmul(fb, buf0-buf1)));
		buf1+=fxmul(f, buf0-buf1);
		return buf1;
	}


private:
	int f;
	long fb;
	int q;
	int buf0,buf1;

	inline
	void setFeedbackf(int f)
	{
		fb = q+fxmul(q, (int)SHIFTED_1 - f);
	}
	
	void setFeedbackq(int q)
	{
		fb = q+fxmul(q, (int)SHIFTED_1 - f);
	}

	inline
	long fx(int i)
	{
		return (i<<FX_SHIFT);
	}


	inline
	long fxmul(long a, int b)
	{
		return ((a*b)>>FX_SHIFT);
	}


};

#endif /* LOWPASS_H_ */
