#include "pds1.h"

#define   NEMAX    500
float sigma1(), sigma2(), sigma3(), sigma4(), sigma5();

/***************************************************************/
/* Monte Carlo scheme for electron and ion-neutrals collisions */

mcc()
{
  static int init_flag=1;
  static float ecol_extra, icol_extra;
  static float max_sigmav_e, max_sigmav_i;
  static float col_prob_e, col_prob_i;
  
  int N, i, isp, k, nnp, j, ien, s, index;
  float random, dum, vel,sigma_total, sum_sigma;
  float engy, rengy, iengy, del, phi1;
  float cosphi, sinphi, coschi, sinchi;
  float temp, vneutr, vneutth, vneutph;
  
  if (init_flag)
  {
    float gden;
    
    gden = NperTORR*pressure/(gtemp+DBL_MIN);  /* Calculating the gas density */
    
    ecolsp= ecollisional-1;
    icolsp= icollisional-1;
    ionsp--;                 /* Fixing the indices into the array of species */
    
    /*****************************************/
    /* Calculating the null collision prob.  */
    
    if(ecollisional)
    {
      max_sigmav_e = 0.0;
      for (i=0; i< 10*NEMAX; i++)
      {
	engy = 0.1*i;
	max_sigmav_e= max(max_sigmav_e, sqrt(2.0*1.602e-19*engy/m[ecolsp])
			  *(sigma1(engy)+sigma2(engy)+sigma3(engy)));
      }
      col_prob_e = 1 -exp(-max_sigmav_e*gden*dt);
    }
    
    if(icollisional)
    {
      max_sigmav_i = 0.0;
      for (i=0; i<10*NEMAX; i++)
      {
	engy = 0.1*i;
	max_sigmav_i= max(max_sigmav_i, sqrt(2.0*1.602e-19*engy/m[icolsp])
			  *(sigma4(engy)+sigma5(engy)));
      }
      col_prob_i = 1 -exp(-max_sigmav_i*gden*dt);
    }
    ecol_extra= icol_extra= 0.5;
    init_flag = 0;
  }
  
  /************************/
  /* Electron collisions  */
  
  if(ecollisional)
  {
    ecol_extra += np[ecolsp]*col_prob_e;
    N = ecol_extra;
    ecol_extra -= N;
    
    nnp = np[ecolsp];
    for(j=0; j< N; j++)
    {
      index= nnp*frand();
      nnp--;
      temp = r[ecolsp][nnp];
      r[ecolsp][nnp] = r[ecolsp][index];
      r[ecolsp][index] = temp;
      
      temp = vr[ecolsp][nnp];
      vr[ecolsp][nnp] = vr[ecolsp][index];
      vr[ecolsp][index] = temp;
      
      temp = vth[ecolsp][nnp];
      vth[ecolsp][nnp] = vth[ecolsp][index];
      vth[ecolsp][index] = temp;
      
      temp = vph[ecolsp][nnp];
      vph[ecolsp][nnp] = vph[ecolsp][index];
      vph[ecolsp][index] = temp;
    }
    
    for(j=nnp; j<nnp+N; j++)
    {
      /* determine if a collision occurs */
      dum = (vr[ecolsp][j]*vr[ecolsp][j] +vth[ecolsp][j]*vth[ecolsp][j]
	     +vph[ecolsp][j]*vph[ecolsp][j]);
      engy= Escale[ecolsp]*dum;
      vel = sqrt(dum);
      sigma_total = max_sigmav_e/(vel*vscale);
      random= frand();
      
      /*********************************************************************/
      /* determine the type of collision and calculate new velocities, etc */
      
      /*******************************/
      /* if the collision is elastic */
      
      if (random <= (sum_sigma =sigma1(engy))/sigma_total)
      {
	/* first normalize vel */
	vr[ecolsp][j] /= vel;
	vth[ecolsp][j] /= vel;
	vph[ecolsp][j] /= vel;
	
	/* scatter the electron */
	newvel(engy, vel, &vr[ecolsp][j], &vth[ecolsp][j], &vph[ecolsp][j], 1);
      }
      /**********************************/
      /* if the collision is excitation */
      
      else if (random <= (sum_sigma += sigma2(engy))/sigma_total)
      {
	/* first normalize vel */
	vr[ecolsp][j] /= vel;
	vth[ecolsp][j] /= vel;
	vph[ecolsp][j] /= vel;
	
	engy -= extengy0;
	vel   = sqrt(fabs(engy)/Escale[ecolsp]);
	
	/* scatter the electron */
	newvel(engy, vel, &vr[ecolsp][j], &vth[ecolsp][j], &vph[ecolsp][j], 0);
	
	/* s= r[ecolsp][j];
	del= r[ecolsp][j] - s;
	extrate[s]  += (!s) ? 2*(1-del) : 1-del;
	extrate[s+1]+= (s== nc-1) ? 2*del : del; */
      }  
      /***************************************************************************/
      /* if the collision is ionization, add the released electron and ion first */
      
      else if(random <= (sum_sigma += sigma3(engy))/sigma_total)
      {
	/* first normalize vel */
	vr[ecolsp][j] /= vel;
	vth[ecolsp][j] /= vel;
	vph[ecolsp][j] /= vel;
	
	/********************************/
	/* subtract the ion. energy and */
	/* partition the remaining energy */
	
	engy -= ionengy0;
	rengy = 10.0*tan(frand()*atan(engy/20.0));
	engy -= rengy;
	
	/********************************/
	/* scatter the created electron */
	
	vel = sqrt(fabs(rengy)/Escale[ecolsp]);
	k = np[ecolsp];
	vr[ecolsp][k] = vr[ecolsp][j];
	vth[ecolsp][k] = vth[ecolsp][j];
	vph[ecolsp][k] = vph[ecolsp][j];
	newvel(rengy, vel, &vr[ecolsp][k], &vth[ecolsp][k], &vph[ecolsp][k], 0);
	r[ecolsp][k] = r[ecolsp][j];
	
	/****************************************/
	/* assign velocities to the created ion */
	
	k = np[ionsp];
	gasvel(&vr[ionsp][k], &vth[ionsp][k], &vph[ionsp][k]);
	r[ionsp][k] = r[ecolsp][j];

	/*****************************************/
	/* finally scatter the incident electron */
	
	vel = sqrt(fabs(engy)/Escale[ecolsp]);
	newvel(engy, vel, &vr[ecolsp][j], &vth[ecolsp][j], &vph[ecolsp][j], 0);
	
	if(++np[ionsp] >maxnp[ionsp] || ++np[ecolsp] >maxnp[ecolsp])
	  puts("mcc(Ionization): too many particles. MUST EXIT!");
	
	s= r[ionsp][k];
	del= r[ionsp][k] - s;
	ionrate[s]  += (!s) ? 2*(1-del) : 1-del;
	ionrate[s+1]+= (s== nc-1) ? 2*del : del;
      }
    }
  }

  /**************************************/

  if(icollisional)
  {
    icol_extra += np[icolsp]*col_prob_i;
    N = icol_extra;
    icol_extra -= N;
    
    nnp = np[icolsp];
    for(j=0; j< N; j++)
    {
      index= nnp*frand();
      nnp--;
      temp = r[icolsp][nnp];
      r[icolsp][nnp] = r[icolsp][index];
      r[icolsp][index] = temp;
      
      temp = vr[icolsp][nnp];
      vr[icolsp][nnp] = vr[icolsp][index];
      vr[icolsp][index] = temp;
      
      temp = vth[icolsp][nnp];
      vth[icolsp][nnp] = vth[icolsp][index];
      vth[icolsp][index] = temp;
      
      temp = vph[icolsp][nnp];
      vph[icolsp][nnp] = vph[icolsp][index];
      vph[icolsp][index] = temp;
    }
    
    for(j=nnp; j<nnp+N; j++)
    {
      gasvel(&vneutr, &vneutth, &vneutph);
      vr[icolsp][j] -= vneutr;
      vth[icolsp][j] -= vneutth;
      vph[icolsp][j] -= vneutph;
      dum = (vr[icolsp][j]*vr[icolsp][j] +vth[icolsp][j]*vth[icolsp][j]
	     +vph[icolsp][j]*vph[icolsp][j]);
      engy= Escale[icolsp]*dum;
      vel = sqrt(dum);
      sigma_total = max_sigmav_i/(vel*vscale);
      random= frand();
      
      /**********************************/
      /* if the collision is scattering */
      
      if (random <= (sum_sigma = sigma5(engy))/sigma_total)
      {
	float up1, up2, up3, mag;
	float r11, r12, r13, r21, r22, r23, r31, r32, r33;
	
	coschi= sqrt(frand());
	sinchi= sqrt(1. -coschi*coschi);
	
	phi1  = TWOPI*frand();
	cosphi= cos(phi1);
	sinphi= sin(phi1);
	
	r13 = vr[icolsp][j]/vel;
	r23 = vth[icolsp][j]/vel;
	r33 = vph[icolsp][j]/vel;
	
	if(r33 == 1.0) { up1= 0;  up2= 1;  up3= 0; }
	else           { up1= 0;  up2= 0;  up3= 1; }
	
	r12 = r23*up3 -r33*up2;
	r22 = r33*up1 -r13*up3;
	r32 = r13*up2 -r23*up1;
	mag = sqrt(r12*r12 + r22*r22 + r32*r32);
	
	r12/= mag;
	r22/= mag;
	r32/= mag;
	
	r11 = r22*r33 -r32*r23;
	r21 = r32*r13 -r12*r33;
	r31 = r12*r23 -r22*r13;
	
	vr[icolsp][j]= vel*coschi*(r11*sinchi*cosphi +r12*sinchi*sinphi +r13*coschi);
	vth[icolsp][j]= vel*coschi*(r21*sinchi*cosphi +r22*sinchi*sinphi +r23*coschi); 
	vph[icolsp][j]= vel*coschi*(r31*sinchi*cosphi +r32*sinchi*sinphi +r33*coschi);
      }
      
      /***************************************/
      /* if the collision is charge exchange */
      
      else if (random <= (sum_sigma += sigma4(engy))/sigma_total)
      {
	vr[icolsp][j] = vth[icolsp][j] = vph[icolsp][j] = 0.0;
	
	s= r[icolsp][j];
	del= r[icolsp][j] - s;
	chrgxrate[s]  += (!s) ? 2*(1-del) : 1-del;
	chrgxrate[s+1]+= (s== nc) ? 2*del : del;
      }
      vr[icolsp][j] += vneutr;
      vth[icolsp][j] += vneutth;
      vph[icolsp][j] += vneutph;
    }
  }
}

