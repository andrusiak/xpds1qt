#include "pds1.h"

/************************************************************************/
/* irand(iseed) returns values 1 through 2**31-2.                       */
/* From "Random number generators: good ones are hard to find", S. Park */
/* and K. Miller, Communications of ACM, October 1988, pp 1192-1201.    */
/* This is from page 1195, and is to work on any system for which       */
/* maxint is 2**31-1 or larger. Due earlier to Schrage, as cited by P&M.*/
/*                                                                      */
/* Note: OK values for iseed are 1 through 2**31-2. Give it 0 or 2*31-1 */
/* and it will return the same values thereafter!                       */
/*                                                                      */
/* C version 6/91, Bruce Langdon.                                       */
/*                                                                      */
/* Algorithm replaces seed by mod(a*seed,m). First represent            */
/* seed = q*hi + lo.  Then                                              */
/* a*seed = a*q*hi + lo = (m - r)*hi + a*lo = (a*lo - r*hi) + m*hi,     */
/* and new seed = a*lo - r*hi unless negative; if so, then add m.       */

float frand()
{
  long a = 16807, m = 2147483647, q = 127773, r = 2836;
  long hi, lo;
  static long seed=31207321;
  float fnumb;

  hi = seed/q;
  lo = seed - q*hi;
  seed = a*lo - r*hi;
  /* "seed" will always be a legal integer of 32 bits (including sign). */
  if(seed <= 0) seed = seed + m;
  fnumb = seed/2147483646.0;

  return(fnumb);
}

/************************************************************************/
/* Returns bit reversed num in base 2.                                  */

float revers(num)
unsigned int num;
{
  double f=0.5, sum=0.0;
  
  while(num)
  {
    if (num & 1) sum += f;      /* is 1st bit set? */
    f *= 0.5;
    num >>= 1;	                /* fast divide by 2 */
  }
  return (sum);
}

/************************************************************************/

double fdist(vv, k, isp)
int k, isp;
double vv;
{
  double u;
  
  u = (vv - v0[isp][k])/vt[isp][k];
  return (exp(-u*u*0.5));
}	/* end FDIST */

/************************************************************************/
/* Function inverse of cumulative normal distribution fcn. This
   approximation is from Abramowitz&Stegun who in turn reference
   C.Hastings jr. */

float gausin(p)
float p;
{
  float px, q, t, xn, xd;
  double log(), sqrt();
  px = p;
  if (p > .5) px = 1.-p;
  q = log(p);
  t = sqrt(-q-q);
  xn = 2.515517 + t*(.802853 + .010328*t);
  xd = 1. + t*(1.432788 + t*(.189269 + .001308*t));
  if (p < .5) return(xn/xd-t);
  return(t-xn/xd);
}	/* end of GAUSIN */

/************************************************************************/
/* time history accumulator; calculates and stores all history values,  */
/* and performs combing on history values when low on memory            */

