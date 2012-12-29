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

#include "MOGA.hpp"
#include "Argument.hpp"
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <cstdio>

// To sort individuals by rank and crowding.
struct compare_ranking
{
	bool operator () ( const individual & i1, const individual & i2 ) const
	{
		if ( i1.rank < i2.rank ) return true;
		else if ( i2.rank < i1.rank ) return false;
		else if ( i2.crowding < i1.crowding ) return true;
		else if ( i1.crowding < i2.crowding ) return false;
		else return false;
	}
};

// To sort individuals according to objective k.
struct compare_objective
{
	const std::vector<individual> & population;
	int k;

	compare_objective( const std::vector<individual> & population, int k ) :
		population( population ), k( k ) {}

	bool operator () ( int i1, int i2 ) const
	{
		return population[i1].obj[k] < population[i2].obj[k];
	}
};

MOGA::MOGA(Box &box, int num_individuals, int num_generations, double Pc, double Pm) :
	data_( box.getData() ),
	num_individuals_( num_individuals ),
	num_generations_( num_generations ),
	Pc_( Pc ),
	Pm_( Pm )
{
	// Save the origin (unsubtractable costs).
	originZ_.resize( getNbObjective() );
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		originZ_[k] = box.getOriginZ(k);
	}

	// Get all open facilities indices.
	for ( unsigned int i = 0; i < data_.getnbFacility(); ++i )
	{
		if ( box.isOpened(i) )
		{
			fac_.push_back(i);
		}
	}

	// Get all non-assigned customers indices.
	for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
	{
		if ( !box.isAssigned(i) )
		{
			cust_.push_back(i);
		}
	}

	// Number of bits required to represent a solution.
	num_bits_ = fac_.size() * cust_.size();

	// Pre-allocate memory for the population.
	population_.reserve( 2 * num_individuals_ );
}

// Main algorithm.
void MOGA::compute()
{
	if ( num_bits_ == 0 ) return;

	double p;

	// Build initial population.
	initialization();

	// Do generations.
	for ( int i = 0; i < num_generations_; ++i )
	{
		while ( (int)population_.size() < 2 * num_individuals_ )
		{
			// Select a pair of individuals.
			std::pair<int, int> s( selection() );

			p = (double)std::rand() / (double)RAND_MAX;
			if ( p <= Pc_ )
			{
				// Cross them.
				std::pair<individual, individual> children(
					crossover( population_[s.first], population_[s.second] ) );

				p = (double)std::rand() / (double)RAND_MAX;
				if ( p <= Pm_ )
				{
					// Mutate children.
					mutation( children.first );
					mutation( children.second );
				}

				// Repair children.
				repair( children.first );
				repair( children.second );

				population_.push_back( children.first );
				population_.push_back( children.second );
			}
			else
			{
				population_.push_back( population_[s.first] );
				population_.push_back( population_[s.second] );
			}
		}

		// Keep the best individuals.
		elitism();
		print();
	}

	// FINISHED - KEEP BEST VALUES

	// Keep only "first rank" solutions.
	solutions_.clear();
	for ( unsigned int i = 0; i < population_.size(); ++i )
	{
		if ( population_[i].rank == 1 )
		{
			// Check objective and constraints
			is_valid( population_[i] );

			Solution sol( getNbObjective() );
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				sol.setObj( k, population_[i].obj[k] );
			}
			solutions_.push_back( sol );
		}
	}
}

void MOGA::initialization()
{
	// Build all individuals with a construction procedure.
	population_.clear();
	for ( int i = 0; i < num_individuals_; ++i )
	{
		individual ind( getNbObjective(), num_bits_ );
		ind.obj = originZ_;

		// Select one assignment per customer.
		for ( unsigned int c = 0; c < cust_.size(); ++c )
		{
			int f = std::rand() % fac_.size();
			int begin = c * fac_.size();

			ind.chr[begin + f] = true;
			add_cost( ind, c, f ); // Add costs
		}

		population_.push_back( ind );
	}

	// Compute all rank and crowding.
	compute_ranking();
}

std::pair<int, int> MOGA::selection()
{
	int i1, i2, a, b;

	a  = std::rand() % num_individuals_;
	b  = std::rand() % num_individuals_;
	i1 = battle( a, b );

	a  = std::rand() % num_individuals_;
	b  = std::rand() % num_individuals_;
	i2 = battle( a, b );

	return std::make_pair( i1, i2 );
}