/**************************************************************/

newvel(energy, vel, vr, vth, vph, e_flag)
int e_flag;
float energy, vel;
float *vr, *vth, *vph;
{
  float phi1, cosphi, sinphi, coschi, sinchi, up1, up2, up3;
  float mag, r11, r12, r13, r21, r22, r23, r31, r32, r33;

  if(energy < 1e-30)  coschi = 1;
  else  coschi = (energy +2 -2*pow(energy +1,frand()))/energy;
  sinchi= sqrt(1. - coschi*coschi);
  
  phi1  = TWOPI*frand();
  cosphi= cos(phi1);
  sinphi= sin(phi1);
  
  if(e_flag)  vel *= sqrt(1 -2*m[ecolsp]*(1-coschi)/m[ionsp]);
  
  r13 = *vr;
  r23 = *vth;
  r33 = *vph;
  
  if(r33 == 1.0) { up1= 0;  up2= 1;  up3= 0; }
  else           { up1= 0;  up2= 0;  up3= 1; }
  
  r12 = r23*up3 -r33*up2;
  r22 = r33*up1 -r13*up3;
  r32 = r13*up2 -r23*up1;
  mag = sqrt(r12*r12 + r22*r22 + r32*r32);
  
  r12/= mag;
  r22/= mag;
  r32/= mag;
  
  r11 = r22*r33 -r32*r23;
  r21 = r32*r13 -r12*r33;
  r31 = r12*r23 -r22*r13;
  
  *vr= vel*(r11*sinchi*cosphi +r12*sinchi*sinphi +r13*coschi);
  *vth= vel*(r21*sinchi*cosphi +r22*sinchi*sinphi +r23*coschi); 
  *vph= vel*(r31*sinchi*cosphi +r32*sinchi*sinphi +r33*coschi);
}