history()
{
  int i, j, k;
  static float jtemp[NSMAX];
  static int count=1;
  
  for (j=0; j<nsp; j++) jtemp[j] +=dt*jwall[j];
  
  if (ecollisional && fmod(t, Trf) <dt)
  {
    avene[0] = avenemp[0]/Trf;
    ionrate[0] +=iontemp[0]/(4.*PI/3.*avenemp[0]
			     *((r0+0.5)*(r0+0.5)*(r0+0.5)-r0*r0*r0)
			     *dr*dr*dr-DBL_MIN)/Trf;
    iontemp[0] =0.0;
    avenemp[0] =0.0;
    
    for (j=1; j<=nc-1; j++)
    {
      avene[j] = avenemp[j]/Trf;
      ionrate[j]+=iontemp[j]/(4.*PI/3.*avenemp[j]*
			      (3.*rg[j]*rg[j]+0.25)*dr*dr*dr-DBL_MIN)/Trf;
      iontemp[j] =0.0;
      avenemp[j] =0.0;
    }
    avene[nc] = avenemp[nc]/Trf;
    ionrate[nc] +=iontemp[nc]/(4.*PI/3.*avenemp[nc]            
			       *(r1*r1*r1-(r1-0.5)*(r1-0.5)*(r1-0.5))
			       *dr*dr*dr-DBL_MIN)/Trf;
    iontemp[nc] =0.0;
    avenemp[nc] =0.0;
  }
  
  if(nfft)
    {
      if (thist_hi >= nfft)
	{
	  freqanalysis();
	  thist_hi=0;
	}
      cur_hist[thist_hi]= exti;
      phi_hist[0][thist_hi] = phi[0];
      phi_hist[1][thist_hi] = phi[nc/2];
      Local_t_array[thist_hi] = t;
      thist_hi++;
    }
  
  if (--count) return;	   /* only accum every interval steps */
  if (hist_hi >= HISTMAX)  /* comb time histories */
    {
      for (j=0; j<nsp; j++)
	{
	  for (i=1, k=4; i<HISTMAX/4; i++, k+=4)
	    {
	      np_hist[j][i] = np_hist[j][k];
	      jwall_hist[j][i]= jwall_hist[j][k];
	      kes_hist[j][i] = kes_hist[j][k];
	    }
	}
      for (i=1, k=4; i<HISTMAX/4; i++, k+=4)
	{
	  com_phi_hist[0][i] = com_phi_hist[0][k];
	  com_phi_hist[1][i] = com_phi_hist[1][k];
	  com_cur_hist[i] = com_cur_hist[k];
	  Power_hist[i] = Power_hist[k];
	  sigma_hist[i] = sigma_hist[k];
	  ese_hist[i] = ese_hist[k];
	  ke_hist[i] = ke_hist[k];
	  te_hist[i] = te_hist[k];
	  t_array[i] = t_array[k];
	}
      hist_hi = i;
      interval *= 4;	
    }
  for (j=0; j<nsp; j++) /* accumulate histories */
    {
      jwall_hist[j][hist_hi]= jtemp[j];
      jtemp[j]= 0.0;
      if(hist_hi) jwall_hist[j][hist_hi] +=jwall_hist[j][hist_hi-1]; 
      np_hist[j][hist_hi] = np[j];
      kes_hist[j][hist_hi] = 0;
      for (i=0; i<np[j]; i++)
	kes_hist[j][hist_hi] += vr[j][i]*vr[j][i] 
	  +(vth[j][i]*vth[j][i] +vph[j][i]*vph[j][i])*r[j][i]*r[j][i];
            
      kes_hist[j][hist_hi] *= Escale[j]*nc2p;
    }
  t_array[hist_hi]= t;
  com_cur_hist[hist_hi]= exti;
  com_phi_hist[0][hist_hi] = phi[0];
  com_phi_hist[1][hist_hi] = phi[nc/2];
  sigma_hist[hist_hi] = sigma;
  Power_hist[hist_hi] = exti*phi[0];
  
  ese_hist[hist_hi] = r0*r0*phi[0]*e[0]*epsilon; /* phi[nc] = 0. */
  for (i=0; i< nc; i++) 
    ese_hist[hist_hi]+= rg[i]*rg[i]*dr*rho[i]*phi[i];
  ese_hist[hist_hi]  *= TWOPI*dr*dr;	 
  ese_hist[hist_hi]  /= 1.602e-19; 
  
  ke_hist[hist_hi] = 0;
  for (j=0; j<nsp; j++)
    {
      ke_hist[hist_hi] += kes_hist[j][hist_hi];
      kes_hist[j][hist_hi] = log10(kes_hist[j][hist_hi]+DBL_MIN);
    }
  te_hist[hist_hi] = log10(fabs(ke_hist[hist_hi]+ese_hist[hist_hi]+DBL_MIN));
  ke_hist[hist_hi] = log10(fabs(ke_hist[hist_hi]+DBL_MIN));
  ese_hist[hist_hi] = log10(fabs(ese_hist[hist_hi]+DBL_MIN));  
  hist_hi++; 																	  
  count = interval;
}

/***************************************************************/

