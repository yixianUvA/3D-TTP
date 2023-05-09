#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#define strcasecmp    _stricmp
#define strncasecmp   _strnicmp
#else
#include <strings.h>
#endif
#include <math.h>

#include "temperature.h"
#include "temperature_block.h"
#include "temperature_grid.h"
#include "flp.h"
#include "util.h"

/* default thermal configuration parameters	*/
thermal_config_t default_thermal_config(void)
{
	thermal_config_t config;

	/* chip specs	*/
	config.t_chip = 0.15e-3;			/* chip thickness in meters	*/
	config.k_chip = 100.0; /* chip thermal conductivity in W/(m-K) */
	config.p_chip = 1.75e6; /* chip specific heat in J/(m^3-K) */
	/* temperature threshold for DTM (Kelvin)*/
	config.thermal_threshold = 81.8 + 273.15;

	/* heat sink specs	*/ 
	config.c_convec = 140.4;			/* convection capacitance in J/K */
	config.r_convec = 0.1;				/* convection resistance in K/W	*/
	config.s_sink = 60e-3;				/* heatsink side in m	*/
	config.t_sink = 6.9e-3; 			/* heatsink thickness  in m	*/
	config.k_sink = 400.0; /* heatsink thermal conductivity in W/(m-K) */
	config.p_sink = 3.55e6; /* heatsink specific heat in J/(m^3-K) */
	

	/* heat spreader specs	*/
	config.s_spreader = 30e-3;			/* spreader side in m	*/
	config.t_spreader = 1e-3;			/* spreader thickness in m	*/
	config.k_spreader = 400.0; /* heat spreader thermal conductivity in W/(m-K) */
	config.p_spreader = 3.55e6; /* heat spreader specific heat in J/(m^3-K) */

	/* interface material specs	*/
	config.t_interface = 20e-6;			/* interface material thickness in m */
	config.k_interface = 4.0; /* interface material thermal conductivity in W/(m-K) */
	config.p_interface = 4.0e6; /* interface material specific heat in J/(m^3-K) */
	
	/* secondary heat transfer path */
	config.model_secondary = FALSE;
	config.r_convec_sec = 1.0;
	config.c_convec_sec = 140.4; //FIXME! need updated value.
	config.n_metal = 8;
	config.t_metal = 10.0e-6;
	config.t_c4 = 0.0001;
	config.s_c4 = 20.0e-6;
	config.n_c4 = 400;
	config.s_sub = 0.021;
	config.t_sub = 0.001;
	config.s_solder = 0.021;
	config.t_solder = 0.00094;
	config.s_pcb = 0.1;
	config.t_pcb = 0.002;
	
	/* others	*/
	config.ambient = 45 + 273.15;		/* in kelvin	*/
	/* initial temperatures	from file	*/
	strcpy(config.init_file, NULLFILE);	
	config.init_temp = 60 + 273.15;		/* in Kelvin	*/
	/* steady state temperatures to file	*/
	strcpy(config.steady_file, NULLFILE);
 	/* 3.33 us sampling interval = 10K cycles at 3GHz	*/
	config.sampling_intvl = 3.333e-6;
	config.base_proc_freq = 3e9;		/* base processor frequency in Hz	*/
	config.dtm_used = FALSE;			/* set accordingly	*/
	
	config.leakage_used = 0;
	config.leakage_mode = 0;
	
	config.package_model_used = 0;
	strcpy(config.package_config_file, NULLFILE);	
	
	/* set block model as default	*/
	strcpy(config.model_type, BLOCK_MODEL_STR);

	/* block model specific parameters	*/
	config.block_omit_lateral = FALSE;	/* omit lateral chip resistances?	*/

	/* grid model specific parameters	*/
	config.grid_rows = 64;				/* grid resolution - no. of rows	*/
	config.grid_cols = 64;				/* grid resolution - no. of cols	*/
	/* layer configuration from	file */
	strcpy(config.grid_layer_file, NULLFILE);
	/* output steady state grid temperatures apart from block temperatures */
	strcpy(config.grid_steady_file, NULLFILE);
       /* output transient temperatures in the init format */
        strcpy(config.all_transient_file, NULLFILE);
	/* 
	 * mapping mode between block and grid models.
	 * default: use the temperature of the center
	 * grid cell as that of the entire block
	 */
	strcpy(config.grid_map_mode, GRID_CENTER_STR);
	strcpy(config.type, "3Dmem");

        config.steady_state_print_disable = 0;
	config.detailed_3D_used = 0;	//BU_3D: by default detailed 3D modeling is disabled.	
	return config;
}

/* 
 * parse a table of name-value string pairs and add the configuration
 * parameters to 'config'
 */
void thermal_config_add_from_strs(thermal_config_t *config, str_pair *table, int size)
{
	int idx;
	if ((idx = get_str_index(table, size, "t_chip")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_chip) != 1)
			fatal("invalid format for configuration  parameter t_chip\n");
	if ((idx = get_str_index(table, size, "k_chip")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->k_chip) != 1)
			fatal("invalid format for configuration  parameter k_chip\n");
	if ((idx = get_str_index(table, size, "p_chip")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->p_chip) != 1)
			fatal("invalid format for configuration  parameter p_chip\n");
	if ((idx = get_str_index(table, size, "thermal_threshold")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->thermal_threshold) != 1)
			fatal("invalid format for configuration  parameter thermal_threshold\n");
	if ((idx = get_str_index(table, size, "c_convec")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->c_convec) != 1)
			fatal("invalid format for configuration  parameter c_convec\n");
	if ((idx = get_str_index(table, size, "r_convec")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->r_convec) != 1)
			fatal("invalid format for configuration  parameter r_convec\n");
	if ((idx = get_str_index(table, size, "s_sink")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_sink) != 1)
			fatal("invalid format for configuration  parameter s_sink\n");
	if ((idx = get_str_index(table, size, "t_sink")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_sink) != 1)
			fatal("invalid format for configuration  parameter t_sink\n");
	if ((idx = get_str_index(table, size, "k_sink")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->k_sink) != 1)
			fatal("invalid format for configuration  parameter k_sink\n");
	if ((idx = get_str_index(table, size, "p_sink")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->p_sink) != 1)
			fatal("invalid format for configuration  parameter p_sink\n");
	if ((idx = get_str_index(table, size, "s_spreader")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_spreader) != 1)
			fatal("invalid format for configuration  parameter s_spreader\n");
	if ((idx = get_str_index(table, size, "t_spreader")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_spreader) != 1)
			fatal("invalid format for configuration  parameter t_spreader\n");
	if ((idx = get_str_index(table, size, "k_spreader")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->k_spreader) != 1)
			fatal("invalid format for configuration  parameter k_spreader\n");
	if ((idx = get_str_index(table, size, "p_spreader")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->p_spreader) != 1)
			fatal("invalid format for configuration  parameter p_spreader\n");
	if ((idx = get_str_index(table, size, "t_interface")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_interface) != 1)
			fatal("invalid format for configuration  parameter t_interface\n");
	if ((idx = get_str_index(table, size, "k_interface")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->k_interface) != 1)
			fatal("invalid format for configuration  parameter k_interface\n");
	if ((idx = get_str_index(table, size, "p_interface")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->p_interface) != 1)
			fatal("invalid format for configuration  parameter p_interface\n");
	if ((idx = get_str_index(table, size, "model_secondary")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->model_secondary) != 1)
			fatal("invalid format for configuration  parameter model_secondary\n");
	if ((idx = get_str_index(table, size, "r_convec_sec")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->r_convec_sec) != 1)
			fatal("invalid format for configuration  parameter r_convec_sec\n");
	if ((idx = get_str_index(table, size, "c_convec_sec")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->c_convec_sec) != 1)
			fatal("invalid format for configuration  parameter c_convec_sec\n");
	if ((idx = get_str_index(table, size, "n_metal")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->n_metal) != 1)
			fatal("invalid format for configuration  parameter n_metal\n");
	if ((idx = get_str_index(table, size, "t_metal")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_metal) != 1)
			fatal("invalid format for configuration  parameter t_metal\n");
	if ((idx = get_str_index(table, size, "t_c4")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_c4) != 1)
			fatal("invalid format for configuration  parameter t_c4\n");
	if ((idx = get_str_index(table, size, "s_c4")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_c4) != 1)
			fatal("invalid format for configuration  parameter s_c4\n");
	if ((idx = get_str_index(table, size, "n_c4")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->n_c4) != 1)
			fatal("invalid format for configuration  parameter n_c4\n");
	if ((idx = get_str_index(table, size, "s_sub")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_sub) != 1)
			fatal("invalid format for configuration  parameter s_sub\n");	
	if ((idx = get_str_index(table, size, "t_sub")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_sub) != 1)
			fatal("invalid format for configuration  parameter t_sub\n");
	if ((idx = get_str_index(table, size, "s_solder")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_solder) != 1)
			fatal("invalid format for configuration  parameter s_solder\n");
	if ((idx = get_str_index(table, size, "t_solder")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_solder) != 1)
			fatal("invalid format for configuration  parameter t_solder\n");
	if ((idx = get_str_index(table, size, "s_pcb")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->s_pcb) != 1)
			fatal("invalid format for configuration  parameter s_pcb\n");
	if ((idx = get_str_index(table, size, "t_pcb")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->t_pcb) != 1)
			fatal("invalid format for configuration  parameter t_pcb\n");		
	if ((idx = get_str_index(table, size, "ambient")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->ambient) != 1)
			fatal("invalid format for configuration  parameter ambient\n");
	if ((idx = get_str_index(table, size, "init_file")) >= 0)
		if(sscanf(table[idx].value, "%s", config->init_file) != 1)
			fatal("invalid format for configuration  parameter init_file\n");
	if ((idx = get_str_index(table, size, "init_temp")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->init_temp) != 1)
			fatal("invalid format for configuration  parameter init_temp\n");
	if ((idx = get_str_index(table, size, "steady_file")) >= 0)
		if(sscanf(table[idx].value, "%s", config->steady_file) != 1)
			fatal("invalid format for configuration  parameter steady_file\n");
	if ((idx = get_str_index(table, size, "sampling_intvl")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->sampling_intvl) != 1)
			fatal("invalid format for configuration  parameter sampling_intvl\n");
	if ((idx = get_str_index(table, size, "base_proc_freq")) >= 0)
		if(sscanf(table[idx].value, "%lf", &config->base_proc_freq) != 1)
			fatal("invalid format for configuration  parameter base_proc_freq\n");
	if ((idx = get_str_index(table, size, "dtm_used")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->dtm_used) != 1)
			fatal("invalid format for configuration  parameter dtm_used\n");
	if ((idx = get_str_index(table, size, "model_type")) >= 0)
		if(sscanf(table[idx].value, "%s", config->model_type) != 1)
			fatal("invalid format for configuration  parameter model_type\n");
		if ((idx = get_str_index(table, size, "leakage_used")) >= 0) 
		if(sscanf(table[idx].value, "%d", &config->leakage_used) != 1)
			fatal("invalid format for configuration  parameter leakage_used\n");
	if ((idx = get_str_index(table, size, "leakage_mode")) >= 0) 
		if(sscanf(table[idx].value, "%d", &config->leakage_mode) != 1)
			fatal("invalid format for configuration  parameter leakage_mode\n");
	if ((idx = get_str_index(table, size, "package_model_used")) >= 0) 
		if(sscanf(table[idx].value, "%d", &config->package_model_used) != 1)
			fatal("invalid format for configuration  parameter package_model_used\n");
	if ((idx = get_str_index(table, size, "package_config_file")) >= 0)
		if(sscanf(table[idx].value, "%s", config->package_config_file) != 1)
			fatal("invalid format for configuration  parameter package_config_file\n");
	if ((idx = get_str_index(table, size, "block_omit_lateral")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->block_omit_lateral) != 1)
			fatal("invalid format for configuration  parameter block_omit_lateral\n");
	if ((idx = get_str_index(table, size, "grid_rows")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->grid_rows) != 1)
			fatal("invalid format for configuration  parameter grid_rows\n");
	if ((idx = get_str_index(table, size, "grid_cols")) >= 0)
		if(sscanf(table[idx].value, "%d", &config->grid_cols) != 1)
			fatal("invalid format for configuration  parameter grid_cols\n");
	if ((idx = get_str_index(table, size, "grid_layer_file")) >= 0)
		if(sscanf(table[idx].value, "%s", config->grid_layer_file) != 1)
			fatal("invalid format for configuration  parameter grid_layer_file\n");
	if ((idx = get_str_index(table, size, "grid_steady_file")) >= 0)
		if(sscanf(table[idx].value, "%s", config->grid_steady_file) != 1)
			fatal("invalid format for configuration  parameter grid_steady_file\n");
	if ((idx = get_str_index(table, size, "grid_map_mode")) >= 0)
		if(sscanf(table[idx].value, "%s", config->grid_map_mode) != 1)
			fatal("invalid format for configuration  parameter grid_map_mode\n");
        if ((idx = get_str_index(table, size, "all_transient_file")) >= 0)
                if(sscanf(table[idx].value, "%s", config->all_transient_file) != 1)
                        fatal("invalid format for configuration  parameter all_transient_file\n");
        if ((idx = get_str_index(table, size, "steady_state_print_disable")) >= 0)
                if(sscanf(table[idx].value, "%d", &config->steady_state_print_disable) != 1)
                        fatal("invalid format for configuration  parameter steady_state_print_disable\n");
        if ((idx = get_str_index(table, size, "type")) >= 0)
                if(sscanf(table[idx].value, "%s", config->type) != 1)
                        fatal("invalid format for configuration  parameter type\n");
	
	
	if ((config->t_chip <= 0) || (config->s_sink <= 0) || (config->t_sink <= 0) || 
		(config->s_spreader <= 0) || (config->t_spreader <= 0) || 
		(config->t_interface <= 0))
		fatal("chip and package dimensions should be greater than zero\n");
	if ((config->t_metal <= 0) || (config->n_metal <= 0) || (config->t_c4 <= 0) || 
		(config->s_c4 <= 0) || (config->n_c4 <= 0) || (config->s_sub <= 0) || (config->t_sub <= 0) ||
		(config->s_solder <= 0) || (config->t_solder <= 0) || (config->s_pcb <= 0) ||
		(config->t_solder <= 0) || (config->r_convec_sec <= 0) || (config->c_convec_sec <= 0))
		fatal("secondary heat tranfer layer dimensions should be greater than zero\n");
	/* leakage iteration is not supported in transient mode in this release */
	if (config->leakage_used == 1) {
		printf("Warning: transient leakage iteration is not supported in this release...\n");
		printf(" ...all transient results are without thermal-leakage loop.\n");
	}		
	if ((config->model_secondary == 1) && (!strcasecmp(config->model_type, BLOCK_MODEL_STR)))
		fatal("secondary heat tranfer path is supported only in the grid mode\n");	
	if ((config->thermal_threshold < 0) || (config->c_convec < 0) || 
		(config->r_convec < 0) || (config->ambient < 0) || 
		(config->base_proc_freq <= 0) || (config->sampling_intvl <= 0))
		fatal("invalid thermal simulation parameters\n");
	if (strcasecmp(config->model_type, BLOCK_MODEL_STR) &&
		strcasecmp(config->model_type, GRID_MODEL_STR))
		fatal("invalid model type. use 'block' or 'grid'\n");
	if(config->grid_rows <= 0 || config->grid_cols <= 0)
		fatal("grid rows and columns should both be greater than zero\n");
	if (strcasecmp(config->grid_map_mode, GRID_AVG_STR) &&
		strcasecmp(config->grid_map_mode, GRID_MIN_STR) &&
		strcasecmp(config->grid_map_mode, GRID_MAX_STR) &&
		strcasecmp(config->grid_map_mode, GRID_CENTER_STR))
		fatal("invalid mapping mode. use 'avg', 'min', 'max' or 'center'\n");
}

