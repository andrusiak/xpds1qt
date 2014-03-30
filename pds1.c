#define GLOBALORIGIN
#include "pds1.h"
#undef GLOBALORIGIN

//#include "pds1.h"

FILE *InputDeck;

int pds_main(int argc, char *argv[])
{
  display_title();
  
  /*********************************************************/
  /* Read input file, and initialize params and vars. This */
  /* function calls SPECIES and LOAD or Restore            */
  start(argc, argv);
  
 // InitWindows(argc, argv);

  history();	
  int step=0;
  while(True)
  {  
    t += dt;
    move();
    adjust();
    mcc();

    history();

    printf("#%d\%f\n",step,t);
    if(step==100){ return;
    } else step++;
//    XGUpdate();
  }

}	/* End of MAIN */

/**************************************************************/

start(iargc, pargv)
int iargc;
char *pargv[];
{
  float revers();
  float jj0[NSMAX][2];
  char aachar[80];
  int i, isp, initnp[NSMAX][2];
  
  /***********************************************/
  /* Open files and read in "general" parameters */
  if (iargc < 2)
  {
    printf("no input deck? \n");
    exit(1);
  }
  InputDeck = fopen(pargv[1], "r");
  if (InputDeck == NULL)
    {
      /* Try to add default .INP */
      for (i=0; pargv[1][i] != 0; i++) aachar[i] = pargv[1][i];
      aachar[i++] = '.';
      aachar[i++] = 'i';
      aachar[i++] = 'n';
      aachar[i++] = 'p';
      aachar[i++] = 0;
      InputDeck = fopen(aachar, "r");
    }
  if (InputDeck==NULL)
    {
      printf("\nCan't find input file %s\n", pargv[1]);
      exit(1);
    }
  
  /* Read lines until we get to numbers */
  while (fscanf(InputDeck,"%d %d %f %f %f %f %f",
		&nsp, &nc, &nc2p, &dt, &r0, &r1, &epsilon) <7)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f %f %f %f",
		&rhoback, &backj, &dde, &extr, &extl, &extc, &extq) <7)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%d %c %f %f %f %f %f",
		&dcramped, &src, &dcbias, &ramp, &acbias, &w0, &theta0) <7)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%d %d %d %d %d", &secsp, &ecollisional,
		&icollisional, &reflux, &nfft) <5)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %d %f %f", &seec[0], &seec[1],
		&ionsp, &pressure, &gtemp) <5)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f", &selsmax, &elsengy0,
		&elsengy1, &elsengy2) <4)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f", &sextmax, &extengy0,
		&extengy1, &extengy2) <4)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f", &sionmax, &ionengy0,
		&ionengy1, &ionengy2) <4)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f", &achrgx, &bchrgx,
		&ascat, &bscat) <4)
    fscanf(InputDeck, "%s", aachar);
  
  /******************************************************/
  /* Check for errors in the "general" input parameters */
  
  if (extl < 0. || extr < 0. || extc < 0.)
    {
      printf("START: dont be cute with R,L,C < 0\n");
      exit(1);
    }
  if (!dt)
    {
      printf("START: dt == 0!!\n");
      exit(1);
    }
  if (nsp > NSMAX)
    {
      printf("START: nsp > NSMAX.\n");
      exit(1);
    }
  
  /* Check to see if nfft is an integer power of 2 */
  if(nfft)
  {
    for(i=0; i<= 15; i++)
    {
      if(nfft == (1<<i)) 
      {
	i=0;
	break;
      }
    }
    if(i)
    {
      printf("START: nfft is not an integer power of 2.\n");
      exit(1);
    }
  }
  ng= nc+1;
  dr = (r1-r0)/nc; 
  
  /*************************************/
  /* Allocate space for field parameters */
  
  rg = (float  *)malloc(ng*sizeof(float));
  rg3= (float  *)malloc(ng*sizeof(float));
  r_array= (float  *)malloc(ng*sizeof(float));
  for(i=0; i< ng; i++) r_array[i]= r0 +i*dr;
  srho= (float  **)malloc(nsp*sizeof(float  *));
  for(i=0; i<nsp; i++)	srho[i]= (float  *)malloc(ng*sizeof(float));
  
  rho= (float  *)malloc(ng*sizeof(float));
  phi= (float  *)malloc(ng*sizeof(float));
  e  = (float  *)malloc(ng*sizeof(float));
  
  /* Allocate space for xsections */
  if(ecollisional)
    {
      sels= (float  *)malloc(NEMAX*sizeof(float));
      sext= (float  *)malloc(NEMAX*sizeof(float));
      sion= (float  *)malloc(NEMAX*sizeof(float));
      
      avene  = (float  *)malloc(ng*sizeof(float));
      avenemp= (float  *)malloc(ng*sizeof(float));
      ionrate= (float  *)malloc(ng*sizeof(float));
      iontemp= (float  *)malloc(ng*sizeof(float));
      for(i=0; i< ng; i++)	iontemp[i]= ionrate[i]= avene[i]= avenemp[i]= 0.;
    }
  
  if(icollisional)
    {
      chrgxrate= (float  *)malloc(ng*sizeof(float));
      for(i=0; i< ng; i++) chrgxrate[i]= 0.;
    }
  
  /* Allocate space for time history diagnostics */
  
  t_array= (float  *)malloc(HISTMAX*sizeof(float));
  ke_hist= (float  *)malloc(HISTMAX*sizeof(float));
  te_hist= (float  *)malloc(HISTMAX*sizeof(float));
  ese_hist= (float  *)malloc(HISTMAX*sizeof(float));
  sigma_hist= (float  *)malloc(HISTMAX*sizeof(float));
  
  com_cur_hist= (float  *)malloc(HISTMAX*sizeof(float));
  com_phi_hist= (float  **)malloc(2*sizeof(float  *));
  com_phi_hist[0]= (float  *)malloc(HISTMAX*sizeof(float));
  com_phi_hist[1]= (float  *)malloc(HISTMAX*sizeof(float));
  
  Power_hist = (float  *)malloc(HISTMAX*sizeof(float));
  
  np_hist= (float  **)malloc(nsp*sizeof(float  *));
  kes_hist= (float  **)malloc(nsp*sizeof(float  *));
  jwall_hist= (float  **)malloc(nsp*sizeof(float  *));
  
  for(i=0; i<nsp; i++)
    {
      np_hist[i]= (float  *)malloc(HISTMAX*sizeof(float));
      kes_hist[i]= (float  *)malloc(HISTMAX*sizeof(float));
      jwall_hist[i]= (float  *)malloc(HISTMAX*sizeof(float));
    }
  
  /* Allocate space for frequency history diagnostics */
  
  if(nfft)
    {
      freq_hi= nfft/2;
      df= 1/(nfft*dt);
      f_array= (float  *)malloc(nfft*sizeof(float));
      for(i=0; i< nfft; i++) f_array[i]= i*df;
      cur_hist= (float  *)malloc(nfft*sizeof(float));
      phi_hist= (float  **)malloc(2*sizeof(float  *));
      phi_hist[0]= (float  *)malloc(nfft*sizeof(float));
      phi_hist[1]= (float  *)malloc(nfft*sizeof(float));
      
      Local_t_array = (float  *)malloc(nfft*sizeof(float));
      
      z_fft= 	(float  *)malloc(nfft*sizeof(float));
      cur_fft= (float  *)malloc(nfft*sizeof(float));
      phi_fft= (float  *)malloc(nfft*sizeof(float));
      mphi_fft=(float  *)malloc(nfft*sizeof(float));
    }
  
  /**************************************/
  /* Conversions to dimensionless units */
  
  dttx = dt*dt/dr;
  vscale = dr/dt;
  
  gden = NperTORR*pressure/(gtemp+DBL_MIN);
  area = FOURPI*r0*r0;
  theta0 *= TWOPI/360.0;
  Trf=1/(w0+DBL_MIN);
  w0 *= TWOPI;
  epsilon*= EPS0;
  extq_1 = extq_2 = extq_3 = extq;
  interval = 1;

  if(dcramped)
  {
    if(ramp*dcbias < 0.0)
    {
      fprintf(stderr, "\nWARNING: Ramp and dcbias have opposite signs.");
      fprintf(stderr, "\nThe external source will be discontineous!\n\n");
    }
    if(ramp) risetime= fabs(dcbias)/fabs(ramp);
    else     risetime= PI/w0;
  }

  secondary = secsp;
  secsp--;
  
  /**************************************/
  /* Set up each "species" parameters   */
  for (isp=0; isp<nsp; ++isp)
    {
      species(initnp, jj0, isp);
      
      vrscale= max(vrscale, v0[isp][0]+vt[isp][0]);
      vrscale= max(vrscale, -v0[isp][1]+vt[isp][1]);
      
      /* Renormalize all velocities */
      v0[isp][0]/= vscale;
      v0[isp][1]/= vscale;
      vc[isp][0]/= vscale;
      vc[isp][1]/= vscale;
      vt[isp][0]/= vscale;
      vt[isp][1]/= vscale;
      vtpt[isp] /= vscale;
      
      Escale[isp]= .5*vscale*vscale*m[isp]/(1.602e-19);
      emin[isp] /= Escale[isp];
      de[isp]   /= Escale[isp];
      
      /* Conversions from physical to code units */
      qm[isp]= q[isp]/m[isp];
      q[isp] *= nc2p;
      
      /* Normalize  "enter" to = no. particles injected PER DT */
      if((enter[isp][0] = jj0[isp][0]*FOURPI*r0*r0*dt/fabs(q[isp])) > 0.) inject[isp]= 1;
      if((enter[isp][1] = jj0[isp][1]*FOURPI*r1*r1*dt/fabs(q[isp])) > 0.) inject[isp]= 1;
      
    }	/* end  "for isp=0 thru nsp-1 "  loop */
  
  if(!vrscale) vrscale = 1e4;
  vrscale *= 5;
  
  /* Allocate particle arrays */
  r  = (float  **)malloc(nsp*sizeof(float  *));
  vr = (float  **)malloc(nsp*sizeof(float  *));
  vth= (float  **)malloc(nsp*sizeof(float  *));
  vph= (float  **)malloc(nsp*sizeof(float  *));
  
  for(i=0; i<nsp; i++)
    {
      r[i]  = (float *)malloc(maxnp[i]*sizeof(float));
      vr[i] = (float *)malloc(maxnp[i]*sizeof(float));
      vth[i]= (float *)malloc(maxnp[i]*sizeof(float));
      vph[i]= (float *)malloc(maxnp[i]*sizeof(float));
    }
  
  if (nsp && (!r[nsp-1] || !vr[nsp-1] || !vth[nsp-1] || !vph[nsp-1]))
  {
    puts("START: r, v malloc failed");
    exit(1);
  }
  
  /*************************************************/
  /* Allocate space for the dist. functions arrays */
  
  fe      = (float  **)malloc(nsp*sizeof(float  *));
  e_array = (float  **)malloc(nsp*sizeof(float  *));
  ftheta  = (float  **)malloc(nsp*sizeof(float  *));
  th_array= (float  **)malloc(nsp*sizeof(float  *));
  
  for(isp=0; isp<nsp; isp++)
    {
      fe[isp]     = (float  *)malloc(nbin[isp]*sizeof(float));
      ftheta[isp]  = (float  *)malloc(nbin[isp]*sizeof(float));
      e_array[isp]= (float  *)malloc(nbin[isp]*sizeof(float));
      th_array[isp]= (float  *)malloc(nbin[isp]*sizeof(float));
    }
  
  if (nsp && (!th_array[nsp-1]))
    {
      puts("START: f(E) malloc failed");
      exit(1);
    }
  
  for(isp=0; isp<nsp; isp++)
    {
      for(i=0; i<nbin[isp]; i++)
	{
	  fe[isp][i]      = 0.0;
	  ftheta[isp][i]  = 0.0;
	  e_array[isp][i] = emin[isp] +i*de[isp];
	  th_array[isp][i]= i*dtheta[isp];
	}
    }
  
  /*****************************************/
  /* Normalization of the input parameters */
  
  r0 /=dr;
  r1 /=dr;
  for ( i=0; i<= nc; i++)
    {
      rg[i] =r0 +i;
      rg3[i]= (rg[i]+.5)*(rg[i]+.5)*(rg[i]+.5);
    }
  
  /********************************************************/
  /* Load in all species' particles, properly distributed */
  
  if (iargc>2 && access(pargv[2],4))
    {
      /* Try to add default .DMP */
      for (i=0; pargv[2][i] != 0; i++) aachar[i] = pargv[2][i];
      aachar[i++] = '.';
      aachar[i++] = 'D';
      aachar[i++] = 'M';
      aachar[i++] = 'P';
      aachar[i++] = 0;
      pargv[2] = aachar;
    }
  
  /**********************************************************/
  /* Set up array of bit-reversed indices. "revers(num)"    */
  /*	reverses the order of the base-i representation	of  */
  /* the number "num",	yielding a fraction between 0 and 1.*/
  /* Index[] is also used in padjus.c.		            */
  
  for (i=0; i<1024; i++) Index[i] = 1024*revers(i);
  
  /**************************************************/
  /* Start from a dump file if given otherwise load */
  /*	the system with the initial distribution    */
  
  if (iargc>2 && !access(pargv[2],4))	Restore(pargv[2]);
  else load(initnp);
  
  /**************************************************************/
  /* Deciding which circuit solver to use  */
  
  if(extc < 1e-30)   /* When C tends to 0.0 */
    {
      if(fabs(dcbias)+fabs(ramp)+fabs(acbias) >0.0)
	printf("START:The active external source is ignored since C<1e-30\n");
      
      bcptr = bc1;
      circuitptr = circuit1;
      ntri =1;  
      b0 = 1.;
    } 		
  else if(src== 'I' || src== 'i')	/* When current source is applied */
    {
      bcptr = bc2;
      circuitptr = circuit2;
      
      ntri =1;
      b0 = 1.;
    }
  else if(extl <1e-30 && extr <1e-30 &&
	  (extc*(r1-r0)/(FOURPI*r0*r1*epsilon)) >= 1e5) 
    {	 /* When L=R=0. and C tends to infinite */
      bcptr = bc3;
      circuitptr = circuit3;
      ntri =2;
      b0 = -((rg[1]+0.5)*(rg[1]+0.5)+(r0+0.5)*(r0+0.5));
    }
  else  /* The general case with external voltage source */
    { 
      bcptr = bc4;	  
      circuitptr = circuit4;
      ntri =1; 
      b0= 1/(2.25*extl/dt/dt +1.5*extr/dt +1/extc);
      b0 = 1.+r0*r0*dr*b0/((r0+0.5)*(r0+0.5)*epsilon*area);
    }
}	/* End START */

