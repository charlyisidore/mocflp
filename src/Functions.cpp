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

#include "Functions.hpp"
#include "MOGA.hpp"
#include "Argument.hpp"
#include <algorithm>
#include <limits>
#include <cstdio>

// Functor to sort an index array (of facilities) by decreasing capacity
struct compare_capacity
{
	compare_capacity(Data &data) : data_(data) {}
	bool operator() ( int i, int j ) const
	{
		return data_.getFacility(i).getCapacity() > data_.getFacility(j).getCapacity();
	}
	Data &data_;
};

// Functor to generate the identity permutation in a vector
struct generate_identity
{
	generate_identity() : i_(0) {}
	int operator() () { return i_++; }
	int i_;
};

long int createBox(std::vector<Box*> &vectorBox, Data &data)
{
	long int nbBoxComputed = 0;

	// Make a list of indices of facilities and sort them
	std::vector<int> sorted_fac( data.getnbFacility() );

	// sorted_fac = 0...n-1
	std::generate( sorted_fac.begin(), sorted_fac.end(), generate_identity() );

	// Compute Box list using a GRASP (for capacitated)
	if ( Argument::capacitated && Argument::paving_grasp )
	{
		initBoxCapacitated(vectorBox, data, sorted_fac);
	}
	// Add the first boxes with only one facility opened (for uncapacitated)
	else
	{
		Box *box0 = new Box(data);
		addChildren(box0, vectorBox, sorted_fac);
		delete box0;
	}
	
	for (unsigned int it = 0; it < vectorBox.size();)
	{
		++nbBoxComputed;

		// If the box gives an unfeasible subproblem
		if ( !vectorBox[it]->isFeasible() )
		{
			addChildren(vectorBox[it], vectorBox, sorted_fac);
			delete vectorBox[it];
			vectorBox.erase(vectorBox.begin() + it);
		}
		//If the box is dominated by its origin by all the existing boxes
		else if ( isDominatedByItsOrigin(vectorBox, vectorBox[it]) )
		{
			delete vectorBox[it];
			vectorBox.erase(vectorBox.begin() + it);
		}
		else
		{
			//Compute all potential children of this box as candidates boxes
			addChildren(vectorBox[it], vectorBox, sorted_fac);
			//Compute bounds of this box
			vectorBox[it]->computeBox();
			//If this box is dominated by all the boxes (existing + its children) regarding to its bounds
			if (isDominatedByItsBox(vectorBox, vectorBox[it]))
			{
				delete vectorBox[it];
				vectorBox.erase(vectorBox.begin() + it);
			}
			else
			{
				//If one of all others boxes are dominated by this box, we delete it
				for (unsigned int it2 = 0; it2 != it;)
				{
					if (isDominatedBetweenTwoBoxes(vectorBox[it2], vectorBox[it]))
					{
						delete vectorBox[it2];
						vectorBox.erase(vectorBox.begin() + it2);
						//We delete an element before (the list shift one step forward), so we shift one step backward
						--it;
					}
					else
					{
						++it2;
					}	
				}
				++it;
			}
		}
	}
	
	if (Argument::mode_export)
	{	
		ToFile::saveInitialBoxes(vectorBox, data);
	}
    
	return nbBoxComputed;
}