std::pair<individual, individual> MOGA::crossover( const individual & i1, const individual & i2 ) const
{
	individual c1( getNbObjective(), num_bits_ ),
	           c2( getNbObjective(), num_bits_ );

	int p = std::rand() % num_bits_;

	// Do crossover
	std::copy( i1.chr.begin(), i1.chr.begin() + p, c1.chr.begin() );
	std::copy( i2.chr.begin(), i2.chr.begin() + p, c2.chr.begin() );
	std::copy( i1.chr.begin() + p, i1.chr.end(), c2.chr.begin() + p );
	std::copy( i2.chr.begin() + p, i2.chr.end(), c1.chr.begin() + p );

	// Reevaluate from zero
	recompute_obj( c1 );
	recompute_obj( c2 );

	return std::make_pair( c1, c2 );
}

void MOGA::mutation( individual & ind ) const
{
	int c, f;
	int p = std::rand() % num_bits_;
	index_to_cust_fac( p, c, f ); // Retrieve corresponding customer and facility for index p.

	ind.chr[p].flip();

	if ( ind.chr[p] ) add_cost( ind, c, f ); // Add costs
	else              subtract_cost( ind, c, f ); // Subtract costs
}

void MOGA::repair( individual & ind ) const
{
	// For each customer, test if there is one assignement alone or not.
	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		int begin = c * fac_.size();
		std::vector<int> assigned;

		// Retrieve assignments for customer c
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			if ( ind.chr[begin + f] )
			{
				assigned.push_back(f);
			}
		}

		// If there are more assignments, select one
		if ( assigned.size() > 1 )
		{
			unsigned int r = std::rand() % assigned.size();
			for ( unsigned int i = 0; i < assigned.size(); ++i )
			{
				if ( i != r )
				{
					int f = assigned[i];
					ind.chr[begin + f] = false;
					subtract_cost( ind, c, f ); // Subtract costs
				}
			}
		}
		// If no assignment, select one
		else if ( assigned.size() < 1 )
		{
			int f = std::rand() % fac_.size();
			ind.chr[begin + f] = true;
			add_cost( ind, c, f ); // Add costs
		}
	}
}

void MOGA::elitism()
{
	// Compute all rank and crowding.
	compute_ranking();

	// Erase all individuals that are in the bottom.
	population_.resize( num_individuals_, individual( getNbObjective(), num_bits_ ) );

	// Re-Compute all rank and crowding.
	compute_ranking();
}

int MOGA::battle( int i1, int i2 ) const
{
	if ( population_[i1].rank < population_[i2].rank ) return i1;
	else if ( population_[i2].rank < population_[i1].rank ) return i2;
	else if ( population_[i2].crowding < population_[i1].crowding ) return i1;
	else if ( population_[i1].crowding < population_[i2].crowding ) return i2;
	else return ( std::rand() % 2 ) ? i1 : i2;
}

void MOGA::add_cost( individual & ind, int c, int f ) const
{
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		ind.obj[k] += data_.getAllocationObjCost(k, cust_[c], fac_[f]);
	}
}

void MOGA::subtract_cost( individual & ind, int c, int f ) const
{
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		ind.obj[k] -= data_.getAllocationObjCost(k, cust_[c], fac_[f]);
	}
}

void MOGA::compute_ranking()
{
	// Reset rank and crowding.
	for ( unsigned int i = 0; i < population_.size(); ++i )
	{
		population_[i].rank = 0;
		population_[i].crowding = 0;
	}

	// While there is elements with no rank
	for ( unsigned int r = 1, num_hidden = 0; num_hidden < population_.size(); ++r )
	{
		// Compare each non-ranked solution with other non-ranked solutions
		std::vector<bool> nondominated( population_.size(), true );
		std::vector<int> same_rank;
		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			// If i has no rank assigned
			if ( population_[i].rank == 0 )
			{
				for ( std::size_t j = 0; j < population_.size() && nondominated[i]; ++j )
				{
					// If j has no rank assigned
					if ( population_[j].rank == 0 && i != j )
					{
						// If j dominates i
						if ( dominates( population_[j], population_[i] ) )
						{
							nondominated[i] = false;
						}
					}
				}
			}
		}

		// Assign ranks to non-dominated solutions hide them
		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			if ( population_[i].rank == 0 && nondominated[i] )
			{
				population_[i].rank = r;
				++num_hidden;
				same_rank.push_back( i ); // Retrieve same-rank indices for crowding measure
			}
		}

		// Compute crowding measure
		for ( int k = 0; k < getNbObjective(); ++k )
		{
			// Sort to have first which have objective k to min and last to max
			std::sort( same_rank.begin(), same_rank.end(), compare_objective( population_, k ) );

			double zI = population_[same_rank.front()].obj[k],
			       zA = population_[same_rank.back() ].obj[k],
			       denominator = zA - zI;

			// First and last are infinity
			population_[same_rank.front()].crowding = std::numeric_limits<double>::infinity();
			population_[same_rank.back() ].crowding = std::numeric_limits<double>::infinity();

			for ( unsigned int i = 1; i < same_rank.size()-1; ++i )
			{
				if ( population_[same_rank[i]].crowding == zI ||
				     population_[same_rank[i]].crowding == zA )
				{
					population_[same_rank[i]].crowding = std::numeric_limits<double>::infinity();
				}
				else if ( population_[same_rank[i]].crowding != std::numeric_limits<double>::infinity() )
				{
					population_[same_rank[i]].crowding +=
						( population_[same_rank[i+1]].obj[k] - population_[same_rank[i-1]].obj[k] )
						/ denominator;
				}
			}
		}
	}

	// Sort to have best solutions on the top.
	std::sort( population_.begin(), population_.end(), compare_ranking() );
}

