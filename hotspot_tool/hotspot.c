/* 
 * This is a trace-level thermal simulator. It reads power values 
 * from an input trace file and outputs the corresponding instantaneous 
 * temperature values to an output trace file. It also outputs the steady 
 * state temperature values to stdout.
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "flp.h"
#include "package.h"
#include "temperature.h"
#include "temperature_block.h"
#include "temperature_grid.h"
#include "util.h"
#include "hotspot.h"

/* HotSpot thermal model is offered in two flavours - the block
 * version and the grid version. The block model models temperature
 * per functional block of the floorplan while the grid model
 * chops the chip up into a matrix of grid cells and models the 
 * temperature of each cell. It is also capable of modeling a
 * 3-d chip with multiple floorplans stacked on top of each
 * other. The choice of which model to choose is done through
 * a command line or configuration file parameter model_type. 
 * "-model_type block" chooses the block model while "-model_type grid"
 * chooses the grid model. 
 */

/* Guidelines for choosing the block or the grid model	*/

/**************************************************************************/
/* HotSpot contains two methods for solving temperatures:                 */
/* 	1) Block Model -- the same as HotSpot 2.0	              						  */
/*	2) Grid Model -- the die is divided into regular grid cells       	  */
/**************************************************************************/
/* How the grid model works: 											                        */
/* 	The grid model first reads in floorplan and maps block-based power	  */
/* to each grid cell, then solves the temperatures for all the grid cells,*/
/* finally, converts the resulting grid temperatures back to block-based  */
/* temperatures.                            														  */
/**************************************************************************/
/* The grid model is useful when 				                    						  */
/* 	1) More detailed temperature distribution inside a functional unit    */
/*     is desired.														                            */
/*  2) Too many functional units are included in the floorplan, resulting */
/*		 in extremely long computation time if using the Block Model        */
/*	3) If temperature information is desired for many tiny units,		      */ 
/* 		 such as individual register file entry.						                */
/**************************************************************************/
/*	Comparisons between Grid Model and Block Model:						            */
/*		In general, the grid model is more accurate, because it can deal    */
/*	with various floorplans and it provides temperature gradient across	  */
/*	each functional unit. The block model models essentially the center	  */
/*	temperature of each functional unit. But the block model is typically */
/*	faster because there are less nodes to solve.						              */
/*		Therefore, unless it is the case where the grid model is 		        */
/*	definitely	needed, we suggest using the block model for computation  */
/*  efficiency.															                              */
/**************************************************************************/

void usage(int argc, char **argv)
{
  fprintf(stdout, "Usage: %s -f <file> -p <file> [-o <file>] [-c <file>] [-d <file>] [-l <leakage_vector>] [-v <volt_vector>] [options]\n", argv[0]);
  fprintf(stdout, "A thermal simulator that reads power trace from a file and outputs temperatures.\n");
  fprintf(stdout, "Options:(may be specified in any order, within \"[]\" means optional)\n");
  fprintf(stdout, "   -f <file>\tfloorplan input file (e.g. ev6.flp) - overridden by the\n");
  fprintf(stdout, "            \tlayer configuration file (e.g. layer.lcf) when the\n");
  fprintf(stdout, "            \tlatter is specified\n");
  fprintf(stdout, "   -p <file>\tpower trace input file (e.g. gcc.ptrace)\n");
  fprintf(stdout, "  [-o <file>]\ttransient temperature trace output file - if not provided, only\n");
  fprintf(stdout, "            \tsteady state temperatures are output to stdout\n");
  fprintf(stdout, "  [-c <file>]\tinput configuration parameters from file (e.g. hotspot.config)\n");
  fprintf(stdout, "  [-d <file>]\toutput configuration parameters to file\n");
  fprintf(stdout, "  [-l <file>]\tEnable/Disable leakage\n");
  fprintf(stdout, "  [-v <file>]\tNormalized voltage\n");
  fprintf(stdout, "  [options]\tzero or more options of the form \"-<name> <value>\",\n");
  fprintf(stdout, "           \toverride the options from config file. e.g. \"-model_type block\" selects\n");
  fprintf(stdout, "           \tthe block model while \"-model_type grid\" selects the grid model\n");
  fprintf(stdout, "  [-detailed_3D <on/off]>\tHeterogeneous R-C assignments for specified layers. Requires a .lcf file to be specified\n"); //BU_3D: added detailed_3D option
  fprintf(stdout, "  [-print_RC_model <file>]\toutput RC thermal model to file\n");
}

