
#include  <def_mni.h>
#include  <def_alloc.h>
#include  <def_minimization.h>
#include  <def_files.h>

#define DEBUGP( x ) x

const double INITIAL_FACTOR = 1.05;

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
                         p, y, psum, ptry, ndim, ihi, nfunk, fac, success )
    downhill_simplex_struct       *fit_data;
    double                        **p;
    double                        y[];
    double                        psum[];
    double                        ptry[];
    int                           ndim;
    int                           ihi;
    int                           *nfunk;
    double                        fac;
    Boolean                       *success;
{
    int      j;
    double   fac1, fac2, ytry;

    fac1 = (1.0 - fac) / ndim;
    fac2 = fac1 - fac;
    for_less( j, 0, ndim )
        ptry[j] = psum[j] * fac1 - p[ihi][j] * fac2;
    
    ytry = fit_data->evaluate_fit_function( fit_data->evaluation_ptr, ptry );

    ++(*nfunk);

    if( ytry < y[ihi] )
    {
        y[ihi] = ytry;
        for_less( j, 0, ndim )
        {
            psum[j] += ptry[j] - p[ihi][j];
            p[ihi][j] = ptry[j];
        }
        *success = TRUE;
    }
    else
        *success = FALSE;

    return( ytry );
}

public  Status  initialize_amoeba( fit_data, ndim, initial_parameters,
                                   evaluate_fit_function, evaluation_ptr )
    downhill_simplex_struct      *fit_data;
    int                          ndim;
    double                       initial_parameters[];
    double                       (*evaluate_fit_function)();
    void                         *evaluation_ptr;
{
    int      i, j;
    Status   status;

    fit_data->evaluate_fit_function = evaluate_fit_function;
    fit_data->evaluation_ptr = evaluation_ptr;

    ALLOC2D( status, fit_data->p, ndim+1, ndim );

    if( status == OK )
        ALLOC( status, fit_data->y, ndim+1 );

    if( status == OK )
        ALLOC( status, fit_data->psum, ndim );

    if( status == OK )
        ALLOC( status, fit_data->ptry, ndim );


    for_less( i, 0, ndim )
        fit_data->p[0][i] = initial_parameters[i];

    for_less( j, 1, ndim+1 )
    {
        for_less( i, 0, ndim )
            fit_data->p[j][i] = fit_data->p[0][i];

        if( fit_data->p[j][j-1] == 0.0 )
            fit_data->p[j][j-1] = 1.0;
        else
            fit_data->p[j][j-1] *= INITIAL_FACTOR;
    }

    for_less( i, 0, ndim + 1 )
        fit_data->y[i] = fit_data->evaluate_fit_function( evaluation_ptr,
                                                          fit_data->p[i] );

    get_psum( ndim, fit_data->p, fit_data->psum );

    return( status );
}

public  Status  terminate_amoeba( fit_data )
    downhill_simplex_struct  *fit_data;
{
    Status   status;

    FREE2D( status, fit_data->p );

    if( status == OK )
        FREE( status, fit_data->y );

    if( status == OK )
        FREE( status, fit_data->psum );

    if( status == OK )
        FREE( status, fit_data->ptry );

    return( status );
}

public  void  amoeba( fit_data, ndim, ftol, max_funk, n_funk, out_parameters )
    downhill_simplex_struct  *fit_data;
    int                      ndim;
    double                   ftol;
    int                      max_funk;
    int                      *n_funk;
    double                   out_parameters[];
{
    int     i, j, ilo, ihi, inhi;
    double  ytry, ysave, rtol, amotry();
    Boolean success;

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
                       ndim, ihi, n_funk, -ALPHA, &success );

        if( ytry <= fit_data->y[ilo] )
        {
            DEBUGP( PRINT( "Successfully mirrored %g\n", ytry ); )
            ytry = amotry( fit_data, fit_data->p, fit_data->y,
                           fit_data->psum, fit_data->ptry,
                           ndim, ihi, n_funk, GAMMA, &success );

            if( success )
            {
                void  print_ranges();
                DEBUGP( PRINT( "Successfully expanded %g\n", ytry ); )
                DEBUGP( print_ranges( ndim, fit_data->p ); )
            }
        }
        else if( ytry >= fit_data->y[inhi] )
        {
            DEBUGP( PRINT( "ytry too high %g, SHRINKING\n", ytry ); )
            ysave = fit_data->y[ihi];
            ytry = amotry( fit_data, fit_data->p, fit_data->y,
                           fit_data->psum, fit_data->ptry,
                           ndim, ihi, n_funk, BETA, &success );
            if( ytry >= ysave )
            {
                DEBUGP( PRINT( "Contracting around smallest.\n" ); )
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
                        fit_data->y[i] = fit_data->evaluate_fit_function(
                                               fit_data->evaluation_ptr,
                                               fit_data->psum );
                    }
                }

                *n_funk += ndim;
                get_psum( ndim, fit_data->p, fit_data->psum );
            }
        }
    }

    for_less( i, 0, ndim )
        out_parameters[i] = fit_data->p[ilo][i];

}

private  void  print_ranges( ndim, p )
    int     ndim;
    double  **p;
{
    int     i, j;
    double  min, max;

    for_less( j, 0, ndim )
    {
        min = 0.0;
        max = 0.0;

        for_less( i, 0, ndim+1 )
        {
            if( i == 0 || p[i][j] < min )  min = p[i][j];
            if( i == 0 || p[i][j] > max )  max = p[i][j];
        }

        PRINT( " %g %g ###", min, max );
    }

    PRINT( "\n" );
}