/* 
 * convert config into a table of name-value pairs. returns the no.
 * of parameters converted
 */
int thermal_config_to_strs(thermal_config_t *config, str_pair *table, int max_entries)
{
	if (max_entries < 51)
		fatal("not enough entries in table\n");

	sprintf(table[0].name, "t_chip");
	sprintf(table[1].name, "k_chip");
	sprintf(table[2].name, "p_chip");
	sprintf(table[3].name, "thermal_threshold");
	sprintf(table[4].name, "c_convec");
	sprintf(table[5].name, "r_convec");
	sprintf(table[6].name, "s_sink");
	sprintf(table[7].name, "t_sink");
	sprintf(table[8].name, "k_sink");
	sprintf(table[9].name, "p_sink");
	sprintf(table[10].name, "s_spreader");
	sprintf(table[11].name, "t_spreader");
	sprintf(table[12].name, "k_spreader");
	sprintf(table[13].name, "p_spreader");
	sprintf(table[14].name, "t_interface");
	sprintf(table[15].name, "k_interface");
	sprintf(table[16].name, "p_interface");
	sprintf(table[17].name, "model_secondary");
	sprintf(table[18].name, "r_convec_sec");
	sprintf(table[19].name, "c_convec_sec");
	sprintf(table[20].name, "n_metal");
	sprintf(table[21].name, "t_metal");
	sprintf(table[22].name, "t_c4");
	sprintf(table[23].name, "s_c4");
	sprintf(table[24].name, "n_c4");
	sprintf(table[25].name, "s_sub");
	sprintf(table[26].name, "t_sub");
	sprintf(table[27].name, "s_solder");
	sprintf(table[28].name, "t_solder");
	sprintf(table[29].name, "s_pcb");
	sprintf(table[30].name, "t_pcb");
	sprintf(table[31].name, "ambient");
	sprintf(table[32].name, "init_file");
	sprintf(table[33].name, "init_temp");
	sprintf(table[34].name, "steady_file");
	sprintf(table[35].name, "sampling_intvl");
	sprintf(table[36].name, "base_proc_freq");
	sprintf(table[37].name, "dtm_used");
	sprintf(table[38].name, "model_type");
	sprintf(table[39].name, "leakage_used");
	sprintf(table[40].name, "leakage_mode");
	sprintf(table[41].name, "package_model_used");
	sprintf(table[42].name, "package_config_file");
	sprintf(table[43].name, "block_omit_lateral");
	sprintf(table[44].name, "grid_rows");
	sprintf(table[45].name, "grid_cols");
	sprintf(table[46].name, "grid_layer_file");
	sprintf(table[47].name, "grid_steady_file");
	sprintf(table[48].name, "grid_map_mode");
        sprintf(table[49].name, "all_transient_file");
        sprintf(table[50].name, "steady_state_print_disable");
        sprintf(table[51].name, "type");

	sprintf(table[0].value, "%lg", config->t_chip);
	sprintf(table[1].value, "%lg", config->k_chip);
	sprintf(table[2].value, "%lg", config->p_chip);
	sprintf(table[3].value, "%lg", config->thermal_threshold);
	sprintf(table[4].value, "%lg", config->c_convec);
	sprintf(table[5].value, "%lg", config->r_convec);
	sprintf(table[6].value, "%lg", config->s_sink);
	sprintf(table[7].value, "%lg", config->t_sink);
	sprintf(table[8].value, "%lg", config->k_sink);
	sprintf(table[9].value, "%lg", config->p_sink);
	sprintf(table[10].value, "%lg", config->s_spreader);
	sprintf(table[11].value, "%lg", config->t_spreader);
	sprintf(table[12].value, "%lg", config->k_spreader);
	sprintf(table[13].value, "%lg", config->p_spreader);
	sprintf(table[14].value, "%lg", config->t_interface);
	sprintf(table[15].value, "%lg", config->k_interface);
	sprintf(table[16].value, "%lg", config->p_interface);
	sprintf(table[17].value, "%d", config->model_secondary);
	sprintf(table[18].value, "%lg", config->r_convec_sec);
	sprintf(table[19].value, "%lg", config->c_convec_sec);
	sprintf(table[20].value, "%d", config->n_metal);
	sprintf(table[21].value, "%lg", config->t_metal);
	sprintf(table[22].value, "%lg", config->t_c4);
	sprintf(table[23].value, "%lg", config->s_c4);
	sprintf(table[24].value, "%d", config->n_c4);
	sprintf(table[25].value, "%lg", config->s_sub);
	sprintf(table[26].value, "%lg", config->t_sub);
	sprintf(table[27].value, "%lg", config->s_solder);
	sprintf(table[28].value, "%lg", config->t_solder);
	sprintf(table[29].value, "%lg", config->s_pcb);
	sprintf(table[30].value, "%lg", config->t_pcb);
	sprintf(table[31].value, "%lg", config->ambient);
	sprintf(table[32].value, "%s", config->init_file);
	sprintf(table[33].value, "%lg", config->init_temp);
	sprintf(table[34].value, "%s", config->steady_file);
	sprintf(table[35].value, "%lg", config->sampling_intvl);
	sprintf(table[36].value, "%lg", config->base_proc_freq);
	sprintf(table[37].value, "%d", config->dtm_used);
	sprintf(table[38].value, "%s", config->model_type);
	sprintf(table[39].value, "%d", config->leakage_used);
	sprintf(table[40].value, "%d", config->leakage_mode);
	sprintf(table[41].value, "%d", config->package_model_used);
	sprintf(table[42].value, "%s", config->package_config_file);
	sprintf(table[43].value, "%d", config->block_omit_lateral);
	sprintf(table[44].value, "%d", config->grid_rows);
	sprintf(table[45].value, "%d", config->grid_cols);
	sprintf(table[46].value, "%s", config->grid_layer_file);
	sprintf(table[47].value, "%s", config->grid_steady_file);
	sprintf(table[48].value, "%s", config->grid_map_mode);
	sprintf(table[49].value, "%s", config->all_transient_file);
	sprintf(table[50].value, "%d", config->steady_state_print_disable);
	sprintf(table[51].value, "%s", config->type);

	return 52;
}

/* package parameter routines	*/
void populate_package_R(package_RC_t *p, thermal_config_t *config, double width, double height)
{
	double s_spreader = config->s_spreader;
	double t_spreader = config->t_spreader;
	double s_sink = config->s_sink;
	double t_sink = config->t_sink;
	double r_convec = config->r_convec;
	
	double s_sub = config->s_sub;
	double t_sub = config->t_sub;
	double s_solder = config->s_solder;
	double t_solder = config->t_solder;
	double s_pcb = config->s_pcb;
	double t_pcb = config->t_pcb;
	double r_convec_sec = config->r_convec_sec;
	
	double k_sink = config->k_sink;
	double k_spreader = config->k_spreader;
 

	/* lateral R's of spreader and sink */
	p->r_sp1_x = getr(k_spreader, (s_spreader-width)/4.0, (s_spreader+3*height)/4.0 * t_spreader);
	p->r_sp1_y = getr(k_spreader, (s_spreader-height)/4.0, (s_spreader+3*width)/4.0 * t_spreader);
	p->r_hs1_x = getr(k_sink, (s_spreader-width)/4.0, (s_spreader+3*height)/4.0 * t_sink);
	p->r_hs1_y = getr(k_sink, (s_spreader-height)/4.0, (s_spreader+3*width)/4.0 * t_sink);
	p->r_hs2_x = getr(k_sink, (s_spreader-width)/4.0, (3*s_spreader+height)/4.0 * t_sink);
	p->r_hs2_y = getr(k_sink, (s_spreader-height)/4.0, (3*s_spreader+width)/4.0 * t_sink);
	p->r_hs = getr(k_sink, (s_sink-s_spreader)/4.0, (s_sink+3*s_spreader)/4.0 * t_sink);

	/* vertical R's of spreader and sink */
	p->r_sp_per_x = getr(k_spreader, t_spreader, (s_spreader+height) * (s_spreader-width) / 4.0);
	p->r_sp_per_y = getr(k_spreader, t_spreader, (s_spreader+width) * (s_spreader-height) / 4.0);
	p->r_hs_c_per_x = getr(k_sink, t_sink, (s_spreader+height) * (s_spreader-width) / 4.0);
	p->r_hs_c_per_y = getr(k_sink, t_sink, (s_spreader+width) * (s_spreader-height) / 4.0);
	p->r_hs_per = getr(k_sink, t_sink, (s_sink*s_sink - s_spreader*s_spreader) / 4.0);
	
	/* vertical R's to ambient (divide r_convec proportional to area) */
	p->r_amb_c_per_x = r_convec * (s_sink * s_sink) / ((s_spreader+height) * (s_spreader-width) / 4.0);
	p->r_amb_c_per_y = r_convec * (s_sink * s_sink) / ((s_spreader+width) * (s_spreader-height) / 4.0);
	p->r_amb_per = r_convec * (s_sink * s_sink) / ((s_sink*s_sink - s_spreader*s_spreader) / 4.0);
	
	/* lateral R's of package substrate, solder and PCB */
	p->r_sub1_x = getr(K_SUB, (s_sub-width)/4.0, (s_sub+3*height)/4.0 * t_sub);
	p->r_sub1_y = getr(K_SUB, (s_sub-height)/4.0, (s_sub+3*width)/4.0 * t_sub);
	p->r_solder1_x = getr(K_SOLDER, (s_solder-width)/4.0, (s_solder+3*height)/4.0 * t_solder);
	p->r_solder1_y = getr(K_SOLDER, (s_solder-height)/4.0, (s_solder+3*width)/4.0 * t_solder);
	p->r_pcb1_x = getr(K_PCB, (s_solder-width)/4.0, (s_solder+3*height)/4.0 * t_pcb);
	p->r_pcb1_y = getr(K_PCB, (s_solder-height)/4.0, (s_solder+3*width)/4.0 * t_pcb);
	p->r_pcb2_x = getr(K_PCB, (s_solder-width)/4.0, (3*s_solder+height)/4.0 * t_pcb);
	p->r_pcb2_y = getr(K_PCB, (s_solder-height)/4.0, (3*s_solder+width)/4.0 * t_pcb);
	p->r_pcb = getr(K_PCB, (s_pcb-s_solder)/4.0, (s_pcb+3*s_solder)/4.0 * t_pcb);

	/* vertical R's of package substrate, solder balls and PCB */
	p->r_sub_per_x = getr(K_SUB, t_sub, (s_sub+height) * (s_sub-width) / 4.0);
	p->r_sub_per_y = getr(K_SUB, t_sub, (s_sub+width) * (s_sub-height) / 4.0);
	p->r_solder_per_x = getr(K_SOLDER, t_solder, (s_solder+height) * (s_solder-width) / 4.0);
	p->r_solder_per_y = getr(K_SOLDER, t_solder, (s_solder+width) * (s_solder-height) / 4.0);
	p->r_pcb_c_per_x = getr(K_PCB, t_pcb, (s_solder+height) * (s_solder-width) / 4.0);
	p->r_pcb_c_per_y = getr(K_PCB, t_pcb, (s_solder+width) * (s_solder-height) / 4.0);
	p->r_pcb_per = getr(K_PCB, t_pcb, (s_pcb*s_pcb - s_solder*s_solder) / 4.0);
	
	/* vertical R's to ambient at PCB (divide r_convec_sec proportional to area) */
	p->r_amb_sec_c_per_x = r_convec_sec * (s_pcb * s_pcb) / ((s_solder+height) * (s_solder-width) / 4.0);
	p->r_amb_sec_c_per_y = r_convec_sec * (s_pcb * s_pcb) / ((s_solder+width) * (s_solder-height) / 4.0);
	p->r_amb_sec_per = r_convec_sec * (s_pcb * s_pcb) / ((s_pcb*s_pcb - s_solder*s_solder) / 4.0);
}

