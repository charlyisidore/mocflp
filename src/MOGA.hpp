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
* \file MOGA.hpp
* \brief Class of the \c multi-objective genetic algorithm.
* \author Charly LERSTEAU
* \date 28 December 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This class represents a \c multi-objective genetic algorithm.
*
*/

#ifndef MOGA_H
#define MOGA_H

#include <iostream>
#include <list>
#include <vector>
#include <utility>
#include <cstdio>

#include "Data.hpp"
#include "Box.hpp"
#include "Solution.hpp"
#include "Functions.hpp"

struct individual;

/*! \class MOGA
* \brief Class to represent a \c multi-objective genetic algorithm.
*
*  This class represents a \c multi-objective genetic algorithm.
*/
// TODO: with capacities
class MOGA
{
public:
	/*!
	*	\brief Default Constructor of the class \c MOGA.
	*
	*	\param[in] box : A \c Box object in which one ones runs the MOGA algorithm.
	*/
	MOGA(Box &box, int num_individuals, int num_generations, double Pc, double Pm);

	/*!
	*	\brief A method to compute solutions into a \c Box.
	*/
	void compute();

	/*!
	*	\brief A method to print information (gnuplot).
	*/
	void print() const;

	/*!
	*	\brief Getter for the number of objectives.
	*	\return A int as the number of objectives.
	*/
	int getNbObjective() const;

	std::list<Solution> solutions_; /*!< A list of \c Solution */

	FILE * pipe_fp_; /*!< A pointer to a gnuplot pipe */

private:
	Data &data_;          /*!< A reference to the \c Data of this \c Box */
	int num_individuals_; /*!< Number of individuals */
	int num_generations_; /*!< Number of generations */
	double Pc_;           /*!< Probability of crossover */
	double Pm_;           /*!< Probability of mutation */

	int num_bits_;           /*!< Size of a chromosome */
	std::vector<int> fac_;   /*!< A vector of indices of open facilities */
	std::vector<int> cust_;  /*!< A vector of indices of which customers are to be assigned */
	std::vector<double> originZ_; /*!< Origin coordinates of the box */
	std::vector<individual> population_; /*!< A vector of \c individuals which represents the population */

	// Build an initial population.
	void initialization();

	// Make a selection of two individuals (by index).
	std::pair<int, int> selection();

	// Make a crossover between two individuals and return the children.
	std::pair<individual, individual> crossover( const individual & i1, const individual & i2 ) const;

	// Do a mutation on an individual.
	void mutation( individual & ind ) const;

	// Repair an individual.
	void repair( individual & ind ) const;

	// Remove weak individuals.
	void elitism();

	// Select the best between two individuals in the population (by index).
	int battle( int i1, int i2 ) const;

	// Add costs to objectives.
	void add_cost( individual & ind, int c, int f ) const;

	// Subtract costs to objectives.
	void subtract_cost( individual & ind, int c, int f ) const;

	// Compute ranking and crowding.
	void compute_ranking();

	// Test if individual 1 dominates individual 2.
	bool dominates( const individual & i1, const individual & i2 ) const;

	// Compute customer and facility indices from an arbitrary index.
	void index_to_cust_fac( int p, int & c, int & f ) const;

	// Re-compute objective.
	void recompute_obj( individual & ind ) const;

	// Check if individual is valid (constraints and objective).
	bool is_valid( individual & ind ) const;
};

// Representation of an individual.
struct individual
{
	individual( int num_objectives, int num_bits ) :
		obj( num_objectives, 0 ), chr( num_bits, false ),
		rank( 0 ), crowding( 0 ) {}

	// Main information
	std::vector<double> obj; // Objective
	std::vector<bool> chr;   // Chromosome

	// Additional information
	int rank;        // Rank
	double crowding; // Crowding
};

std::ostream & operator << ( std::ostream & os, const individual & ind );

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline int MOGA::getNbObjective() const
{
	return data_.getNbObjective();
}

#endif
