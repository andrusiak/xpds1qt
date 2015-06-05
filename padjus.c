#include "pds1.h"
#include <malloc.h>

/****************************************************************/
/* Routine to adjust (initialize, re-pack and inject) particles */
/*	to the desired boundary conditions                           */

adjust()
{
  static float extra[NSMAX][2], iv[NSMAX][2];
  int i, ii, isp, k, nnp, j, secountl, secountr, ien, s;
  static int init_flag=1;
  float vnext(), del_t, vmag, theta, dum;
  int nreflux[NSMAX], ninjected;
  
  /* INITIALIZE array for computing positions of injected particles */
  if (init_flag)
    {
      /* "enter" is now the no. of particles injected each "dt" */
      for (isp=0; isp<nsp; isp++)
	{
	  if (fabs(enter[isp][0]) > 0.) extra[isp][0] = 0.5123123;
	  else extra[isp][0] = 0.;
	  if (fabs(enter[isp][1]) > 0.) extra[isp][1] = 0.5123123;
	  else extra[isp][1] = 0.;
	}
      init_flag = 0;
    }
  
  for (isp=0; isp <nsp; isp++) jwall[isp] = 0.0;
  
  if(secondary) secountl = secountr = 0;
  
  for (isp=0; isp<nsp; isp++)
    {
      nreflux[isp]= 0;	/* nreflux[isp] equals zero */
      if (np[isp] > 0)
	{
	  nnp = np[isp] - 1;
	  i = 0;
	  
	  /* eliminate "outsiders", allow for secondary electron emission, 
	     and if it left thru LH plate, 
	     ADD charge there to sigma (plate surface density). */
	  do
	    { 
	      if (r[isp][i] <= r0) 
		{
		  dum = (vr[isp][i]*vr[isp][i] + vth[isp][i]*vth[isp][i]
			 +vph[isp][i]*vph[isp][i] - emin[isp])/de[isp];
		  s = dum;
		  if (s<nbin[isp]-1 && dum>=0)
		    {
		      dum -= s;
		      
		      fe[isp][s]  += (!s) ? 2*(1-dum) : 1-dum;
		      fe[isp][s+1]+= (s==nbin[isp]-1) ? 2*dum : dum;
		    }
		  
		  dum = -atan(sqrt(vth[isp][i]*vth[isp][i]
				   +vph[isp][i]*vph[isp][i])
			      /vr[isp][i])/dtheta[isp];
		  s = dum;
		  dum -= s;
		  ftheta[isp][s]  += (!s) ? 2*(1-dum) : 1-dum;
		  ftheta[isp][s+1]+= (s==nbin[isp]-1) ? 2*dum : dum;
		  
          energy_flux[isp] = m[isp]*(vr[isp][i]*vr[isp][i] + vth[isp][i]*vth[isp][i]+vph[isp][i]*vph[isp][i])*vscale*vscale/2/area/dt;
		  r[isp][i] = r[isp][nnp];
		  vr[isp][i] = vr[isp][nnp];
		  vth[isp][i] = vth[isp][nnp];
		  vph[isp][i] = vph[isp][nnp];
		  nnp--;

          //TODO
          if(reflux)	nreflux[isp]++;
		  
          jwall[isp] += q[isp]/area;

		  if (secondary)
		    {
		      if (frand() < seec[isp])
			{
			  secountl++;
			  jwall[secsp] -= q[secsp]/area;
			}
		    }
		}
	      
	      else if (r[isp][i] >= r1)
		{
		  r[isp][i] = r[isp][nnp];
		  vr[isp][i] = vr[isp][nnp];
		  vth[isp][i] = vth[isp][nnp];
		  vph[isp][i] = vph[isp][nnp];
					nnp--;
		  if(reflux)	nreflux[isp]++;
		  else if (secondary)
		    if (frand() < seec[isp])	secountr++;
		}
	      else i++;
	    } while (i < nnp);
	  
	  np[isp] = nnp +1;
	}	/* end of "if(np[isp]>0...)" loop */
      
    }	/* end of "for( isp..)"  loop */
  
  /* INJECT new particles at walls, one species at a time */
  
  for (isp=0; isp<nsp; isp++)
    {
      if (inject[isp] || nreflux[isp])
	{
	  for (k=0; k<2; k++)
	    {
	      extra[isp][k] += enter[isp][k];
	      if (reflux && k == 1) extra[isp][k] += nreflux[isp];
	      /* "extra" = the fraction of "dt" when emitted */
	      ninjected = extra[isp][k];
	      
	      while (extra[isp][k] >= 1.)
		{
		  del_t= ((int)extra[isp][k])/(ninjected +1.);
		  ii = np[isp];
		  np[isp]++;
		  
		  if (ii > maxnp[isp]) 	 /* Move array boundaries here */
		    {
		      printf("ADJUST: too many particles, species %d",isp);
		      exit(1);
		    }
		  
		  /* Choose Vr in bit-reversed order */
		  vr[isp][ii] = vnext(k,isp);
		  vmag = vtpt[isp]*sqrt(2*fabs(log(frand()+DBL_MIN)));
		  theta = TWOPI*frand();
		  vth[isp][ii] = vmag*sin(theta);
		  vph[isp][ii] = vmag*cos(theta);
		  
		  /* Adjust Vr for effect of electric field */
          if(MDFIELDS){
              int index = k*(nsp*np[isp]-1); //TODO
              vr[isp][ii] += qm[isp]*(del_t-0.5)*e[index]*dttx;
          }else{
              vr[isp][ii] += qm[isp]*(del_t-0.5)*e[k*nc]*dttx;
          }
		  r[isp][ii] = r0 +k*nc + del_t*vr[isp][ii];
          if(!k){	jwall[isp] -= q[isp]/area;  }
		  extra[isp][k] -= 1;
		}	/* end "while extra..)" loop */
	      
	    }	/* end "for k= 0,1" loop */
	  
	}	/* end "if( inject..)"  loop */
      
    }  /* end "for( isp...)"  loop */

  if(secondary)
    {
      i = np[secsp];
      np[secsp] += secountl + secountr;
      
      if(np[secsp] > maxnp[secsp])
	{
	  printf("ADJUST(Secondaries): too many particles, MUST EXIT!");
	  exit(0);
	}
      for(j=i; j< i+secountl; j++)
	{
	  r[secsp][j]= r0 +frand();
	  vr[secsp][j]= vnext(0,secsp);
	  vmag= vtpt[secsp]*sqrt(2*fabs(log(frand()+DBL_MIN)));
	  theta= TWOPI*frand();
	  vth[secsp][j] = vmag*sin(theta);
	  vph[secsp][j] = vmag*cos(theta);
	}
      i += secountl;
      for(j=i; j< i+secountr; j++)
	{
	  r[secsp][j]= r1 -frand();
	  vr[secsp][j]= vnext(1,secsp);
	  vmag= vtpt[secsp]*sqrt(2*fabs(log(frand()+DBL_MIN)));
	  theta= TWOPI*frand();
	  vth[secsp][j] = vmag*sin(theta);
	  vph[secsp][j] = vmag*cos(theta);
	}
    }/* end of if(secondary) */
  
  //Accumulate total charge of dust grain
  for (isp=0; isp<nsp; isp++) qdust += jwall[isp]*area;

  //Accumulate total energy transmitted to the dust grain
//  for (isp=0; isp<nsp; isp++){ energy_flux[isp]+= jwall[isp]*area/q[isp]*m[isp]*;}

  for (isp=0; isp<nsp; isp++) jwall[isp] /= dt;

}/* end ADJUST */