void populate_package_C(package_RC_t *p, thermal_config_t *config, double width, double height)
{
	double s_spreader = config->s_spreader;
	double t_spreader = config->t_spreader;
	double s_sink = config->s_sink;
	double t_sink = config->t_sink;
	double c_convec = config->c_convec;
	
	double s_sub = config->s_sub;
	double t_sub = config->t_sub;
	double s_solder = config->s_solder;
	double t_solder = config->t_solder;
	double s_pcb = config->s_pcb;
	double t_pcb = config->t_pcb;
	double c_convec_sec = config->c_convec_sec;
	
	double p_sink = config->p_sink;
	double p_spreader = config->p_spreader;	

	/* vertical C's of spreader and sink */
	p->c_sp_per_x = getcap(p_spreader, t_spreader, (s_spreader+height) * (s_spreader-width) / 4.0);
	p->c_sp_per_y = getcap(p_spreader, t_spreader, (s_spreader+width) * (s_spreader-height) / 4.0);
	p->c_hs_c_per_x = getcap(p_sink, t_sink, (s_spreader+height) * (s_spreader-width) / 4.0);
	p->c_hs_c_per_y = getcap(p_sink, t_sink, (s_spreader+width) * (s_spreader-height) / 4.0);
	p->c_hs_per = getcap(p_sink, t_sink, (s_sink*s_sink - s_spreader*s_spreader) / 4.0);

	/* vertical C's to ambient (divide c_convec proportional to area) */
	p->c_amb_c_per_x = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_spreader+height) * (s_spreader-width) / 4.0);
	p->c_amb_c_per_y = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_spreader+width) * (s_spreader-height) / 4.0);
	p->c_amb_per = C_FACTOR * c_convec / (s_sink * s_sink) * ((s_sink*s_sink - s_spreader*s_spreader) / 4.0);
	
	/* vertical C's of package substrate, solder balls, and PCB */
	p->c_sub_per_x = getcap(SPEC_HEAT_SUB, t_sub, (s_sub+height) * (s_sub-width) / 4.0);
	p->c_sub_per_y = getcap(SPEC_HEAT_SUB, t_sub, (s_sub+width) * (s_sub-height) / 4.0);
	p->c_solder_per_x = getcap(SPEC_HEAT_SOLDER, t_solder, (s_solder+height) * (s_solder-width) / 4.0);
	p->c_solder_per_y = getcap(SPEC_HEAT_SOLDER, t_solder, (s_solder+width) * (s_solder-height) / 4.0);
	p->c_pcb_c_per_x = getcap(SPEC_HEAT_PCB, t_pcb, (s_solder+height) * (s_solder-width) / 4.0);
	p->c_pcb_c_per_y = getcap(SPEC_HEAT_PCB, t_pcb, (s_solder+width) * (s_solder-height) / 4.0);
	p->c_pcb_per = getcap(SPEC_HEAT_PCB, t_pcb, (s_pcb*s_pcb - s_solder*s_solder) / 4.0);

	/* vertical C's to ambient at PCB (divide c_convec_sec proportional to area) */
	p->c_amb_sec_c_per_x = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_solder+height) * (s_solder-width) / 4.0);
	p->c_amb_sec_c_per_y = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_solder+width) * (s_solder-height) / 4.0);
	p->c_amb_sec_per = C_FACTOR * c_convec_sec / (s_pcb * s_pcb) * ((s_pcb*s_pcb - s_solder*s_solder) / 4.0);
}

/* debug print	*/
void debug_print_package_RC(package_RC_t *p)
{
	fprintf(stdout, "printing package RC information...\n");
	fprintf(stdout, "r_sp1_x: %f\tr_sp1_y: %f\n", p->r_sp1_x, p->r_sp1_y);
	fprintf(stdout, "r_sp_per_x: %f\tr_sp_per_y: %f\n", p->r_sp_per_x, p->r_sp_per_y);
	fprintf(stdout, "c_sp_per_x: %f\tc_sp_per_y: %f\n", p->c_sp_per_x, p->c_sp_per_y);
	fprintf(stdout, "r_hs1_x: %f\tr_hs1_y: %f\n", p->r_hs1_x, p->r_hs1_y);
	fprintf(stdout, "r_hs2_x: %f\tr_hs2_y: %f\n", p->r_hs2_x, p->r_hs2_y);
	fprintf(stdout, "r_hs_c_per_x: %f\tr_hs_c_per_y: %f\n", p->r_hs_c_per_x, p->r_hs_c_per_y);
	fprintf(stdout, "c_hs_c_per_x: %f\tc_hs_c_per_y: %f\n", p->c_hs_c_per_x, p->c_hs_c_per_y);
	fprintf(stdout, "r_hs: %f\tr_hs_per: %f\n", p->r_hs, p->r_hs_per);
	fprintf(stdout, "c_hs_per: %f\n", p->c_hs_per);
	fprintf(stdout, "r_amb_c_per_x: %f\tr_amb_c_per_y: %f\n", p->r_amb_c_per_x, p->r_amb_c_per_y);
	fprintf(stdout, "c_amb_c_per_x: %f\tc_amb_c_per_y: %f\n", p->c_amb_c_per_x, p->c_amb_c_per_y);
	fprintf(stdout, "r_amb_per: %f\n", p->r_amb_per);
	fprintf(stdout, "c_amb_per: %f\n", p->c_amb_per);
	fprintf(stdout, "r_sub1_x: %f\tr_sub1_y: %f\n", p->r_sub1_x, p->r_sub1_y);
	fprintf(stdout, "r_sub_per_x: %f\tr_sub_per_y: %f\n", p->r_sub_per_x, p->r_sub_per_y);
	fprintf(stdout, "c_sub_per_x: %f\tc_sub_per_y: %f\n", p->c_sub_per_x, p->c_sub_per_y);
	fprintf(stdout, "r_solder1_x: %f\tr_solder1_y: %f\n", p->r_solder1_x, p->r_solder1_y);
	fprintf(stdout, "r_solder_per_x: %f\tr_solder_per_y: %f\n", p->r_solder_per_x, p->r_solder_per_y);
	fprintf(stdout, "c_solder_per_x: %f\tc_solder_per_y: %f\n", p->c_solder_per_x, p->c_solder_per_y);
	fprintf(stdout, "r_pcb1_x: %f\tr_pcb1_y: %f\n", p->r_pcb1_x, p->r_pcb1_y);
	fprintf(stdout, "r_pcb2_x: %f\tr_pcb2_y: %f\n", p->r_pcb2_x, p->r_pcb2_y);
	fprintf(stdout, "r_pcb_c_per_x: %f\tr_pcb_c_per_y: %f\n", p->r_pcb_c_per_x, p->r_pcb_c_per_y);
	fprintf(stdout, "c_pcb_c_per_x: %f\tc_pcb_c_per_y: %f\n", p->c_pcb_c_per_x, p->c_pcb_c_per_y);
	fprintf(stdout, "r_pcb: %f\tr_pcb_per: %f\n", p->r_pcb, p->r_pcb_per);
	fprintf(stdout, "c_pcb_per: %f\n", p->c_pcb_per);
	fprintf(stdout, "r_amb_sec_c_per_x: %f\tr_amb_sec_c_per_y: %f\n", p->r_amb_sec_c_per_x, p->r_amb_sec_c_per_y);
	fprintf(stdout, "c_amb_sec_c_per_x: %f\tc_amb_sec_c_per_y: %f\n", p->c_amb_sec_c_per_x, p->c_amb_sec_c_per_y);
	fprintf(stdout, "r_amb_sec_per: %f\n", p->r_amb_sec_per);
	fprintf(stdout, "c_amb_sec_per: %f\n", p->c_amb_sec_per);
}

/* 
 * wrapper routines interfacing with those of the corresponding 
 * thermal model (block or grid)
 */

/* 
 * allocate memory for the matrices. for the block model, placeholder 
 * can be an empty floorplan frame with only the names of the functional 
 * units. for the grid model, it is the default floorplan
 */
RC_model_t *alloc_RC_model(thermal_config_t *config, flp_t *placeholder, int do_detailed_3D) //BU_3D: do_detailed_3D option added.
{
	RC_model_t *model= (RC_model_t *) calloc (1, sizeof(RC_model_t));
	if (!model)
		fatal("memory allocation error\n");
	if(!(strcasecmp(config->model_type, BLOCK_MODEL_STR))) {
		model->type = BLOCK_MODEL;
		model->block = alloc_block_model(config, placeholder);
		model->config = &model->block->config;
	} else if(!(strcasecmp(config->model_type, GRID_MODEL_STR))) {
		model->type = GRID_MODEL;
		model->grid = alloc_grid_model(config, placeholder, do_detailed_3D);
		model->config = &model->grid->config;
	} else 
		fatal("unknown model type\n");
	return model;	
}

