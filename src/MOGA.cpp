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

// Macro constant which checks solutions from zero (slower)
#define STRICT_CHECK 0

#include "MOGA.hpp"
#include "LocalSearch.hpp"
#include <limits>
#include <cstdio>

// To sort individuals by rank and crowding.
struct compare_ranking
{
	bool operator () ( const individual & i1, const individual & i2 ) const
	{
		bool feas1 = i1.is_feasible(),
		     feas2 = i2.is_feasible();

		if ( feas1 && !feas2 ) return true;
		else if ( feas2 && !feas1 ) return false;
		else if ( i1.rank < i2.rank ) return true;
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

	// LOCAL SEARCH
	solutions_.clear();
	if ( Argument::local_search )
	{
		LocalSearch local( data_, cust_, fac_ );
		local.pipe_fp_ = pipe_fp_;

		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			local.search( population_[i] );
			solutions_.merge( local.solutions_ );
		}
	}

	// FINISHED - KEEP BEST VALUES

	// Keep only "first rank" solutions.
	for ( unsigned int i = 0; i < population_.size(); ++i )
	{
#if STRICT_CHECK
		if ( population_[i].rank == 1 && is_feasible( population_[i] ) && population_[i].is_feasible() )
#else
		if ( population_[i].rank == 1 && population_[i].is_feasible() )
#endif
		{
#if STRICT_CHECK
			// Check objective
			is_valid( population_[i] );
#endif

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
		if ( Argument::grasp )
		{
			population_.push_back( initialization_grasp( Argument::alpha ) );
		}
		else
		{
			population_.push_back( initialization_random() );
		}
	}

	// Compute all rank and crowding.
	compute_ranking();
}

individual MOGA::initialization_random() const
{
	individual ind( this );

	// Select one assignment per customer.
	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		int f = std::rand() % fac_.size();
		assign( ind, c, f ); // Add costs
	}
	return ind;
}

individual MOGA::initialization_grasp( double alpha ) const
{
	individual ind( this );
	std::vector<int> p( cust_.size(), 0 );
	std::vector<double> u( fac_.size(), 0 );
	double umin, umax, ulimit, dir( 0.5 );
	int d( 0 );

	// Choose a direction (bi-objective)
	if ( Argument::num_directions > 1 )
	{
		d = std::rand() % Argument::num_directions;
		dir = (double)d / (Argument::num_directions - 1.);
	}

	// Initialize a random permutation to select customers in a random order (inside-out Fisher-Yates shuffle)
	for ( unsigned int i = 0; i < p.size(); ++i )
	{
		int j = std::rand() % (i+1);
		p[i] = p[j];
		p[j] = i;
	}

	for ( unsigned int i = 0; i < p.size(); ++i )
	{
		std::vector<int> RCL;
		int c( p[i] ), r;

		// Compute utility
		grasp_compute_utility( c, ind, u, umin, umax, dir );
		ulimit = umin + alpha * ( umax - umin );

		// Build RCL
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			if ( u[f] >= ulimit )
			{
				RCL.push_back( f );
			}
		}

		// Select an assignment
		r = std::rand() % RCL.size();
		assign( ind, c, RCL[r] ); // Add costs
	}
	return ind;
}

void MOGA::grasp_compute_utility( int c, const individual & ind, std::vector<double> & u, double & umin, double & umax, double dir ) const
{
	double demand_c( data_.getCustomer(cust_[c]).getDemand() );

	umin = std::numeric_limits<double>::infinity();
	umax = -std::numeric_limits<double>::infinity();

	for ( unsigned int f = 0; f < fac_.size(); ++f )
	{
		double cost_f     = 0,
		       capacity_f = ind.q[f];

		if ( Argument::num_directions > 0 && getNbObjective() == 2 )
		{
			// Assume we have two objectives
			cost_f =    dir     * data_.getAllocationObjCost(0, cust_[c], fac_[f])
			       + (1. - dir) * data_.getAllocationObjCost(1, cust_[c], fac_[f]);
		}
		else
		{
			// For more objectives, sum of costs
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				cost_f += data_.getAllocationObjCost(k, cust_[c], fac_[f]);
			}
		}

		// Add a tiny value to prevent division by zero
		cost_f += std::numeric_limits<double>::epsilon();

		if ( Argument::capacitated )
		{
			u[f] = ( capacity_f - demand_c ) / cost_f;
		}
		else
		{
			u[f] = 1. / cost_f;
		}

		// Warning: Negative utilities will give unfeasible solutions
		umin = std::min( u[f], umin );
		umax = std::max( u[f], umax );
	}
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
	individual c1( this ),
	           c2( this );

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

	// Partial evaluation
	if ( ind.chr[p] ) unassign( ind, c, f ); // Subtract costs
	else              assign( ind, c, f );   // Add costs
}