/***********************************/
/* XSection for Elastic Collisions */

float sigma1(energy)
float energy;
{
  int i;
  static float *sels;
  static int init_flag=1;
  
  /******************  Initialization   ********************/
  if(init_flag)
  {
    sels= (float *)malloc(NEMAX*sizeof(float));
    for (i=0; i<NEMAX; i++)
    {
      if (i <= elsengy0) sels[i] = 0;
      else if (i > elsengy0 && i <= elsengy1)
	sels[i] = selsmax*(i - elsengy0)/(elsengy1 - elsengy0);
      else if (i > elsengy1 && i <= elsengy2)
	sels[i] = selsmax;
      else					
	sels[i] = selsmax*elsengy2/log(elsengy2)*log((float)i)/i;
    }
    init_flag =0;
  }
  /**************************************/
  
  i= energy +.5;
  if(NEMAX <= i) i= NEMAX-1;
  return(sels[i]);
}

/**************************************/
/* XSection for Excitation Collisions */

float sigma2(energy)
float energy;
{
  int i;
  static float *sext;
  static int init_flag=1;
  
  /******************  Initialization   ********************/
  if(init_flag)
  {
    sext= (float *)malloc(NEMAX*sizeof(float));
    for (i=0; i<NEMAX; i++)
    {
      if (i <= extengy0) sext[i] = 0;
      else if (i > extengy0 && i <= extengy1)
	sext[i] = sextmax*(i - extengy0)/(extengy1 - extengy0);
      else if (i > extengy1 && i <= extengy2)
	sext[i] = sextmax;
      else					
	sext[i] = sextmax*extengy2/log(extengy2)*log((float)i)/i;
    }
    init_flag =0;
  }
  /**************************************/

  i= energy +.5;
  if(NEMAX <= i) i= NEMAX-1;
  return(sext[i]);
}