/* Sobhan: convert grid model to an equivalent bloack model */
RC_model_t *convert_grid2block(RC_model_t *model)
{
	RC_model_t *new_model= (RC_model_t *) calloc (1, sizeof(RC_model_t));
	if (!new_model)
		fatal("memory allocation error\n");
	new_model->type = BLOCK_MODEL;
	new_model->block = (block_model_t *) calloc (1, sizeof(block_model_t));

  	/* shortcuts for cell width(cw) and cell height(ch)	*/
  	double cw = model->grid->width / model->grid->cols;
  	double ch = model->grid->height / model->grid->rows;

    int n, i, j;
	/* shortcuts	*/
	
  	package_RC_t *pk = &model->grid->pack;
  	//thermal_config_t *c = &model->config;
  	layer_t *l = model->grid->layers;
  	int nl = model->grid->n_layers;
  	int nr = model->grid->rows;
  	int nc = model->grid->cols;
  	int spidx, hsidx, subidx, solderidx, pcbidx;
  	int model_secondary = model->grid->config.model_secondary;

  	spidx = nl - DEFAULT_PACK_LAYERS + LAYER_SP;
  	hsidx = nl - DEFAULT_PACK_LAYERS + LAYER_SINK;
  	if (model_secondary) {
      subidx = LAYER_SUB;
      solderidx = LAYER_SOLDER;
      pcbidx = LAYER_PCB;	
  	}

	int extra_node;	
	if(model_secondary)
		extra_node = EXTRA+EXTRA_SEC;
	else
		extra_node = EXTRA;

	new_model->block->n_units = nl*nr*nc;
	new_model->block->n_nodes = nl*nr*nc + extra_node; 

	double **b = dmatrix(nl*nr*nc+extra_node, nl*nr*nc+extra_node);
	double **g2b_power = dmatrix(nl*nr*nc+extra_node, model->grid->total_n_blocks+extra_node);
	double **g2b_temp = dmatrix(nl*nr*nc+extra_node, model->grid->total_n_blocks+extra_node);
	double *a = dvector(nl*nr*nc+extra_node);
	double *inva = dvector(nl*nr*nc+extra_node);	
	double **c = dmatrix(nl*nr*nc+extra_node, nl*nr*nc+extra_node);
	double *g_amb = dvector(nl*nr*nc+extra_node);
	double *t_vector = dvector(nl*nr*nc+extra_node);
	
	zero_dmatrix(b, nl*nr*nc+extra_node, nl*nr*nc+extra_node);
	zero_dmatrix(g2b_power, nl*nr*nc+extra_node, model->grid->total_n_blocks+extra_node);
	zero_dmatrix(g2b_temp, nl*nr*nc+extra_node, model->grid->total_n_blocks+extra_node);
	zero_dvector(a, nl*nr*nc+extra_node);
	zero_dvector(inva, nl*nr*nc+extra_node);
	zero_dvector(g_amb, nl*nr*nc+extra_node);
	zero_dvector(t_vector, nl*nr*nc+extra_node);
	new_model->block->b = b;
	new_model->block->g2b_power = g2b_power;
	new_model->block->g2b_temp = g2b_temp;
	new_model->block->a = a;
	new_model->block->inva = inva;
	new_model->block->c = c;
	new_model->block->g_amb = g_amb;
	new_model->block->t_vector = t_vector;


	
	blist_t *ptr;
	flp_t *flp;
	double *v;
	int type;
	double area = (model->grid->width * model->grid->height) / (model->grid->cols * model->grid->rows);
	int base = 0;
  	for(n=0; n < nl; n++){
      for(i=0; i < nr; i++){
        for(j=0; j < nc; j++) {
            /* for each grid cell, the power density is 
             * the average of the power densities of the 
             * blocks in it weighted by their occupancies
             */
			ptr = model->grid->layers[n].b2gmap[i][j];
			flp = model->grid->layers[n].flp;
  			for(; ptr; ptr = ptr->next)
			{
				g2b_power[n*nr*nc + i*nc + j][base + ptr->idx] = ptr->occupancy * area / (flp->units[ptr->idx].width * flp->units[ptr->idx].height);
				g2b_temp[n*nr*nc + i*nc + j][base + ptr->idx] = ptr->occupancy;
			}
		}
	  }
	  base += model->grid->layers[n].flp->n_units;
	}
	for(int i=0; i<extra_node; i++){
		g2b_power[nl*nr*nc + i][base + i] = 1;
		g2b_temp[nl*nr*nc + i][base + i] = 1;
	}

 	/* for each grid cell	*/
  	for(n=0; n < nl; n++)
	{ 
    	for(i=0; i < nr; i++)
      		for(j=0; j < nc; j++) {
				if(model->grid->config.detailed_3D_used == 1){
				//if(n > 0) // top neighbor
					b[n*nr*nc+i*nc+j][(n-1)*nr*nc+i*nc+j] = (n > 0) ? -1.0/find_res_3D(n-1, i, j, model->grid,3) : 0.0;
				//if(n < nl-1) // bottom neighbor
					b[n*nr*nc+i*nc+j][(n+1)*nr*nc+i*nc+j] = (n < nl-1) ? -1.0/find_res_3D(n, i, j, model->grid,3) : 0.0;			
				//if(i > 0)// north neighbor
					b[n*nr*nc+i*nc+j][n*nr*nc+(i-1)*nc+j] = (i > 0) ? -1.0/find_res_3D(n, i-1, j, model->grid,2) : 0.0; 
				//if(i < nr-1)// south neighbor
					b[n*nr*nc+i*nc+j][n*nr*nc+(i+1)*nc+j] = (i < nr-1) ? -1.0/find_res_3D(n, i+1, j, model->grid,2) : 0.0; 
				//if(j > 0) // west neighbor
					b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+(j-1)] = (j > 0) ? -1.0/find_res_3D(n, i, j-1, model->grid,1) : 0.0; 
				//if(j < nc-1) // east neighbor
					b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+(j+1)] = (j < nc-1) ? -1.0/find_res_3D(n, i, j+1, model->grid,1) : 0.0;
				}
				else
				{
					b[n*nr*nc+i*nc+j][(n-1)*nr*nc+i*nc+j] = (n > 0) ? -1.0/l[n-1].rz : 0.0;
					b[n*nr*nc+i*nc+j][(n+1)*nr*nc+i*nc+j] = (n < nl-1) ? -1.0/l[n].rz : 0.0;			
					b[n*nr*nc+i*nc+j][n*nr*nc+(i-1)*nc+j] = (i > 0) ? -1.0/l[n].ry : 0.0; 
					b[n*nr*nc+i*nc+j][n*nr*nc+(i+1)*nc+j] = (i < nr-1) ? -1.0/l[n].ry : 0.0; 
					b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+(j-1)] = (j > 0) ? -1.0/l[n].rx : 0.0; 
					b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+(j+1)] = (j < nc-1) ? -1.0/l[n].rx : 0.0;
				}
				
				if(n == spidx){
					if(i == 0){
						b[n*nr*nc+i*nc+j][nl*nr*nc+SP_N] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sp1_y);
						b[nl*nr*nc+SP_N][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sp1_y);
					}
					if(i == nr-1){
						b[n*nr*nc+i*nc+j][nl*nr*nc+SP_S] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sp1_y);
						b[nl*nr*nc+SP_S][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sp1_y);					
					}
					if(j == nc-1){
						b[n*nr*nc+i*nc+j][nl*nr*nc+SP_E] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sp1_x);
						b[nl*nr*nc+SP_E][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sp1_x);
					}
					if(j == 0){
						b[n*nr*nc+i*nc+j][nl*nr*nc+SP_W] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sp1_x);
						b[nl*nr*nc+SP_W][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sp1_x);
					}
				} else if (n == hsidx) {
              		/* all nodes are connected to the ambient	*/
            		b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+j] = 1.0/l[n].rz;
					g_amb[n*nr*nc+i*nc+j] = 1.0/l[n].rz;
              		/* northern boundary - edge cell has half the ry	*/
              		if (i == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SINK_C_N] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_hs1_y);
					    b[nl*nr*nc+SINK_C_N][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_hs1_y); 
					}
              		/* southern boundary - edge cell has half the ry	*/
              		if (i == nr-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SINK_C_S] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_hs1_y);
						b[nl*nr*nc+SINK_C_S][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_hs1_y); 
					}
              		/* eastern boundary	 - edge cell has half the rx	*/
              		if (j == nc-1){
					    b[n*nr*nc+i*nc+j][nl*nr*nc+SINK_C_E] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_hs1_x); 
					    b[nl*nr*nc+SINK_C_E][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_hs1_x);
					}
					/* western boundary	 - edge cell has half the rx	*/
              		if (j == 0){
					  	b[n*nr*nc+i*nc+j][nl*nr*nc+SINK_C_W] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_hs1_x);
					  	b[nl*nr*nc+SINK_C_W][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_hs1_x);
					}
          		} else if (n == pcbidx && model_secondary) {
              		/* all nodes are connected to the ambient	*/
				    b[n*nr*nc+i*nc+j][n*nr*nc+i*nc+j] = 1.0/(model->grid->config.r_convec_sec * (model->grid->config.s_pcb * model->grid->config.s_pcb) / (cw * ch));
              		
					g_amb[n*nr*nc+i*nc+j] = 1.0/(model->grid->config.r_convec_sec * (model->grid->config.s_pcb * model->grid->config.s_pcb) / (cw * ch));
					/* northern boundary - edge cell has half the ry	*/
              		if (i == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+PCB_C_N] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_pcb1_y); 
                		b[nl*nr*nc+PCB_C_N][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_pcb1_y); 
					}
              		/* southern boundary - edge cell has half the ry	*/
              		if (i == nr-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+PCB_C_S] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_pcb1_y);
						b[nl*nr*nc+PCB_C_S][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_pcb1_y); 
					} 
              		/* eastern boundary	 - edge cell has half the rx	*/
              		if (j == nc-1){
           				b[n*nr*nc+i*nc+j][nl*nr*nc+PCB_C_E] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_pcb1_x); 
           				b[nl*nr*nc+PCB_C_E][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_pcb1_x); 
					}
              		/* western boundary	 - edge cell has half the rx	*/
              		if (j == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+PCB_C_W] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_pcb1_x); 
                		b[nl*nr*nc+PCB_C_W][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_pcb1_x); 
					}
          		} else if (n == subidx && model_secondary) {
              		/* northern boundary - edge cell has half the ry	*/
              		if (i == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SUB_N] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sub1_y); 
                		b[nl*nr*nc+SUB_N][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sub1_y); 
					}
					/* southern boundary - edge cell has half the ry	*/
              		if (i == nr-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SUB_S] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sub1_y); 
                		b[nl*nr*nc+SUB_S][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_sub1_y); 
					}
					/* eastern boundary	 - edge cell has half the rx	*/
              		if (j == nc-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SUB_E] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sub1_x); 
                		b[nl*nr*nc+SUB_E][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sub1_x); 
					}
					/* western boundary	 - edge cell has half the rx	*/
              		if (j == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SUB_W] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sub1_x); 
          		        b[nl*nr*nc+SUB_W][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_sub1_x); 
					}
				} else if (n == solderidx && model_secondary) {
              		/* northern boundary - edge cell has half the ry	*/
              		if (i == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SOLDER_N] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_solder1_y); 
              		    b[nl*nr*nc+SOLDER_N][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_solder1_y); 
					}
					/* southern boundary - edge cell has half the ry	*/
              		if (i == nr-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SOLDER_S] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_solder1_y); 
                		b[nl*nr*nc+SOLDER_S][n*nr*nc+i*nc+j] = -1.0/(l[n].ry/2.0 + nc*model->grid->pack.r_solder1_y); 
					}
					/* eastern boundary	 - edge cell has half the rx	*/
              		if (j == nc-1){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SOLDER_E] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_solder1_x); 
                		b[nl*nr*nc+SOLDER_E][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_solder1_x); 
					}
					/* western boundary	 - edge cell has half the rx	*/
              		if (j == 0){
                		b[n*nr*nc+i*nc+j][nl*nr*nc+SOLDER_W] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_solder1_x); 
                		b[nl*nr*nc+SOLDER_W][n*nr*nc+i*nc+j] = -1.0/(l[n].rx/2.0 + nr*model->grid->pack.r_solder1_x); 
					}
				}
 			
			 	/* load cell's capacitance in matrix A */	    
          		if(model->grid->config.detailed_3D_used == 1)//BU_3D: use find_cap_3D is detailed_3D model is used.
            		a[n*nr*nc+i*nc+j] = find_cap_3D(n, i, j, model->grid);
          		else
            		a[n*nr*nc+i*nc+j] = l[n].c;
			}
	}
  
	/* load the matrices A, B, and G_amb for the package nodes	*/
  	int extra_idx = nl*nr*nc;
	//model->block->n_nodes = nl*nr*nc + extra_node;
  	/* sink outer north/south	*/
  	b[extra_idx+SINK_N][extra_idx+SINK_N] = 1.0/(pk->r_hs_per + pk->r_amb_per);
  	b[extra_idx+SINK_C_N][extra_idx+SINK_N] = -1.0/(pk->r_hs2_y + pk->r_hs);
  	b[extra_idx+SINK_N][extra_idx+SINK_C_N] = -1.0/(pk->r_hs2_y + pk->r_hs);

  	g_amb[extra_idx+SINK_N] = 1.0/(pk->r_hs_per + pk->r_amb_per);

	a[extra_idx + SINK_N] = pk->c_hs_per + pk->c_amb_per;

  	b[extra_idx+SINK_S][extra_idx+SINK_S] = 1.0/(pk->r_hs_per + pk->r_amb_per);
  	b[extra_idx+SINK_C_S][extra_idx+SINK_S] = -1.0/(pk->r_hs2_y + pk->r_hs);
  	b[extra_idx+SINK_S][extra_idx+SINK_C_S] = -1.0/(pk->r_hs2_y + pk->r_hs);

  	g_amb[extra_idx+SINK_S] = 1.0/(pk->r_hs_per + pk->r_amb_per);
  	
	a[extra_idx + SINK_S] = pk->c_hs_per + pk->c_amb_per;

  	/* sink outer west/east	*/
  	b[extra_idx+SINK_W][extra_idx+SINK_W] = 1.0/(pk->r_hs_per + pk->r_amb_per);
  	b[extra_idx+SINK_C_W][extra_idx+SINK_W] = -1.0/(pk->r_hs2_x + pk->r_hs);
  	b[extra_idx+SINK_W][extra_idx+SINK_C_W] = -1.0/(pk->r_hs2_x + pk->r_hs);

  	g_amb[extra_idx + SINK_W] = 1.0/(pk->r_hs_per + pk->r_amb_per);

  	a[extra_idx + SINK_W] = pk->c_hs_per + pk->c_amb_per;

  	b[extra_idx+SINK_E][extra_idx+SINK_E] = 1.0/(pk->r_hs_per + pk->r_amb_per);
  	b[extra_idx+SINK_C_E][extra_idx+SINK_E] = -1.0/(pk->r_hs2_x + pk->r_hs);
  	b[extra_idx+SINK_E][extra_idx+SINK_C_E] = -1.0/(pk->r_hs2_x + pk->r_hs);

  	g_amb[extra_idx + SINK_E] = 1.0/(pk->r_hs_per + pk->r_amb_per);

  	a[extra_idx + SINK_E] = pk->c_hs_per + pk->c_amb_per;

  	/* sink inner north/south	*/
  	/* partition r_hs1_y among all the nc grid cells. edge cell has half the ry	*/

  	for(j=0; j < nc; j++){
  		b[(hsidx)*(nr)*(nc)+ (0)*(nc) + (j)][extra_idx + SINK_C_N] = -1.0/(l[hsidx].ry / 2.0 + nc * pk->r_hs1_y);
  		b[extra_idx + SINK_C_N][(hsidx)*(nr)*(nc)+ (0)*(nc) + (j)] = -1.0/(l[hsidx].ry / 2.0 + nc * pk->r_hs1_y);
  	}
	b[extra_idx + SINK_C_N][extra_idx + SINK_C_N] = 1.0/(pk->r_hs_c_per_y + pk->r_amb_c_per_y);
  	b[extra_idx + SP_N][extra_idx + SINK_C_N] = -1.0/pk->r_sp_per_y;
  	b[extra_idx + SINK_C_N][extra_idx + SP_N] = -1.0/pk->r_sp_per_y;
  	b[extra_idx + SINK_N][extra_idx + SINK_C_N] = -1.0/(pk->r_hs2_y + pk->r_hs);
  	b[extra_idx + SINK_C_N][extra_idx + SINK_N] = -1.0/(pk->r_hs2_y + pk->r_hs);
    
	g_amb[extra_idx + SINK_C_N] = 1.0/(pk->r_hs_c_per_y + pk->r_amb_c_per_y);
  	a[extra_idx + SINK_C_N] = pk->c_hs_c_per_y + pk->c_amb_c_per_y;

  	for(j=0; j < nc; j++){
		b[(hsidx)*(nr)*(nc)+ (nr-1)*(nc) + (j)][extra_idx + SINK_C_S] = -1.0/(l[hsidx].ry / 2.0 + nc * pk->r_hs1_y);
		b[extra_idx + SINK_C_S][(hsidx)*(nr)*(nc)+ (nr-1)*(nc) + (j)] = -1.0/(l[hsidx].ry / 2.0 + nc * pk->r_hs1_y);
  	}

	b[extra_idx + SINK_C_S][extra_idx + SINK_C_S] = 1.0/(pk->r_hs_c_per_y + pk->r_amb_c_per_y);
  	b[extra_idx + SP_S][extra_idx + SINK_C_S] = -1.0/pk->r_sp_per_y;
  	b[extra_idx + SINK_C_S][extra_idx + SP_S] = -1.0/pk->r_sp_per_y;
  	b[extra_idx + SINK_S][extra_idx + SINK_C_S] = -1.0/(pk->r_hs2_y + pk->r_hs);
  	b[extra_idx + SINK_C_S][extra_idx + SINK_S] = -1.0/(pk->r_hs2_y + pk->r_hs);

  	g_amb[extra_idx + SINK_C_S] = 1.0/(pk->r_hs_c_per_y + pk->r_amb_c_per_y);
  	a[extra_idx + SINK_C_S] = pk->c_hs_c_per_y + pk->c_amb_c_per_y;

  	/* sink inner west/east	*/
  	/* partition r_hs1_x among all the nr grid cells. edge cell has half the rx	*/
  
  	for(i=0; i < nr; i++){
		b[(hsidx)*(nr)*(nc)+ (i)*(nc) + (0)][extra_idx + SINK_C_W] = -1.0/(l[hsidx].rx / 2.0 + nr * pk->r_hs1_x);
		b[extra_idx + SINK_C_W][(hsidx)*(nr)*(nc)+ (i)*(nc) + (0)] = -1.0/(l[hsidx].rx / 2.0 + nr * pk->r_hs1_x);
  	}
	b[extra_idx + SINK_C_W][extra_idx + SINK_C_W] = 1.0/(pk->r_hs_c_per_x + pk->r_amb_c_per_x);
  	b[extra_idx + SP_W][extra_idx + SINK_C_W] = -1.0/pk->r_sp_per_x;
  	b[extra_idx + SINK_C_W][extra_idx + SP_W] = -1.0/pk->r_sp_per_x ;
  	b[extra_idx + SINK_W][extra_idx + SINK_C_W] = -1.0/(pk->r_hs2_x + pk->r_hs);
  	b[extra_idx + SINK_C_W][extra_idx + SINK_W] = -1.0/(pk->r_hs2_x + pk->r_hs);

  	g_amb[extra_idx + SINK_C_W] = 1.0/(pk->r_hs_c_per_x + pk->r_amb_c_per_x);
  	a[extra_idx + SINK_C_W] = pk->c_hs_c_per_x + pk->c_amb_c_per_x;

  	for(i=0; i < nr; i++){
		b[(hsidx)*(nr)*(nc) + (i)*(nc) + (nc-1)][extra_idx + SINK_C_E] = -1.0/(l[hsidx].rx / 2.0 + nr * pk->r_hs1_x);
		b[extra_idx + SINK_C_E][(hsidx)*(nr)*(nc) + (i)*(nc) + (nc-1)] = -1.0/(l[hsidx].rx / 2.0 + nr * pk->r_hs1_x);
  	}
	b[extra_idx + SINK_C_E][extra_idx + SINK_C_E] = 1.0/(pk->r_hs_c_per_x + pk->r_amb_c_per_x);
  	b[extra_idx + SP_E][extra_idx + SINK_C_E] = -1.0/pk->r_sp_per_x;
  	b[extra_idx + SINK_C_E][extra_idx + SP_E] = -1.0/pk->r_sp_per_x ;
  	b[extra_idx + SINK_E][extra_idx + SINK_C_E] = -1.0/(pk->r_hs2_x + pk->r_hs);
  	b[extra_idx + SINK_C_E][extra_idx + SINK_E] = -1.0/(pk->r_hs2_x + pk->r_hs);

  	g_amb[extra_idx + SINK_C_E] = 1.0/(pk->r_hs_c_per_x + pk->r_amb_c_per_x);
  	a[extra_idx + SINK_C_E] = pk->c_hs_c_per_x + pk->c_amb_c_per_x;

  	/* spreader north/south	*/
   	/* partition r_sp1_y among all the nc grid cells. edge cell has half the ry	*/
  	for(j=0; j < nc; j++){
		b[(spidx)*(nr)*(nc) + (0)*(nc) + (j)][extra_idx + SP_N] = -1.0/(l[spidx].ry / 2.0 + nc * pk->r_sp1_y);
		b[extra_idx + SP_N][(spidx)*(nr)*(nc) + (0)*(nc) + (j)] = -1.0/(l[spidx].ry / 2.0 + nc * pk->r_sp1_y);
  	}
	b[extra_idx + SP_N][extra_idx + SINK_C_N] = -1.0/pk->r_sp_per_y;
	b[extra_idx + SINK_C_N][extra_idx + SP_N] = -1.0/pk->r_sp_per_y;
	a[extra_idx + SP_N] = pk->c_sp_per_y;

 	for(j=0; j < nc; j++){
		b[(spidx)*(nr)*(nc) + (nr-1)*(nc) + (j)][extra_idx + SP_S] = -1.0/(l[spidx].ry / 2.0 + nc * pk->r_sp1_y);
		b[extra_idx + SP_S][(spidx)*(nr)*(nc) + (nr-1)*(nc) + (j)] = -1.0/(l[spidx].ry / 2.0 + nc * pk->r_sp1_y);
	}
	b[extra_idx + SP_S][extra_idx + SINK_C_S] = -1.0/pk->r_sp_per_y;
	b[extra_idx + SINK_C_S][extra_idx + SP_S] = -1.0/pk->r_sp_per_y; 
  	a[extra_idx + SP_S] =  pk->c_sp_per_y;

	/* spreader west/east	*/
	/* partition r_sp1_x among all the nr grid cells. edge cell has half the rx	*/
  	for(i=0; i < nr; i++){
		b[(spidx)*(nr)*(nc) + (i)*(nc) + (0)][extra_idx + SP_W] = -1.0/(l[spidx].rx / 2.0 + nr * pk->r_sp1_x);
		b[extra_idx + SP_W][(spidx)*(nr)*(nc) + (i)*(nc) + (0)] = -1.0/(l[spidx].rx / 2.0 + nc * pk->r_sp1_x);
  	}
	b[extra_idx + SP_W][extra_idx + SINK_C_W] = -1.0/pk->r_sp_per_x;
	b[extra_idx + SINK_C_W][extra_idx + SP_W] = -1.0/pk->r_sp_per_x;
	a[extra_idx + SP_W] = pk->c_sp_per_x;

 	for(i=0; i < nr; i++){
		b[(spidx)*(nr)*(nc) + (i)*(nc) + (nc-1)][extra_idx + SP_E] = -1.0/(l[spidx].rx / 2.0 + nr * pk->r_sp1_x);
		b[extra_idx + SP_E][(spidx)*(nr)*(nc) + (i)*(nc) + (nc-1)] = -1.0/(l[spidx].rx / 2.0 + nr * pk->r_sp1_x);
	}
	b[extra_idx + SP_E][extra_idx + SINK_C_E] = -1.0/pk->r_sp_per_x;
	b[extra_idx + SINK_C_E][extra_idx + SP_E] = -1.0/pk->r_sp_per_x;

  	a[extra_idx + SP_E] =  pk->c_sp_per_x;

	if (model_secondary) {
      /* PCB outer north/south	*/
		b[extra_idx + PCB_N][extra_idx + PCB_N] = 1.0/pk->r_amb_sec_per;
		b[extra_idx + PCB_C_N][extra_idx + PCB_N] = -1.0/(pk->r_pcb2_y + pk->r_pcb);
		b[extra_idx + PCB_N][extra_idx + PCB_C_N] = -1.0/(pk->r_pcb2_y + pk->r_pcb);

		g_amb[extra_idx + PCB_N] = 1.0/pk->r_amb_sec_per;

		a[extra_idx + PCB_N] = pk->c_pcb_per + pk->c_amb_sec_per;

		b[extra_idx + PCB_S][extra_idx + PCB_S] = 1.0/pk->r_amb_sec_per;
		b[extra_idx + PCB_C_S][extra_idx + PCB_S] = -1.0/(pk->r_pcb2_y + pk->r_pcb);
		b[extra_idx + PCB_S][extra_idx + PCB_C_S] = -1.0/(pk->r_pcb2_y + pk->r_pcb);

		g_amb[extra_idx + PCB_S] = 1.0/pk->r_amb_sec_per;

		a[extra_idx + PCB_S] = pk->c_pcb_per + pk->c_amb_sec_per;

		/* PCB outer west/east	*/

		b[extra_idx + PCB_W][extra_idx + PCB_W] = 1.0/pk->r_amb_sec_per;
		b[extra_idx + PCB_C_W][extra_idx + PCB_W] = -1.0/(pk->r_pcb2_x + pk->r_pcb);
		b[extra_idx + PCB_W][extra_idx + PCB_C_W] = -1.0/(pk->r_pcb2_x + pk->r_pcb);

		g_amb[extra_idx + PCB_W] = 1.0/pk->r_amb_sec_per;

		a[extra_idx + PCB_W] = pk->c_pcb_per + pk->c_amb_sec_per;

		b[extra_idx + PCB_E][extra_idx + PCB_E] = 1.0/pk->r_amb_sec_per;
		b[extra_idx + PCB_C_E][extra_idx + PCB_E] = -1.0/(pk->r_pcb2_x + pk->r_pcb);
		b[extra_idx + PCB_E][extra_idx + PCB_C_E] = -1.0/(pk->r_pcb2_x + pk->r_pcb);

		g_amb[extra_idx + PCB_E] = 1.0/pk->r_amb_sec_per;

		a[extra_idx + PCB_E] = pk->c_pcb_per + pk->c_amb_sec_per;

		/* PCB inner north/south	*/
		/* partition r_pcb1_y among all the nc grid cells. edge cell has half the ry	*/
      
		for(j=0; j < nc; j++){
			b[(pcbidx)*(nr)*(nc) + (0)*(nc) + j][extra_idx + PCB_C_N] = -1.0/(l[pcbidx].ry / 2.0 + nc * pk->r_pcb1_y);
			b[extra_idx + PCB_C_N][(pcbidx)*(nr)*(nc) + (0)*(nc) + j] = -1.0/(l[pcbidx].ry / 2.0 + nc * pk->r_pcb1_y);
		}
		b[extra_idx + PCB_C_N][extra_idx + PCB_C_N] = 1.0/(pk->r_amb_sec_c_per_y);
		b[extra_idx + SOLDER_N][extra_idx + PCB_C_N] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + PCB_C_N][extra_idx + SOLDER_N] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + PCB_N][extra_idx + PCB_C_N] = -1.0/(pk->r_pcb2_y + pk->r_pcb);
		b[extra_idx + PCB_C_N][extra_idx + PCB_N] = -1.0/(pk->r_pcb2_y + pk->r_pcb);

		g_amb[extra_idx + PCB_C_N] = 1.0/(pk->r_amb_sec_c_per_y);

		a[extra_idx + PCB_C_N] = pk->c_pcb_c_per_y + pk->c_amb_sec_c_per_y;

		for(j=0; j < nc; j++){
			b[(pcbidx)*(nr)*(nc) + (nr-1)*(nc) + j][extra_idx + PCB_C_S] = -1.0/(l[pcbidx].ry / 2.0 + nc * pk->r_pcb1_y);
			b[extra_idx + PCB_C_S][(pcbidx)*(nr)*(nc) + (nr-1)*(nc) + j] = -1.0/(l[pcbidx].ry / 2.0 + nc * pk->r_pcb1_y);
		}
		b[extra_idx + PCB_C_S][extra_idx + PCB_C_S] = 1.0/(pk->r_amb_sec_c_per_y);
		b[extra_idx + SOLDER_S][extra_idx + PCB_C_S] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + PCB_C_S][extra_idx + SOLDER_S] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + PCB_S][extra_idx + PCB_C_S] = -1.0/(pk->r_pcb2_y + pk->r_pcb);
		b[extra_idx + PCB_C_S][extra_idx + PCB_S] = -1.0/(pk->r_pcb2_y + pk->r_pcb);

		g_amb[extra_idx + PCB_C_S] = 1.0/(pk->r_amb_sec_c_per_y);

		a[extra_idx + PCB_C_S] = pk->c_pcb_c_per_y + pk->c_amb_sec_c_per_y;

		/* PCB inner west/east	*/
		/* partition r_pcb1_x among all the nr grid cells. edge cell has half the rx	*/
      
		for(i=0; i < nr; i++){
			b[(pcbidx)*(nr)*(nc) + (i)*(nc) + 0][extra_idx + PCB_C_W] = -1.0/(l[pcbidx].rx / 2.0 + nr * pk->r_pcb1_x);
			b[extra_idx + PCB_C_W][(pcbidx)*(nr)*(nc) + (i)*(nc) + 0] = -1.0/(l[pcbidx].rx / 2.0 + nr * pk->r_pcb1_x);
		}
		b[extra_idx + PCB_C_W][extra_idx + PCB_C_W] = 1.0/(pk->r_amb_sec_c_per_x);
		b[extra_idx + SOLDER_W][extra_idx + PCB_C_W] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + PCB_C_W][extra_idx + SOLDER_W] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + PCB_W][extra_idx + PCB_C_W] = -1.0/(pk->r_pcb2_x + pk->r_pcb);
		b[extra_idx + PCB_C_W][extra_idx + PCB_W] = -1.0/(pk->r_pcb2_x + pk->r_pcb);

		g_amb[extra_idx + PCB_C_W] = 1.0/(pk->r_amb_sec_c_per_x);

		a[extra_idx + PCB_C_W] = pk->c_pcb_c_per_x + pk->c_amb_sec_c_per_x;

		for(i=0; i < nr; i++){
			b[(pcbidx)*(nr)*(nc) + (i)*(nc) + nc-1][extra_idx + PCB_C_E] = -1.0/(l[pcbidx].rx / 2.0 + nr * pk->r_pcb1_x);
			b[extra_idx + PCB_C_E][(pcbidx)*(nr)*(nc) + (i)*(nc) + nc-1] = -1.0/(l[pcbidx].rx / 2.0 + nr * pk->r_pcb1_x);
		}
		b[extra_idx + PCB_C_E][extra_idx + PCB_C_E] = 1.0/(pk->r_amb_sec_c_per_x);
		b[extra_idx + SOLDER_E][extra_idx + PCB_C_E] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + PCB_C_E][extra_idx + SOLDER_E] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + PCB_E][extra_idx + PCB_C_E] = -1.0/(pk->r_pcb2_x + pk->r_pcb);
		b[extra_idx + PCB_C_E][extra_idx + PCB_E] = -1.0/(pk->r_pcb2_x + pk->r_pcb);

		g_amb[extra_idx + PCB_C_E] = 1.0/(pk->r_amb_sec_c_per_x);

		a[extra_idx + PCB_C_E] = pk->c_pcb_c_per_x + pk->c_amb_sec_c_per_x;

		/* solder ball north/south	*/
		/* partition r_solder1_y among all the nc grid cells. edge cell has half the ry	*/
      
		for(j=0; j < nc; j++){
			b[(solderidx)*(nr)*(nc) + (0)*(nc) + j][extra_idx + SOLDER_N] = -1.0/(l[solderidx].ry / 2.0 + nc * pk->r_solder1_y);
			b[extra_idx + SOLDER_N][(solderidx)*(nr)*(nc) + (0)*(nc) + j] = -1.0/(l[solderidx].ry / 2.0 + nc * pk->r_solder1_y);
		}
		b[extra_idx + PCB_C_N][extra_idx + SOLDER_N] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + SOLDER_N][extra_idx + PCB_C_N] = -1.0/pk->r_pcb_c_per_y;

		a[extra_idx + SOLDER_N] = pk->c_solder_per_y;

		for(j=0; j < nc; j++){
			b[(solderidx)*(nr)*(nc) + (nr-1)*(nc) + j][extra_idx + SOLDER_S] = -1.0/(l[solderidx].ry / 2.0 + nc * pk->r_solder1_y);
			b[extra_idx + SOLDER_S][(solderidx)*(nr)*(nc) + (nr-1)*(nc) + j] = -1.0/(l[solderidx].ry / 2.0 + nc * pk->r_solder1_y);
		}
		b[extra_idx + PCB_C_S][extra_idx + SOLDER_S] = -1.0/pk->r_pcb_c_per_y;
		b[extra_idx + SOLDER_S][extra_idx + PCB_C_S] = -1.0/pk->r_pcb_c_per_y;

		a[extra_idx + SOLDER_S] = pk->c_solder_per_y;

		/* solder ball west/east	*/
		/* partition r_solder1_x among all the nr grid cells. edge cell has half the rx	*/
		for(i=0; i < nr; i++){
			b[(solderidx)*(nr)*(nc) + (i)*(nc) + 0][extra_idx + SOLDER_W] = -1.0/(l[solderidx].rx / 2.0 + nr * pk->r_solder1_x);
			b[extra_idx + SOLDER_W][(solderidx)*(nr)*(nc) + (i)*(nc) + 0] = -1.0/(l[solderidx].rx / 2.0 + nr * pk->r_solder1_x);
		}
		b[extra_idx + PCB_C_W][extra_idx + SOLDER_W] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + SOLDER_W][extra_idx + PCB_C_W] = -1.0/pk->r_pcb_c_per_x;

		a[extra_idx + SOLDER_W] = pk->c_solder_per_x;

		for(i=0; i < nr; i++){
			b[(solderidx)*(nr)*(nc) + (i)*(nc) + nc-1][extra_idx + SOLDER_E] = -1.0/(l[solderidx].rx / 2.0 + nr * pk->r_solder1_x);
			b[extra_idx + SOLDER_E][(solderidx)*(nr)*(nc) + (i)*(nc) + nc-1] = -1.0/(l[solderidx].rx / 2.0 + nr * pk->r_solder1_x);
		}
		b[extra_idx + PCB_C_E][extra_idx + SOLDER_E] = -1.0/pk->r_pcb_c_per_x;
		b[extra_idx + SOLDER_E][extra_idx + PCB_C_E] = -1.0/pk->r_pcb_c_per_x;

		a[extra_idx + SOLDER_E] = pk->c_solder_per_x;

		/* package substrate north/south	*/
		/* partition r_sub1_y among all the nc grid cells. edge cell has half the ry	*/
		for(j=0; j < nc; j++){
			b[(subidx)*(nr)*(nc) + (0)*(nc) + j][extra_idx + SUB_N] = -1.0/(l[subidx].ry / 2.0 + nc * pk->r_sub1_y);
			b[extra_idx + SUB_N][(subidx)*(nr)*(nc) + (0)*(nc) + j] = -1.0/(l[subidx].ry / 2.0 + nc * pk->r_sub1_y);
		}
		b[extra_idx + SOLDER_N][extra_idx + SUB_N] = -1.0/pk->r_solder_per_y;
		b[extra_idx + SUB_N][extra_idx + SOLDER_N] = -1.0/pk->r_solder_per_y;

		a[extra_idx + SUB_N] = pk->c_sub_per_y;

		for(j=0; j < nc; j++){
			b[(subidx)*(nr)*(nc) + (nr-1)*(nc) + j][extra_idx + SUB_S] = -1.0/(l[subidx].ry / 2.0 + nc * pk->r_sub1_y);
			b[extra_idx + SUB_S][(subidx)*(nr)*(nc) + (nr-1)*(nc) + j] = -1.0/(l[subidx].ry / 2.0 + nc * pk->r_sub1_y);
		}
		b[extra_idx + SOLDER_S][extra_idx + SUB_S] = -1.0/pk->r_solder_per_y;
		b[extra_idx + SUB_S][extra_idx + SOLDER_S] = -1.0/pk->r_solder_per_y;

		a[extra_idx + SUB_S] = pk->c_sub_per_y;

		/* sub ball west/east	*/
		/* partition r_sub1_x among all the nr grid cells. edge cell has half the rx	*/

		for(i=0; i < nr; i++){
			b[(subidx)*(nr)*(nc) + (i)*(nc) + 0][extra_idx + SUB_W] = -1.0/(l[subidx].rx / 2.0 + nr * pk->r_sub1_x);
			b[extra_idx + SUB_W][(subidx)*(nr)*(nc) + (i)*(nc) + 0] = -1.0/(l[subidx].rx / 2.0 + nr * pk->r_sub1_x);
		}
		b[extra_idx + SOLDER_W][extra_idx + SUB_W] = -1.0/pk->r_solder_per_x;
		b[extra_idx + SUB_W][extra_idx + SOLDER_W] = -1.0/pk->r_solder_per_x;

		a[extra_idx + SUB_W] = pk->c_sub_per_x;
	  
		for(i=0; i < nr; i++){
			b[(subidx)*(nr)*(nc) + (i)*(nc) + nc-1][extra_idx + SUB_E] = -1.0/(l[subidx].rx / 2.0 + nr * pk->r_sub1_x);
			b[extra_idx + SUB_E][(subidx)*(nr)*(nc) + (i)*(nc) + nc-1] = -1.0/(l[subidx].rx / 2.0 + nr * pk->r_sub1_x);
		}
		b[extra_idx + SOLDER_E][extra_idx + SUB_E] = -1.0/pk->r_solder_per_x;
		b[extra_idx + SUB_E][extra_idx + SOLDER_E] = -1.0/pk->r_solder_per_x;

		a[extra_idx + SUB_E] = pk->c_sub_per_x;
  	}

	/* sum up the conductances	*/
  	for(i = 0; i < new_model->block->n_nodes; i++)
	{
		for(j = 0; j < new_model->block->n_nodes; j++)
		{
			if(i != j)
				b[i][i] -= b[i][j];
		}
	}
	//printf("check B matrix: b[%d][%d] == b [%d][%d] (%f,%f), b[%d][%d] == b [%d][%d](%f,%f), b[%d][%d] == b [%d][%d](%f,%f)\n", 1,2,2,1,b[1][2],b[2][1],20,21,21,20,b[20][21],b[21][20],0,64,64,0,b[0][64],b[64][0]); 
	/* calculate A^-1 (for diagonal matrix A) such that A(dT) + BT = POWER */
	for (i = 0; i < new_model->block->n_nodes; i++)
	{
		inva[i] = 1.0/a[i];
	}
	/* we are always going to use the eqn dT + A^-1 * B T = A^-1 * POWER. so, store  C = A^-1 * B	*/
	diagmatmult(c, inva, b, new_model->block->n_nodes); 
    
	/*	done	*/
	new_model->block->c_ready = TRUE; 
	new_model->block->r_ready = TRUE;

	return new_model;	
}