/* 
 * parse a table of name-value string pairs and add the configuration
 * parameters to 'config'
 */
void global_config_from_strs(global_config_t *config, str_pair *table, int size)
{
  int idx;
  // if ((idx = get_str_index(table, size, "f")) >= 0) {
  //     if(sscanf(table[idx].value, "%s", config->flp_file) != 1)
  //       fatal("invalid format for configuration  parameter flp_file\n");
  // } else {
  //     fatal("required parameter flp_file missing. check usage\n");
  // }

  if ((idx = get_str_index(table, size, "grid_layer_file")) < 0)
      fatal("required parameter grid_layer_file missing. check usage\n");

  if ((idx = get_str_index(table, size, "p")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->p_infile) != 1)
        fatal("invalid format for configuration  parameter p_infile\n");
  } else {
      fatal("required parameter p_infile missing. check usage\n");
  }
 
 if ((idx = get_str_index(table, size, "t")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->p_outfile) != 1)
        fatal("invalid format for configuration  parameter p_outfile\n");
  } else {
      fatal("required parameter p_outfile missing. check usage\n");
  }

  if ((idx = get_str_index(table, size, "o")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->t_outfile) != 1)
        fatal("invalid format for configuration  parameter t_outfile\n");
  } else {
      strcpy(config->t_outfile, NULLFILE);
  }
  if ((idx = get_str_index(table, size, "c")) >= 0) {
      // printf("idx = %u\n", idx);
      // printf("table[idx].value = %s\n", table[idx].value);
      if(sscanf(table[idx].value, "%s", config->config) != 1)
        fatal("invalid format for configuration  parameter config\n");
  } else {
      strcpy(config->config, NULLFILE);
  }
  if ((idx = get_str_index(table, size, "d")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->dump_config) != 1)
        fatal("invalid format for configuration  parameter dump_config\n");
  } else {
      strcpy(config->dump_config, NULLFILE);
  }
  if ((idx = get_str_index(table, size, "detailed_3D")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->detailed_3D) != 1)	
        fatal("invalid format for configuration  parameter lc\n");
  } else {
      strcpy(config->detailed_3D, "off");
  }
  if ((idx = get_str_index(table, size, "print_RC_model")) >= 0) {
      if(sscanf(table[idx].value, "%s", config->RC_model_outfile) != 1)
        fatal("invalid format for configuration  parameter print_RC_model\n");
  } else {
      strcpy(config->RC_model_outfile, NULLFILE);
  }

  if ((idx = get_str_index(table, size, "l")) >= 0) {
      printf("idx = %u\n", idx);
      printf("table[idx].value = %s\n", table[idx].value);
      if(sscanf(table[idx].value, "%s", leakage_vector) != 1)
        fatal("invalid format for leakage_vector\n");
   } else {
       strcpy(leakage_vector, "");
   }


  if ((idx = get_str_index(table, size, "v")) >= 0) {
      printf("idx = %u\n", idx);
      printf("table[idx].value = %s\n", table[idx].value);
      if(sscanf(table[idx].value, "%s", volt_vector) != 1)
        fatal("invalid format for volt_vector\n");
   } else {
       strcpy(volt_vector, "");
   }


}

/* 
 * convert config into a table of name-value pairs. returns the no.
 * of parameters converted
 */
int global_config_to_strs(global_config_t *config, str_pair *table, int max_entries)
{
  if (max_entries < 9)
    fatal("not enough entries in table\n");

  sprintf(table[0].name, "f");
  sprintf(table[1].name, "p");
  sprintf(table[2].name, "t");
  sprintf(table[3].name, "o");
  sprintf(table[4].name, "c"); 
  sprintf(table[5].name, "d");
  sprintf(table[6].name, "detailed_3D");
  sprintf(table[7].name, "print_RC_model");
  sprintf(table[8].name, "l");
  sprintf(table[0].value, "%s", config->flp_file);
  sprintf(table[1].value, "%s", config->p_infile);
  sprintf(table[2].value, "%s", config->p_outfile);
  sprintf(table[3].value, "%s", config->t_outfile);
  sprintf(table[4].value, "%s", config->config);
  sprintf(table[5].value, "%s", config->dump_config);
  sprintf(table[6].value, "%s", config->detailed_3D);
  sprintf(table[7].value, "%s", config->RC_model_outfile);
  sprintf(table[8].value, "%s", leakage_vector);
  sprintf(table[9].value, "%s", volt_vector);

  return 8;
}

