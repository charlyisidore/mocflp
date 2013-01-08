/*
 #License and Copyright
 
 #Version : 1.1
 
 #This file is part of BiUFLv2012.
 
 #BiUFLv2012 is Copyright © 2012, University of Nantes
 
 #BiUFLv2012 is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 
 #This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 #You should have received a copy of the GNU General Public License along with this program; if not, you can also find the GPL on the GNU web site.
 
 #In addition, we kindly ask you to acknowledge BiUFLv2012 and its authors in any program or publication in which you use BiUFLv2012. (You are not required to do so; it is up to your common sense to decide whether you want to comply with this request or not.) For general publications, we suggest referencing:  BiUFLv2012, MSc ORO, University of Nantes.
 
 #Non-free versions of BiUFLv2012 are available under terms different from those of the General Public License. (e.g. they do not require you to accompany any object code using BiUFLv2012 with the corresponding source code.) For these alternative terms you must purchase a license from Technology Transfer Office of the University of Nantes. Users interested in such a license should contact us (valorisation@univ-nantes.fr) for more information.
*/

#include "Argument.hpp"
#include <getopt.h>
#include <sstream>
#include <cstdlib>
#include <ctime>

int Argument::filtering( 0 );
int Argument::reconstruction( 0 );
int Argument::capacitated( 0 );
int Argument::lagrangian( 0 );
int Argument::mip_solver( 0 );
int Argument::moga( 0 );
int Argument::num_individuals( 100 );
int Argument::num_generations( 20 );
int Argument::grasp( 0 );
int Argument::num_directions( 5 );
int Argument::local_search( 0 );
int Argument::search_depth( 8 );
int Argument::paving_grasp( 1 );
int Argument::paving_directions( 50 );
int Argument::interactive( 0 );
int Argument::mode_export( 0 );
int Argument::verbose( 0 );
int Argument::help( 0 );
unsigned int Argument::random_seed( 0 );
double Argument::Pc( 0.9 );
double Argument::Pm( 0.5 );
double Argument::alpha( 0.8 );
double Argument::paving_alpha( 0.5 );
std::string Argument::filename;

// getopt long options array
static const struct option long_options[] = {
	{ "filtering",         no_argument,       &Argument::filtering,         1                               },
	{ "reconstruction",    no_argument,       &Argument::reconstruction,    1                               },
	{ "capacitated",       no_argument,       &Argument::capacitated,       1                               },
	{ "lagrangian",        no_argument,       &Argument::lagrangian,        1                               },
	{ "mip-solver",        no_argument,       &Argument::mip_solver,        1                               },
	{ "moga",              no_argument,       &Argument::moga,              1                               },
	{ "individuals",       required_argument, 0,                            Argument::id_num_individuals    },
	{ "generations",       required_argument, 0,                            Argument::id_num_generations    },
	{ "Pc",                required_argument, 0,                            Argument::id_Pc                 },
	{ "Pm",                required_argument, 0,                            Argument::id_Pm                 },
	{ "grasp",             no_argument,       &Argument::grasp,             1                               },
	{ "directions",        required_argument, 0,                            Argument::id_num_directions     },
	{ "alpha",             required_argument, 0,                            Argument::id_alpha              },
	{ "local-search",      no_argument,       &Argument::local_search,      1                               },
	{ "search-depth",      required_argument, 0,                            Argument::id_search_depth       },
	{ "paving-grasp",      no_argument,       &Argument::paving_grasp,      1                               },
	{ "no-paving-grasp",   no_argument,       &Argument::paving_grasp,      0                               },
	{ "paving-alpha",      required_argument, 0,                            Argument::id_paving_alpha       },
	{ "paving-directions", required_argument, 0,                            Argument::id_paving_directions  },
	{ "random-seed",       required_argument, 0,                            Argument::id_random_seed        },
	{ "interactive",       no_argument,       &Argument::interactive,       1                               },
	{ "export",            no_argument,       &Argument::mode_export,       1                               },
	{ "verbose",           no_argument,       &Argument::verbose,           1                               },
	{ "quiet",             no_argument,       &Argument::verbose,           0                               },
	{ "help",              no_argument,       &Argument::help,              1                               },
	{ 0, 0, 0, 0 }
};