/* populate the thermal restistance values */
void populate_R_model(RC_model_t *model, flp_t *flp)
{
	if (model->type == BLOCK_MODEL)
		populate_R_model_block(model->block, flp);
	else if (model->type == GRID_MODEL)	
		populate_R_model_grid(model->grid, flp);
	else fatal("unknown model type\n");	
}

/* populate the thermal capacitance values */
void populate_C_model(RC_model_t *model, flp_t *flp)
{
	if (model->type == BLOCK_MODEL)
		populate_C_model_block(model->block, flp);
	else if (model->type == GRID_MODEL)	
		populate_C_model_grid(model->grid, flp);
	else fatal("unknown model type\n");	
}

/* calculate temperature-dependent leakage power */
/* will support HotLeakage in future releases */
double calc_lc_leakage(int mode, double h, double w, double temp)
{
	/* a simple leakage model.
	 * Be aware -- this model may not be accurate in some cases.
	 * You may want to use your own temperature-dependent leakage model here.
	 */ 
	double leak_alpha = 1.5e+4;
	double leak_beta = 0.036;
	double leak_Tbase = 383.15; /* 110C according to the above paper */

	double leakage_power;
	
	if (mode)
		fatal("HotLeakage currently is not implemented in this release of HotSpot, please check back later.\n");
		
	//leakage_power = leak_alpha*h*w*exp(leak_beta*(temp-leak_Tbase)); // Default
	//leakage_power = (4709.086 + (424.8759 - 4709.086)/(1 + pow((temp/368.8059),49.15633)))/1000; // 1Gb Bank. 
	//leakage_power = 0.0;
	// leakage_power = 0.0896; // 0.448 * 0.2
	leakage_power = (5482.052 + (140.2015 - 5482.052) /(1+ pow((temp/440.6993),16.57078)))/1000;
//	leakage_power = 0.148;					// At 298 (25)
//	leakage_power = 0.272;					// At 353 (80)

	leakage_power = 1.0 * leakage_power; 
	//printf("leak = %f\n",leakage_power);
	return leakage_power;	
}

