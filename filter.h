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
	
	void setType(unsigned char filtertype)
	{
	    if(filtertype<40) type = 0; // LP
	    if(filtertype>=40 && filtertype<80) type = 1; // BP
	    if(filtertype>=80) type = 2; // HP
        }

	inline
	
	int next(int in)
	{
	    int ret;
	    hp = in-buf0;	
	    bp = buf0-buf1;
	    buf0+=fxmul(f,  (hp  + fxmul(fb, bp)));
	    buf1+=fxmul(f, buf0-buf1);
	    if(type==0) ret = buf1;
	    if(type==1) ret = bp;
	    if(type==2) ret = hp;
	    return ret;
	}


private:
	int f;
	long fb;
	int q;
	int buf0,buf1,hp,bp;
	unsigned char type;

	inline
	void setFeedbackf(long f)
	{
	   fb = q+fxmul(q, (int)SHIFTED_1 - (f/128));
	}
	
	void setFeedbackq(long q)
	{
	   fb = q+fxmul(q, (int)SHIFTED_1 - (f/128));
	}

	// convert an int into to its fixed representation
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
