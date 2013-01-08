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

#include "LocalSearch.hpp"
#include <iostream>
#include <vector>

LocalSearch::LocalSearch( Data &data, const std::vector<int> & cust, const std::vector<int> & fac ) :
	pipe_fp_( 0 ),
	data_( data ),
	cust_( cust ),
	fac_( fac )
{
}

void LocalSearch::search( const individual & ind )
{
	lssolution sol( to_lssolution( ind ) );

	for ( int depth = 0; depth < Argument::search_depth; ++depth )
	{
		std::vector<lssolution> list_sol;
		for ( unsigned int i = 0; i < data_.getnbCustomer(); ++i )
		{
			double demand( data_.getCustomer(i).getDemand() );

			for ( unsigned int j1 = 0; j1 < data_.getnbFacility(); ++j1 )
			{
				if ( sol.x[ index_of( i, j1 ) ] )
				{
					for ( unsigned int j2 = 0; j2 < data_.getnbFacility(); ++j2 )
					{
						if ( j1 != j2 && sol.q[j2] >= demand )
						{
							lssolution assigned( sol );
							unassign( assigned, i, j1 );
							assign( assigned, i, j2 );
							if ( dominates( assigned, sol ) )
							{
								list_sol.push_back( assigned );
							}
						}
					}
				}
			}
		}

		if ( list_sol.empty() ) break;

		print( list_sol );

		int r = std::rand() % list_sol.size();
		sol = list_sol[r];

		// Save solutions
		for ( unsigned int i = 0; i < list_sol.size(); ++i )
		{
			if ( list_sol[i].is_feasible() )
			{
				Solution s( getNbObjective() );
				for ( int k = 0; k < getNbObjective(); ++k )
				{
					s.setObj( k, list_sol[i].obj[k] );
				}
				solutions_.push_back( s );
			}
		}
	}
}

lssolution LocalSearch::to_lssolution( const individual & ind ) const
{
	lssolution sol( this );

	for ( unsigned int c = 0; c < cust_.size(); ++c )
	{
		for ( unsigned int f = 0; f < fac_.size(); ++f )
		{
			if ( ind.chr[c * fac_.size() + f] )
			{
				assign( sol, c, fac_[f] );
			}
		}
	}
	return sol;
}

bool LocalSearch::dominates( const lssolution & i1, const lssolution & i2 ) const
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

void LocalSearch::assign( lssolution & sol, int c, int f ) const
{
	if ( sol.x[ index_of(c, f) ] == true ) return;

	sol.x[ index_of(c, f) ] = true;
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		sol.obj[k] += data_.getAllocationObjCost( k, cust_[c], f );
	}

	if ( sol.nc[f] == 0 )
	{
		for ( int k = 0; k < getNbObjective(); ++k )
		{
			sol.obj[k] += data_.getFacility( f ).getLocationObjCost( k );
		}
	}
	++sol.nc[f];

	sol.q[f] -= data_.getCustomer(cust_[c]).getDemand();
}

void LocalSearch::unassign( lssolution & sol, int c, int f ) const
{
	if ( sol.x[ index_of(c, f) ] == false ) return;

	sol.x[ index_of(c, f) ] = false;
	for ( int k = 0; k < getNbObjective(); ++k )
	{
		sol.obj[k] -= data_.getAllocationObjCost(k, cust_[c], f);
	}

	--sol.nc[f];
	if ( sol.nc[f] == 0 )
	{
		for ( int k = 0; k < getNbObjective(); ++k )
		{
			sol.obj[k] -= data_.getFacility( f ).getLocationObjCost( k );
		}
	}

	sol.q[f] += data_.getCustomer(cust_[c]).getDemand();
}

void LocalSearch::print( const std::vector<lssolution> & list_sol ) const
{
	if (pipe_fp_)
	{
		std::fputs( "plot '-' title '1' linecolor rgb 'blue' pt 9\n", pipe_fp_ );

		for ( unsigned int i = 0; i < list_sol.size(); ++i )
		{
			for ( int k = 0; k < getNbObjective(); ++k )
			{
				std::fprintf( pipe_fp_, "%f ", list_sol[i].obj[k] );
			}
			std::fputs( "\n", pipe_fp_ );
		}
		std::fputs( "e\n", pipe_fp_ );
	}
}

std::ostream & operator << ( std::ostream & os, const lssolution & sol )
{
	for ( unsigned int k = 0; k < sol.obj.size(); ++k )
	{
		os << sol.obj[k] << ' ';
	}
	/*for ( unsigned int i = 0; i < sol.x.size(); ++i )
	{
		os << sol.x[i];
	}*/
	return os;
}