void initBoxCapacitated(std::vector<Box*> &vectorBox, Data &data, std::vector<int> & sorted_fac)
{
	// sorted_fac is sorted by decreasing capacities
	std::sort( sorted_fac.begin(), sorted_fac.end(), compare_capacity(data) );

	// Compute the total demand
	double dtotal( 0 );
	for ( unsigned int i = 0; i < data.getnbCustomer(); ++i )
	{
		dtotal += data.getCustomer(i).getDemand();
	}

	// Compute the minimum number of facilities to open (left).
	double qminsum( 0 );
	unsigned int minfac( 0 );
	while ( minfac < sorted_fac.size() && qminsum < dtotal )
	{
		qminsum += data.getFacility(sorted_fac[minfac]).getCapacity();
		++minfac;
	}

	// Check if the problem can be feasible
	if ( qminsum < dtotal )
	{
		std::cerr << "The problem is infeasible !" << std::endl;
		return;
	}

	// Compute the minimum number of facilities to open (right).
	double qmaxsum( 0 );
	unsigned int maxfac( sorted_fac.size() );
	while ( maxfac >= 0 && qmaxsum < dtotal )
	{
		--maxfac;
		qmaxsum += data.getFacility(sorted_fac[maxfac]).getCapacity();
	}

	for ( int it = 0; it < Argument::paving_directions; ++it )
	{
		// GRASP
		std::vector<bool> facilityOpen(data.getnbFacility(), false);
		std::vector<int> not_open( sorted_fac );
		double qsum( 0 ), alpha( 0.5 ),
		       dir( (double)it / (double)(Argument::paving_directions - 1.) );

		while ( qsum < dtotal )
		{
			std::vector<double> u( not_open.size() );
			std::vector<int> RCL;
			double umin( std::numeric_limits<double>::infinity() ),
			       umax( -std::numeric_limits<double>::infinity() ),
			       ulimit;

			// Build utility function
			for ( unsigned int j = 0; j < not_open.size(); ++j )
			{
				double cost( 0 );
				if ( data.getNbObjective() > 2 )
				{
					for ( int k = 0; k < data.getNbObjective(); ++k )
					{
						cost += data.getFacility( not_open[j] ).getLocationObjCost( k );
					}
				}
				else
				{
					cost = dir * data.getFacility( not_open[j] ).getLocationObjCost( 0 )
					 + ( 1. - dir ) * data.getFacility( not_open[j] ).getLocationObjCost( 1 );
				}

				u[j] = data.getFacility( not_open[j] ).getCapacity() / cost;
				if ( u[j] < umin ) umin = u[j];
				if ( u[j] > umax ) umax = u[j];
			}
			ulimit = umin + alpha * ( umax - umin );

			// Build RCL
			for ( unsigned int j = 0; j < u.size(); ++j )
			{
				if ( u[j] >= ulimit )
				{
					RCL.push_back( j );
				}
			}

			// Select a random facility
			int r = std::rand() % RCL.size();

			// Open it
			facilityOpen[not_open[RCL[r]]] = true;
			qsum += data.getFacility( not_open[RCL[r]] ).getCapacity();
			not_open.erase( not_open.begin() + RCL[r] );
		}
		vectorBox.push_back( new Box(data, facilityOpen) );
	}
}

void addChildren(Box *boxMother, std::vector<Box*> &vBox, const std::vector<int> & sorted_fac)
{
	Data& data = boxMother->getData();

	// To find the last digit at 1 in the open facility
	std::vector<bool> facilityOpen(data.getnbFacility());
	int indexLastOpened = -1;

	double qsum( 0 );
	static std::vector<double> qremaining( data.getnbFacility(), 0 );
	static double dtotal( -1 );

	// Compute some data (once)
	if ( Argument::capacitated && !Argument::paving_grasp && dtotal < 0 )
	{
		// Compute the total demand (once)
		dtotal = 0;
		for ( unsigned int i = 0; i < data.getnbCustomer(); ++i )
		{
			dtotal += data.getCustomer(i).getDemand();
		}

		// Compute the remaining capacities (once)
		for ( unsigned int i = data.getnbFacility()-1; i > 0; --i )
		{
			qremaining[i-1] = qremaining[i] + data.getFacility(sorted_fac[i]).getCapacity();
		}
	}

	// Compute the facilityOpen vector of the current Box
	for (unsigned int i = 0 ; i < data.getnbFacility() ; ++i)
	{
		if (boxMother->isOpened(sorted_fac[i]))
		{
			indexLastOpened = i;
			qsum += data.getFacility(sorted_fac[i]).getCapacity();
		}
		facilityOpen[sorted_fac[i]] = boxMother->isOpened(sorted_fac[i]);
	}

	// If Box was constructed using GRASP heuristic
	if ( Argument::capacitated && Argument::paving_grasp )
	{
		for (unsigned int i = 0 ; i < data.getnbFacility() ; ++i)
		{
			if (!facilityOpen[sorted_fac[i]])
			{
				facilityOpen[sorted_fac[i]] = true;
				vBox.push_back( new Box(data, facilityOpen) );
				facilityOpen[sorted_fac[i]] = false;
			}
		}
	}
	else // Enumerate all (extremely slow if capacitated)
	{
		// For each digit 0 of facilities not yet affected, we change it in 1 to create the corresponding a new combination
		for (unsigned int i = indexLastOpened + 1; i < data.getnbFacility(); ++i)
		{
			// If the remaining facilites exactly cover the rest of the demand,
			// Skip infeasible boxes
			if ( Argument::capacitated && qsum + qremaining[i] < dtotal )
			{
				while ( i < data.getnbFacility() )
				{
					facilityOpen[sorted_fac[i]] = true;
					++i;
				}
				vBox.push_back( new Box(data, facilityOpen) );
			}
			else
			{
				facilityOpen[sorted_fac[i]] = true;
				vBox.push_back( new Box(data, facilityOpen) );
				facilityOpen[sorted_fac[i]] = false;
			}
		}
	}
}