bool MOGA::dominates( const individual & i1, const individual & i2 ) const
{
	std::vector<double>::const_iterator first1( i1.obj.begin() ),
	                                    first2( i2.obj.begin() ),
	                                    last1( i1.obj.end() );

	bool result = false;
	for ( ; first1 != last1; ++first1, ++first2 )
	{
		if ( *first2 < *first1 ) return false;
		result = result || ( *first1 < *first2 );
	}
	return result;
}

void MOGA::index_to_cust_fac( int p, int & c, int & f ) const
{
	std::div_t d = std::div( p, fac_.size() );
	c = d.quot;
	f = d.rem;
}

void MOGA::recompute_obj( individual & ind ) const
{
	ind.obj = originZ_;
	for ( unsigned int p = 0; p < ind.chr.size(); ++p )
	{
		if ( ind.chr[p] )
		{
			int c, f;
			index_to_cust_fac( p, c, f );
			add_cost( ind, c, f );
		}
	}
}

bool MOGA::is_valid( individual & ind ) const
{
	bool valid( true );

	// Check objective
	std::vector<double> old_obj = ind.obj;
	recompute_obj( ind );
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		if ( old_obj[k] != ind.obj[k] )
		{
			std::cerr << "Error: invalid objective, value is " << old_obj[k] << ", expected was " << ind.obj[k] << std::endl;
			valid = false;
		}
	}

	// For each customer, test if there is one assignement alone or not.
	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		// Count assignments for customer c
		int num_assigned = std::count(
			ind.chr.begin() + c * fac_.size(),
			ind.chr.begin() + (c+1) * fac_.size(),
			true );

		if ( num_assigned > 1 )
		{
			std::cerr << "Error: too many assignments for a customer, " << num_assigned << " counted" << std::endl;
			valid = false;
		}
		else if ( num_assigned < 1 )
		{
			std::cerr << "Error: no assignment for a customer, " << num_assigned << " counted" << std::endl;
			valid = false;
		}
	}
	return valid;
}

void MOGA::print() const
{
#if 0
	if (Argument::verbose)
	{
		std::cout << "#### Population ####" << std::endl;
		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			std::cout << population_[i] << std::endl;
		}
		std::cout << "# cust: " << cust_.size() << ", fac: " << fac_.size() << std::endl;
	}
#endif

	if (pipe_fp_)
	{
		int rank_max( 0 );

		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			rank_max = std::max( population_[i].rank, rank_max );
		}

		fputs( "plot ", pipe_fp_ );

		for ( int r = rank_max; r > 1; --r )
		{
			fprintf( pipe_fp_, "'-' title '%d', ", r );
		}
		fputs( "'-' title '1' linecolor rgb 'blue' pt 9\n", pipe_fp_ );

		for ( int r = rank_max; r >= 1; --r )
		{
			for ( unsigned int i = 0; i < population_.size(); ++i )
			{
				if ( population_[i].rank == r )
				{
					for ( int k = 0; k < getNbObjective(); ++k )
					{
						fprintf( pipe_fp_, "%f ", population_[i].obj[k] );
					}
					fputs( "\n", pipe_fp_ );
				}
			}
			fputs( "e\n", pipe_fp_ );
		}
		fputs( "\n", pipe_fp_ );
	}
}

std::ostream & operator << ( std::ostream & os, const individual & ind )
{
	os << ind.rank << ' ' << ind.crowding << "  ";
	for ( unsigned int k = 0; k < ind.obj.size(); ++k )
	{
		os << ind.obj[k] << ' ';
	}
	for ( unsigned int i = 0; i < ind.chr.size(); ++i )
	{
		os << ind.chr[i];
	}
	return os;
}