void MOGA::repair( individual & ind ) const
{
	// Save assignments into an integer (indices) vector
	std::vector<unsigned int> assignment( cust_.size() );

	// For each customer, test if there is one assignement alone or not.
	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		std::vector<int> list_assigned;

		// Retrieve assignments for customer c
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			if ( ind.chr[ index_of(c, f) ] )
			{
				list_assigned.push_back(f);
			}
		}

		// If there are more assignments, select one
		if ( list_assigned.size() > 1 )
		{
			unsigned int r = std::rand() % list_assigned.size();
			for ( unsigned int i = 0; i < list_assigned.size(); ++i )
			{
				if ( i != r )
				{
					int f = list_assigned[i];
					unassign( ind, c, f ); // Subtract costs
				}
			}
			assignment[c] = list_assigned[r];
		}
		// If no assignment, select one
		else if ( list_assigned.size() < 1 )
		{
			int f = std::rand() % fac_.size();
			assign( ind, c, f ); // Add costs
			assignment[c] = f;
		}
		// One assignment: OK but save it
		else
		{
			assignment[c] = list_assigned[0];
		}
	}

	// Search for violated capacities
	if ( Argument::capacitated )
	{
		// Find facilities for which capacity is violated
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			for ( unsigned int c = 0; ind.q[f] < 0 && c < assignment.size(); ++c )
			{
				// Find customers that cause capacity to be violated
				for ( unsigned int g = 0; assignment[c] == f && ind.q[f] < 0 && g < fac_.size(); ++g )
				{
					// Try to assign the customer to an other facility
					if ( ind.q[g] >= data_.getCustomer(cust_[c]).getDemand() )
					{
						unassign(ind, c, f);
						assign(ind, c, g);
						assignment[c] = g;
					}
				}
			}
		}
	}
}

void MOGA::elitism()
{
	// Compute all rank and crowding.
	compute_ranking();

	// Erase all individuals that are in the bottom.
	population_.resize( num_individuals_, individual( this ) );

	// Re-Compute all rank and crowding.
	compute_ranking();
}

int MOGA::battle( int i1, int i2 ) const
{
	bool feas1 = population_[i1].is_feasible(),
	     feas2 = population_[i2].is_feasible();

	if ( feas1 && !feas2 ) return i1;
	else if ( feas2 && !feas1 ) return i2;
	else if ( population_[i1].rank < population_[i2].rank ) return i1;
	else if ( population_[i2].rank < population_[i1].rank ) return i2;
	else if ( population_[i2].crowding < population_[i1].crowding ) return i1;
	else if ( population_[i1].crowding < population_[i2].crowding ) return i2;
	else return ( std::rand() % 2 ) ? i1 : i2;
}

void MOGA::assign( individual & ind, int c, int f ) const
{
	if ( !ind.chr[ index_of(c, f) ] )
	{
		ind.chr[ index_of(c, f) ] = true;
		for ( int k = 0; k < getNbObjective(); ++k )
		{
			ind.obj[k] += data_.getAllocationObjCost(k, cust_[c], fac_[f]);
		}
		ind.q[f] -= data_.getCustomer(cust_[c]).getDemand();
	}
}