/***************************************************************/

species(initnp, jj0, isp)
     float jj0[][2];
     int isp, initnp[][2];
{
  char aachar[80];
  float iinitn, j0l, j0r, v0l, v0r, vcl, vcr, vtl, vtr,
  fluxl, fluxr, temin, temax;
  
  /* Read in SPECIES parameters... */
  while (fscanf(InputDeck, "%f %f %f %f %f", &q[isp], &m[isp],
		&j0l, &j0r, &iinitn) <5)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%f %f %f %f %f %f %f", &v0l, &v0r, &vtl,
		&vtr, &vcl, &vcr, &vtpt[isp]) <7)
    fscanf(InputDeck, "%s", aachar);
  
  while (fscanf(InputDeck, "%d %f %f %d", &nbin[isp], &temin,
		&temax, &maxnp[isp]) <4)
    fscanf(InputDeck, "%s", aachar);
  
  /* Assign input parameters to arguments */
  jj0[isp][0]= fabs(j0l);
  jj0[isp][1]= fabs(j0r);
  v0[isp][0] = fabs(v0l);
  v0[isp][1] = -fabs(v0r);
  vc[isp][0] = fabs(vcl);
  vc[isp][1] = -fabs(vcr);
  vt[isp][0] = fabs(vtl);
  vt[isp][1] = fabs(vtr);
  vtpt[isp]  = fabs(vtpt[isp]);
  emin[isp]  = temin;
  de[isp]	  = (temax-temin)/(nbin[isp]-1);
  dtheta[isp]= PI/2/(nbin[isp]-1);
  
  if((vt[isp][0] || v0[isp][0]) && (vt[isp][1] ==0.0 && v0[isp][1] ==0.0)) 
  {
    initnp[isp][0] = iinitn*FOURPI*(r1*r1*r1-r0*r0*r0)/(3.*nc2p) +.5;
    initnp[isp][1] = 0.0;
  }
  else if((vt[isp][1] || v0[isp][1]) && (vt[isp][0] ==0.0 && v0[isp][0] ==0.0)) 
  {
    initnp[isp][0] = 0.0;
    initnp[isp][1] = iinitn*FOURPI*(r1*r1*r1-r0*r0*r0)/(3.*nc2p) +.5;
  }
  else if(!vt[isp][0] && !vt[isp][1] && !v0[isp][0] && !v0[isp][1])
  {
    initnp[isp][0] = iinitn*FOURPI*(r1*r1*r1-r0*r0*r0)/(3.*nc2p) +.5;
    initnp[isp][1] = 0.0;
  }
  else
  {
    initnp[isp][0] = .5*iinitn*FOURPI*(r1*r1*r1-r0*r0*r0)/(3.*nc2p) +.5;
    initnp[isp][1] = .5*iinitn*FOURPI*(r1*r1*r1-r0*r0*r0)/(3.*nc2p) +.5;
  }
}	/* End SPECIES */