/* calculate temperature-dependent leakage power */
/* will support HotLeakage in future releases */
double calc_core_leakage(int mode, double h, double w, double temp)
{
	/* a simple leakage model.
	 * Be aware -- this model may not be accurate in some cases.
	 * You may want to use your own temperature-dependent leakage model here.
	 */ 
	double leak_alpha = 1.5e+4;
	double leak_beta = 0.036;
	double leak_Tbase = 383.15; /* 110C according to the above paper */

	double leakage_power;
	
	if (mode)
		fatal("HotLeakage currently is not implemented in this release of HotSpot, please check back later.\n");
	
	//printf("Hello\n");	
	//leakage_power = leak_alpha*h*w*exp(leak_beta*(temp-leak_Tbase)); // Default
	//leakage_power = (4709.086 + (424.8759 - 4709.086)/(1 + pow((temp/368.8059),49.15633)))/1000; // 1Gb Bank. 
	//leakage_power = (1005.412 + (67.78504 - 1005.412)/(1 + pow((temp/373.242), 43.36131)))/1000;  //64Mb bank. Check modelling in /home/siddhulokesh/lazy/c/leakage/test.c
	//leakage_power = 2.21902 + (89242.78 + (-6914.301 - 89242.78)/(1+ pow((temp/596.4987),4.321001)))/1000;
	//leakage_power = (89242.78 + (-6914.301 - 89242.78)/(1+ pow((temp/596.4987),4.321001)))/1000;
	leakage_power = (9447.469 + (643.729 - 9447.469)/(1 + pow((temp/373.2385),43.37272)))/1000;

	leakage_power = 1.0 * leakage_power;
	//printf("leak = %f\n",leakage_power);
	return leakage_power;	
}