void filter(std::vector<Box*> &vectorBox, long int &nbToCompute, long int &nbWithNeighbor)
{
	//Initializing booleans
	for (unsigned int it = 0; it < vectorBox.size(); ++it)
	{
		vectorBox[it]->setHasNeighbor(false);
	}
	//Filtering and update of booleans
	for (unsigned int it = 0; it < vectorBox.size();)
	{	
		for (unsigned int it2 = it + 1 ; it2 < vectorBox.size();)
		{
			if ( isDominatedBetweenTwoBoxes(vectorBox[it2] , vectorBox[it]) )
			{
				delete vectorBox[it2];
				vectorBox.erase(vectorBox.begin() + it2);
			}
			else
			{
				if ( isDominatedBetweenTwoBoxes(vectorBox[it] , vectorBox[it2]) )
				{
					delete vectorBox[it];
					vectorBox.erase(vectorBox.begin() + it);
					it2 = it + 1; //Initialize it2 at the begin of the vector
				}
				else
				{
					if ( (vectorBox[it]->getHasNeighbor() == false) || (vectorBox[it2]->getHasNeighbor() == false) )
					{
						bool less1( true ), less2( true );
						for ( int k = 0; k < vectorBox[it]->getNbObjective() && less1 && less2; ++k )
						{
							if ( !(vectorBox[it]->getMinZ(k) < vectorBox[it2]->getMaxZ(k)) )
							{
								less1 = false;
							}
							if ( !(vectorBox[it2]->getMinZ(k) < vectorBox[it]->getMaxZ(k)) )
							{
								less2 = false;
							}
						}
						if ( less1 || less2 )
						{
							vectorBox[it]->setHasNeighbor(true);
							vectorBox[it2]->setHasNeighbor(true);
						}
					}
				}
				//it2 wasn't deleted
				++it2;
			}
		}
		//it wasn't deleted
		++it;
	}
	
	//Compute counters
	nbToCompute = 0;
	nbWithNeighbor = 0;
	for (unsigned int it = 0; it < vectorBox.size(); ++it)
	{
		if ( vectorBox[it]->getHasMoreStepWS() )
		{
			++nbToCompute;
		}
		if (vectorBox[it]->getHasNeighbor() )
		{
			++nbWithNeighbor;
		}
	}
}

void boxFiltering(std::vector<Box*> &vectorBox, Data &data, long int &nbToCompute, long int &nbWithNeighbor)
{
	int i = 1;
	filter(vectorBox, nbToCompute, nbWithNeighbor);
	
	//Parameter turns to 3
	while (nbWithNeighbor != 0 && i <= 3)
	{
		++i;
		weightedSumOneStep(vectorBox, data);
		filter(vectorBox, nbToCompute, nbWithNeighbor);
	}
    
	if (Argument::mode_export)
	{
		ToFile::saveFilteringBoxes(vectorBox, data);
	}
    
}