void MOGA::unassign( individual & ind, int c, int f ) const
{
	if ( ind.chr[ index_of(c, f) ] )
	{
		ind.chr[ index_of(c, f) ] = false;
		for ( int k = 0; k < getNbObjective(); ++k )
		{
			ind.obj[k] -= data_.getAllocationObjCost(k, cust_[c], fac_[f]);
		}
		ind.q[f] += data_.getCustomer(cust_[c]).getDemand();
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

void MOGA::recompute_obj( individual & ind ) const
{
	// Reset residual capacities
	for ( unsigned int f = 0; f < fac_.size(); ++f )
	{
		ind.q[f] = data_.getFacility(fac_[f]).getCapacity();
	}

	// Reset objectives
	ind.obj = originZ_;
	for ( unsigned int p = 0; p < ind.chr.size(); ++p )
	{
		if ( ind.chr[p] )
		{
			int c, f;
			index_to_cust_fac( p, c, f );

			for ( int k = 0; k < getNbObjective(); ++k )
			{
				ind.obj[k] += data_.getAllocationObjCost(k, cust_[c], fac_[f]);
			}

			ind.q[f] -= data_.getCustomer(cust_[c]).getDemand();
		}
	}
}

bool MOGA::is_feasible( const individual & ind ) const
{
	// For each customer, test if there is one assignement alone or not.
	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		// Count assignments for customer c
		int num_assigned = std::count(
			ind.chr.begin() + index_of( c, 0 ),
			ind.chr.begin() + index_of( c+1, 0 ),
			true );

		if ( num_assigned > 1 )
		{
			std::cerr << "Error: too many assignments for a customer, " << num_assigned << " counted" << std::endl;
			return false;
		}
		else if ( num_assigned < 1 )
		{
			std::cerr << "Error: no assignment for a customer, " << num_assigned << " counted" << std::endl;
			return false;
		}
	}

	// Test if demands fit to capacities
	if ( Argument::capacitated )
	{
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			double capacity( data_.getFacility(fac_[f]).getCapacity() ),
			       demand( 0 );

			for ( unsigned int c = 0; c < cust_.size(); ++c )
			{
				if ( ind.chr[ index_of(c, f) ] )
				{
					demand += data_.getCustomer(cust_[c]).getDemand();
				}
			}

			if ( ind.q[f] != ( capacity - demand ) )
				std::cerr << "Error: expected residual capacity: " << (capacity - demand) << ", computed: " << ind.q[f] << std::endl;

			if ( demand > capacity )
				return false;
		}
	}
	return true;
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

	return valid;
}

void MOGA::print() const
{
	if (pipe_fp_)
	{
		std::vector< std::vector<int> > sol_of_rank;
		std::vector<bool> feasible( population_.size() );
		int rank_max( 0 );
		bool plotted_before( false );

		// Compute feasibility and rank max
		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			feasible[i] = population_[i].is_feasible();
			rank_max = std::max( population_[i].rank, rank_max );
		}

		// Sort population into subpopulations of same rank
		sol_of_rank.resize( rank_max+1 );
		for ( unsigned int i = 0; i < population_.size(); ++i )
		{
			if ( feasible[i] && population_[i].rank > 0 )
			{
				sol_of_rank[population_[i].rank-1].push_back( i );
			}
			else
			{
				sol_of_rank[rank_max].push_back( i );
			}
		}

		// Begin plot
		std::fputs( "plot ", pipe_fp_ );

		// Show the infeasible solutions in background
		if ( sol_of_rank[rank_max].size() > 0 )
		{
			std::fputs( "'-' title 'infeasible' linecolor rgb 'turquoise' pt 9", pipe_fp_ );
			plotted_before = true;
		}

		// Show the feasible solutions of rank > 1
		for ( unsigned int r = rank_max-1; r > 0; --r )
		{
			if ( sol_of_rank[r].size() > 0 )
			{
				if ( plotted_before == true )
					std::fputs( ", ", pipe_fp_ );

				std::fprintf( pipe_fp_, "'-' title '%d'", r );
				plotted_before = true;
			}
		}

		// Show the feasible solutions of rank 1 in foreground
		if ( sol_of_rank[0].size() > 0 )
		{
			if ( plotted_before == true )
				std::fputs( ", ", pipe_fp_ );

			std::fputs( "'-' title '1' linecolor rgb 'blue' pt 9", pipe_fp_ );
		}
		std::fputs( "\n", pipe_fp_ );

		// Transfer points
		for ( int r = rank_max; r >= 0; --r )
		{
			if ( sol_of_rank[r].size() > 0 )
			{
				for ( unsigned int i = 0; i < sol_of_rank[r].size(); ++i )
				{
					for ( int k = 0; k < getNbObjective(); ++k )
					{
						std::fprintf( pipe_fp_, "%f ", population_[sol_of_rank[r][i]].obj[k] );
					}
					std::fputs( "\n", pipe_fp_ );
				}
				std::fputs( "e\n", pipe_fp_ );
			}
		}

		std::fputs( "\n", pipe_fp_ );
	}
}

std::ostream & operator << ( std::ostream & os, const individual & ind )
{
	os << ind.rank << ' ' << ind.crowding << "  ";
	for ( unsigned int k = 0; k < ind.obj.size(); ++k )
	{
		os << ind.obj[k] << ' ';
	}

	unsigned int ncust = ind.chr.size() / ind.q.size();
	for ( unsigned int i = 0; i < ncust; ++i )
	{
		if ( i > 0 )
			os << '|';

		for ( unsigned int j = 0; j < ind.q.size(); ++j )
		{
			os << ind.chr[i * ind.q.size() + j];
		}
	}
	return os;
}

