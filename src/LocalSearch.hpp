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
* \file LocalSearch.hpp
* \brief Class to make a local search.
* \author Charly LERSTEAU
* \date 7 January 2013
* \version 1.1
* \copyright GNU General Public License 
*
* This class makes a Pareto local search for post-optimization.
*
*/

#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include "Data.hpp"
#include "Box.hpp"
#include "Solution.hpp"
#include "MOGA.hpp"
#include "Argument.hpp"

struct individual;
struct lssolution;

/*! \class LocalSearch
* \brief Class to make post-optimization.
*
* This class makes a Pareto local search for post-optimization.
*/
class LocalSearch
{
public:
	/*!
	*	\brief Constructor
	*/
	LocalSearch( Data &data, const std::vector<int> & cust, const std::vector<int> & fac );

	/*!
	*	\brief Do the local search
	*/
	void search( const individual & ind );

	/*!
	*	\brief Convert MOGA individual to local search solution.
	*/
	lssolution to_lssolution( const individual & ind ) const;

	/*!
	*	\brief Test if assignment 1 to customer c dominates assignment 2.
	*/
	bool dominates( const lssolution & i1, const lssolution & i2 ) const;

	/*!
	*	\brief Add costs to objectives of a solution.
	*	\param[out] sol : The solution for which costs will be added.
	*	\param[in] c : Index of the customer assigned.
	*	\param[in] f : Index of the facility assigned.
	*/
	void assign( lssolution & sol, int c, int f ) const;

	/*!
	*	\brief Subtract costs to objectives of a solution.
	*	\param[out] sol : The solution for which costs will be subtracted.
	*	\param[in] c : Index of the customer unassigned.
	*	\param[in] f : Index of the facility unassigned.
	*/
	void unassign( lssolution & sol, int c, int f ) const;

	/*!
	*	\brief Getter for the number of objectives.
	*	\return A int as the number of objectives.
	*/
	int getNbObjective() const;

	/*!
	*	\brief Compute customer and facility indices from an arbitrary index.
	*	\param[in] p : General index in the chromosome array.
	*	\param[out] c : Index of the customer.
	*	\param[out] f : Index of the facility.
	*/
	void index_to_cust_fac( int p, int & c, int & f ) const;

	/*!
	*	\brief Compute a global index using customer and facility indices.
	*	\param[in] c : Index of the customer.
	*	\param[in] f : Index of the facility.
	*/
	int index_of( int c, int f ) const;

	/*!
	*	\brief A method to print information (gnuplot).
	*/
	void print( const std::vector<lssolution> & list_sol ) const;

	std::list<Solution> solutions_; /*!< A list of \c Solution */
	FILE * pipe_fp_; /*!< A pointer to a gnuplot pipe */

private:
	Data &data_;          /*!< A reference to the \c Data of this \c Box */
	std::vector<int> cust_;  /*!< A vector of indices of which customers are to be assigned */
	std::vector<int> fac_;   /*!< A vector of indices of initially open facilities */

	friend class lssolution;
};

// Representation of an solution.
struct lssolution
{
	lssolution( const LocalSearch * m ) :
		obj( m->getNbObjective(), 0 ),
		x( m->cust_.size() * m->data_.getnbFacility(), false ),
		q( m->data_.getnbFacility(), 0 ),
		nc( m->data_.getnbFacility(), 0 )
	{
		for ( unsigned int i = 0; i < m->data_.getnbFacility(); ++i )
		{
			q[i] = m->data_.getFacility(i).getCapacity();
		}
	}

	// Main information
	std::vector<double> obj; // Objective
	std::vector<bool>   x;   // x_ij (=> y_j is known)

	// Additional information
	std::vector<double> q; // Remaining capacity of facility j
	std::vector<int>   nc; // Number of customers assigned to j

	// Check for capacity constraints
	bool is_feasible() const
	{
		if ( !Argument::capacitated ) return true;
		for ( unsigned int i = 0; i < q.size(); ++i )
		{
			if ( q[i] < 0 )
				return false;
		}
		return true;
	}
};

std::ostream & operator << ( std::ostream & os, const lssolution & sol );

////////////////////////////////////////////////////////////////////////////////

// Inline functions

inline int LocalSearch::getNbObjective() const
{
	return data_.getNbObjective();
}

inline void LocalSearch::index_to_cust_fac( int p, int & c, int & f ) const
{
	std::div_t d = std::div( p, data_.getnbFacility() );
	c = d.quot;
	f = d.rem;
}

inline int LocalSearch::index_of( int c, int f ) const
{
	return c * data_.getnbFacility() + f;
}

#endif