void recomposition(std::vector<Box*> &vectorBox, std::vector<Box*> &vectorBoxFinal, Data &data, long int &nbToCompute, long int &nbWithNeighbor)
{
	std::map<std::string,int> mapBox;
	std::map<std::string,int>::iterator itM;
	//Add boxes in the map
	for(unsigned int it = 0; it < vectorBox.size(); ++it)
	{
		itM = mapBox.find(vectorBox[it]->getId());
		if(itM == mapBox.end())
		{
			mapBox[vectorBox[it]->getId()] = 1;
		}
		else
		{
			mapBox[vectorBox[it]->getId()] += 1;
		}
	}
	//Creating boxes with relation to the map
	for(itM = mapBox.begin(); itM != mapBox.end(); ++itM)
	{
		Box *box0 = new Box(data);
		std::string tempString = itM->first;
		for(unsigned int i = 0; i < tempString.size(); ++i)
		{
			if(tempString[i] == '1')
			{
				box0->openFacility(i);
			}
		}
		box0->setId(tempString);
		box0->computeBox();
		for (int k = 0; k < box0->getNbObjective(); ++k)
		{
			box0->setMinZ(k, std::numeric_limits<double>::infinity());
			box0->setMaxZ(k, 0);
		}
		vectorBoxFinal.push_back(box0);
	}
	//Updating bounds
	for (unsigned int it = 0; it < vectorBox.size(); ++it)
	{
		for (unsigned int it2 = 0; it2 < vectorBoxFinal.size(); ++it2)
		{
			if (vectorBoxFinal[it2]->getId() == vectorBox[it]->getId())
			{
				for (int k = 0; k < vectorBox[it]->getNbObjective(); ++k)
				{
					if ( vectorBox[it]->getMinZ(k) < vectorBoxFinal[it2]->getMinZ(k) )
					{
						vectorBoxFinal[it2]->setMinZ(k, vectorBox[it]->getMinZ(k));
					}
					if ( vectorBox[it]->getMaxZ(k) > vectorBoxFinal[it2]->getMaxZ(k) )
					{
						vectorBoxFinal[it2]->setMaxZ(k, vectorBox[it]->getMaxZ(k));
					}
				}
			}
		}
	}
	
	
	filter(vectorBoxFinal, nbToCompute, nbWithNeighbor);
    
	if (Argument::mode_export)
	{
		ToFile::saveReconstructionBoxes(vectorBoxFinal, data);
	}
}

void weightedSumOneStep(std::vector<Box*> &vectorBox, Data &data)
{
	unsigned int size = vectorBox.size();
	for (unsigned int it = 0; it < size; ++it)
	{
		if ( vectorBox[it]->getHasNeighbor() && vectorBox[it]->getHasMoreStepWS() )
		{
			double alpha =	( vectorBox[it]->getMaxZ(0) - vectorBox[it]->getMinZ(0) ) / 
			                ( vectorBox[it]->getMaxZ(1) - vectorBox[it]->getMinZ(1) ) ;

			double z_obj1 = vectorBox[it]->getOriginZ(0);
			double z_obj2 = vectorBox[it]->getOriginZ(1);
            
			for (unsigned int i = 0; i < data.getnbCustomer(); ++i)
			{
				if (! (vectorBox[it]->isAssigned(i)))
				{
					int ind = -1;
					double val = std::numeric_limits<double>::infinity();
					for (unsigned int j = 0; j < data.getnbFacility(); ++j)
					{
						if ( vectorBox[it]->isOpened(j) )
						{
							if (val > data.getAllocationObjCost(0, i, j) + alpha * data.getAllocationObjCost(1, i, j))
							{
								ind = j;
								val = data.getAllocationObjCost(0, i, j) + alpha * data.getAllocationObjCost(1, i, j);
							}
						}
					}
					z_obj1 += data.getAllocationObjCost(0, i, ind);
					z_obj2 += data.getAllocationObjCost(1, i, ind);
				}
			}
			
			if((z_obj1 + alpha * z_obj2) < (vectorBox[it]->getMinZ(0) + alpha * vectorBox[it]->getMaxZ(1)))
			//We found a solution. We split the box in 2. We copy it and modify its bounds
			{
				Box* copyBox = new Box(vectorBox[it]);				
				vectorBox[it]->setMaxZ(0, z_obj1);
				vectorBox[it]->setMinZ(1, z_obj2);
				copyBox->setMinZ(0, z_obj1);
				copyBox->setMaxZ(1, z_obj2);
				vectorBox.push_back(copyBox);
			}
			else
			{
				vectorBox[it]->setHasMoreStepWS(false);
			}
		}
		else
		{
			
		}
	}
	
}