/* 
 * read a single line of trace file containing names
 * of functional blocks
 */
int read_names(FILE *fp, char **names)
{
  char line[LINE_SIZE], temp[LINE_SIZE], *src;
  int i;

  /* skip empty lines	*/
  do {
      /* read the entire line	*/
      fgets(line, LINE_SIZE, fp);
      if (feof(fp))
        fatal("not enough names in trace file\n");
      strcpy(temp, line);
      src = strtok(temp, " \r\t\n");
  } while (!src);

  /* new line not read yet	*/	
  if(line[strlen(line)-1] != '\n')
    fatal("line too long\n");

  /* chop the names from the line read	*/
  for(i=0,src=line; *src && i < MAX_UNITS; i++) {
      if(!sscanf(src, "%s", names[i]))
        fatal("invalid format of names\n");
      src += strlen(names[i]);
      while (isspace((int)*src))
        src++;
  }
  if(*src && i == MAX_UNITS)
    fatal("no. of units exceeded limit\n");

  return i;
}

/* read a single line of power trace numbers	*/
int read_vals(FILE *fp, double *vals)
{
  char line[LINE_SIZE], temp[LINE_SIZE], *src;
  int i;

  /* skip empty lines	*/
  do {
      /* read the entire line	*/
      fgets(line, LINE_SIZE, fp);
      if (feof(fp))
        return 0;
      strcpy(temp, line);
      src = strtok(temp, " \r\t\n");
  } while (!src);

  /* new line not read yet	*/	
  if(line[strlen(line)-1] != '\n')
    fatal("line too long\n");

  /* chop the power values from the line read	*/
  for(i=0,src=line; *src && i < MAX_UNITS; i++) {
      if(!sscanf(src, "%s", temp) || !sscanf(src, "%lf", &vals[i]))
        fatal("invalid format of values\n");
      src += strlen(temp);
      while (isspace((int)*src))
        src++;
  }
  if(*src && i == MAX_UNITS)
    fatal("no. of entries exceeded limit\n");

  return i;
}

/* write a single line of functional unit names	*/
void write_names(FILE *fp, char **names, int size)
{
  int i;
  for(i=0; i < size-1; i++)
    fprintf(fp, "%s\t", names[i]);
  fprintf(fp, "%s\n", names[i]);
}

/* write a single line of temperature trace(in degree C)	*/
void write_temp_vals(FILE *fp, double *vals, int size)
{
  int i;
  for(i=0; i < size-1; i++)
    fprintf(fp, "%.2f\t", vals[i]-273.15);
  fprintf(fp, "%.2f\n", vals[i]-273.15);
}

/* write a single line of power trace(in degree W)	*/
void write_pow_vals(FILE *fp, double *vals, int size)
{
  int i;
  for(i=0; i < size-1; i++)
    fprintf(fp, "%.5f\t", vals[i]);
  fprintf(fp, "%.5f\n", vals[i]);
}

char **alloc_names(int nr, int nc)
{
  int i;
  char **m;

  m = (char **) calloc (nr, sizeof(char *));
  assert(m != NULL);
  m[0] = (char *) calloc (nr * nc, sizeof(char));
  assert(m[0] != NULL);

  for (i = 1; i < nr; i++)
    m[i] =  m[0] + nc * i;

  return m;
}

void free_names(char **m)
{
  free(m[0]);
  free(m);
}

/* 
 * main function - reads instantaneous power values (in W) from a trace
 * file (e.g. "gcc.ptrace") and outputs instantaneous temperature values (in C) to
 * a trace file("gcc.ttrace"). also outputs steady state temperature values
 * (including those of the internal nodes of the model) onto stdout. the
 * trace files are 2-d matrices with each column representing a functional
 * functional block and each row representing a time unit(sampling_intvl).
 * columns are tab-separated and each row is a separate line. the first
 * line contains the names of the functional blocks. the order in which
 * the columns are specified doesn't have to match that of the floorplan 
 * file.
 */