/* steady state temperature	*/
void steady_state_temp(RC_model_t *model, double *power, double *temp) 
{
//	if (model->type == BLOCK_MODEL)
//		steady_state_temp_block(model->block, power, temp);
//	else if (model->type == GRID_MODEL)	
//		steady_state_temp_grid(model->grid, power, temp);
//	else fatal("unknown model type\n");	

	int leak_convg_true = 0;
	int leak_iter = 0;
	int n, base=0;
	//int idx=0;
	double blk_height, blk_width;
	int i, j, k;
	
	double *d_temp = NULL;
	double *temp_old = NULL;
	double *power_new = NULL;
	double d_max=0.0;
	
	if (model->type == BLOCK_MODEL) {
		n = model->block->flp->n_units;
		if (model->config->leakage_used) { // if considering leakage-temperature loop
			d_temp = hotspot_vector(model);
			temp_old = hotspot_vector(model);
			power_new = hotspot_vector(model);
			for (leak_iter=0;(!leak_convg_true)&&(leak_iter<=LEAKAGE_MAX_ITER);leak_iter++){
				for(i=0; i < n; i++) {
					blk_height = model->block->flp->units[i].height;
					blk_width = model->block->flp->units[i].width;
					power_new[i] = power[i] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[i]);
					temp_old[i] = temp[i]; //copy temp before update
				}
				steady_state_temp_block(model->block, power_new, temp); // update temperature
				d_max = 0.0;
				for(i=0; i < n; i++) {
					d_temp[i] = temp[i] - temp_old[i]; //temperature increase due to leakage
					if (d_temp[i]>d_max) {
						d_max = d_temp[i];
					}
				}
				if (d_max < LEAK_TOL) {// check convergence
					leak_convg_true = 1;
				}
				if (d_max > TEMP_HIGH && leak_iter > 1) {// check to make sure d_max is not "nan" (esp. in natural convection)
					fatal("temperature is too high, possible thermal runaway. Double-check power inputs and package settings.\n");
				}
			}
			free(d_temp);
			free(temp_old);
			free(power_new);
			/* if no convergence after max number of iterations, thermal runaway */
			if (!leak_convg_true)
				fatal("too many iterations before temperature-leakage convergence -- possible thermal runaway\n");
		} else // if leakage-temperature loop is not considered
			steady_state_temp_block(model->block, power, temp);
	}
	else if (model->type == GRID_MODEL)	{
		if (model->config->leakage_used) { // if considering leakage-temperature loop
			// printf("I AM HERE\n");
			d_temp = hotspot_vector(model);
			temp_old = hotspot_vector(model);
			power_new = hotspot_vector(model);
                        printf("Arch type : %s\n", model->config->type);
			for (leak_iter=0;(!leak_convg_true)&&(leak_iter<=LEAKAGE_MAX_ITER);leak_iter++){
                
                if(strcmp(model->config->type,"3Dmem")==0 || strcmp(model->config->type,"DDR")==0){
                	printf("Memory type 3Dmem/DDR: %s\n", model->config->type);
					for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==3){ 	// Layer0 : In 3Dmem is an SRAM layer its leakage model is different.
									if (leakage[j] == 0)
										power_new[base+j] = 0;
									else
										power_new[base+j] = power[base+j] + calc_lc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);	
										//printf("%f ", power[base+j]);
							}
							else{		// Layer above the base layer in 3Dmem, have a DRAM leakage model.
									if (leakage[j] == 0)
										power_new[base+j] = 0;
									else	
										power_new[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);
				 					// printf("YES");
							}
							temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
			}
					}


//for Core
            else if(strcmp(model->config->type,"Core")==0){
            	printf("Arch type: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							
							if (leakage[j] == 0)
								power_new[base+j] = 0;
							else	
								power_new[base+j] = power[base+j] + ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);
		 					// printf("YES");
							
							temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
			}


//for 3D (WIO)
                    else if(strcmp(model->config->type,"3D")==0){
                                printf("Arch type 3D: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==19){ 	// Layer0 : In 3Dmem is an SRAM layer its leakage model is different.
									power_new[base+j] = power[base+j] + ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);										//printf("%f ", power[base+j]);
							}
							else{		// Layer above the base layer in 3Dmem, have a DRAM leakage model.
									if (leakage[j] == 0)
										power_new[base+j] = 0;
									else	
										power_new[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);
				 					// printf("YES");
							}
							temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }
//For 2.5D
                        
                    else if(strcmp(model->config->type,"2.5D")==0){
                                printf("Arch type 2.5D: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==5){ // Layer0 : Interposer, Layer 1: TIM, layer 2 in 3Dmem is an SRAM layer its leakage model is different.
								if ( (j==22) || (j==21) || (j==20) )	// No leakeage in air
									power_new[base+j] = 0;
								else{
									if ( (j>=0) && (j<=3) )	// Leakage for Host core
										{power_new[base+j] = power[base+j] + ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);
				 						//printf("YES calc_core_leakage, power = %f, power_new[%d + %d] = %f\n", power[base+j], base, j, power_new[base+j]);
				 					}
									else				// Leakage for 3Dmem logic core
									{
										if (leakage[j-4] == 0)
										 	{ power_new[base+j] = 0; //printf("NO lc_leakage\n");
											}
										else
										power_new[base+j] = power[base+j] + calc_lc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);	
										//printf("YES calc_lc_leakage, power = %f, power_new[%d + %d] = %f\n", power[base+j], base, j, power_new[base+j]);
									}
								}						
							}
							else{	// layer above the base layer in 3Dmem, have a DRAM leakage model.
								if ( (j==19) || (j==18) || (j==17) || (j==16))	// No leakeage in air
									power_new[base+j] = 0;
								else{
										if (leakage[j] == 0)
											{ power_new[base+j] = 0; //printf("NO bank_leakage\n");
											}
										else	
											power_new[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp[base+j]);
				 					//printf("YES calc_leakage, power = %f, power_new[%d + %d] = %f\n", power[base+j], base, j, power_new[base+j]);
				 					// printf("YES");
								}						
							}
							temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }//end of if memory type
                                
				steady_state_temp_grid(model->grid, power_new, temp);
				d_max = 0.0;
				for(k=0, base=0; k < model->grid->n_layers; k++) {
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
							//printf("temp[%d + %d] = %f\n", base, j, temp[base+j]);
							d_temp[base+j] = temp[base+j] - temp_old[base+j]; //temperature increase due to leakage
							if (d_temp[base+j]>d_max)
								d_max = d_temp[base+j];
						}
					base += model->grid->layers[k].flp->n_units;	
				}
				if (d_max < LEAK_TOL) {// check convergence
					leak_convg_true = 1;
					printf("leak_convg_true\n");
				}
				if (d_max > TEMP_HIGH && leak_iter > 0) {// check to make sure d_max is not "nan" (esp. in natural convection)
					fatal("temperature is too high, possible thermal runaway. Double-check power inputs and package settings.\n");
				}
	 		}
			free(d_temp);
			free(temp_old);
			free(power_new);
			/* if no convergence after max number of iterations, thermal runaway */
			if (!leak_convg_true)
				fatal("too many iterations before temperature-leakage convergence -- possible thermal runaway\n");			
		} else // if leakage-temperature loop is not considered
			steady_state_temp_grid(model->grid, power, temp);
	}
	else fatal("unknown model type\n");	
}

double *temp_first_time = NULL;