//LABELSETTING
long int runLabelSetting(std::vector<Box*> &vectorBox, Data &data, std::list<Solution> & allSolution)
{
	std::vector<Box*>::iterator itVector;
	for (itVector = vectorBox.begin(); itVector != vectorBox.end(); ++itVector)
	{						
		LabelSetting m(*(*itVector));
		m.compute();

		allSolution.merge(m.nodes_[m.getRank() - 1].labels_);
	}
	
	filterListSolution(allSolution);
    
	if (Argument::mode_export)
	{
		ToFile::saveYN(allSolution, data);
	}
	
	return (long int) allSolution.size();
}

// Multi-objective genetic algorithm
long int runMOGA(std::vector<Box*> &vectorBox, Data &data, std::list<Solution> & allSolution)
{
	std::vector<Box*>::iterator itVector;
	FILE * pipe_fp = 0; // Gnuplot pipe
	int object_id = 0;

	// OPEN GNUPLOT (interactive mode)
	if (Argument::interactive)
	{
		if ( ( pipe_fp = popen("gnuplot", "w") ) != 0 )
		{
			int num_obj = (*vectorBox.begin())->getNbObjective();
			std::vector<double> minZ( num_obj, std::numeric_limits<double>::infinity() ),
			                    maxZ( num_obj, -std::numeric_limits<double>::infinity() );
			
			for (itVector = vectorBox.begin(); itVector != vectorBox.end(); ++itVector)
			{
				for ( int k = 0; k < (*(*itVector)).getNbObjective(); ++k )
				{
					minZ[k] = std::min( (*(*itVector)).getMinZ( k ), minZ[k] );
					maxZ[k] = std::max( (*(*itVector)).getMaxZ( k ), maxZ[k] );
				}
			}

			fputs( "set nokey\n", pipe_fp );
			fputs( "set xlabel \"$z_1$\"\n", pipe_fp );
			fputs( "set ylabel \"$z_2$\"\n", pipe_fp );
			fprintf( pipe_fp, "set xrange [%f:%f]\n", minZ[0], maxZ[0] );
			fprintf( pipe_fp, "set yrange [%f:%f]\n", minZ[1], maxZ[1] );
		}
		else
		{
			std::cerr << "Error: gnuplot pipe failed" << std::endl;
		}
	}

	// Apply MOGA on each box
	for (itVector = vectorBox.begin(); itVector != vectorBox.end(); ++itVector)
	{
		// Draw the box associated
		if ( pipe_fp )
		{
			++object_id;
			fprintf( pipe_fp, "set object %d rectangle from %f,%f to %f,%f fillstyle empty\n", object_id,
				(*(*itVector)).getMinZ(0), (*(*itVector)).getMinZ(1),
				(*(*itVector)).getMaxZ(0), (*(*itVector)).getMaxZ(1) );
		}

		MOGA m(*(*itVector), Argument::num_individuals, Argument::num_generations, Argument::Pc, Argument::Pm);
		m.pipe_fp_ = pipe_fp;
		m.compute();

		allSolution.merge(m.solutions_);
	}

	// CLOSE GNUPLOT
	if ( pipe_fp )
	{
		pclose(pipe_fp);
	}

	// FILTERING DOMINATED SOLUTIONS
	for ( std::list<Solution>::iterator it1 = allSolution.begin(); it1 != allSolution.end(); ++it1 )
	{
		for ( std::list<Solution>::iterator it2 = it1; it2 != allSolution.end(); ++it2 )
		{
			bool dominates = true, dominated = true;
			for (int k = 0; k < (*it1).getNbObjective() && (dominates || dominated); ++k)
			{
				if ( !( (*it1).getObj( k ) < (*it2).getObj( k ) ) )
					dominates = false;

				if ( !( (*it2).getObj( k ) < (*it1).getObj( k ) ) )
					dominated = false;
			}
			if ( dominates )
			{
				it2 = allSolution.erase( it2 );
				--it2;
			}
			if ( dominated )
			{
				it1 = allSolution.erase( it1 );
				--it1;
				break;
			}
		}
	}

	if (Argument::mode_export)
	{
		ToFile::saveYN(allSolution, data);
	}

	return (long int) allSolution.size();
}

