/* ----------------------------------------------------------------------------
@COPYRIGHT   :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/callbacks/seal.cc,v 1.1 2005-04-03 03:33:30 stever Exp $";
#endif

#include <iostream>
#include  <display.h>

#undef public  
#undef private
#include <limits.h>
#include <bicseal/SULGRAPHDATA.h>
#include <bicseal/prototypes.h>

SULGRAPHDATA    SULCUS_graph; //global variable for this file
extern char GLOBAL_tab_of_name[NUMBER_SPM_VOL][SIZ_SUL_NAME];


int  GRAPH_READ = NO;
int INIT_GUESS = NO;

int NEW_name_indice = 0;
int UNKNOWN_visible = 1;

static Status Read_New_Graph(STRING *file_graph);
static Status Is_Different(int nb_arc_left, int nb_arc_right, 
			   int nb_arc_left_etiq, int nb_arc_right_etiq,
			   int nb_vertex_left, int nb_vertex_right,
			   int nb_vertex_left_etiq, int nb_vertex_right_etiq);


using std::cout;
using std::endl;
using std::flush;


/* ----------------------------- MNI Header -----------------------------------
@NAME       : compute_hem
@INPUT      : tab_points, nbp Pointsin this tab, hemisphere  *which_hem
@OUTPUT     : 
@RETURNS    : which hemisphere
@DESCRIPTION: compute to which hemisphere this set belongs (left or right)
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1997    Georges Le Goualher
@MODIFIED   : 
----------------------------------------------------------------------------
*/

void compute_hem( Point *tab_points, int nbp, hemisphere  *which_hem){

float percent_left, percent_right;
float x_world, y_world, z_world;
int   i, sum_nbp_left, sum_nbp_right;

*which_hem = unknown_hemisphere;

sum_nbp_left  = 0;
sum_nbp_right = 0;

              for(i=0; i< nbp; i++){

              x_world=  Point_coord( tab_points[i], 0 );
              y_world=  Point_coord( tab_points[i], 1 );
              z_world=  Point_coord( tab_points[i], 2 );
            
              if(x_world < 0.0 )
                sum_nbp_left +=1;
              else
                sum_nbp_right +=1;

             }


             percent_left  = ((float) sum_nbp_left/(float)nbp)*100.0;
             percent_right = ((float) sum_nbp_right/(float)nbp)*100.0;
             
             if( percent_left > 50.0){
                *which_hem = left;
             }
             else{
               if(percent_left > percent_right)
               *which_hem = left;
               else
               *which_hem = right;
              }

}//end compute hem


/* ----------------------------- MNI Header -----------------------------------
@NAME       : check_representation
@INPUT      : 
@OUTPUT     : 
@RETURNS    : ERROR : is associated arc data can not be found
@DESCRIPTION: check that the selected object with 'obj_index' index can
              be matched with an arc of the graph of IDdisplay == obj_index
              compute also the hemisphere where this curve is
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1997    Georges Le Goualher
@MODIFIED   : 
----------------------------------------------------------------------------
*/

Status check_representation( object_struct *current_object,
                            int obj_index, hemisphere *which_hem){
        

int i, nbp,nbd, nb_length, nb_depth;

int object_found;
 lines_struct    *lines;
 quadmesh_struct *quadmesh;
Point  point;
float x_world, y_world, z_world;
float  percent_left,  percent_right;

int sum_nbp_left, sum_nbp_right,sum_nbp_inter;   
hemisphere last_possibility;

Status status;


 object_found = NO;
       
   if( get_object_type( current_object ) == LINES )
     {
      lines = get_lines_ptr( current_object );
     


        nbp = lines->n_points;

 sum_nbp_left = 0; sum_nbp_right =0;sum_nbp_inter = 0;

        for(i=0;i< nbp ;i++){
           point = lines->points[i];
           
           x_world=  (float)Point_coord( point, 0 );
           y_world=  (float)Point_coord( point, 1 );
           z_world=  (float)Point_coord( point, 2 );




           if(x_world < -0.5 ){    
                sum_nbp_left +=1;  
             }
                     
           if(x_world >= -0.5 && x_world <= 0.5){    
                sum_nbp_inter +=1;
             }

           
           if(x_world >  0.5 ){  
                sum_nbp_right +=1;
             }



        }//end for i

           percent_left  = ((float) sum_nbp_left/(float)nbp)*100.0;
           percent_right = ((float) (sum_nbp_right+sum_nbp_inter)/(float)nbp)*100.0;
             
         

  *which_hem = right;          /*COMPARISON OF A DIGITAL CURVE WITH A SPLINE!!*/
             if( percent_left > 50.0){
                *which_hem = left;
             }

            if( percent_right > 50.0){
                *which_hem = right;
             }

            /* else{
               if(percent_left > percent_right)
               *which_hem = left;
               else
               *which_hem = right;
              }*/
  
    

     status = SULCUS_graph.getDigitalRepresentation(obj_index,*which_hem, 
                                               &nb_length, &nb_depth);
             

      if(nb_length != nbp){
        /* apparent conflictual digital representation */
        /* apparent only : because the decision on the appartenance
           of a curve is not compute with the same object :
           skeleton before SSpline versus Skeleton after SSpline */

         if(*which_hem == left)
            last_possibility  = right;
         else
             last_possibility = left;



         status = SULCUS_graph.getDigitalRepresentation(obj_index,last_possibility, 
                                               &nb_length, &nb_depth);

          if(nb_length != nbp) {//this is for sure an ERROR
          status = ERROR;
          object_found = NO;
            }//end if 
          else{
             *which_hem = last_possibility ;
             status = OK;
             object_found = YES;   /* ok my rabbit ... */
          }
      }
      else{
         
        status = OK;
        object_found = YES;
       }
    }//end case of LINES



     if( get_object_type( current_object ) == QUADMESH )
        {
            quadmesh= get_quadmesh_ptr( current_object );
            
            get_quadmesh_n_objects( quadmesh, &nb_length, &nb_depth); 
            
            nb_length = nb_length+1;
            nbp       = nb_length;
            nb_depth  = nb_depth +1;

           *which_hem = unknown_hemisphere;
           sum_nbp_left  = 0;
           sum_nbp_right = 0;
           sum_nbp_inter = 0;
 
          for(i=0;i<nb_length;i++){

             get_quadmesh_point(quadmesh, i, 0, &point);
             
              x_world=  (float)Point_coord( point, 0 );
              y_world=  (float)Point_coord( point, 1 );
              z_world=  (float)Point_coord( point, 2 );
            
              if(x_world < -0.5 ){    
                sum_nbp_left +=1;  
             }
                     
           if(x_world >= -0.5 && x_world <= 0.5 ){    
                sum_nbp_inter +=1;
             }

           
           if(x_world >  0.5 ){  
                sum_nbp_right +=1;
             }



        }//end for i

           percent_left  = ((float) sum_nbp_left/(float)nbp)*100.0;
           percent_right = ((float) (sum_nbp_right+sum_nbp_inter)/(float)nbp)*100.0;
             
         

  *which_hem = right;          /*COMPARISON OF A DIGITAL CURVE WITH A SPLINE!!*/
             if( percent_left > 50.0){
                *which_hem = left;
              }

            if( percent_right > 50.0){
                *which_hem = right;
             }


      
                          
   
      status = SULCUS_graph.getDigitalRepresentation(obj_index,*which_hem, 
                                             &nbp, &nbd); 


        
          
         if(nb_length != nbp || nb_depth != nbd){
            /* apparent conflictual digital representation */

           if(*which_hem == left)
              last_possibility  = right;
           else
             last_possibility = left;



         status = SULCUS_graph.getDigitalRepresentation(obj_index,last_possibility, 
                                               &nb_length, &nb_depth);

          if(nb_length != nbp || nb_depth != nbd) {//this is for sure an ERROR
          status = ERROR;
          object_found = NO;
          }//end if
          else{
             *which_hem = last_possibility ; 
             status = OK;
             object_found = YES;   /* ok my rabbit ... */
          }       
            
        }
         else{
          status = OK;
          object_found = YES;
          }



            
        }//end quadmesh

    
if(object_found == NO){
        printf("selected object is not part of the graph structure ! \n");
        status = ERROR;  
   }
            
           return( status );


}//end check_representation