/**************************************************************/
/* Function to give bit-reversed velocities to the injection
   scheme of ADJUST, one at a time */

float vnext(k, isp)
int k, isp;
{
  static int iv[NSMAX][2], qinit=1;
  static double svf[NSMAX][2];
  static float vel[1024][NSMAX][2];
  int iiv, ksign, ii, i;
  double v1, v2, ddv, vv, vf, df, sumvf, ss, ds;
  extern double fdist(), revers();
  
  /* At t=0., set up iv arrays */
  if (qinit)
    {
      for (i=0; i<NSMAX; i++)	iv[i][0] = iv[i][1] = 0;
      qinit = 0;
    }
  
  /* For a COLD beam, injection is simple: */
  if (vt[isp][k]==0.) return (v0[isp][k]);
  
  if (iv[isp][k]==0)
    {
      /* Integrate velocity-distribution function
	 using Simpsons's rule (to normalize it) */
      if (k==0)
	{
	  v1 = max(vc[isp][0], v0[isp][0]-6.*vt[isp][0]);
	  v2 = max(v0[isp][0]+6.*vt[isp][0], v1+3.*vt[isp][0]);
	}
      else
	{
	  v1 = min(vc[isp][1], v0[isp][1]+6.*vt[isp][1]);
	  v2 = min(v0[isp][1]-6.*vt[isp][1], v1-3.*vt[isp][1]);
	}
      
      ddv = (v2-v1)/2048.;
      vv = v1;
      svf[isp][k] = vv*fdist(vv,k,isp)/2.;
      
      for (i=0; i<1023; i++)
	{
	  vv += ddv;
	  df = fdist(vv,k,isp);
	  svf[isp][k] += vv*(df+df);
	  vv += ddv;
	  df = fdist(vv,k,isp);
	  svf[isp][k] += vv*df;
	}	/* end "for(i...)" loop */
      
      df = fdist(v2,k,isp)/2.;
      svf[isp][k] = (svf[isp][k] + v2*df)*2.*ddv/3.;
    }	/* end "if(iv[...)" loop */
  
  iiv = iv[isp][k]%1024;
  if (iiv==0)
    {
      ksign =  1 - 2*k;
      /* ksign = +1(k=0)  or -1(k=1) */
      sumvf = 0.;
      ii = 0;
      if (k==0) vv = max(vc[isp][0], v0[isp][0]-6.*vt[isp][0]);
      else vv = min(vc[isp][1], v0[isp][1]+6.*vt[isp][1]);
      ds = svf[isp][k]/1024.;
      iiv = iv[isp][k]/1024.;
      ss = revers(iiv)*ds;
      
L11:  ddv = ksign*vt[isp][k]/128.;
      vf = vv*fdist(vv,k,isp);
      
      if (vf!=0.) ddv=ksign*min(fabs(ddv),.45*ds/fabs(vf));
      /* Vary the integration step to aim for the next
	 desired value for the integral */
      
      do
	{
	  df = ddv*(vf + 4.*(vv+ddv)*fdist(vv+ddv,k,isp) +
		    (vv+ddv+ddv)*fdist(vv+ddv+ddv,k,isp))/3.;
	  ddv *= 0.5;
	} while (df >= ds);
      /* If integral increases too much, go back w. dv/2 step */
      ddv *= 2.;
      
      vv += 2.*ddv;
      sumvf += df;
      if (sumvf < ss) goto L11;
      
      vel[ii][isp][k] = vv - (sumvf-ss)*ddv/df;
      /* Interpolate to find the desired velocity */
      
      ii += 1;
      ss += ds;
      if (ii < 1024) goto L11;
      
      if (iv[isp][k]==0) iv[isp][k] = 1;
      /* Skip the very first velocity */
    }  /* end "if(iiv..)"  loop */
  
  ii = iv[isp][k]%1024;
  iiv = Index[ii];
  
  iv[isp][k] += 1;
  if (iv[isp][k] < 0) iv[isp][k] = 1;
  /* Keep track of the total number of particles injected */
  
  return (vel[iiv][isp][k]);
}	/* end of VNEXT */

/**************************************************************/