/* transient (instantaneous) temperature	*/
void compute_temp(RC_model_t *model, double *power, double *temp, double time_elapsed)
{
	
//	double *temp_old = NULL;
//	double *power_new = NULL;

	if (model->type == BLOCK_MODEL)
		compute_temp_block(model->block, power, temp, time_elapsed);
	else if (model->type == GRID_MODEL)	{
			if (model->config->leakage_used) { // if considering leakage-temperature loop
				//printf("In compute_temp\n");					
				static int count = 0;

				// Initilize the pointer first. Subsequent calls temp_first_time will hold the value of temperature for the last iteration. 
				// [FIX]: We might not be able to free the temp_first_time.

				int base=0;
				int j, k;
				double *power_new = hotspot_vector(model);
				double blk_height, blk_width;

				if (count++ == 0){
					temp_first_time = temp;
				}

//				for(k=0, base=0; k < model->grid->n_layers; k++) {
//				  	//printf("\nk=%d",k);					
//					if(model->grid->layers[k].has_power){
//						// printf("Hi\n");
//						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
//
//							blk_height = model->grid->layers[k].flp->units[j].height;
//							blk_width = model->grid->layers[k].flp->units[j].width;
//
//					 		//printf("j=%d,",j);					
//
//							if (k==3){ 	// Layer0 : In 3Dmem is an SRAM layer its leakage model is different.
//									power_new[base+j] = power[base+j] + calc_lc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);	
//							}
//							else{		// Layer above the base layer in 3Dmem, have a DRAM leakage model.
//									power_new[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
//				 					// printf("YES");
//							}
//							// temp_old[base+j] = temp[base+j]; //copy temp before update
//						}
//				// printf("\n");					
//				// printf("k=%d",k);					
//				}
//					base += model->grid->layers[k].flp->n_units;	
//			}
                            if(strcmp(model->config->type,"3Dmem")==0 || strcmp(model->config->type,"DDR")==0){
                                printf("Memory type 3Dmem/DDR: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 //printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			//printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==3){ 	// Layer0 : In 3Dmem is an SRAM layer its leakage model is different.
									if (leakage[j] == 0)
										power[base+j] = 0;
									else
										power[base+j] = power[base+j] + calc_lc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);	
										//printf("%f ", power[base+j]);
							}
							else{		// Layer above the base layer in 3Dmem, have a DRAM leakage model.
									if (leakage[j] == 0)
										power[base+j] = 0;
									else	
										power[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
				 					// printf("YES");
							}
							//temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }


// Core
                    else if(strcmp(model->config->type,"Core")==0){
                                printf("Arch type: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (leakage[j] == 0)
								power[base+j] = 0;
							else{	
									//printf("volt[j] = %d\n", volt[j]);
									power[base+j] = power[base+j] +  ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
		 					// printf("YES");
							}
							//temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }




//for 3D (WIO)
                    else if(strcmp(model->config->type,"3D")==0){
                                printf("Arch type 3D: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==19){ 	// Layer0 : In 3Dmem is an SRAM layer its leakage model is different.
									power[base+j] = power[base+j] + ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);										//printf("%f ", power[base+j]);
							}
							else{		// Layer above the base layer in 3Dmem, have a DRAM leakage model.
									if (leakage[j] == 0)
										power[base+j] = 0;
									else	
										power[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
				 					// printf("YES");
							}
							//temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }
//For 2.5D
                        
                    else if(strcmp(model->config->type,"2.5D")==0){
                                printf("Arch type 2.5D: %s\n", model->config->type);
				for(k=0, base=0; k < model->grid->n_layers; k++) {
				 // printf("k=%d\n",k);					
					if(model->grid->layers[k].has_power)
						for(j=0; j < model->grid->layers[k].flp->n_units; j++) {
				 			// printf("j=%d,",j);					
							blk_height = model->grid->layers[k].flp->units[j].height;
							blk_width = model->grid->layers[k].flp->units[j].width;
							if (k==5){ // Layer0 : Interposer, Layer 1: TIM, layer 2 in 3Dmem is an SRAM layer its leakage model is different.
								if ( (j==22) || (j==21) || (j==20) )	// No leakeage in air. Assuming 4 cores + 16 LC
									power[base+j] = power[base+j];
								else{
									if ( (j>=0) && (j<=3) )	// Leakage for Host core. Assuming 4 cores
										power[base+j] = power[base+j] + ((float) volt[j]/10) * calc_core_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
				 					// printf("YES");
									else				// Leakage for 3Dmem logic core
									{
										if (leakage[j-4] == 0)
											power[base+j] = 0;
										else
											power[base+j] = power[base+j] + calc_lc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);	
									}
								}						
							}
							else{	// layer above the base layer in 3Dmem, have a DRAM leakage model.
								if ( (j==19) || (j==18) || (j==17) || (j==16))	// No leakeage in air
									power[base+j] = power[base+j];
								else{
										if (leakage[j] == 0)
											power[base+j] = 0;
										else	
											power[base+j] = power[base+j] + calc_leakage(model->config->leakage_mode,blk_height,blk_width,temp_first_time[base+j]);
				 					// printf("YES");
								}						
							}
                                                        //printf ("\nPower: %.3f, Power_calc: %.3f, j: %d", power[base+j], power_new[base+j], j);
							//temp_old[base+j] = temp[base+j]; //copy temp before update
						}
					base += model->grid->layers[k].flp->n_units;	
				// printf("\n");					
				// printf("k=%d",k);					
				}
                        }//end of if memory type

			// printf("Calling compute_temp_grid\n");					
			compute_temp_grid(model->grid, power, temp_first_time, time_elapsed);
			
			/*Sobhan added start*/
			int extra_nodes;
  			grid_model_vector_t *p, *t;

  			if (model->grid->config.model_secondary)
    			extra_nodes = EXTRA + EXTRA_SEC;
  			else 
    			extra_nodes = EXTRA;

  			p = new_grid_model_vector(model->grid);
  			t = new_grid_model_vector(model->grid);

  			/* package nodes' power numbers	*/
  			set_internal_power_grid(model->grid, power);  

  			/* map the block power/temp numbers to the grid	*/
  			xlate_vector_b2g(model->grid, power, p, V_POWER);

  			/* if temp is NULL, re-use the temperature from the
  			* last call. otherwise, translate afresh and remember 
   			* the grid and block temperature arrays for future use
   			*/
  			if (temp_first_time != NULL) {
      			xlate_vector_b2g(model->grid, temp_first_time, t, V_TEMP);
      			//model->grid->last_temp = temp_first_time;
			}
			int nl = model->grid->n_layers;
			int nr = model->grid->rows;
			int nc = model->grid->cols;
			double *power_block = dvector(nl*nr*nc+extra_nodes);
			double *temp_block = dvector(nl*nr*nc+extra_nodes);
			zero_dvector(power_block, nl*nr*nc+extra_nodes);
			zero_dvector(temp_block, nl*nr*nc+extra_nodes);
			for(int n=0; n < nl; n++)
      			for(int i=0; i < nr; i++)
        			for(int j=0; j < nc; j++) {
						//printf("[%d][%d][%d] = %f %f", n, i, j, p->cuboid[n][i][j], t->cuboid[n][i][j]-273.15); 
						power_block[n*nr*nc + i*nc + j] = p->cuboid[n][i][j] + model->grid->config.ambient * model->block->g_amb[n*nr*nc + i*nc + j];
						temp_block[n*nr*nc + i*nc + j] = t->cuboid[n][i][j];
					}
			for(int k=0; k<extra_nodes; k++){						
				//printf("extra [%d] = %f %f", k, p->extra[k], t->extra[k]-273.15); 
				power_block[nl*nr*nc + k] = p->extra[k] + model->grid->config.ambient * model->block->g_amb[nl*nr*nc + k];
				temp_block[nl*nr*nc + k] = t->extra[k];
			}

  			/*int hsidx, pcbidx;
  			int model_secondary = model->grid->config.model_secondary;

  			hsidx = nl - DEFAULT_PACK_LAYERS + LAYER_SINK;
  			if (model_secondary)
      			pcbidx = LAYER_PCB;	

			int i;
			for(i=0; i < nl*nr*nc + EXTRA_SEC; i++)
				power_block[i*nr*nc+i] = model->grid->config.ambient * model->block->g_amb[i];
			*/

			//fprintf(stdout, "number of nodes: %d\n", model->block->n_nodes);
			//fprintf(stdout, "number of blocks: %d\n", model->grid->total_n_blocks);

			compute_temp_block(model->block, power_block, temp_block, time_elapsed);  
  			
			grid_model_vector_t *temp;
  			temp = new_grid_model_vector(model->grid);
			for(int n=0; n < nl; n++)
			{
      			for(int i=0; i < nr; i++)
				{
					/*int u;
					int reach = 0;
					for(u=0; u<model->grid->layers[n].flp->n_units;u++)
					{
						//if(u==0 || u==1)
						//	printf ("%d, %d, %d, %d\n",model->grid->layers[n].g2bmap[u].i1, model->grid->layers[n].g2bmap[u].i2, model->grid->layers[n].g2bmap[u].j1, model->grid->layers[n].g2bmap[u].j2);
						if(i == model->grid->layers[n].g2bmap[u].i1 || i == model->grid->layers[n].g2bmap[u].i2)
							reach++;
					}
					if(reach !=0)
					{
						for(int j=0; j < nc; j++)
							printf ("	-");
						printf ("\n");
					}*/
					for(int j=0; j < nc; j++)
					{
						//if(j == model->grid->layers[n].g2bmap[0].j1 || j == model->grid->layers[n].g2bmap[1].j1 || j == model->grid->layers[n].g2bmap[2].j1 || j == model->grid->layers[n].g2bmap[3].j1 || j == model->grid->layers[n].g2bmap[0].j2 || j == model->grid->layers[n].g2bmap[1].j2 || j == model->grid->layers[n].g2bmap[2].j2 || j == model->grid->layers[n].g2bmap[3].j2)
						//	printf ("| ");
						temp->cuboid[n][i][j] = temp_block[n*nr*nc + i*nc + j];//model->block->t_vector[n*nr*nc + i*nc + j];
						//printf ("%f ",temp_block[n*nr*nc + i*nc + j]-273.15);
						//if((n*nr*nc + i*nc + j)%8 == 7)
						//	printf ("\n");
					}
				}
				//printf ("\n");   
			}
			for(int k=0; k<extra_nodes; k++)
			{
			 	temp->extra[k] = temp_block[nl*nr*nc + k];//model->block->t_vector[nl*nr*nc + k];
			   	//printf ("%f ",temp_block[nl*nr*nc + k]-273.15);
			}
			//printf ("%d\n",model->grid->map_mode);

			// /* map the temperature numbers back	*/
			free_dvector(model->block->t_vector);
			model->block->t_vector = dvector(model->grid->total_n_blocks+extra_nodes);
			//printf ("%d %d\n",model->grid->total_n_blocks,extra_nodes);
  			xlate_temp_g2b(model->grid, model->block->t_vector, temp);

			/*int x = 0;
			for(int i=0; i<model->grid->n_layers; i++)
			{
				printf ("%d %d ", i, model->grid->layers[i].flp->n_units);
				for(int j=0; j<model->grid->layers[i].flp->n_units; j++)
				{

					printf ("%f(%f) ",model->block->t_vector[x + j] - 273.15, model->grid->last_temp[x + j] - 273.15);
				}
				x += model->grid->layers[i].flp->n_units;
				printf ("\n");
			}
			for(int i=0; i<extra_nodes; i++)
				printf ("%f(%f) ",model->block->t_vector[model->grid->total_n_blocks + i]-273.15, model->grid->last_temp[model->grid->total_n_blocks + i] - 273.15);
			*/
			/*Sobhan added finish*/

			// temp_first_time will hold the value of temperature for the last iteration. 
			// printf("temp=%u\n",temp);
			// printf("Returned compute_temp_grid\n");	
			// printf ("model->last_temp[0] = %f\n",model->last_temp[0]);

				free(power_new);
			}
			else{		
				compute_temp_grid(model->grid, power, temp, time_elapsed);	

				/* Sobhan added start*/
  				int extra_nodes;
  				grid_model_vector_t *p;

  				if (model->grid->config.model_secondary)
    				extra_nodes = EXTRA + EXTRA_SEC;
  				else
    				extra_nodes = EXTRA;

  				p = new_grid_model_vector(model->grid);

  				/* package nodes' power numbers	*/
  				set_internal_power_grid(model->grid, power);

  				/* map the block power/temp numbers to the grid	*/
  				xlate_vector_b2g(model->grid, power, p, V_POWER);

  				/* if temp is NULL, re-use the temperature from the
  				* last call. otherwise, translate afresh and remember 
  				* the grid and block temperature arrays for future use
  				*/
  				if (temp != NULL) {
					xlate_vector_b2g(model->grid, temp, model->grid->last_trans, V_TEMP);
      				model->grid->last_temp = temp;
				}
  				int nl = model->grid->n_layers;
  				int nr = model->grid->rows;
  				int nc = model->grid->cols;
  				double *power_block = dvector(nl*nr*nc+extra_nodes);
  				double *temp_block = dvector(nl*nr*nc+extra_nodes);
  				for(int n=0; n < nl; n++)
					for(int i=0; i < nr; i++)
        				for(int j=0; j < nc; j++) {
							power_block[n*nr*nc + i*nc + j] = p->cuboid[n][i][j];
							temp_block[n*nr*nc + i*nc + j] = model->grid->last_trans->cuboid[n][i][j];
						}	
				for(int k=0; k<extra_nodes; k++){
					power_block[nl*nr*nc + k] = p->extra[k];
					temp_block[nl*nr*nc + k] = model->grid->last_trans->extra[k];
				}
				fprintf(stdout, "number of units: %d\n", model->block->n_units);
				fprintf(stdout, "number of nodes: %d\n", model->block->n_nodes);


				compute_temp_block(model->block, power_block, temp_block, time_elapsed);
							
				grid_model_vector_t *temp;
  				temp = new_grid_model_vector(model->grid);
				for(int n=0; n < nl; n++)
      			 	for(int i=0; i < nr; i++)
        		 		for(int j=0; j < nc; j++)
				 			temp->cuboid[n][i][j] = model->block->t_vector[n*nr*nc + i*nc + j];
				for(int k=0; k<extra_nodes; k++)
				 	temp->extra[nl*nr*nc + k] = model->block->t_vector[nl*nr*nc + k];
			  
				/* map the temperature numbers back	*/
				free_dvector(model->block->t_vector);
				model->block->t_vector = dvector(model->grid->total_n_blocks+extra_nodes);
  				xlate_temp_g2b(model->grid, model->block->t_vector, temp);
				/*Sobhan added finish*/
			}	
		}
	else fatal("unknown model type\n");	
}

/* differs from 'dvector()' in that memory for internal nodes is also allocated	*/
double *hotspot_vector(RC_model_t *model)
{
	if (model->type == BLOCK_MODEL)
		return hotspot_vector_block(model->block);
	else if (model->type == GRID_MODEL)	
		return hotspot_vector_grid(model->grid);
	else fatal("unknown model type\n");	
	return NULL;
}

/* copy 'src' to 'dst' except for a window of 'size'
 * elements starting at 'at'. useful in floorplan
 * compaction
 */
void trim_hotspot_vector(RC_model_t *model, double *dst, double *src, 
						 int at, int size)
{
	if (model->type == BLOCK_MODEL)
		trim_hotspot_vector_block(model->block, dst, src, at, size);
	else if (model->type == GRID_MODEL)	
		trim_hotspot_vector_grid(model->grid, dst, src, at, size);
	else fatal("unknown model type\n");	
}

/* update the model's node count	*/						 
void resize_thermal_model(RC_model_t *model, int n_units)
{
	if (model->type == BLOCK_MODEL)
		resize_thermal_model_block(model->block, n_units);
	else if (model->type == GRID_MODEL)	
		resize_thermal_model_grid(model->grid, n_units);
	else fatal("unknown model type\n");	
}

/* sets the temperature of a vector 'temp' allocated using 'hotspot_vector'	*/
void set_temp(RC_model_t *model, double *temp, double val)
{
	if (model->type == BLOCK_MODEL)
		set_temp_block(model->block, temp, val);
	else if (model->type == GRID_MODEL)	
		set_temp_grid(model->grid, temp, val);
	else fatal("unknown model type\n");	
}

/* dump temperature vector alloced using 'hotspot_vector' to 'file' */ 
void dump_temp(RC_model_t *model, double *temp, char *file)
{
	if (model->type == BLOCK_MODEL)
		dump_temp_block(model->block, temp, file);
	else if (model->type == GRID_MODEL)	
	{
		dump_temp_grid(model->grid, temp, file);
  		//dump_temp_block(model->block, model->block->t_vector, file);
	}
	else fatal("unknown model type\n");	
}

/* calculate average heatsink temperature for natural convection package */ 
double calc_sink_temp(RC_model_t *model, double *temp)
{
	if (model->type == BLOCK_MODEL)
		return calc_sink_temp_block(model->block, temp, model->config);
	else if (model->type == GRID_MODEL)	
		return calc_sink_temp_grid(model->grid, temp, model->config);
	else fatal("unknown model type\n");	
	return 0.0;
}

/* copy temperature vector from src to dst */ 
void copy_temp(RC_model_t *model, double *dst, double *src)
{
	if (model->type == BLOCK_MODEL)
		copy_temp_block(model->block, dst, src);
	else if (model->type == GRID_MODEL)	
		copy_temp_grid(model->grid, dst, src);
	else fatal("unknown model type\n");	
}

/* 
 * read temperature vector alloced using 'hotspot_vector' from 'file'
 * which was dumped using 'dump_temp'. values are clipped to thermal
 * threshold based on 'clip'
 */ 
void read_temp(RC_model_t *model, double *temp, char *file, int clip)
{
	if (model->type == BLOCK_MODEL)
		read_temp_block(model->block, temp, file, clip);
	else if (model->type == GRID_MODEL)	
		read_temp_grid(model->grid, temp, file, clip);
	else fatal("unknown model type\n");	
}

/* dump power numbers to file	*/
void dump_power(RC_model_t *model, double *power, char *file)
{
	if (model->type == BLOCK_MODEL)
		dump_power_block(model->block, power, file);
	else if (model->type == GRID_MODEL)	
		dump_power_grid(model->grid, power, file);
	else fatal("unknown model type\n");	
}

/* 
 * read power vector alloced using 'hotspot_vector' from 'file'
 * which was dumped using 'dump_power'. 
 */ 
void read_power(RC_model_t *model, double *power, char *file)
{
	if (model->type == BLOCK_MODEL)
		read_power_block(model->block, power, file);
	else if (model->type == GRID_MODEL)	
		read_power_grid(model->grid, power, file);
	else fatal("unknown model type\n");	
}

/* peak temperature on chip	*/
double find_max_temp(RC_model_t *model, double *temp)
{
	if (model->type == BLOCK_MODEL)
		return find_max_temp_block(model->block, temp);
	else if (model->type == GRID_MODEL)	
		return find_max_temp_grid(model->grid, temp);
	else fatal("unknown model type\n");	
	return 0.0;
}

/* average temperature on chip	*/
double find_avg_temp(RC_model_t *model, double *temp)
{
	if (model->type == BLOCK_MODEL)
		return find_avg_temp_block(model->block, temp);
	else if (model->type == GRID_MODEL)	
		return find_avg_temp_grid(model->grid, temp);
	else fatal("unknown model type\n");	
	return 0.0;
}

/* debug print	*/
void debug_print_model(RC_model_t *model)
{
	if (model->type == BLOCK_MODEL)
		debug_print_block(model->block);
	else if (model->type == GRID_MODEL)	
		debug_print_grid(model->grid);
	else fatal("unknown model type\n");	
}

/* calculate temperature-dependent leakage power */
/* will support HotLeakage in future releases */
double calc_leakage(int mode, double h, double w, double temp)
{
	/* a simple leakage model.
	 * Be aware -- this model may not be accurate in some cases.
	 * You may want to use your own temperature-dependent leakage model here.
	 */ 
	double leak_alpha = 1.5e+4;
	double leak_beta = 0.036;
	double leak_Tbase = 383.15; /* 110C according to the above paper */

	double leakage_power;
	
	if (mode)
		fatal("HotLeakage currently is not implemented in this release of HotSpot, please check back later.\n");
		
	//leakage_power = leak_alpha*h*w*exp(leak_beta*(temp-leak_Tbase)); // Default
	//leakage_power = (4709.086 + (424.8759 - 4709.086)/(1 + pow((temp/368.8059),49.15633)))/1000; // 1Gb Bank. 
	//leakage_power = (1005.412 + (67.78504 - 1005.412)/(1 + pow((temp/373.242),43.36131)))/1000;  //64Mb bank. Check modelling in /home/siddhulokesh/lazy/c/leakage/test.c


	leakage_power = (1070.333 + (2.10718  - 1070.333)/(1 + pow((temp/456.2595),5.405409)))/1000;
//	leakage_power = 0.09922325617;			// At 298 (25)
//	leakage_power = 0.2156328354;			// At 353 (80)
	leakage_power = 1.0 * leakage_power;


	//printf("leak = %f\n",leakage_power);
	return leakage_power;	
}

/* destructor */
void delete_RC_model(RC_model_t *model)
{
	if (model->type == BLOCK_MODEL)
		delete_block_model(model->block);
	else if (model->type == GRID_MODEL)	
		delete_grid_model(model->grid);
	else fatal("unknown model type\n");	
	free(model);
}