/***************************************************************/
load(initnp)
int initnp[][2];
{
  double fdist();
  int i, ix, j, k, isp, ksign, ii;
  float xx, v1, v2, ddv, vv, sf, svf, ddf, sumf, ss, ds, ff, 
  Vpp, vmag, theta, vel[1024];
  
  /************************************************/
  /* Load one species and one direction at a time */
  
  for (isp=0; isp<nsp; isp++)
  {
    np[isp]= 0;
    
    for (k=0; k<2; k++)
    {
      if (!initnp[isp][k]) continue;
      
      if (np[isp] + initnp[isp][k] > maxnp[isp])
      {
	printf("LOAD: too many particles, species %2d",isp);
	exit(1);
      }
      if (vt[isp][k]==0. )	/* Loader for a COLD beam   */
      {
	Vpp= (r1*r1*r1-r0*r0*r0)/initnp[isp][k];
	for(i=0; i< initnp[isp][k]; i++)
	{
	  ix = np[isp] +i;
	  if(!i) r[isp][ix]= pow((Vpp +r0*r0*r0) ,1./3);
	  else	 r[isp][ix]= pow((Vpp+r[isp][ix-1]*r[isp][ix-1]*r[isp][ix-1]) ,1./3);
	  vr[isp][ix] = v0[isp][k];
	  vth[isp][ix] =0.;
	  vph[isp][ix] =0.;
	}
	if(dde)
	{
	  for(i=0; i< initnp[isp][k]; i++)
	  {
	    ix = np[isp] +i;
	    xx = TWOPI*(r[isp][ix] -r0)/(r1 -r0);
	    r[isp][ix] += dde*sin(xx);
	  }
	}
      }	/*end V0 != 0. */
      else /* Loader for THERMAL distribution */
      {	
	ksign = 1 - 2*k;
	
	/* Integrate the [distsfcn] AND v*[dist fcn] from v1
	   to v2, using Simpson's rule, to normalize them */
	
	if (k==0)
	{
	  v1 = max(vc[isp][0], v0[isp][0]-6.*vt[isp][0] );
	  v2 = max(v0[isp][0]+6.*vt[isp][0], v1+3.*vt[isp][0]);
	}
	else
	{
	  v1 = min(vc[isp][1], v0[isp][1]+6.*vt[isp][1] );
	  v2 = min(v0[isp][1]-6.*vt[isp][1], v1-3.*vt[isp][1]) ;
	}
	
	ddv = (v2-v1)/2048.;
	vv = v1;
	sf = fdist(vv,k,isp)/2.;
	svf= vv*sf;
	
	for (i=0; i<1024; i++)
	{
	  vv += ddv;
	  ddf = fdist(vv, k, isp);
	  sf += ddf + ddf;
	  svf += vv*(ddf + ddf);
	  vv += ddv;
	  ddf = fdist(vv, k, isp);
	  sf += ddf;
	  svf += vv*ddf;
	}
	
	ddf = fdist(v2, k, isp);
	sf -= ddf;
	svf -= v2*ddf;
	sf = (sf + sf)*fabs(ddv)/3.;
	svf = (svf + svf)*fabs(ddv)/3.;
	/* Integrate again, varying the stepsize to aim for the
	   values of the integral at which particle velocities
	   are desired */
	
	sumf = 0.;
	ii = 0;
	vv = v1;
	ds = sf/1024.;	 
	ss = ds/2.;
	
L12:	ff = fdist(vv, k, isp);
	ddv = ksign*vt[isp][k]/128.;
	if (ff > 0.) ddv = ksign*min(fabs(ddv), .45*ds/ff);
	do
	{
	  ddf = fdist(vv, k, isp) + 4.*fdist(vv + ddv, k, isp)
	    + fdist(vv+ddv+ddv, k, isp);	
	  ddf *= fabs(ddv)/3.;		
	  if (ddf >= ds) ddv *= 0.5;
	} while (ddf >= ds);
	
	vv += ddv+ddv;
	sumf += ddf;
	if (sumf < ss) goto L12;
	
	/* When a desired velocity has been passed, interpolate
	   to find it, and start looking for the next velocity */
	vel[ii++] = vv - (sumf - ss)*ddv/ddf;
	ss += ds;
	if (ii < 1024) goto L12;
	
	/* Compute the number of particles to be loaded, and load
	   the 1024 velocities uniformly in space, in bit-rev order */
	
	Vpp = (r1*r1*r1-r0*r0*r0)/initnp[isp][k];
	for(i=0; i< initnp[isp][k]; i++)
	{
	  ix = np[isp]+ i;
	  if(!i)	r[isp][ix]= pow((Vpp +r0*r0*r0) ,1./3);
	  else	r[isp][ix]=pow((Vpp +r[isp][ix-1]*r[isp][ix-1]*r[isp][ix-1]) ,1./3);
	  vr[isp][ix]	 = vel[Index[(i+k)%1024]];
	  vmag = vtpt[isp]*sqrt(fabs(2*log(frand()+DBL_MIN)));
	  theta = TWOPI*frand();
	  vth[isp][ix] = vmag*cos(theta);
	  vph[isp][ix] = vmag*sin(theta);
	}
	if(dde)
	{
	  for(i=0; i< initnp[isp][k]; i++)
	  {
	    ix = np[isp] +i;
	    xx = TWOPI*(r[isp][ix] -r0)/(r1 -r0);
	    r[isp][ix] += dde*sin(xx);
	  }
	}
      }	/* end of THERMAL loader  "if( VT>0.." */
      
      np[isp] +=initnp[isp][k];
      
    }	/* end  "for( k=0,1...)" loop */
  }  /* end for(isp=0 thru nsp-1..)" loop */
}  /* end LOAD  */

/***************************************************************/

display_title()
{
  printf("\n\nXPDS1-Spherical Bounded Electrostatic 1 Dimensional Code\n");
  printf("version 2.2\n");
  printf("(c) Copyright 1989-1992 Regents of the University of California\n");
  printf("Plasma Theory and Simulation Group\n");
  printf("University of California - Berkeley\n");
}						   

/*****************************************************************/