/* ----------------------------- MNI Header -----------------------------------
@NAME       : set_color_GLOBAL_tab_of_name(current_object)
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: set a predifined color with respect to the name of the sulcus
              for the current object
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1997    Georges Le Goualher
@MODIFIED   : 
----------------------------------------------------------------------------
*/


Status set_color_GLOBAL_tab_of_name(object_struct *current_object,
                                    int obj_index,
                                    seg_name actual_name){

Colour col;
int   n_items, n_points;


Status status;

lines_struct     *lines;
quadmesh_struct  *quadmesh;

status = ERROR;

///supress a warning: int obj_index nor used

if(obj_index == 1000)
    cout<<"adirondack  "<<endl;

if( get_object_type( current_object ) == LINES)
        {

        lines = get_lines_ptr( current_object );
        
        n_items=lines->n_items;
        if(n_items != 1){
           cout<<"don't know how to deal with several lines !"<<endl;
           cout<<"nitems = "<<n_items<<endl;
           exit(0);
        }

        
        n_points=lines->n_points;
        lines->colour_flag = ONE_COLOUR;

        
        col = Global_tab_of_color_name(actual_name);

        lines->colours[0] = col;   
        
        

        status = OK;
        }


if( get_object_type( current_object ) == QUADMESH )
        {

        quadmesh= get_quadmesh_ptr( current_object );
           

      //PUT HERE : set quadmesh colour
        /* quadmesh->colours = col; */

         quadmesh->colour_flag =  ONE_COLOUR;
         col = Global_tab_of_color_name(actual_name);

         quadmesh->colours[0] = col; 
        
        

        status = OK;
        }





return status;


}



/* ----------------------------- MNI Header -----------------------------------
@NAME       : find_model_hem
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: find the hemisphere wher belongs the selected model
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1998    Georges Le Goualher
@MODIFIED   : 
----------------------------------------------------------------------------
*/


Status find_model_hem( model_struct *model, SULGRAPHDATA &SULCUS_graph,
                              hemisphere *which_hem){
 

object_struct    **list_obj;
int              number_obj, obj_index;
int              nb_arc_left, nb_arc_right;
int              hem_found;

Status           status;

status = OK;



number_obj = model->n_objects;


   SULCUS_graph.valsGet(NULL,
                        NULL,
                        NULL,
                        &nb_arc_left,  NULL,
                        NULL,NULL,
                        &nb_arc_right, NULL);
   

hem_found = NO;


    
   
 if( nb_arc_left != nb_arc_right){
    
                       if(number_obj == nb_arc_left ){
                           
                            *which_hem = left ;
                            hem_found = YES;
                            status = OK; 
                         }

                        if(number_obj == nb_arc_right){
                                                            
                             *which_hem = right; 
                             hem_found = YES;
                             status = OK;
                             
                          }

         if(hem_found == NO){                    
           cout<<"hemisphere not found"<<endl;
           cout<<"ERROR: please select the model of the left or the right hemisphere";
           cout<<endl;
           status = ERROR;
                        }
                         

                    }//end if the number of arc is different from
                       //left to right

else{ //case where the left hemisphere has the same number of
         //arcs than the right hemisphere

list_obj   = model->objects;

int score_left =0 , score_right = 0;

    for(obj_index=0; obj_index < 10 ; obj_index++){ /*we compare only the first 10*/

       
       status = check_representation( list_obj[obj_index],
                            obj_index, which_hem);
       
          if(*which_hem == left)
             score_left +=1;
          if(*which_hem == right)
             score_right +=1;
          

          
         if(status == ERROR){
            cout<<"ERROR: in find_model_hem("<<endl;
            exit(0);
         }


    }//end for obj_index

       if(score_left == 10 && score_right == 0 ) /*once again 10*/
          *which_hem = left;
       if(score_right == 10 && score_left == 0)
           *which_hem  = right;


   }//end else where the left hemisphere has the same number of
         //arcs than the right hemisphere


return status;


}//end match_name_with_color