/**************************************/
/* XSection for Ionization Collisions */

float sigma3(energy)
float energy;
{
  int i;
  static float *sion;
  static int init_flag=1;
  
  /******************  Initialization   ********************/
  if(init_flag)
  {
    sion= (float *)malloc(NEMAX*sizeof(float));
    for (i=0; i<NEMAX; i++)
    {
      if (i <= ionengy0) sion[i] = 0;
      else if (i > ionengy0 && i <= ionengy1)
	sion[i] = sionmax*(i - ionengy0)/(ionengy1 - ionengy0);
      else if (i > ionengy1 && i <= ionengy2)
	sion[i] = sionmax;
      else					
	sion[i] = sionmax*ionengy2/log(ionengy2)*log((float)i)/i;
    }
    init_flag =0;
  }
  /**************************************/
  
  i= energy +.5;
  if(NEMAX <= i) i= NEMAX-1;
  return(sion[i]);
}

/*******************************************/
/* XSection for Charge Exchange Collisions */

float sigma4(energy)
float energy;
{
  return(achrgx + bchrgx/(sqrt(energy) +1e-30));
}

/**************************************/
/* XSection for Scattering Collisions */

float sigma5(energy)
float energy;
{
  return(ascat + bscat/(sqrt(energy) +1e-30));
}

