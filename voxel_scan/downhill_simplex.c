
#include  <def_standard.h>
#include  <def_alloc.h>
#include  <def_minimization.h>

#define DEBUGP( x )

const double ALPHA = 1.0;
const double BETA = 0.5;
const double GAMMA = 2.0;

private  void  get_psum( ndim, p, psum )
    int     ndim;
    double  **p;
    double  psum[];
{
    double   sum;
    int      i, j;

    for_less( j, 0, ndim )
    {
        sum = 0.0;
        for_less( i, 0, ndim+1 )
            sum += p[i][j];
        psum[j] = sum;
    }
}

private  double  amotry( fit_data,
                         p, y, psum, ptry, ndim, ihi, nfunk, fac )
    minimization_struct       *fit_data;
    double                    **p;
    double                    y[];
    double                    psum[];
    double                    ptry[];
    int                       ndim;
    int                       ihi;
    int                       *nfunk;
    double                    fac;
{
    int      j;
    double   fac1, fac2, ytry;
    double   evaluate_fit();

    fac1 = (1.0 - fac) / ndim;
    fac2 = fac1 - fac;
    for_less( j, 0, ndim )
        ptry[j] = psum[j] * fac1 - p[ihi][j] * fac2;
    
    ytry = evaluate_fit( fit_data->evaluation_ptr, ptry );

    ++(*nfunk);

    if( ytry < y[ihi] )
    {
        y[ihi] = ytry;
        for_less( j, 0, ndim )
        {
            psum[j] += ptry[j] - p[ihi][j];
            p[ihi][j] = ptry[j];
        }
    }

    return( ytry );
}

public  Status  initialize_amoeba( fit_data, evaluation_ptr,
                                   initial_parameters, ndim )
    minimization_struct      *fit_data;
    void                     *evaluation_ptr;
    double                   initial_parameters[];
    int                      ndim;
{
    int      i, j;
    Status   status;
    double   evaluate_fit();

    fit_data->evaluation_ptr = evaluation_ptr;

    ALLOC2( status, fit_data->p, ndim+1, ndim, double );

    if( status == OK )
        ALLOC1( status, fit_data->y, ndim+1, double );

    if( status == OK )
        ALLOC1( status, fit_data->psum, ndim, double );

    if( status == OK )
        ALLOC1( status, fit_data->ptry, ndim, double );


    for_less( i, 0, ndim )
        fit_data->p[0][i] = initial_parameters[i];

    for_less( j, 1, ndim+1 )
    {
        for_less( i, 0, ndim )
            fit_data->p[j][i] = fit_data->p[0][i];

        if( fit_data->p[j][j-1] == 0.0 )
            fit_data->p[j][j-1] = 1.0;
        else
            fit_data->p[j][j-1] *= 1.01;
    }

    for_less( i, 0, ndim + 1 )
        fit_data->y[i] = evaluate_fit( evaluation_ptr, fit_data->p[i] );

    get_psum( ndim, fit_data->p, fit_data->psum );

    return( status );
}

public  Status  terminate_amoeba( fit_data )
    minimization_struct  *fit_data;
{
    Status   status;

    FREE2( status, fit_data->p );

    if( status == OK )
        FREE1( status, fit_data->y );

    if( status == OK )
        FREE1( status, fit_data->psum );

    if( status == OK )
        FREE1( status, fit_data->ptry );

    return( status );
}

public  void  amoeba( fit_data, ndim, ftol, max_funk, n_funk, out_parameters )
    minimization_struct  *fit_data;
    int                  ndim;
    double               ftol;
    int                  max_funk;
    int                  *n_funk;
    double               out_parameters[];
{
    int     i, j, ilo, ihi, inhi;
    double  ytry, ysave, rtol, amotry();
    double  evaluate_fit();

    *n_funk = 0;

    while( TRUE )
    {
        ilo = 0;
        ihi = fit_data->y[0] > fit_data->y[1] ? (inhi=1,0) : (inhi=0,1);
        for_less( i, 0, ndim+1 )
        {
            if( fit_data->y[i] < fit_data->y[ilo] )   ilo = i;
            if( fit_data->y[i] > fit_data->y[ihi] ) 
            {
                inhi = ihi;
                ihi = i;
            }
            else if( fit_data->y[i] > fit_data->y[inhi] )
                if( i != ihi ) inhi = i;
        }

        rtol = 2.0 * fabs(fit_data->y[ihi] - fit_data->y[ilo]) /
               (fabs(fit_data->y[ihi]) + fabs(fit_data->y[ilo]));

        DEBUGP( PRINT( "%g %g %g\n", fit_data->y[ihi], fit_data->y[ilo], rtol);)

        if( rtol < ftol )  break;

        if( *n_funk >= max_funk )
        {
            DEBUGP( PRINT( "Too many iterations in amoeba [%d]\n", *n_funk ); )
            break;
        }

        ytry = amotry( fit_data, 
                       fit_data->p, fit_data->y, fit_data->psum, fit_data->ptry,
                       ndim, ihi, n_funk, -ALPHA );

        if( ytry <= fit_data->y[ilo] )
        {
            DEBUGP( PRINT( "Successfully mirrored %g\n", ytry ); )
            ytry = amotry( fit_data, fit_data->p, fit_data->y,
                           fit_data->psum, fit_data->ptry,
                           ndim, ihi, n_funk, GAMMA );
        }
        else if( ytry >= fit_data->y[inhi] )
        {
            DEBUGP( PRINT( "ytry too high %g, SHRINKING\n", ytry ); )
            ysave = fit_data->y[ihi];
            ytry = amotry( fit_data, fit_data->p, fit_data->y,
                           fit_data->psum, fit_data->ptry,
                           ndim, ihi, n_funk, BETA );
            if( ytry >= ysave )
            {
                for_less( i, 0, ndim+1 )
                {
                    if( i != ilo )
                    {
                        for_less( j, 0, ndim ) 
                        {
                            fit_data->psum[j] = 
                              0.5 * ( fit_data->p[i][j] + fit_data->p[ilo][j] );
                            fit_data->p[i][j] = fit_data->psum[j];
                        }
                        fit_data->y[i] = evaluate_fit( fit_data->evaluation_ptr,
                                                       fit_data->psum );
                    }
                }

                *n_funk += ndim;
                get_psum( ndim, fit_data->p, fit_data->psum );
            }
        }
    }

    for_less( i, 0, ndim )
        out_parameters[i] = fit_data->psum[i] / (double) (ndim + 1);

}