/* ----------------------------- MNI Header -----------------------------------
@NAME       : match_name_with_color
@INPUT      : 
@OUTPUT     : 
@RETURNS    : 
@DESCRIPTION: set a predifined color with respect to the name of the sulcus
              for a model
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    :         1997    Georges Le Goualher
@MODIFIED   : 
----------------------------------------------------------------------------
*/


Status match_name_with_color( model_struct *model, SULGRAPHDATA &SULCUS_graph,
                              hemisphere which_hem){
 

object_struct    **list_obj;
seg_name         name;
int              number_obj, obj_index;
Status           status;

status = OK;


    number_obj = model->n_objects;
    list_obj   = model->objects;
    
    for(obj_index=0; obj_index < number_obj; obj_index++){

       
       
          status = SULCUS_graph.get_arc_name(obj_index, which_hem, &name);//without assumptions
       
          if(status == ERROR){
            cout<<"ERROR: arc not found in get_arc_name"<<endl;
            exit(0);
            }

          status = set_color_GLOBAL_tab_of_name( list_obj[obj_index],
                                                 obj_index,
                                                 name);
         if(status == ERROR){
            cout<<"ERROR: found in set_color_GLOBAL_tab_of_name"<<endl;
            exit(0);
         }


    }//end for obj_index



return status;


}//end match_name_with_color



/************************************************************


     BUTTONS

*************************************************************/


/************************************************************


     BUTTON Q : LOAD GRAPH

*************************************************************/


/* ARGSUSED */

 DEF_MENU_FUNCTION( sulci_menu1 )
{
    Status   status;
    STRING   file_graph;
    int      ret ;
    char     *lastpoint;
             lastpoint = NULL;

    extern char SULDATAEXTENSION[10];

    print( "Enter graph filename: " );

    status = input_string( stdin, &file_graph, ' ' );

    (void) input_newline( stdin );

    lastpoint=strrchr(file_graph,'.');

      if(lastpoint == NULL){
         cout<<"please enter filename with extension "<<endl<<flush;
         status = ERROR;
      }
      else{
          ret = strcmp(lastpoint,SULDATAEXTENSION);
          if(ret == 0)
             status = OK;
          else{
             cout<<"bad extension for file name "<<endl<<flush;
             status = ERROR;
          }
      }
    

    if( status == OK ){
        SULCUS_graph.read_bin_graph(file_graph);
        GRAPH_READ = YES;

     }
    else
       GRAPH_READ = NO;
        

    delete_string( file_graph);

    return( status );
      
    

        
}

/* ARGSUSED */

  DEF_MENU_UPDATE( sulci_menu1 )
{
    return( TRUE );  /*--- always active */
}




/************************************************************


     BUTTON G : Get Morphometrics

*************************************************************/

