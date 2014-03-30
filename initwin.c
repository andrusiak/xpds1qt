#include "pds1.h"

/****************************************************************/

InitWindows(argc, argv)
int argc;
char *argv[];
{
  int i, j;
  char **buffer1, **buffer2;
  
//  XGInit(argc, argv, &t);
  
  /*********************************************/
  /* Set up each window structure              */

//  XGSet2D("linlin", "R", "Vr-R Phase Space", "open", 300, 10, dr, dr/dt,
//      False, False, r0*dr, r1*dr, -vrscale, vrscale);

//  XGSet2D("linlin", "R", buffer, "closed", 300, 10, dr, dr/dt,
//       False, False, r0*dr, r1*dr, vscale*(v0[isp][1]+5*vt[isp][1]),
//           vscale*(v0[isp][0]+5*vt[isp][0]));

//  XGSet2D(const char *type, const char *xlabel,
//                  const char *ylabel,
//                  const char *state, int ulx, int uly,
//                  SCALAR xscale, SCALAR yscale, int xautorescale,
//                  int yautorescale, SCALAR xmin, SCALAR xmax,
//                  SCALAR ymin, SCALAR ymax);

//  for (i=nsp-1; i>=0; i--) XGScat2D(r[i], vr[i], &np[i], i);

  /*********************************************/
//TODO
//  XGSet2D("linlin", "Time", "Number(t)", "iconic", 100, 400, 1.0, 1.0, True, True);

//  for (i=nsp-1; i>=0; i--) XGCurve(t_array, np_hist[i], &hist_hi, i);

 /*********************************************/

//TODO
//  XGSet2D("linlin", "R", "rho(r)", "iconic", 400, 100, 1.0, 1.0, False, True,r0*dr,r1*dr);

//  XGCurve(r_array, rho, &ng, 2);

  /*********************************************/
  
//  XGSet2D("linlin", "R", "E field(r)", "open", 500, 10, 1.0, 1.0, False, True, r0*dr, r1*dr);
  
//  XGCurve(r_array, e, &ng, 3);
  
  /*********************************************/

//  XGSet2D("linlin", "R", "Potential(r)", "open", 400, 300, 1.0, 1.0, False, True, r0*dr,r1*dr);
  
//  XGCurve(r_array, phi, &ng, 4);
  
//  /*********************************************/

//  XGSet2D("linlin", "R", "Density n(r)", "open", 600, 500, 1.0, nc2p,
//	  False, True, r0*dr,r1*dr);

//  for (i=nsp-1; i>=0; i--) XGCurve(r_array, srho[i], &ng, i);

  /*********************************************/

//  XGSet2D("linlin", "Time", "Mid Potential(t)", "iconic", 500, 400, 1.0, 1.0, True, True);
  
//  XGCurve(t_array, com_phi_hist[1], &hist_hi, 4);

//  /*********************************************/
  
//  XGSet2D("linlin", "Time", "LHS Potential(t)", "iconic", 700, 400, 1.0, 1.0, True, True);
  
//  XGCurve(t_array, com_phi_hist[0], &hist_hi, 4);

//  /*********************************************/

//  XGSet2D("linlin", "Time", "Current(t)", "iconic", 600, 400, 1.0, 1.0, True, True);
  
//  XGCurve(t_array, com_cur_hist, &hist_hi, 4);

//  /*********************************************/

//  XGSet2D("linlin", "Time", "Sigma(t)", "iconic", 500, 500, 1.0, 1.0, True, True);
  
//  XGCurve(t_array, sigma_hist, &hist_hi, 3);

//  /*********************************************/

//  XGSet2D("linlin", "Time", "Wall Charge(t)", "iconic", 600, 200, 1.0, 1.0, True, True);
  
//  for (i=nsp-1; i>=0; i--) XGCurve(t_array, jwall_hist[i], &hist_hi, i);

//  /*********************************************/

//  XGSet2D("linlog", "Time", "Ave. Kinetic Energy(t) [eV]", "iconic", 500, 200,
//	  1.0, 1.0, True, True);
  
//  for (i=nsp-1; i>=0; i--) XGCurve(t_array, kes_hist[i], &hist_hi, i);

//  /*********************************************/
  
//  XGSet2D("linlog", "Time", "Field Energy(t) [eV]", "iconic", 650, 400, 1.0, 1.0, True, True);
  
//  XGCurve(t_array, ese_hist, &hist_hi, 4);

}

/***************************************************************/
/* Dumping the current state of the system into a binary file. */
/* Note: the binary file is written in the format used by IBM  */
/* and DEC compatible machines (low bytes followed by high     */
/* bytes). For this Dump() and Restore() call the functions    */
/* sun_read() and sun_write().                                 */

char Revision[]={'1','.','0','0'};

void Dump(filename)
char *filename;
{

}

/****************************************************************/

void Restore(filename)
char *filename;
{

}

/****************************************************************/

Quit()
{

}

/****************************************************************/