freqanalysis()
{
  int i, j;
  float  *temp1,  *temp2;
  double atan2(), sqrt(), log10();
  
  if(!(temp1= (float  *)malloc((nfft/2)*sizeof(float))))
    printf("Null ptr returned in freqanalysis()");
  if(!(temp2= (float  *)malloc((nfft/2)*sizeof(float))))
    printf("Null ptr returned in freqanalysis()");
  
  for(i=0; i< nfft; i++)
    {
      cur_fft[i]= cur_hist[i];
      phi_fft[i]= phi_hist[0][i];
      mphi_fft[i]= phi_hist[1][i];
    } 
  
  realft(phi_fft-1, freq_hi, 1);
  realft(mphi_fft-1,freq_hi, 1);
  realft(cur_fft-1, freq_hi, 1);
  
  /**** Computing mag and phase of the current signal ***/
  temp2[0]= (cur_fft[0] > 0.0) ? 0.0 : 180.0;
  temp1[0]= fabs(cur_fft[0])/freq_hi/2;
  for(i=1, j=2; i< freq_hi; i++, j+=2)
    {
      temp1[i]= sqrt(cur_fft[j]*cur_fft[j]+ cur_fft[j+1]*cur_fft[j+1])/freq_hi;
      if(fabs(cur_fft[j+1]) < 1e-30 && fabs(cur_fft[j]) < 1e-30)
	temp2[i]= 0.0;
      else
	temp2[i]= (-180./PI)*atan2(cur_fft[j+1], cur_fft[j]);
    }
  for(i=0; i< freq_hi; i++)
    {
      cur_fft[i]= log10(temp1[i] + DBL_MIN);
      cur_fft[freq_hi+i]= temp2[i];
    }
  
  /**** Computing mag and phase of the voltage signal ***/
  temp2[0]= (phi_fft[0] > 0.0) ? 0.0 : 180.0;
  temp1[0]= fabs(phi_fft[0])/freq_hi/2;
  for(i=1, j=2; i< freq_hi; i++, j+=2)
    {
      temp1[i]= sqrt(phi_fft[j]*phi_fft[j]+ phi_fft[j+1]*phi_fft[j+1])/freq_hi;
      if(fabs(phi_fft[j+1]) < 1e-30 && fabs(phi_fft[j]) < 1e-30)
	temp2[i]= 0.0;
      else
	temp2[i]= (-180./PI)*atan2(phi_fft[j+1], phi_fft[j]);
    }
  for(i=0; i< freq_hi; i++)
    {
      phi_fft[i]= log10(temp1[i] + DBL_MIN);
      phi_fft[freq_hi+i]= temp2[i];
    }
  
  /**** Computing mag and phase of the mid-potential signal ***/
  temp2[0]= (mphi_fft[0] > 0.0) ? 0.0 : 180.0;
  temp1[0]= fabs(mphi_fft[0])/freq_hi/2;
  for(i=1, j=2; i< freq_hi; i++, j+=2)
    {
      temp1[i]= sqrt(mphi_fft[j]*mphi_fft[j]+ mphi_fft[j+1]*mphi_fft[j+1])/freq_hi;
      if(fabs(mphi_fft[j+1]) < 1e-30 && fabs(mphi_fft[j]) < 1e-30)
	temp2[i]= 0.0;
      else
	temp2[i]= (-180./PI)*atan2(mphi_fft[j+1], mphi_fft[j]);
    }
  for(i=0; i< freq_hi; i++)
    {
      mphi_fft[i]= log10(temp1[i] + DBL_MIN);
      mphi_fft[freq_hi+i]= temp2[i];
    }
  
  /**** Computing mag and phase of Z(f) ***/
  for(i=0; i< freq_hi; i++)
    {
      z_fft[i]= phi_fft[i] -cur_fft[i];
      z_fft[i+freq_hi]= phi_fft[i+freq_hi] -cur_fft[i+freq_hi];
      
		if(z_fft[i+freq_hi] > 180.0)
		  z_fft[i+freq_hi] -= 360.0;
      
		else if(z_fft[i+freq_hi] < -180.0)
		  z_fft[i+freq_hi] += 360.0;
    }
  free(temp1);
  free(temp2);
}

/***************************************************************/

/* float revers(num,n)
 **	int num,n;
 **	{
 **		float power, rev;
 **		int inum, iquot, irem;
 **		rev = 0.;
 **		inum = num;
 **		power = 1.;
 **	
 **		do
 **		{
 **			iquot = inum/n;
 **			irem = inum - n*iquot;
 **			power /= n;
 **			rev += irem*power;
 **			inum = iquot;
 **		} while (inum > 0);
 **	
 **		return (rev);
 **	}
 */ 