void filterListSolution(std::list<Solution> &lsol)
{
	std::list<Solution>::iterator iterSuivant = lsol.begin();
	++iterSuivant;
	std::list<Solution>::iterator iter        = lsol.begin();
	while (iterSuivant != lsol.end())
	{
		if ( (*iter).getObj(0) == (*iterSuivant).getObj(0) )
		{
			//We have to delete one of these two elements
			if ( (*iter).getObj(1) > (*iterSuivant).getObj(1) )
			{
				++iterSuivant;
				iter = lsol.erase(iter);
			}
			else
			{
				iterSuivant = lsol.erase(iterSuivant);
			}
		}
		else
		{
			if ( (*iter).getObj(1) > (*iterSuivant).getObj(1) )
			{
				++iter;
				++iterSuivant;
			}
			else
			{
				iterSuivant = lsol.erase(iterSuivant);
			}
		}
	}
}

double computeCorrelation(Data &data)
{
	double correl(0);
	double meanX(0), meanY(0), num(0), den1(0), den2(0);
    
	for(unsigned int i = 0; i < data.getnbCustomer(); ++i)
	{
		for(unsigned int j = 0; j < data.getnbFacility(); ++j)
		{
			meanX += data.getAllocationObjCost(0, i, j);
			meanY += data.getAllocationObjCost(1, i, j);
		}
	}
	meanX /= ( data.getnbCustomer() * data.getnbFacility() );
	meanY /= ( data.getnbCustomer() * data.getnbFacility() );
    
	for(unsigned int i = 0; i < data.getnbCustomer(); ++i)
	{
		for(unsigned int j = 0; j < data.getnbFacility(); ++j)
		{
			num += (data.getAllocationObjCost(0, i, j) - meanX) * (data.getAllocationObjCost(1, i, j) - meanY) ;
			den1 += pow((data.getAllocationObjCost(0, i, j) - meanX), 2);
			den2 += pow((data.getAllocationObjCost(1, i, j) - meanY), 2);
		}
	}
    
	correl = num / (std::sqrt(den1) * std::sqrt(den2));

	if (Argument::mode_export)
	{
		ToFile::saveCorrelation(data);
	}
	
	return correl;	
}

float time_ms_Diff(timeval tvStart, timeval tvEnd)
{
    float time;
    
    if(tvEnd.tv_sec < tvStart.tv_sec)
    {
        time = 0.0;
    }
    else
    {
        if(tvEnd.tv_usec >= tvStart.tv_usec)
        {
            time =  ((tvEnd.tv_usec - tvStart.tv_usec)/1000);
        }
        else
        {
            time = ((1000000 + tvEnd.tv_usec - tvStart.tv_usec)/1000);
        }
    }
    
    return time;
}

float time_s_Diff(timeval tvStart, timeval tvEnd)
{
    float time(0.0);
    
    if(tvEnd.tv_sec < tvStart.tv_sec)
    {
        time = 0.0;
    }
    else
    {
    	if((tvEnd.tv_usec - tvStart.tv_usec < 0) && (tvEnd.tv_sec == tvStart.tv_sec + 1) )
    	{
    		time = 0.0;
    	}
    	else if((tvEnd.tv_usec - tvStart.tv_usec < 0) && (tvEnd.tv_sec != tvStart.tv_sec + 1) )
    	{
    		time = tvEnd.tv_sec - tvStart.tv_sec - 1.0;
    	}
    	else
    	{
        	time = tvEnd.tv_sec - tvStart.tv_sec;
        }
    }
    
    return time;
}
