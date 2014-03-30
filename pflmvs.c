#include "pds1.h"

double source();

/***************************************************************/
fields()
{
  static float  se;
  static float  *a,  *b,  *c,  *gam;
  static int init_flag1=1;
  int isp, i, j;
  float s,	bet ;
  
  if(init_flag1)
    {  
      a = (float  *)malloc(nc*sizeof(float));
      b = (float  *)malloc(nc*sizeof(float));
      c = (float  *)malloc(nc*sizeof(float));
      gam=(float  *)malloc(nc*sizeof(float));

      if(!a || !b || !c || !gam )
	{
	  printf("fields(): malloc() returned NULL");
	  exit(1);
	}
      for (i=1; i<nc; i++)
	{
	  a[i]= (rg[i]-0.5)*(rg[i]-0.5);
	  b[i]= -((rg[i]-0.5)*(rg[i]-0.5)+(rg[i]+0.5)*(rg[i]+0.5));
	  c[i]= (rg[i]+0.5)*(rg[i]+0.5);
	}	/* end for i...*/
      se= dr*dr/(3.*epsilon);
      c[0] = -1;
      
      init_flag1 =0;
    }    /* end it==0...*/		  
  
  /* Find rho(x) = total charge density...
     ..first put in background (rigid) charge density */
  for (i=0; i<=nc; i++) rho[i]= rhoback;
  
  /* Accumulate charge density "particles" almost LINEARLY to grids */
  for (isp=0; isp<nsp; isp++)
    {  
      for (i=0; i<=nc; i++) srho[isp][i] = 0.;
      
      for (i=np[isp]-1; i>=0; i--)
	{
	  if (r[isp][i] > r0 && r[isp][i] < r1)
	    {
	      j = r[isp][i]-r0;
	      
	      if ( r[isp][i] < r0+.5)
		s =((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-rg3[0])/
		  ((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-r0*r0*r0);
	      
	      else if (r[isp][i] > r1-.5)
		s =(r1*r1*r1-r[isp][i]*r[isp][i]*r[isp][i])/
		  (r1*r1*r1-(r[isp][i]-.5)*(r[isp][i]-.5)*(r[isp][i]-.5));
	      
	      else
		s =((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-rg3[j])/
		  (3.*r[isp][i]*r[isp][i]+.25);
	      
	      srho[isp][j] += (1. -s);
	      srho[isp][j+1] += s;
	    }
	}
      /* Bring "srho(x) to physical normalization... */
      s = FOURPI*dr*dr*dr/3.;
      /*Define 2 HALF-cells at ends */
      srho[isp][0] /= s*((r0+0.5)*(r0+0.5)*(r0+0.5)-r0*r0*r0); 	
      srho[isp][nc]/= s*(r1*r1*r1-(r1-0.5)*(r1-0.5)*(r1-0.5)); 	
      for (i=1; i<nc; i++)	srho[isp][i] /= s*(3.*rg[i]*rg[i] +0.25);
      
      /* ..and gather the separate"srho" + "rhoback" into TOTAL */
      for (i=0; i<=nc; i++) rho[i] += q[isp]*srho[isp][i];
      
    } /* end "for (isp=0 thru nsp-1)" loop */
  if(ecollisional) for (i=0; i<=nc; i++) avenemp[i] += srho[ecolsp][i];
  
  /* Solve a tridiagnal matrix to get Phi[i] at time t*/
  
  jp = backj;
  for (isp=0; isp< nsp; isp++) jp += jwall[isp];
  
  if(ntri != 1) phi[0] = source();
  bet = b0;
  phi[ntri-1] = (*bcptr)()/bet;	 	 
  for(i=ntri; i<=(nc-1); i++)
    {
      gam[i] = c[i-1]/bet;
      bet= b[i] - a[i]*gam[i];
      phi[i] = -(se*(3.*rg[i]*rg[i]+0.25)*rho[i] + a[i]*phi[i-1])/bet;
    }
  
  phi[nc] = 0.;  
  for (i=nc-2; i>= ntri-1; i--) phi[i] -= gam[i+1]*phi[i+1];
  
  (*circuitptr)();
  
  /* ..and calculate E(x) from phi(x)... */
  
  s = 0.5/dr;
  for (i=1; i<nc; i++) e[i] = (phi[i-1] - phi[i+1])*s; 
  s = dr/(3.*epsilon);
  e[0] =(r0+0.5)*(r0+0.5)*(phi[0]-phi[1])/dr;  	
  e[0] -=((r0+0.5)*(r0+0.5)*(r0+0.5) -r0*r0*r0)*s*rho[0];
  e[0] /= r0*r0;
  e[nc] =(r1-0.5)*(r1-0.5)*(phi[nc-1]-phi[nc])/dr; 	
  e[nc] +=(r1*r1*r1 -(r1-0.5)*(r1-0.5)*(r1-0.5))*s*rho[nc];
  e[nc] /=r1*r1;
  
}	/* end of FIELDS */
/***************************************************************/
double source()
{
  if (dcramped)
    {
      if(t < risetime)	return (ramp*t + .5*dcbias*(1- cos(t*w0)));
      else					return (dcbias);
    }
  else return (dcbias + ramp*t + acbias*sin(t*w0 + theta0));
} 

/***************************************************************/
move ()
{
  int isp, i, j;
  static float *a1;
  static int init_flag2=1;
  float s, cthe, vptemp;
  
  if(init_flag2)
  {
    dttx *= 0.5;  /* ...advances only HALF dt */
    
    if(!(a1 = (float *)malloc(ng*sizeof(float))))
    {
      printf("move(): Can not allocate space for the a1 array!");
      exit(0);
    }
  }
  
  for (isp=0; isp<nsp; isp++)
  {	
    for (j=0; j<=nc; j++) a1[j] = e[j]*qm[isp]*dttx;
    
    /* ONE-velocity accelerator, plus mover */
    for (i=np[isp]-1; i>=0; i--)
    {
      if ((r[isp][i] > r0) && (r[isp][i] < r1))
      {
	vptemp=sqrt(vth[isp][i]*vth[isp][i]+vph[isp][i]*vph[isp][i]);
	cthe = r[isp][i]*vptemp;
	j = r[isp][i]-r0;
	
	if ( r[isp][i] < r0+.5)
	  s = ((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-rg3[0])/
	    ((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-r0*r0*r0);	
	else
	{
	  s =((r[isp][i]+.5)*(r[isp][i]+.5)*(r[isp][i]+.5)-rg3[j])
	    /(3.*r[isp][i]*r[isp][i]+.25);
	}
	if (r[isp][i] > r1-.5)
	  s =(r1*r1*r1-r[isp][i]*r[isp][i]*r[isp][i])/
	    (r1*r1*r1-(r[isp][i]-.5)*(r[isp][i]-.5)*(r[isp][i]-.5));
	
	vr[isp][i] += a1[j] + s*(a1[j+1] - a1[j])+vptemp*vptemp/r[isp][i];
	r[isp][i] += vr[isp][i];
	vptemp = cthe/r[isp][i];
	s= frand();
	vth[isp][i] = s*vptemp;
	vph[isp][i] = sqrt(1-s*s)*vptemp;
      }
    }
  }
  if(init_flag2)
  {
    dttx *= 2;  		
    init_flag2 = 0;
  }
}	/* end of MOVE	*/

/***************************************************************/
/* When C = 0.0 */

float bc1()
{ 
  return((((r0+0.5)*(r0+0.5)*(r0+0.5)-r0*r0*r0)*dr*rho[0]/3. 
	  +r0*r0*(sigma + dt*jp))*dr/((r0+0.5)*(r0+0.5)*epsilon));	 
}

void circuit1()
{
  oldsigma = sigma;
  sigma += jp*dt;
}

/***************************************************************/
/* When current source is applied */

float bc2()
{ 
  return ((r0*r0*(4.*sigma -oldsigma +2.*dt*(source()/area +jp))/3. 
	   +((r0+.5)*(r0+.5)*(r0+.5)-r0*r0*r0)*rho[0]*dr/3.)
	  *dr/(epsilon*(r0+.5)*(r0+.5)));
}

void circuit2()
{
  float temp;
  
  exti= source();
  temp= sigma;
  sigma = 4.*sigma -oldsigma +2*dt*(jp+exti/area);
  sigma /= 3.0;
  oldsigma= temp;
}

/***************************************************************/
/* When C tends to infinite */

float bc3()
{
  return(-((3.*rg[1]*rg[1]+0.25)*dr*dr*rho[1]/(3.*epsilon)
	   +(r0+0.5)*(r0+0.5)*source()));
}

void circuit3()
{
  oldsigma = sigma;
  sigma = (r0+0.5)*(r0+0.5)*(phi[0]-phi[1])*epsilon/dr;
  sigma -= ((r0+0.5)*(r0+0.5)*(r0+0.5)-r0*r0*r0)*dr*rho[0]/3.;
  sigma /= r0*r0;
  exti = area*((sigma -oldsigma)/dt - jp);
}

/***************************************************************/
/* The general case */

float bc4()
{
  static float a0, a1, a2, a3, a4;
  float k;
  
  if(!a0)
    {
      a0= 2.25*extl/dt/dt +1.5*extr/dt +1/extc;
      a1= -6*extl/dt/dt -2*extr/dt;
      a2= 5.5*extl/dt/dt +.5*extr/dt;
      a3= -2*extl/dt/dt;
      a4= .25*extl/dt/dt;
    }
  k= (a1*extq +a2*extq_1 +a3*extq_2 +a4*extq_3)/a0;
  return ((((r0+0.5)*(r0+0.5)*(r0+0.5)-r0*r0*r0)*dr*rho[0]/3.
	   +r0*r0*(sigma +dt*jp) +r0*r0*(source()/a0 -k -extq)/area)
	  *dr/((r0+0.5)*(r0+0.5)*epsilon));
}

void circuit4()
{
  static float a0, a1, a2, a3, a4;
  float k;
  
  if(!a0)
    {
      a0= 2.25*extl/dt/dt +1.5*extr/dt +1/extc;
      a1= -6*extl/dt/dt -2*extr/dt;
      a2= 5.5*extl/dt/dt +.5*extr/dt;
      a3= -2*extl/dt/dt;
      a4= .25*extl/dt/dt;
    }
  k= (a1*extq +a2*extq_1 +a3*extq_2 +a4*extq_3)/a0;
  extq_3= extq_2;
  extq_2= extq_1;
  extq_1= extq;
  extq= (source() -phi[0])/a0 -k;
  
  exti= (extq -extq_1)/dt;
  oldsigma = sigma;
  sigma += jp*dt +(extq -extq_1)/area;
}

/***************************************************************/
