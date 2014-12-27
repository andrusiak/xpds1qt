/* 
**	PDS1.H
**	
**	Header file for all PDS1 modules.
*/ 


#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include<X11/Xlib.h>
//#include<xgrafix.h>

#define TWOPI			6.28318530717959
#define FOURPI			12.5663706
#define PI			3.141592653589793
#define EPS0			8.854e-12	/* (F/m)	*/
#define NperTORR		8.3221e20		
#define NSMAX 			4
#define NEMAX 			500
#define HISTMAX 		512  	/* upper bound on histories */

#ifndef max
#define max(x, y)       (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef DBL_MIN
#define DBL_MIN 	1E-200
#endif

#ifndef True
#define True            1
#endif

#ifndef False
#define False           0
#endif

/*******************************************************************/
#if defined(GLOBALORIGIN)
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL float nc2p, r0, r1, area, rhoback, backj, dde, epsilon,extr,
             extl, extc, w0, dcbias, acbias, extq, extq_1, extq_2, extq_3,
             exti, sigma, oldsigma, jp, dr, dt, vrscale, vscale, dttx,
             pressure, gtemp, gden, selsmax, elsengy0, elsengy1, elsengy2,
             sextmax, extengy0, extengy1, extengy2, sionmax, ionengy0,
             ionengy1, ionengy2, achrgx, bchrgx, ascat, bscat, seec[NSMAX],
             df, ramp, theta0, b0, jwall[NSMAX], qm[NSMAX], q[NSMAX], m[NSMAX],
             jj0[NSMAX][2], v0[NSMAX][2], vt[NSMAX][2], vc[NSMAX][2],
             vtpt[NSMAX], emin[NSMAX], de[NSMAX], dtheta[NSMAX],
             Escale[NSMAX], risetime, enter[NSMAX][2],
             qdust, //the charge of dust grain
             energy_flux[NSMAX];//the flux of energy on the particle

GLOBAL int nsp, nc, ng, ntri, secondary, ecolsp, icolsp, secsp, ionsp, 
           ecollisional, icollisional, hist_hi, thist_hi, freq_hi, interval,
           np[NSMAX], Index[1024], nfft, src, dcramped, nbin[NSMAX], reflux,
           inject[NSMAX], maxnp[NSMAX],
           energy[NSMAX];

GLOBAL char freqbuf[30];

GLOBAL double t, Trf;

GLOBAL float **r, **vr, **vth, **vph, **srho, *rho, *e, *phi, *r_array,
             *t_array, *f_array, *sels, *sext, *sion, **np_hist,
             **jwall_hist, **phi_hist, *sigma_hist, *ese_hist, *ke_hist,
             *te_hist, **kes_hist, *cur_hist, *com_cur_hist, **com_phi_hist,
             *cur_fft, *phi_fft, *z_fft, *mphi_fft, *Local_t_array, *ionrate,
             *iontemp, *chrgxrate, *rg, *rg3, *avene, *avenemp, **fe, **ftheta,
             **e_array, **th_array, *Power_hist,
             **qdust_hist;

GLOBAL void  circuit1(), circuit2(), circuit3(), circuit4(), (*circuitptr)();
GLOBAL float frand(), bc1(), bc2(), bc3(), bc4(), (*bcptr)();

GLOBAL int pds_main(int, char **);

GLOBAL int start_(int, char **);
GLOBAL void history(), move(), adjust(), mcc(), fields();

#undef GLOBAL