int main(int argc, char **argv)
{
  int i, j, idx, base = 0, count = 0, n = 0;
  int num, size, lines = 0, do_transient = TRUE;
  char **names;
  double *vals, *pvals;
  /* trace file pointers	*/
  FILE *pin, *pout, *tout = NULL;
  /* floorplan	*/
  flp_t *flp;
  /* hotspot temperature model	*/
  RC_model_t *model;
  /* instantaneous temperature and power values	*/
  double *temp = NULL, *power;
  double total_power = 0.0;

  /* steady state temperature and power values	*/
  double *overall_power, *steady_temp;
  /* thermal model configuration parameters	*/
  thermal_config_t thermal_config;
  /* global configuration parameters	*/
  global_config_t global_config;
  /* table to hold options and configuration */
  str_pair table[MAX_ENTRIES];

  /* variables for natural convection iterations */
  int natural = 0; 
  double avg_sink_temp = 0;
  int natural_convergence = 0;
  double r_convec_old;

  /*BU_3D: variable for heterogenous R-C model */
  int do_detailed_3D = FALSE; //BU_3D: do_detailed_3D, false by default
  int do_print_RC_model = FALSE; //Sobhan: To write matrices of the RC model into a text file, false by default
  if (!(argc >= 5 && argc % 2)) {
      usage(argc, argv);
      return 1;
  }

  size = parse_cmdline(table, MAX_ENTRIES, argc, argv);
  global_config_from_strs(&global_config, table, size);

  // printf("LOKESH leakage_vector: %s, length = %d",leakage_vector, strlen(leakage_vector));
  int length_lv = strlen(leakage_vector);	
for (i = 0; i < (length_lv/2); ++i)
    leakage[i] = 1;

for (i = 0; i < length_lv; ++i)
{

  if (leakage_vector[i] == '0')
    leakage[(int)i/2] = 0;
  if (leakage_vector[i] == '1')
    leakage[(int)i/2] = 1;
  if (leakage_vector[i] == ',')
    continue;
}


  int length_v = strlen(volt_vector);
//  printf("length_v = %d\n", length_v);  

//for (i = 0; i < (int)(length_v+1)/4; ++i)
for (i = 0; i < 4; ++i)
    volt[i] = 10;

for (i = 0; i < length_v; ++i)
{
  if (i==2)
  {
    volt[0] = 10 * (volt_vector[0] - '0') + (volt_vector[2] - '0');
    continue;
  }
  if (i>2 && volt_vector[i-3] == ',')
//    printf ("i = %d", i);
    volt[(int)i/4] = 10 * (volt_vector[i-2] - '0') + (volt_vector[i] - '0');      
}



//   printf("LOKESH leakage\n", leakage[i]);

// for (i = 0; i < 16; ++i)
// {
//   printf("%d,", leakage[i]);
// }

//   printf("LOKESH volt\n");

// for (i = 0; i < (int)(length_v+1)/4; ++i)
// {
//   printf("%d,", volt[i]);
// }

  /* no transient simulation, only steady state	*/
  if(!strcmp(global_config.t_outfile, NULLFILE))
    do_transient = FALSE;

  /* read configuration file	*/
  if (strcmp(global_config.config, NULLFILE))
    size += read_str_pairs(&table[size], MAX_ENTRIES, global_config.config);

  /* earlier entries override later ones. so, command line options 
   * have priority over config file 
   */
  size = str_pairs_remove_duplicates(table, size);

  /* BU_3D: check if heterogenous R-C modeling is on */
  if(!strcmp(global_config.detailed_3D, "on")){
      do_detailed_3D = TRUE;
  }
  else if(strcmp(global_config.detailed_3D, "off")){
      fatal("detailed_3D parameter should be either \'on\' or \'off\'\n");
  }//end->BU_3D

  /* Sobhan: check if RC model printing is on */
  if(strcmp(global_config.RC_model_outfile, NULLFILE))
      do_print_RC_model = TRUE;

  /* get defaults */
  thermal_config = default_thermal_config();
  /* modify according to command line / config file	*/
  thermal_config_add_from_strs(&thermal_config, table, size);

  /* if package model is used, run package model */
  if (((idx = get_str_index(table, size, "package_model_used")) >= 0) && !(table[idx].value==0)) {
      if (thermal_config.package_model_used) {
          avg_sink_temp = thermal_config.ambient + SMALL_FOR_CONVEC;
          natural = package_model(&thermal_config, table, size, avg_sink_temp);
          if (thermal_config.r_convec<R_CONVEC_LOW || thermal_config.r_convec>R_CONVEC_HIGH)
            printf("Warning: Heatsink convection resistance is not realistic, double-check your package settings...\n"); 
      }
  }

  /* dump configuration if specified	*/
  if (strcmp(global_config.dump_config, NULLFILE)) {
      size = global_config_to_strs(&global_config, table, MAX_ENTRIES);
      size += thermal_config_to_strs(&thermal_config, &table[size], MAX_ENTRIES-size);
      /* prefix the name of the variable with a '-'	*/
      dump_str_pairs(table, size, global_config.dump_config, "-");
  }

  /* initialization: the flp_file global configuration 
   * parameter is overridden by the layer configuration 
   * file in the grid model when the latter is specified.
   */

  // flp = read_flp(global_config.flp_file, FALSE);
  // LOKESH
  printf("flp_file= %s\n", global_config.flp_file);
  printf("do_detailed_3D= %d\n", do_detailed_3D);

  //BU_3D: added do_detailed_3D to alloc_RC_model. Detailed 3D modeling can only be used with grid-level modeling.
  /* allocate and initialize the RC model	*/
  model = alloc_RC_model(&thermal_config, flp, do_detailed_3D); 

  if (model->type == BLOCK_MODEL && do_detailed_3D) 
    fatal("Detailed 3D option can only be used with grid model\n"); //end->BU_3D
  if ((model->type == GRID_MODEL) && !model->grid->has_lcf && do_detailed_3D) 
    fatal("Detailed 3D option can only be used in 3D mode\n");

  populate_R_model(model, flp);

  if (do_transient)
    populate_C_model(model, flp);

  //Sobhan: Convert the grid model to an equivalent block model 
  if(model->type == GRID_MODEL)
  { 
    int nl = model->grid->n_layers;
    int nr = model->grid->rows;
    int nc = model->grid->cols;
    RC_model_t *temp_block = convert_grid2block(model);
    model->block = temp_block->block;
  }

  if(do_print_RC_model)
  {

    int i, n, base = 0;
    char str[STR_SIZE];
    FILE *fp;	
    fp = fopen (global_config.RC_model_outfile,"w");

    if (!fp) {
      sprintf (str,"error: %s could not be opened for writing\n", global_config.RC_model_outfile);
      fatal(str);
    }
    if(model->type == BLOCK_MODEL){
      /* number of functional units on the floorplan */
      fprintf(fp, "%d\n", model->block->n_units);
      /* number thermal nodes in the RC thermal model*/
      fprintf(fp, "%d\n", model->block->n_nodes); 
    }
    else if(model->type == GRID_MODEL){

      /* number of layers and some primary information about each layer*/
      fprintf(fp, "%d\n", model->grid->n_layers);
      for(int i=0; i<model->grid->n_layers; i++){
        fprintf(fp, "%d\n", model->grid->layers[i].flp->n_units);
        fprintf(fp, "%d\n", model->grid->layers[i].has_power);
        for(int j=0; j<model->grid->layers[i].flp->n_units; j++){
          fprintf(fp, "%s\n", model->grid->layers[i].flp->units[j].name);
          fprintf(fp, "%d\n", model->grid->layers[i].g2bmap[j].i1);
          fprintf(fp, "%d\n", model->grid->layers[i].g2bmap[j].j1);
          fprintf(fp, "%d\n", model->grid->layers[i].g2bmap[j].i2);
          fprintf(fp, "%d\n", model->grid->layers[i].g2bmap[j].j2);
        }
      }

      /* number of layers for the primary path */
      fprintf(fp, "%d\n", DEFAULT_PACK_LAYERS);

      /* number of layers for the secondary path */
      if(model->grid->config.model_secondary)
        fprintf(fp, "%d\n", SEC_PACK_LAYERS);
      else
        fprintf(fp, "%d\n", 0);

      /* number of grid rows */
      fprintf(fp, "%d\n", model->grid->rows);

      /* number of grid columns */
      fprintf(fp, "%d\n", model->grid->cols);

      /* number of functional units on the floorplan */
      fprintf(fp, "%d\n", model->grid->total_n_blocks); 

      /* number thermal nodes in the RC thermal model*/
      fprintf(fp, "%d\n", model->block->n_nodes);

      /*Number of extra thermal nodes */
      int extra_nodes = EXTRA;
      if(model->grid->config.model_secondary)
        extra_nodes += EXTRA_SEC;
      fprintf(fp, "%d\n", extra_nodes); 

      for(int i=0; i<model->block->n_nodes; i++)
        for(int j=0; j<model->block->n_nodes; j++)
          fprintf(fp, "%lf\n", model->block->b[i][j]);

      for(int i=0; i<model->block->n_nodes; i++)
          fprintf(fp, "%lf\n", model->block->inva[i]);

      for(int i=0; i<model->block->n_nodes; i++)
        for(int j=0; j<model->block->n_nodes; j++)
          fprintf(fp, "%lf\n", model->block->c[i][j]);
      
      for(int i=0; i<model->block->n_nodes; i++)
          fprintf(fp, "%lf\n", model->block->g_amb[i]);

      for(int i=0; i<model->block->n_nodes; i++)
        for(int j=0; j<model->grid->total_n_blocks+extra_nodes; j++)
          fprintf(fp, "%lf\n", model->block->g2b_power[i][j]);

      for(int i=0; i<model->block->n_nodes; i++)
        for(int j=0; j<model->grid->total_n_blocks+extra_nodes; j++)
          fprintf(fp, "%lf\n", model->block->g2b_temp[i][j]);

      fprintf(fp, "%d\n", model->grid->map_mode);
    }
    fclose(fp);	
    printf("[HotSpot] thermal model file was generated.\n");
    /*double x;
    int v;
    fp = fopen (global_config.RC_model_outfile,"r");
    fscanf(fp, "%d\n", &v);
    printf("%d != %d\n", v, model->block->n_units);
    fscanf(fp, "%d\n", &v);    
    printf("%d != %d\n", v, model->block->n_nodes);
    for(int i=0; i<model->block->n_nodes; i++)
        for(int j=0; j<model->block->n_nodes; j++)
        {
          fscanf(fp, "%lf", &x);
          printf("%lf != %lf\n", x, model->block->b[i][j]);
        }*/


  }
  //   if(!strcmp(global_config.t_outfile, NULLFILE))
  //     fatal("Printing RC model can only be done when giving an output file name\n");
    
  //   FILE *fp;
  //   fp = fopen(global_config.t_outfile, "w+");
  //   fputs("This is testing for fputs...\n", fp);
  
	// 	if (fp != NULL){

	// 		fputs((const char*)&model->block->base_n_units, fp);
  //     model->grid->layers[0].
	// 		//dumpFile.write((const char*)&rcModel.numberNodesAmbient, sizeof(int));
	// 		//dumpFile.write((const char*)&rcModel.numberThermalNodes, sizeof(int));

	// 		/*for(int unit = 0; unit < floorplan.getNumberFunctionalUnits(); unit++){
	// 			dumpFile << floorplan.units[unit].name << endl;
	// 		}

	// 		for(int row = 0; row < rcModel.Binv.rows(); row++){
	// 			for(int column = 0; column < rcModel.Binv.cols(); column++){
	// 				dumpFile.write((const char*)&rcModel.Binv(row,column), sizeof(double));
	// 			}
	// 		}

	// 		for(int row = 0; row < rcModel.Gamb.rows(); row++){
	// 			dumpFile.write((const char*)&rcModel.Gamb(row), sizeof(double));
	// 		}

	// 		for(int row = 0; row < rcModel.numberThermalNodes; row++){
	// 			dumpFile.write((const char*)&eigenValues[row], sizeof(double));
	// 		}

	// 		for(int row = 0; row < rcModel.numberThermalNodes; row++){
	// 			for(int column = 0; column < rcModel.numberThermalNodes; column++){
	// 				dumpFile.write((const char*)&eigenVectors[row][column], sizeof(double));
	// 			}
	// 		}

	// 		for(int row = 0; row < rcModel.numberThermalNodes; row++){
	// 			for(int column = 0; column < rcModel.numberThermalNodes; column++){
	// 				dumpFile.write((const char*)&eigenVectorsInv[row][column], sizeof(double));
	// 			}
	// 		}

	// 		if(dumpFile.good() == false){
	// 			cout << "Error: There was an error writing to the eigenvalues and eigenvectors dump file: \"" << fileName << "\"." << endl;
	// 			dumpFile.close();
	// 			exit(1);
	// 		}*/

	// 		fclose(fp);
	// 	}
  // }
#if VERBOSE > 2
  debug_print_model(model);
#endif

  /* allocate the temp and power arrays	*/
  /* using hotspot_vector to internally allocate any extra nodes needed	*/
  if (do_transient)
    temp = hotspot_vector(model);
  power = hotspot_vector(model);
  steady_temp = hotspot_vector(model);
  overall_power = hotspot_vector(model);

  /* set up initial instantaneous temperatures */
  if (do_transient && strcmp(model->config->init_file, NULLFILE)) {
      if (!model->config->dtm_used)	/* initial T = steady T for no DTM	*/
        read_temp(model, temp, model->config->init_file, FALSE);
      else	/* initial T = clipped steady T with DTM	*/
        read_temp(model, temp, model->config->init_file, TRUE);
  } else if (do_transient)	/* no input file - use init_temp as the common temperature	*/
    set_temp(model, temp, model->config->init_temp);

  /* n is the number of functional blocks in the block model
   * while it is the sum total of the number of functional blocks
   * of all the floorplans in the power dissipating layers of the 
   * grid model. 
   */
  if (model->type == BLOCK_MODEL)
    n = model->block->flp->n_units;
  else if (model->type == GRID_MODEL) {
      for(i=0; i < model->grid->n_layers; i++)
        if (model->grid->layers[i].has_power)
          n += model->grid->layers[i].flp->n_units;
  } else 
    fatal("unknown model type\n");

  if(!(pin = fopen(global_config.p_infile, "r")))
    fatal("unable to open power trace input file\n");
 if(!(pout = fopen(global_config.p_outfile, "w")))
    fatal("unable to open power trace output file\n");
  if(do_transient && !(tout = fopen(global_config.t_outfile, "w")))
    fatal("unable to open temperature trace file for output\n");

  /* names of functional units	*/
  names = alloc_names(MAX_UNITS, STR_SIZE);
  if(read_names(pin, names) != n)
    fatal("no. of units in floorplan and trace file differ\n");

  /* header line of temperature trace	*/
  if (do_transient)
    write_names(tout, names, n);
  write_names(pout, names, n);

  /* read the instantaneous power trace	*/
  vals = dvector(MAX_UNITS);
  pvals = dvector(MAX_UNITS);
  while ((num=read_vals(pin, vals)) != 0) {
      if(num != n)
        fatal("invalid trace file format\n");

      /* permute the power numbers according to the floorplan order	*/
      if (model->type == BLOCK_MODEL)
        for(i=0; i < n; i++)
          power[get_blk_index(flp, names[i])] = vals[i];
      else
        for(i=0, base=0, count=0; i < model->grid->n_layers; i++) {
            if(model->grid->layers[i].has_power) {
                for(j=0; j < model->grid->layers[i].flp->n_units; j++) {
                    idx = get_blk_index(model->grid->layers[i].flp, names[count+j]);
                    power[base+idx] = vals[count+j];
                }
                count += model->grid->layers[i].flp->n_units;
            }	
            base += model->grid->layers[i].flp->n_units;	
        }

      /* compute temperature	*/
      if (do_transient) {
          /* if natural convection is considered, update transient convection resistance first */
          if (natural) {
              avg_sink_temp = calc_sink_temp(model, temp);
              natural = package_model(model->config, table, size, avg_sink_temp);
              populate_R_model(model, flp);
          }
          /* for the grid model, only the first call to compute_temp
           * passes a non-null 'temp' array. if 'temp' is  NULL, 
           * compute_temp remembers it from the last non-null call. 
           * this is used to maintain the internal grid temperatures 
           * across multiple calls of compute_temp
           */
          if (model->type == BLOCK_MODEL || lines == 0)
            compute_temp(model, power, temp, model->config->sampling_intvl);
          else
            compute_temp(model, power, NULL, model->config->sampling_intvl);

          
          //fclose(pin);
          /* permute back to the trace file order	*/
          if (model->type == BLOCK_MODEL)
            for(i=0; i < n; i++)
              vals[i] = temp[get_blk_index(flp, names[i])];
          else{
            for(i=0, base=0, count=0; i < model->grid->n_layers; i++) {
                if(model->grid->layers[i].has_power) {
                    for(j=0; j < model->grid->layers[i].flp->n_units; j++) {
                        idx = get_blk_index(model->grid->layers[i].flp, names[count+j]);
                        vals[count+j] = temp[base+idx];
                        pvals[count+j] = power[base+idx];
                        //printf("(%s %d %lf)",names[count+j],count+j,pvals[count+j]); 
                    }
                    count += model->grid->layers[i].flp->n_units;	
                }	
                base += model->grid->layers[i].flp->n_units;	
            }
          }
  
          ///* output instantaneous temperature trace	*/
          write_pow_vals(pout, pvals, n);
          /* output instantaneous temperature trace	*/
          write_temp_vals(tout, vals, n);
      }		

      /* for computing average	*/
      if (model->type == BLOCK_MODEL)
        for(i=0; i < n; i++)
          overall_power[i] += power[i];
      else
        for(i=0, base=0; i < model->grid->n_layers; i++) {
            if(model->grid->layers[i].has_power)
              for(j=0; j < model->grid->layers[i].flp->n_units; j++)
                overall_power[base+j] += power[base+j];
            base += model->grid->layers[i].flp->n_units;	
        }

      lines++;
  }

  if(!lines)
    fatal("no power numbers in trace file\n");

  /* for computing average	*/
  if (model->type == BLOCK_MODEL)
    for(i=0; i < n; i++) {
        overall_power[i] /= lines;
        total_power += overall_power[i];
    }
  else
    for(i=0, base=0; i < model->grid->n_layers; i++) {
        if(model->grid->layers[i].has_power)
          for(j=0; j < model->grid->layers[i].flp->n_units; j++) {
              overall_power[base+j] /= lines;
              total_power += overall_power[base+j];
          }
        base += model->grid->layers[i].flp->n_units;	
    }

  /* natural convection r_convec iteration, for steady-state only */ 		
  natural_convergence = 0;
  if (natural) { /* natural convection is used */
      while (!natural_convergence) {
          r_convec_old = model->config->r_convec;
          /* steady state temperature	*/
          steady_state_temp(model, overall_power, steady_temp);
          avg_sink_temp = calc_sink_temp(model, steady_temp) + SMALL_FOR_CONVEC;
          natural = package_model(model->config, table, size, avg_sink_temp);
          populate_R_model(model, flp);
          if (avg_sink_temp > MAX_SINK_TEMP)
            fatal("too high power for a natural convection package -- possible thermal runaway\n");
          if (fabs(model->config->r_convec-r_convec_old)<NATURAL_CONVEC_TOL) 
            natural_convergence = 1;
      }
  }	else /* natural convection is not used, no need for iterations */
    /* steady state temperature	*/
    steady_state_temp(model, overall_power, steady_temp);

  /* print steady state results	*/
  //BU_3D: Only print steady state results to stdout when DEBUG3D flag is not set
    // printf(" HERE 1\n");
#if DEBUG3D < 1
// printf("Value=%d\n",model->config->steady_state_print_disable);
 if(model->config->steady_state_print_disable == 0) {
  fprintf(stdout, "Unit\tSteady(Kelvin)\n");
  dump_temp(model, steady_temp, "stdout");
 }
#endif //end->BU_3D

  /* dump steady state temperatures on to file if needed	*/
  if (strcmp(model->config->steady_file, NULLFILE))
    dump_temp(model, steady_temp, model->config->steady_file);

  // printf(" HERE 2\n");
  /* for the grid model, optionally dump the most recent 
   * steady state temperatures of the grid cells	
   */
  if (model->type == GRID_MODEL &&
      strcmp(model->config->grid_steady_file, NULLFILE))
    dump_steady_temp_grid(model->grid, model->config->grid_steady_file);

  // printf(" HERE 3\n");
#if VERBOSE > 2
  if (model->type == BLOCK_MODEL) {
      if (do_transient) {
          fprintf(stdout, "printing temp...\n");
          dump_dvector(temp, model->block->n_nodes);
      }
      fprintf(stdout, "printing steady_temp...\n");
      dump_dvector(steady_temp, model->block->n_nodes);
  } else {
      if (do_transient) {
          fprintf(stdout, "printing temp...\n");
          dump_dvector(temp, model->grid->total_n_blocks + EXTRA);
      }
      fprintf(stdout, "printing steady_temp...\n");
      dump_dvector(steady_temp, model->grid->total_n_blocks + EXTRA);
  }
#endif

  fprintf(stdout, "Dumping transient temperatures for init in file %s\n", model->config->all_transient_file);
  fprintf(stdout, "Unit\tSteady(Kelvin)\n");
  dump_temp(model, temp, model->config->all_transient_file);

  /* cleanup	*/
  fclose(pin);
  if (do_transient)
    fclose(tout);
  delete_RC_model(model);
  //free_flp(flp, FALSE);
  if (do_transient)
    free_dvector(temp);
  free_dvector(power);
  free_dvector(steady_temp);
  free_dvector(overall_power);
  free_names(names);
  free_dvector(vals);

  return 0;
}