void Argument::parse( int argc, char *argv[] )
{
	int next_option;
	int option_index( 0 );

	const char * const short_options = "frvqc";

	do
	{
		next_option = getopt_long( argc, argv, short_options, long_options, &option_index );
		switch ( next_option )
		{
			case 'f':
				filtering = 1;
				break;

			case 'r':
				reconstruction = 1;
				break;

			case 'c':
				capacitated = 1;
				break;

			case 'v':
				verbose = 1;
				break;

			case 'q':
				verbose = 0;
				break;

			case id_num_individuals:
				std::istringstream( optarg ) >> num_individuals;
				break;

			case id_num_generations:
				std::istringstream( optarg ) >> num_generations;
				break;

			case id_Pc:
				std::istringstream( optarg ) >> Pc;
				break;

			case id_Pm:
				std::istringstream( optarg ) >> Pm;
				break;

			case id_num_directions:
				std::istringstream( optarg ) >> num_directions;
				break;

			case id_alpha:
				std::istringstream( optarg ) >> alpha;
				break;

			case id_search_depth:
				std::istringstream( optarg ) >> search_depth;
				break;

			case id_paving_alpha:
				std::istringstream( optarg ) >> paving_alpha;
				break;

			case id_paving_directions:
				std::istringstream( optarg ) >> paving_directions;
				break;

			case id_random_seed:
				std::istringstream( optarg ) >> random_seed;
				break;

			case 0:
			case -1:
				break;

			default:
				abort();
		}
	}
	while ( next_option != -1 );

	if ( optind < argc )
	{
		filename = argv[optind];
	}

	if ( random_seed == 0 )
	{
		random_seed = std::time( 0 );
	}

	if ( capacitated )
	{
		moga = 1;
	}
	else
	{
		if ( local_search )
		{
			std::cerr << "UFLP : Local search disabled." << std::endl;
			local_search = 0;
		}

		if ( paving_grasp )
		{
			paving_grasp = 0;
		}
	}
}

void Argument::print( std::ostream & os )
{
	os
		<< "File: " << filename << std::endl
		<< "Options:" << std::endl
		<< "\tfiltering      = " << filtering       << std::endl
		<< "\treconstruction = " << reconstruction  << std::endl
		<< "\tcapacitated    = " << capacitated     << std::endl
		<< "\tmip-solver     = " << mip_solver      << std::endl
		<< "\tmoga           = " << moga            << std::endl;

	if ( moga )
	{
		os
			<< "\tindividuals    = " << num_individuals << std::endl
			<< "\tgenerations    = " << num_generations << std::endl
			<< "\tPc             = " << Pc              << std::endl
			<< "\tPm             = " << Pm              << std::endl
			<< "\tgrasp          = " << grasp           << std::endl;

		if ( grasp )
		{
			os
				<< "\tdirections     = " << num_directions  << std::endl
				<< "\talpha          = " << alpha           << std::endl;
		}

		if ( capacitated )
		{
			os
				<< "\tlocal-search      = " << local_search      << std::endl;

			if ( local_search )
			{
				os
					<< "\tsearch-depth      = " << search_depth      << std::endl;
			}

			os
				<< "\tpaving-grasp      = " << paving_grasp  << std::endl;
		}

		if ( paving_grasp )
		{
			os
				<< "\tpaving-alpha      = " << paving_alpha      << std::endl
				<< "\tpaving-directions = " << paving_directions << std::endl;
		}

		os
			<< "\trandom-seed    = " << random_seed     << std::endl;
	}

	os
		<< "\tinteractive    = " << interactive     << std::endl
		<< "\texport         = " << mode_export     << std::endl
		<< "\tverbose        = " << verbose         << std::endl
		<< std::endl;
}

void Argument::usage( const char * program_name, std::ostream & os )
{
	os
		<< "Command is: " << program_name << " <instance> OPTIONS" << std::endl
		<< "## OPTIONS ##" << std::endl
		<< "-f   for filtering method"        << std::endl
		<< "-r   for reconstruction method"   << std::endl
		<< "-c   for capacitated problem"     << std::endl
		<< "--mip-solver  to use MIP solver"  << std::endl
		<< "--moga        to use MOGA"        << std::endl
		<< "--individuals <num_individuals>"  << std::endl
		<< "--generations <num_generations>"  << std::endl
		<< "--Pc          <proba_crossover>"  << std::endl
		<< "--Pm          <proba_mutation>"   << std::endl
		<< "--grasp       to use GRASP"       << std::endl
		<< "--directions  <num_directions>"   << std::endl
		<< "--alpha       <GRASP_greediness>" << std::endl
		<< "--local-search    to use local search" << std::endl
		<< "--search-depth    <depth>"             << std::endl
		<< "--no-paving-grasp    to not use GRASP for paving" << std::endl
		<< "--paving-directions  <num_directions>"     << std::endl
		<< "--paving-alpha       <GRASP_greediness>"   << std::endl
		<< "--random-seed <random_seed>"      << std::endl
		<< "--interactive for interactive mode" << std::endl
		<< "--verbose     for verbose mode"   << std::endl
		<< "--export      to export results"  << std::endl;
}