/* ARGSUSED */

 DEF_MENU_FUNCTION( sulci_menu2 )
{
    
    int             obj_index;
    object_struct   *current_object;
    hemisphere      which_hem;
    
    Status           status;

    
   

    if( get_current_object( display, &current_object ) )
    {
       obj_index = get_current_object_index( display );

       print(" \n");
       print(" \n");
       print( "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
       print( "Selected Object Index: %d\n", obj_index );
        
              
       status= check_representation(current_object,
                            obj_index, &which_hem);
        
     
      if( status == OK){
       
        
        
        SULCUS_graph.dump_arc_info(obj_index,which_hem);
        
        
     }


       
       print(" \n");
        


     }//if get current object



    return( status );


}


/* ARGSUSED */

  DEF_MENU_UPDATE(sulci_menu2 )
{
   

    return( GRAPH_READ );   /*--- not always active */
}






/************************************************************


     BUTTON S : SET NAME

*************************************************************/



/* ARGSUSED */

  DEF_MENU_FUNCTION( sulci_menu3 )
{
    int             obj_index;
    object_struct   *current_object;
    hemisphere      which_hem;
    seg_name        new_name;
     
    Status           status;


if( get_current_object( display, &current_object ) )
    {
       obj_index = get_current_object_index( display );

       print(" \n");
       print(" \n");
       print( "*************************************************   \n");
       print( "The current selected object is index: %d\n", obj_index );
        
        
       status= check_representation(current_object,
                            obj_index, &which_hem);
        
     
      if( status == OK){
       
        

        SULCUS_graph.set_arc_name(obj_index, which_hem, &new_name,&NEW_name_indice);

        /* HERE :change the color of the selected curve */

        status =set_color_GLOBAL_tab_of_name(current_object, obj_index, new_name);


        /* think of the ADD button*/
        

        graphics_models_have_changed( display );
        
     }



       print(" \n");
        print( "************************************************  \n");


     }//if get current object



    return( status );



    
}

/* ARGSUSED */

 DEF_MENU_UPDATE(sulci_menu3 )
{
    return( GRAPH_READ );   /*--- not always active */
}

/************************************************************


     BUTTON S : ADD with SAME NAME

*************************************************************/



/* ARGSUSED */

  DEF_MENU_FUNCTION( sulci_menu7 )
{
    int             obj_index;
    object_struct   *current_object;
    hemisphere      which_hem;
    seg_name        new_name;


     
    Status           status;


if( get_current_object( display, &current_object ) )
    {
       obj_index = get_current_object_index( display );

       print(" \n");
       print(" \n");
       print( "*************************************************   \n");
       print( "The current selected object is index: %d\n", obj_index );
        
        
       status= check_representation(current_object,
                            obj_index, &which_hem);
        


       status = check_name_with_hemisphere(NEW_name_indice, which_hem);

       if( status != OK){
         
         cout<<" ---> CONFLICT BETWEEN NAME AND HEMISPHERE ! "<<endl;
         cout<<"  name : no change  "<<endl;
       }
       else{

     
      if( status == OK){
       
        

        status=SULCUS_graph.set_arc_name_with_indice(obj_index, which_hem, 
                                              NEW_name_indice, &new_name, 1.0);

        /* HERE :change the color of the selected curve */


        if(status == OK)
        status =set_color_GLOBAL_tab_of_name(current_object, obj_index, new_name);
        
        graphics_models_have_changed( display );
        
      }
       }
    



       print(" \n");
        print( "************************************************  \n");


     }//if get current object

 //set_menu_text_int( menu_window, menu_entry, NEW_name_indice ); 
 set_menu_text_string(menu_window, menu_entry,(STRING)GLOBAL_tab_of_name[NEW_name_indice]);


    return( status );



    
}

/* ARGSUSED */

 DEF_MENU_UPDATE(sulci_menu7 )
{
    return( GRAPH_READ );   /*--- not always active */
}



/************************************************************


     BUTTON V :  SCAN TO  VOL
     
     scan the buried part of a sulcus onto the MRI volume.

*************************************************************/




/* ARGSUSED */

  DEF_MENU_FUNCTION( sulci_menu4 )
{
    int             obj_index, i, j, p_exist;
    object_struct   *current_object;
    object_struct   *depth_representation;
    quadmesh_struct quadmesh, *quadmesh_copy;
    Point           point;
    int             seg_nbl, seg_nbd;
    int             label;
    display_struct  *slice_window;
    hemisphere      which_hem;
    Colour          actual_col, scanning_color;
    Colour_flags    col_flag;
    lines_struct    *input_lines;
    quadmesh_struct *input_quadmesh;
         
    Status           status;

depth_representation = NULL;


if( get_current_object( display, &current_object ) )
    {
       obj_index = get_current_object_index( display );

       print(" \n");
       print(" \n");
       print( "*************************************************   \n");
       print( "The current selected object is index: %d\n", obj_index );
        
        
       status= check_representation(current_object,
                            obj_index, &which_hem);
        
     
if( status == OK){

          

    if( get_slice_window( display, &slice_window ) &&
        get_n_volumes( slice_window ) > 0 )
    {
        if( Clear_before_polygon_scan )
            clear_labels( slice_window, get_current_volume_index(slice_window));

        

      
       SULCUS_graph.convertArcToQuadmesh(obj_index,which_hem,
                                        &seg_nbl, &seg_nbd, &quadmesh);
      
 
        /*-- extract actual color of the object --*/
         switch(get_object_type( current_object )){

           case LINES :  input_lines  = get_lines_ptr( current_object );
                         col_flag     = input_lines->colour_flag;
                         if(col_flag != ONE_COLOUR)
                            cout<<"WARNING: col_flag != ONE_COLOUR "<<endl;
                         actual_col   = input_lines->colours[0];
                    break;
  
           case QUADMESH : input_quadmesh = get_quadmesh_ptr( current_object );
                           col_flag       = input_quadmesh->colour_flag;
                           if(col_flag != ONE_COLOUR)
                                cout<<"WARNING: col_flag != ONE_COLOUR "<<endl;
                           actual_col     = input_quadmesh->colours[0];
                    break;

            default :      cout<<"ERROR: unexpected object_type received in Vol Scan";
                           cout<<endl<<flush;
                           exit(0);

            }//end switch

        
        depth_representation = create_object(QUADMESH);
        quadmesh_copy = get_quadmesh_ptr(depth_representation);

         
         cout<<"actual color:"<<actual_col<<endl<<flush;
         initialize_quadmesh(quadmesh_copy,actual_col , NULL, seg_nbl, seg_nbd);

        for(j=0;j<seg_nbd;j++){
           for(i=0;i<seg_nbl;i++){

            p_exist =get_quadmesh_point(&quadmesh, i, j, &point);
            set_quadmesh_point(quadmesh_copy, i,j, &point, NULL);
   
           }//end for i
        }//end for j

       // label=slice_window->slice.current_paint_label; //HERE !!!
        label=compute_label_from_color(actual_col, &scanning_color);//cf label_graph in LibSul
        slice_window->slice.current_paint_label = label; 
           cout<<"label: "<<label<<endl;

        
        set_colour_of_label( slice_window,
                             get_current_volume_index(slice_window),
                             label, scanning_color );

        scan_object_to_current_volume( slice_window, depth_representation ); 
        
        delete_object(depth_representation);

        print( " Scan To Vol done.\n" );

        set_slice_window_all_update( slice_window,
                     get_current_volume_index(slice_window), UPDATE_LABELS );

     }//if get slice

}//if status is OK

else{

         print( "no representation of the buried part of the \
selected sulcus available... \n");

      }



    }//end if get current_object

    

    return( status );

 }




/* ARGSUSED */

  DEF_MENU_UPDATE(sulci_menu4 )
{
    return( GRAPH_READ );   /*--- always active */
}



/************************************************************


     BUTTON W :  Init Guess
     
     give an initial labelling of the graph given SC_PM data.

     The rule used is :

     for each ARC
            find max proba
            if this proba is > threshold_proba && depth of that is > threshold_depth
               then affect the name associated with lab to that ARC.
         
*************************************************************/




/* ARGSUSED */

  DEF_MENU_FUNCTION( sulci_menu5 )
{

 object_struct   *current_object, **current_object_list;
 int             number_obj ;
 int             nb_arc_left =0, nb_arc_right=0;
 int             hem_found ;
 Real           thresh_proba, thresh_depth;
 model_struct    *model;
 Object_types    object_type ;
 Status          status;
 hemisphere      which_hem;
 STRING          answer;



  if( get_current_object( display, &current_object) )
    {
       
       object_type = get_object_type(current_object);

       
       switch(object_type){
          
          case MODEL : model    = get_model_ptr(current_object);
                       number_obj = model->n_objects;
                       current_object_list = model->objects;

                        /*-- which hemisphere --*/
                       status=find_model_hem(model, SULCUS_graph,
                                             &which_hem);
                                                                        


                       cout<<"which hem : "<<which_hem<<endl;

                       if(status == OK){

                 print( "********************************************************* \n");
                 print( "       GIVE AN INITIAL LABELLING BASED TRUE SP_AMss         \n");
                 print( " RULE : if ( max_proba( ARC )  >= thresh_proba &&         \n");
                 print( "             mean_depth(ARC )  >= thresh_depth ) )        \n");
                 print( "                 set_color_and_name(ARC, name_max_proba)  \n");
                 print( "        else                                              \n");
                 print( "         no change : name : UNKNOWN, color : default color\n");
                 print( "********************************************************* \n");

                 print( "WARNING : THIS WILL CHANGE THE WHOLE LABELLING OF THE GRAPH \n");
                 print( "Continue ? (answer by yes or no) \n");
                 if(input_string( stdin, &answer, ' ' ) == OK ){

                    if(strcmp(answer,"yes") ==0){
                             status = OK;
                     }
                           else
                              status = ERROR;
                        }

                   if(status ==OK){

                        print( "Enter proba_threshold  and depth_threshold:");
                        if( input_real( stdin, &thresh_proba ) == OK && 
                            input_real( stdin, &thresh_depth ) == OK ){

                    cout<<"echo : proba: "<<thresh_proba<<" ; depth: "<<thresh_depth<<endl;
                          
                           if(thresh_proba < 0.0 || thresh_proba > 1.0){
                              print(" input failed \n");
                              status = ERROR;
                           }
                           else{
                                                  
                          SULCUS_graph.init_name_SCPM( (float)thresh_proba,
                                                       (float)thresh_depth, 
                                                              which_hem);
                           
                          status =match_name_with_color(model, SULCUS_graph, which_hem );
                          graphics_models_have_changed( display );
                          if(status == ERROR){
                             cout<<"ERROR: in match_name_with_color "<<endl;
                             exit(0);
                          }
                          else{//no errors found in previous steps
                          
                          cout<<" initial guess  computed "<<endl;
                           
                          status = OK;
                           }//end else

                           }//input thresholds OK
                        }//input given

                        else{
                           print(" input failed \n");
                           status = ERROR;
                        }//end else

                     }//if status == OK
                     else
                        print(" aborted ! \n");

                        (void) input_newline( stdin );

                       }

                       
                       break;
       
          default    : cout<<"ERROR: please select the model of the left or the right hemisphere";
                       cout<<endl;

                       status = ERROR;
                       break;
 
        }//end switch
           
   }
  else{
       cout<<"sulci_menu5 : no curent object !"<<endl;
       status = ERROR;
    }














if( current_object_is_top_level(display) )
    {
        current_object = display->models[THREED_MODEL];
        
       //  cout<<"top level"<<endl; /*what does it mean ? */
    }

 else
    {
      //  cout<<"not top level"<<endl;
       
       
    }
 


//delete_string(answer);

return status;

}

/* ARGSUSED */

  DEF_MENU_UPDATE(sulci_menu5 )
{
    return( GRAPH_READ );   /*--- not always active */
}




/************************************************************


     BUTTON E :  Find Sides Branches
     
     test to find the side brances

     The rule used is :

     for each ARC
            find for each ending  arc of this 
*                                 HEMISPHERE (which_hem) if this arc
*                                 is a side branche 
*                                 based on this rule:
*                                 if the arc is an ending arc
*                                    AND
*                                 the depth of this arc is inferior to 
*                                 thresh_depth
*                                    AND 
*                                  the length of this arc is inferior to
*                                   a length_thresh
*                                    AND the scalar product of
*                                    its normal vector with
*                                    normal vector of connecting
*                                    surfaces never have a 
*                                    value superior scalar_thresh
*                                    THEN
*                                     assign to this ARC the name 
*                                     "side branche"
*                                     ELSE
*                                      assign "unknown" name 
*                                      and default color 
*                                      (COLOR_SURF_TERM)
         
*************************************************************/




/* ARGSUSED */

  DEF_MENU_FUNCTION( sulci_menu9 )
{

 object_struct   *current_object, **current_object_list;
 int             number_obj ;
 int             nb_arc_left =0, nb_arc_right=0;
 int             hem_found ;
 Real           thresh_length, thresh_depth, thresh_scalar;
 model_struct    *model;
 Object_types    object_type ;
 Status          status;
 hemisphere      which_hem;
 STRING          answer;



if( get_current_object( display, &current_object) )
  {
   object_type = get_object_type(current_object);

       
 switch(object_type){
          
  case MODEL : model = get_model_ptr(current_object);
               number_obj = model->n_objects;
               current_object_list = model->objects;

                       /*-- which hemisphere --*/
                        SULCUS_graph.valsGet(NULL,
                                             NULL,
                                             NULL,
                                             &nb_arc_left,  NULL,
                                             NULL,NULL,
                                             &nb_arc_right, NULL);
                   hem_found = NO;

                        if(number_obj == nb_arc_left ){
                           
                            which_hem = left ;
                            hem_found = YES;
                            status = OK; 
                         }

                        if(number_obj == nb_arc_right){
                                                            
                             which_hem = right; 
                             hem_found = YES;
                             status = OK; 
                          }



         if(hem_found == NO){                    
         cout<<"hemisphere not found"<<endl;
         cout<<"ERROR: please select the model of the left or the right hemisphere";
         cout<<endl;
         status = ERROR;
          }
                                                     


        cout<<"which hem : "<<which_hem<<endl;


       if(status ==OK){ //hemisphere found

                 print( "********************************************************* \n");
                 print( " INFO : FIND AUTOMATICALLY THE SIDES BRANCHES        \n");
                 print( "********************************************************* \n");

                 print( "WARNING : THIS WILL CHANGE THE WHOLE LABELLING OF THE GRAPH \n");

                 print( "Continue ? (answer by yes or no) \n");

                 if(input_string( stdin, &answer, ' ' ) == OK ){

                    if(strcmp(answer,"yes") ==0){
                             status = OK;
                     }
                           else
                              status = ERROR;

                 }//if answer given



            if(status == OK ){


            print( "Enter depth_threshold  , length_threshold and scalar_threshold:");

                        if( input_real( stdin, &thresh_depth)   == OK && 
                            input_real( stdin, &thresh_length ) == OK && 
                            input_real( stdin, &thresh_scalar ) == OK  ){

cout<<"echo : depth: "<<thresh_depth<<" ; length: "<<thresh_length;
cout<<" ; scalar: "<<thresh_scalar<<endl;
                          
                          
                                                  
                          SULCUS_graph.find_side_branche(thresh_depth, 
                                     thresh_length,
                                     thresh_scalar,
                                      which_hem);
                           
                          status =match_name_with_color(model, SULCUS_graph, which_hem );
                          graphics_models_have_changed( display );
                          if(status == ERROR){
                             cout<<"ERROR: in match_name_with_color "<<endl;
                             exit(0);
                          }
                          else{//no errors found in previous steps
                          
                          cout<<" initial guess  computed "<<endl;
                           
                          status = OK;
                           }//end else

                           }//input thresholds OK
                        
                        else{
                           print(" input failed \n");
                           status = ERROR;
                        }//end else

                     }//if status == OK
                     else {
                        print(" aborted ! \n");

                        (void) input_newline( stdin );
                     }

                   break;

                      }//end if hemisphere found
       
   default    : cout<<"ERROR: please select the model of the left or the right hemisphere";
                cout<<endl;
                status = ERROR;
               break;
 

 








  }//end switch object type


 }//if get current object 
else{
 cout<<"sulci_menu9 : no curent object !"<<endl;
 status = ERROR;
    }






if( current_object_is_top_level(display) )
    {
        current_object = display->models[THREED_MODEL];
        
       //  cout<<"top level"<<endl; /*what does it mean ? */
    }
   
 else
    {
      //  cout<<"not top level"<<endl;
       
       
    }
 


//delete_string(answer);

return status;

}

/* ARGSUSED */

  DEF_MENU_UPDATE(sulci_menu9 )
{
    return( GRAPH_READ );   /*--- not always active */
}





/**********************************************************
   
    
     Save the '.sulgraph' file



**********************************************************/


  DEF_MENU_FUNCTION( sulci_menu6 )
{



   char      file_graph[200];

   int       continu;
    int      ret ;
    char     *lastpoint;
             lastpoint = NULL;
    
    Status     status;

    extern char SULDATAEXTENSION[10];


    print( "Enter the new graph filename : " );
    fscanf(stdin,"%s", file_graph);
   

   // status = input_string( stdin, &file_graph, ' ' );

   // (void) input_newline( stdin );

   
    lastpoint=strrchr(file_graph,'.');

      if(lastpoint == NULL){
         //cout<<"please enter filename with extension "<<endl<<flush;
         strcat(file_graph,SULDATAEXTENSION);
         fprintf(stdout,"after strcat file_graph = %s \n",file_graph);
         status = OK;
      }
      else{
          ret = strcmp(lastpoint,SULDATAEXTENSION);
          if(ret == 0)
             status = OK;
          else{
             cout<<"bad extension for file name "<<endl<<flush;
             status = ERROR;
          }
      }
    

    if( status == OK ){

       continu=control_write(file_graph);

       if(continu == YES){
        SULCUS_graph.write_bin_graph(file_graph);
     }
       else{
          cout<<"NO WRITING " <<endl<<flush;
       }
        

     }
    else
    {

     print("ERROR writing the graph file ... \n");

     }
        

    //delete_string(file_graph);
    //print("after delete_string ...status = %d\n",status);
  
    return( status );
      


}

  DEF_MENU_UPDATE(sulci_menu6 )
{
    return( GRAPH_READ );   /*--- not always active */
}


/**********************************************************
   
    
     Display the anatomical list



**********************************************************/


  DEF_MENU_FUNCTION( sulci_menu8 )
{

  
    int      indice ;
    Status     status;
                 status = OK;

cout<<endl;
cout<<"TABLE: (note: add 100 for Right Hemisphere)"<<endl;


for(indice =0;  indice <  NUMBER_SPM_VOL; indice++){
if( (indice >= 0 && indice <= 20) 
   || indice == 50 || indice == 51 || indice == 52 || indice == 53 || indice == 54 ||
   indice == 60 || indice == 70 || indice == 71 || indice == 72 || indice == 73 || indice == 99 
   || indice == 100)
      cout<<GLOBAL_tab_of_name[indice]<<":"<<indice<<endl<<flush;
   }
      
    return( status );
      
}

  DEF_MENU_UPDATE(sulci_menu8 )
{
    return( GRAPH_READ );   /*--- not always active */
}




/************************************************* 

          BUTTON B choose invisible

**************************************************/


/* ARGSUSED */

  DEF_MENU_FUNCTION(choose_invisible )
{
  object_struct  	  *current_object,**lst_obj; 
  int  		 	  nb_obj;
  int			  obj_index;
  seg_name		  name_arc,name;
  model_struct         	  *model;
  hemisphere		  which_hem;
  Status		  status;
  
  if( get_current_object( display, &current_object) )
  {  
     switch(get_object_type(current_object))
     {
     case MODEL :
          
	  model    = get_model_ptr(current_object);
          if (find_model_hem(model, SULCUS_graph, &which_hem) == ERROR) break;
	  /*printf("enter a name of arc : ");
	  scanf("%i",&name_arc);*/
	  name_arc = unknown_name;
  	  cout<<"name  :"<<GLOBAL_tab_of_name[name_arc]<<endl;
	  	
  	  nb_obj = model->n_objects;
	  lst_obj = model->objects;
	    
          for(obj_index=0; obj_index < nb_obj; obj_index++)
	  {
	     SULCUS_graph.get_arc_name(obj_index, which_hem, &name);
	     if (name == name_arc && UNKNOWN_visible ==1 ){
	       set_object_visibility(lst_obj[obj_index] , FALSE );
             }
             else{
                if (name == name_arc && UNKNOWN_visible ==0 ){
                 set_object_visibility(lst_obj[obj_index] , TRUE );
              }//end if
              }//end else

	  }//end for


           //Toggle visible-invisible

            if(UNKNOWN_visible == 0 ){
                  UNKNOWN_visible =1;
               }
            else{
                if(UNKNOWN_visible == 1)
                  UNKNOWN_visible =0;
             }
  
          graphics_models_have_changed( display );
          break;
	  
     default    :  cout<<"ERROR: please select the model of the left or the right hemisphere"<<endl;   
     }//end switch
   }//endif
	
   return( OK );

}


/* ARGSUSED */

 DEF_MENU_UPDATE(choose_invisible )
{
    return( GRAPH_READ );
    //return( current_object_exists(display) );
   
}



/************************************************* 

          BUTTON C  small invisible

**************************************************/


/* ARGSUSED */

  DEF_MENU_FUNCTION(smalls_invisible )
{
  object_struct  	  *current_object,**lst_obj; 
  int  		 	  nb_obj;
  int			  obj_index, nb_arc_left, nb_arc_right;
  model_struct         	  *model;
  Status		  status;
  Real                    length_thresh;
  float                   real_length, real_max_depth, real_mean_depth ;
  ARCDATA	          *ARC_left, *ARC_right, *ARC;
  hemisphere              which_hem;

  if( get_current_object( display, &current_object) )
  {  
     switch(get_object_type(current_object))
     {
     case MODEL :
        print( "Enter length threshold : " );
        input_real( stdin, &length_thresh); 
          

	  model    = get_model_ptr(current_object);
          nb_obj  = model->n_objects;
	  lst_obj = model->objects;
          
          status   = find_model_hem(model, SULCUS_graph, &which_hem);

          SULCUS_graph.valsGet(NULL,
  		       NULL, NULL,
		       &nb_arc_left    , &ARC_left,
		       NULL, NULL,
		       &nb_arc_right   , &ARC_right);
	  	
  	  
	  if( which_hem == left ){
             ARC = ARC_left;
          }
          else
             ARC = ARC_right;

        for(obj_index=0; obj_index < nb_obj; obj_index++)
	  {
             ARC[obj_index].numericalGet( &real_length,
                                          &real_max_depth,
                                          &real_mean_depth);

             if(real_length <= (float) length_thresh)
                 set_object_visibility(lst_obj[obj_index] , FALSE );
          }//end for

        
  
          graphics_models_have_changed( display );
          break;
	  
     default    :  cout<<"ERROR: please select the model of the left or the right hemisphere"<<endl;   
     }//end switch
   }//endif
	
   return( OK );

}


/* ARGSUSED */

 DEF_MENU_UPDATE(smalls_invisible )
{
    return( GRAPH_READ );
   
   
}




/***************************************************************

	comparison between 2 labelling graphs(auto & manually)
	
	BUTTON R : Compare
	
*****************************************************************/

  DEF_MENU_FUNCTION( sulci_menu10)
{
  int             obj_index,nb_arc_max;
  object_struct   *current_object;
  //Object_types    object_type ;
  model_struct    *model;
  hemisphere      which_hem;
  int 		  nb_arc_left,nb_arc_right,nb_arc_left_etiq,nb_arc_right_etiq;
  int		  nb_vertex_left,nb_vertex_right,nb_vertex_left_etiq,nb_vertex_right_etiq;
  Status          status, statusbis;
  seg_name        name_arc, name_arc_etiq;
  STRING          file_graph;
  SULGRAPHDATA    SULCUS_graph_etiq;
  ARCDATA	  *ARC_to_etiq_left, *ARC_to_etiq_right;
  VERTEXDATA	  *VERTEX_left, *VERTEX_right;
  
  printf("graph comparison \n");
  printf("");
  //test of the hemisphere
  
  if( get_current_object( display, &current_object) )
    {
       
	switch(get_object_type(current_object)){
          
                   case MODEL :{
  
  //what are the differents caracteristics of the graph.
  model    = get_model_ptr(current_object);
  status = find_model_hem(model, SULCUS_graph, &which_hem);
  
  
     
  SULCUS_graph.valsGet(NULL,
  		       &nb_vertex_left , &VERTEX_left,
		       &nb_arc_left    , &ARC_to_etiq_left,
		       &nb_vertex_right, &VERTEX_right,
		       &nb_arc_right   , &ARC_to_etiq_right);
	
   
   printf("current graph : nb of node per hemisphere and arc \n");
   printf("left  : %i %i\nright : %i %i\n",nb_arc_left, nb_vertex_left, nb_arc_right, nb_vertex_right);
      
   if( Read_New_Graph( &file_graph ) == OK ) status = SULCUS_graph_etiq.read_bin_graph(file_graph);
   else printf("can not read graph\n");
   
   delete_string( file_graph);
   if(status == OK){
   SULCUS_graph_etiq.valsGet(NULL,
  		             &nb_vertex_left_etiq ,NULL,
		             &nb_arc_left_etiq    ,NULL,
		             &nb_vertex_right_etiq,NULL,
		             &nb_arc_right_etiq   ,NULL);
   
   printf("load graph : nb of node per hemisphere\n");
   printf("left  : %i %i\nright : %i %i\n",nb_arc_left_etiq, nb_vertex_left_etiq, nb_arc_right_etiq, nb_vertex_right_etiq);

   status = Is_Different(nb_arc_left, nb_arc_right, nb_arc_left_etiq, nb_arc_right_etiq,
   			 nb_vertex_left,nb_vertex_right,nb_vertex_left_etiq,nb_vertex_right_etiq);
   }
   switch (status) 
   {
     case ERROR : printf("ERROR:: the graphs are from different subject\n");
                  break;
	  
     case OK    : printf("OK::  same subject...\n");
     		  //OK let's work
		  
		  if (which_hem == left) nb_arc_max = nb_arc_left;
		  else nb_arc_max = nb_arc_right;
		  
		  for(obj_index = 0 ; obj_index<nb_arc_max ; obj_index++)
   		  {
      		     status = SULCUS_graph.get_arc_name(obj_index, which_hem, &name_arc);
      		     statusbis = SULCUS_graph_etiq.get_arc_name(obj_index, which_hem, &name_arc_etiq);
		     
		     
		     if ((status == OK) && (statusbis == OK))
		        if (name_arc != name_arc_etiq){
		        cout<<"number:"<<obj_index<<"  name etiq: "<<GLOBAL_tab_of_name[name_arc_etiq]<<"  name initial: "<<GLOBAL_tab_of_name[name_arc]<<endl;
			switch (which_hem){
			
			  case (left) : cout<<"                    "<<GLOBAL_tab_of_name[name_arc]<<" changed to L___ERROR"<<endl;
			  		ARC_to_etiq_left[obj_index].autonameSet(L___ERROR);
			  		break;
			  case (right): cout<<"                    "<<GLOBAL_tab_of_name[name_arc]<<" changed to R___ERROR"<<endl;
			  		ARC_to_etiq_right[obj_index].autonameSet(R___ERROR);
			}
			}	
   		  }
		  
		  
		   SULCUS_graph.valsSet(NULL,
		  	nb_vertex_left , VERTEX_left,
		        nb_arc_left    , ARC_to_etiq_left,
		        nb_vertex_right, VERTEX_right,
		        nb_arc_right   , ARC_to_etiq_right);
			
	   	  cout<<"updating colour..."<<flush;
		  status = match_name_with_color(model, SULCUS_graph, which_hem );
		  graphics_models_have_changed( display );
		  cout<<" ok"<<endl;
     		  
                  break;//ok case(OK)
	     
     default : printf("default\n");
   }//of switch(status)
   }//of case(MODEL)
   break;
   
   			default    : cout<<"ERROR: please select the model of the left or the right hemisphere";
                       		     cout<<endl;
                       		     status = ERROR;
 
   }//end switch of MODEL
   }//endif
  return(status);

}

  DEF_MENU_UPDATE(sulci_menu10 )
{
    return( GRAPH_READ && INIT_GUESS );   /*--- not always active ---*/
}

static Status Read_New_Graph(STRING *file_graph)
{    
   Status   status;
   int      ret ;
   char     *lastpoint;
   extern char SULDATAEXTENSION[10];
             
   lastpoint = NULL;
   print( "Enter a labelling graph filename: " );

   status = input_string( stdin, file_graph, ' ' );

   (void) input_newline( stdin );

   lastpoint=strrchr(*(file_graph),'.');
   if(lastpoint == NULL)
   {
         cout<<"please enter filename with extension "<<endl<<flush;
         status = ERROR;
   }
   else
   {
          ret = strcmp(lastpoint,SULDATAEXTENSION);
          if(ret == 0) status = OK;  
          else
	  {
             cout<<"bad extension for file name "<<endl<<flush;
             status = ERROR;
          }
    }
   return(status);
}


static Status Is_Different(int nb_arc_left, int nb_arc_right, 
			   int nb_arc_left_etiq, int nb_arc_right_etiq,
			   int nb_vertex_left, int nb_vertex_right,
			   int nb_vertex_left_etiq, int nb_vertex_right_etiq)
{
   Status status;
   
   if ((nb_arc_left == nb_arc_left_etiq) && (nb_arc_right == nb_arc_right_etiq) &&
   	(nb_vertex_left == nb_vertex_left_etiq) && (nb_vertex_right == nb_vertex_right_etiq))
   	status = OK;
   else status = ERROR;
   
   return (status);
}















