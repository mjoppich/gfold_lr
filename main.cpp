/*
 * Copyright (C) 2012-2017 Jianxing Feng
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

// =====================================================================================
//       Filename:  main.cc
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  04/06/2011 19:49:32
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Jianxing Feng (feeldead), feeldead@gmail.com
//        Company:  Tongji Univ.
// 
// =====================================================================================

#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include "GeneInfo.h"
#include "GFOLD.h"
#include "Utility.h"

#define VERSION "V1.0.0"
#define DATE "November 2018"


using namespace std;

void
Help()
{
    cout << endl;
    cout << "      =============================================================================== " << endl;
    cout << "          gfold_lr  :   Generalized fold change for ranking differentially expressed   " << endl;
    cout << "                       genes from RNA-seq data in long-read datasets." << endl;
    cout <<                                                     endl;
    cout << "          Original Author   :   Jianxing Feng  (jianxing.tongji@gmail.com)" << endl;
    cout << "          Long-Read Author  :   Markus Joppich (joppich@bio.ifi.lmu.de)" << endl;
    cout << "            Date   :   " << DATE << endl; 
    cout << "          Version  :   " << VERSION << endl;
    cout << "      =============================================================================== " << endl;
    cout <<                                                   endl;
    cout << "      USAGE:   Please go to the source code directory and use command 'man doc/gfold.man' " << endl;
    cout << "               or open doc/gfold.html to find documentation." << endl;
    cout <<                                endl;
    cout << "      Quick Examples: " << endl;
    cout << "               gfold_lr count -ann hg19Ref.gtf -tag sample1.sam -o sample1.read_cnt          " << endl;
    cout << "               gfold_lr count -ann hg19Ref.gtf -tag sample2.sam -o sample2.read_cnt          " << endl;
    cout << "               gfold_lr diff -s1 sample1 -s2 sample2 -suf .read_cnt -o sample1VSsample2.diff " << endl;
    cout <<                                endl;
}

void
die_arg_missing(bool b_die, string argument)
{
    if (b_die)
    {
        cerr << "ERROR: argument " << argument << " is required" << endl;
        Help();
        exit(0);
    }   
}

void
die_arg_wrong(bool b_die, string argument)
{
    if (b_die)
    {
        cerr << "ERROR: Parameter for argument " << argument << " is invalid. Please refer to the manual" << endl;
        Help();
        exit(0);
    }   
}


int 
main(int argc, char* argv[])
{
    int startArg = 1;

    string job = "";

    string ref_seq_file = "";
    string gene_annotation = "";
    string gene_annotation_format = "GTF";
    string short_reads_file = ""; 
    string short_reads_format = "SAM"; 
    string genedesc = "";

    int strand_specific_code = 0;
    string output_file = "";
    string sample_suffix = "";

    int verbos_level = 2;
    int burn_in_count = 1000;
    int sampled_count = 1000;
    double significant_cutoff = 0.01;        
    int random_sampled_pairs = 20;
    bool b_accurate = true;

    // For job 'diff'
    vector<string> first_group_samples;
    vector<string> second_group_samples;
    string normal_method = "DESeq";

    string smethod = "matches";

    // Begin parsing the jobs and parameters
    for (int i = startArg; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            Help();
            return 0;
        }

        else if (strcmp(argv[i], "count") == 0)
            job = "count";
        else if (strcmp(argv[i], "diff") == 0)
            job = "diff";
        else if (strcmp(argv[i], "-ann") == 0)
            gene_annotation = argv[++i];
        else if (strcmp(argv[i], "-annf") == 0)
            gene_annotation_format = argv[++i];
        else if (strcmp(argv[i], "-tag") == 0)
            short_reads_file = argv[++i];
        else if (strcmp(argv[i], "-tagf") == 0)
            short_reads_format = argv[++i];
        else if (strcmp(argv[i], "-d") == 0)
            genedesc = argv[++i];
        else if (strcmp(argv[i], "-o") == 0)
            output_file = argv[++i];
        else if (strcmp(argv[i], "-m") == 0)
            smethod = argv[++i];
        else if (strcmp(argv[i], "-suf") == 0)
            sample_suffix = argv[++i];
        else if (strcmp(argv[i], "-s") == 0)
            if (argv[++i][0] == 'T') strand_specific_code = 1;
            else strand_specific_code = 2;
        else if (strcmp(argv[i], "-acc") == 0)
	    b_accurate = (argv[++i][0] == 'T');
        else if (strcmp(argv[i], "-si") == 0)
            sampled_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "-bi") == 0)
            burn_in_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "-v") == 0)
            verbos_level = atoi(argv[++i]);
        else if (strcmp(argv[i], "-s1") == 0)
        {
            vector<string> temp;
            split(argv[++i], ',', temp);
            first_group_samples.insert(first_group_samples.end(), temp.begin(), temp.end());
        }
        else if (strcmp(argv[i], "-s2") == 0)
        {
            vector<string> temp;
            split(argv[++i], ',', temp);
            second_group_samples.insert(second_group_samples.end(), temp.begin(), temp.end());
        }
        else if (strcmp(argv[i], "-norm") == 0)
            normal_method = argv[++i];
        else if (strcmp(argv[i], "-sc") == 0)
            significant_cutoff = atof(argv[++i]);
        else if (strcmp(argv[i], "-r") == 0)
            random_sampled_pairs = atoi(argv[++i]);
        else 
        {
            Help();
            cerr << "Wrong parameter " << argv[i] << endl;
            exit(0);
        }
    }

    die_arg_missing(job == "", "Job"); 
    die_arg_missing(output_file == "", "-o");

    GeneInfo::COUNT_METHOD oMethod = GeneInfo::matches;

    if (smethod == "matches")
    {
        oMethod = GeneInfo::matches;
    }

    if (job == "count")
    {
        die_arg_missing(gene_annotation == "", "-ann"); 
        die_arg_missing(short_reads_file == "", "-tag"); 
        die_arg_wrong(gene_annotation_format != "GPF" && gene_annotation_format != "BED" && gene_annotation_format != "GTF", "-annf");
        die_arg_wrong(short_reads_format != "SAM" && short_reads_format != "BED", "-tagf");

        GeneInfo gene_info(output_file, 1, strand_specific_code == 1, oMethod, false, verbos_level);
        if (gene_annotation_format == "GPF")
            scanGPF(gene_annotation, gene_info, verbos_level);
        else if (gene_annotation_format == "BED")
            scanAnnotBED(gene_annotation, gene_info, verbos_level);
        else
            scanGTF(gene_annotation, gene_info, verbos_level);

        if (short_reads_format == "SAM")
            scanSAM(short_reads_file, gene_info, verbos_level);
        else
            scanReadsBED(short_reads_file, gene_info, verbos_level);

        gene_info.PrintGeneReadCount();
    }
    else if (job == "diff")
    {
        die_arg_missing(first_group_samples.size() == 0, "-s1"); 
        die_arg_missing(second_group_samples.size() == 0, "-s2"); 

        if (normal_method != "NO" && normal_method != "Count" && normal_method != "TMM" && normal_method != "DESeq")
        {
            vector<string> normconst;
            split(normal_method, ',', normconst);
            if (normconst.size() != first_group_samples.size() + second_group_samples.size())
            {
                cerr << "ERROR: Unknown normalization method : " << normal_method << endl;
                exit(1);
            }
        }

        if (first_group_samples == second_group_samples)
        {
            cerr << "ERROR: Two input groups of samples are exactly the same!" << endl;
            exit(1);
        }

        GFOLD gfold(VERSION, verbos_level, normal_method, burn_in_count, sampled_count, significant_cutoff, random_sampled_pairs, b_accurate);
        gfold.CalculateAll(first_group_samples, second_group_samples, sample_suffix, genedesc, output_file, output_file + ".ext");
    }
    cerr << "Job " << job << " is DONE!" << endl;

    return 0;
}
