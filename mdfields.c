/*
 *  Author: Yurii Andrusiak
 *  yuriy.andrusyak[цуцик]gmail.com
 */

#include "pds1.h"
#include <memory.h>

#define MD_DEBUG 1


// np[isp]      - number of particle of each sort
static long tnp=0;       // total number of particle( of both sorts)

//maxnp[isp]    -  MAX number of particle of each sort
static long tnp_max=0;   // MAX total number of particle( of both sorts)

int recalcBounds(){
    tnp = tnp_max = 0;
    int isp;
    for(isp=0;isp<nsp;isp++){
        tnp += np[isp];
        tnp_max += maxnp[isp];
    }
    return 1;
}

int qCmp(const void * a, const void * b)
{
    int ai = *(const int *)(a);
    int bi = *(const int *)(b);
    float ac=r1d[ai];
    float bc=r1d[bi];

    if (ac < bc)
    {
        return -1;
    }
    else if (ac > bc)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/** Calculates the eletric field near each macroparticle using molecular dynamics approach */
void mdfields()
{
    /****************
     *     INIT     *
     ****************/
    //int np[nsp];
    int isp, i, j;//define iteration indices;
    long *idx;
    static int init_flag1=1;

//    int np_all = 0;
//    int tnp =0; // Total number of particles
//    for(isp=0;isp<nsp;isp++) np_all+=maxnp[isp];
//    for(isp=0;isp<nsp;isp++) tnp+=np[isp];

//    int npmax=np_all/nsp;
//    int tnps= tnp/nsp; // Total number of particles per species
//    //TODO FIX maxnp[isp] vs np[isp] conflict

    recalcBounds();

    if(init_flag1)
    {
        idx   = (typeof(idx))malloc(tnp_max*sizeof(*idx));
        r1d   = (typeof(r1d))malloc(tnp_max*sizeof(*r1d));
        q1    = (typeof(q1))malloc(tnp_max*sizeof(*q1));
        e     = (typeof(e))malloc(tnp_max*sizeof(*e));
        tmp   = (typeof(tmp))malloc(tnp_max*sizeof(*tmp));
        tmp2d = (typeof(tmp2d))malloc(nsp*sizeof(*tmp2d));

//        idx= (typeof(idx))malloc(np_all*sizeof(*idx));
//        r1d= (typeof(r1d))malloc(np_all*sizeof(*r1d));
//        q1= (typeof(q1))malloc(np_all*sizeof(*q1));
//        e = (typeof(e))malloc(np_all*sizeof(*e));
//        tmp =(typeof(tmp))malloc(np_all*sizeof(*tmp));
//        tmp2d =(typeof(tmp2d))malloc(nsp*sizeof(*tmp2d));

        if(!idx || !r1d || !q1 || !e || !tmp)
        {
            printf("mdfields(): malloc() returned NULL");
            exit(1);
        }

//        for (i=0; i<np_all; i++){
//            idx[i]= i;
//        }

//        for(isp=0;isp<nsp;isp++){
//            for (i=0; i<npmax; i++){
//                q1[i+npmax*isp]= isp;
//            }
//        }
        init_flag1 =0;
    }
//    for(i=0; i<ng; i++) phi[i]=0;

    for (i=0; i<tnp; i++){
        idx[i]= i;
    }

    for(isp=0;isp<nsp;isp++){
        for (i=0; i<np[isp]; i++){
            q1[i+np[isp]*isp]= isp;
        }
    }

    for(isp=0;isp<nsp;isp++){
        for(i=0; i<np[isp]; i++){
            r1d[i+np[isp]*isp]= r[isp][i];
        }
    }

    /****************
     *     SORT     *
     ****************/
    qsort(idx, tnp, sizeof(*idx), qCmp);

    //change the order of all related arrays to correspond the increasing r
    swap_idx(r1d, idx, tmp, tnp);
    swap_idx(q1, idx, tmp, tnp);

    int ni[nsp];//number of particle inside the sphere with radius r
    for(isp=0;isp<nsp;isp++) ni[isp]=0;
    //Calculate electric field
    for(i=0;i<tnp;i++){
        //Count how many particles are inside of sphere with radius r_i
        isp = q1[i];
        long index = isp*np[isp]+ni[isp];
        e[index] = e0*(ni[1]-ni[0])/(FOURPI*EPS0*r1d[i]*dr);
        ni[isp]++;\
    }

    //Electrons first, ions last
    long* idx_sp = (long *)malloc(tnp *sizeof(long));
    int ii[nsp];
    for(isp=0;isp<nsp;isp++) ii[isp]=0;

    for(i=0;i<tnp;i++){
        isp = q1[i];
        idx_sp[isp*np[isp]+ii[isp]] = idx[i];
        ii[isp]++;
    }

    //Swap velocities with EFIL order
    swap2d_idx(vr,  idx_sp, tmp, tnp);
    swap2d_idx(vth, idx_sp, tmp, tnp);
    swap2d_idx(vph, idx_sp, tmp, tnp);

    swap2d_idx(r, idx_sp, tmp, tnp);

//  DON'T release @tmp since it's been allocated once per program run
//  free(tmp);

    for(isp=0;isp<nsp;isp++){
        for (i=0; i<np[i]; i++){
            r[isp][i]=r1d[i+np[isp]*isp];
        }
    }
}

/**
 * Swaps the array elements(@arr) according to new indices(@idx)
 *
 *      arr = {a1, a2, a3, a4}
 *      idx = {4,  2,   1,  3}
 *  new arr = {a4, a2, a1, a3}
 *
 * @tmp - temporal holder
 * n - size of arrays
 */
void swap_idx(float* arr, long* idx, float* tmp, long n){
    int i;
    for(i=0;i<n;i++) tmp[i]=arr[idx[i]];
    for(i=0;i<n;i++) arr[i]=tmp[i];
//    memcpy(arr, tmp, n*sizeof(arr));
}
/*
 *  i+isp*len - index in 2d array
 *  []
 *
 */
void swap2d_idx(float** arr, long* idx, float* tmp, long n){
    int i, isp;
    float temp;

    //allocate memory and fill in with zeroes
    float* flag = malloc(n*sizeof(*flag));
    for(i=0;i<n;i++){ flag[i]=0; }

    long len = n/nsp;
    for(isp=0;isp<nsp;isp++){
        for(i=0;i<len;i++){
            int idx2d = i+isp*len;
            if(!flag[idx2d]){
                tmp[idx2d]=arr[isp][idx[idx2d]%nsp];

                flag[idx2d]=1;
            }
        }
    }

    for(isp=0;isp<nsp;isp++){
        for(i=0;i<len;i++){
            int idx2d = i+isp*len;
            if(!flag[idx2d]){
                tmp[idx2d]=arr[isp][idx[idx2d]%nsp];

            }
        }
    }
//    for(isp=0;isp<nsp;isp++){
//        for(i=0;i<len;i++){

//            tmp[i+isp*len]=arr[isp][idx[i]/isp];
//        }
//    }
//    for(isp=0;isp<nsp;isp++){
//        memcpy(arr[isp], tmp+isp*len, len*sizeof(float));
//    }
}

