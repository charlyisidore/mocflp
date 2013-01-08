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

/*!
* \file Argument.hpp
* \brief Class to handle program options.
* \author Charly LERSTEAU
* \date 25 December 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This class uses getopt to handle standard POSIX arguments.
*
*/

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <iostream>
#include <string>

/*! \class Argument
* \brief Class to handle POSIX program options easily using getopt.
*
* This class parses and represents arguments values.
*/
struct Argument
{
	/*!
	*	\brief Parse the arguments.
	*
	*	\param[in] argc : The number of arguments.
	*	\param[in] argv : The array of strings of the arguments.
	*/
	static void parse( int argc, char * argv[] );

	/*!
	*	\brief Display arguments parsed to see what the program has understood.
	*
	*	\param[in] os : An output stream.
	*/
	static void print( std::ostream & os = std::cout );

	/*!
	*	\brief Display a short user manual.
	*
	*	\param[in] program_name : The executable name (use argv[0]).
	*	\param[in] os : An output stream.
	*/
	static void usage( const char * program_name, std::ostream & os = std::cout );

	// Integer or boolean parameters
	static int
		filtering,
		reconstruction,
		capacitated,
		lagrangian,
		mip_solver,
		moga,
		num_individuals,
		num_generations,
		grasp,
		num_directions,
		local_search,
		search_depth,
		paving_grasp,
		paving_directions,
		interactive,
		mode_export,
		verbose,
		help;

	// Positive integer parameters
	static unsigned int
		random_seed;

	// Floating point parameters
	static double
		Pc,
		Pm,
		alpha,
		paving_alpha;

	// Identifiers
	enum
	{
		id_random_seed = 0x100,
		id_num_individuals,
		id_num_generations,
		id_Pc,
		id_Pm,
		id_num_directions,
		id_alpha,
		id_search_depth,
		id_paving_alpha,
		id_paving_directions
	};

	// Instance file name
	static std::string filename;
};

#endif
