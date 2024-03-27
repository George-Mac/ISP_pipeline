#include "Config.h"

Config::Config()
{

}

Config::Config(char* filename)
{
    FILE* fp = fopen(filename, "r");
	char row[512];
	char* key;
	char* val;

    while (fgets(row, 512, fp) != NULL)
	{
		key = strtok(row, ",");
		val = strtok(NULL, ",");

		if (key == NULL || val == NULL) { throw "Wrong config file"; }
        if (strcmp(key, "dpc_threshold") == 0) { this->dpc_threshold = atof(val); }

        // BLC
        if (strcmp(key, "blc_r_offset")  == 0) {this->blc_r_offset = atof(val); }   
        if (strcmp(key, "blc_gr_offset") == 0) {this->blc_gr_offset = atof(val); }
        if (strcmp(key, "blc_gb_offset") == 0) {this->blc_gb_offset = atof(val); }
        if (strcmp(key, "blc_b_offset") == 0) {this->blc_b_offset = atof(val); }
        if (strcmp(key, "blc_alpha") == 0) {this->blc_alpha = atof(val); }
        if (strcmp(key, "blc_beta") == 0) {this->blc_beta = atof(val); }

        // AWB
        if (strcmp(key, "awb_gain") == 0) { this->awb_gain = atof(val); }

        if (strcmp(key, "in_bit") == 0) {this->in_bit = atof(val); }
        if (strcmp(key, "out_bit") == 0) {this->out_bit = atof(val); }

        // EE
        if (strcmp(key, "ee_threshod_1") == 0) { this->ee_threshod[0] = atof(val); }
        if (strcmp(key, "ee_threshod_2") == 0) { this->ee_threshod[1] = atof(val); }
        if (strcmp(key, "ee_threshod_3") == 0) { this->ee_threshod[2] = atof(val); }
        if (strcmp(key, "ee_gain_1") == 0) { this->ee_gain[0] = atof(val); }
        if (strcmp(key, "ee_gain_2") == 0) { this->ee_gain[1] = atof(val); }

        // FCS
        if (strcmp(key, "fcs_edge_min") == 0) {this->fcs_edge_min = atof(val); }
        if (strcmp(key, "fcs_edge_max") == 0) {this->fcs_edge_max = atof(val); }

        // HSC
        if (strcmp(key, "hue") == 0) { this->hue = atof(val); }
        if (strcmp(key, "saturation") == 0) { this->saturation = atof(val); }

        // GC
        if (strcmp(key, "gamma") == 0) {this->gamma = atof(val); }
        
        // BCC
        if (strcmp(key, "brightness") == 0) {this->brightness = atof(val); }
        if (strcmp(key, "contrast") == 0) {this-> contrast = atof(val) / pow(2, 5); }

        while (val != NULL) { val = strtok(NULL, ","); }
    }

}

Config::~Config()
{

}