/**************************************************************/

#define  NBIN     1000
#define  NVEL    10000
#define  NVTS        5.0

gasvel(vr, vth, vph)
float *vr, *vth, *vph;
{
  static int init_flag= 1;
  static float *vgasr, *vgasth, *vgasph;

  int n, ibin, ibinsrch;
  float vmag, aphi, ddv, ccv, rr;
  float vte, sintheta, costheta;
  float *fcum, *gg, *vv;

  if (init_flag) 
  {
    fcum = (float *) malloc((NBIN+1)*sizeof(float));
    gg   = (float *) malloc((NBIN+1)*sizeof(float));
    vv   = (float *) malloc((NBIN+1)*sizeof(float));

    vgasr= (float *) malloc(NVEL*sizeof(float));
    vgasth= (float *) malloc(NVEL*sizeof(float));
    vgasph= (float *) malloc(NVEL*sizeof(float));

    init_flag= 0;
    vte= sqrt(gtemp/Escale[icolsp]);
    ccv=4.0/sqrt(PI);
    ddv=NVTS/NBIN;
    vv[0]=0.0;
    vv[NBIN]=NVTS;
    gg[0]=0.0;
    for (ibin=1; ibin<NBIN; ibin++) 
    {
      vv[ibin]=ibin*ddv;
      gg[ibin]=ccv*vv[ibin]*vv[ibin]*exp(-vv[ibin]*vv[ibin]);
    }
    gg[NBIN]=0.0;
    
    fcum[0]=0.0;
    for (ibin=1; ibin<NBIN; ibin++)
      fcum[ibin]=fcum[ibin-1]+(ddv/2.0)*(gg[ibin-1]+gg[ibin]);
    fcum[NBIN]=1.0;
    
    ibinsrch=3;
    for(n=0; n< NVEL; n++)
    {
      rr=(1.0*n)/NVEL;
      ibinsrch-=3;
      while (fcum[ibinsrch+1]< rr) ibinsrch += 1;
      vmag=(vv[ibinsrch]*(fcum[ibinsrch+1]-rr)
	    +vv[ibinsrch+1]*(rr-fcum[ibinsrch]))/(fcum[ibinsrch+1]-fcum[ibinsrch]);
      aphi=TWOPI*frand();
      costheta = 1-2*frand();
      sintheta = sqrt(1-costheta*costheta);
      vgasr[n] = vte*vmag*sintheta*cos(aphi);
      vgasth[n] = vte*vmag*sintheta*sin(aphi);
      vgasph[n] = vte*vmag*costheta;
    }
    free(fcum); free(gg); free(vv);
  }
  n = NVEL*frand();
  *vr = vgasr[n];
  *vth = vgasth[n];
  *vph = vgasph[n];
}

/**************************************************************/

writethefile()
{
  int i, s;
  float e;
  FILE *DMPFile;

  DMPFile = fopen("xsections", "w"); 

  for(i=0; i<100; i++)
  {
    e = .01*i;
    fprintf(DMPFile, "%f %e %e %e %e %e\n", e, sigma1(e),
	    sigma2(e), sigma3(e), sigma4(e), sigma5(e));
  }
  for(i=10; i<200; i++)
  {
    e = .1*i;
    fprintf(DMPFile, "%f %e %e %e %e %e\n", e, sigma1(e),
	    sigma2(e), sigma3(e), sigma4(e), sigma5(e));
  }
  for(i=20; i<600; i++)
  {
    e = i;
    fprintf(DMPFile, "%f %e %e %e %e %e\n", e, sigma1(e),
	    sigma2(e), sigma3(e), sigma4(e), sigma5(e));
  }
  fclose(DMPFile);
}

/**************************************************************/
