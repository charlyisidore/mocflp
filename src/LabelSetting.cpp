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

#include "LabelSetting.hpp"
#include "Argument.hpp"

LabelSetting::LabelSetting(Box &box):
boundZ_(2),
data_(box.getData())
{
	Solution sol( getNbObjective() );

	for (int k = 0; k < getNbObjective(); ++k)
	{
		boundZ_[k] = box.getMaxZ(k);
		sol.setObj(k, box.getOriginZ(k));
	}
	nbRank_ = box.getnbCustomerNotAffected() + 1;

	//Compute n level + initial level
	nodes_.resize(nbRank_);
	nodes_[0].setSize(1);
	nodes_[0].labels_.push_front(sol);

	unsigned int nbNodeNonDominated;
	//In each line, there is maximum getNbFacilityOpen() nodes
	std::vector< std::vector<double> > tabZ( getNbObjective(), std::vector<double>(box.getNbFacilityOpen()) );

	int indexOpen = 0;
	for (unsigned int i = 0; i < data_.getnbCustomer(); ++i)
	{
		//If the customer is not affected 
		if ( !box.isAssigned(i) )
		{
			++indexOpen;
			nbNodeNonDominated = 0;
			for (unsigned int j = 0; j < data_.getnbFacility(); ++j)
			{
				//Find the list of potential allocations
				if ( box.isOpened(j) )
				{
					//If facility is opened, we test if the allocation isn't dominated
					bool toKeep = true;
					for (unsigned int k = 0; (k < nbNodeNonDominated) && toKeep; ++k)
					{
						bool dominates = true, dominated = true;
						for (int l = 0; l < getNbObjective() && (dominates || dominated); ++l)
						{
							if ( !( data_.getAllocationObjCost(l, i, j) < tabZ[l][k] ) )
								dominates = false;

							if ( !( data_.getAllocationObjCost(l, i, j) > tabZ[l][k] ) )
								dominated = false;
						}

						if (dominates)
						{
							//If the allocation is lower on all costs, we replace existing values
							for (int l = 0; l < getNbObjective(); ++l)
							{
								tabZ[l][k] = data_.getAllocationObjCost(l, i, j);
							}
							toKeep = false;
						}
						else if (dominated)
						{	
							//We delete don't keep it because it is dominated
							toKeep = false;
						}
					}
					if (toKeep)
					{
						//We keep it as non dominated
						for (int k = 0; k < getNbObjective(); ++k)
						{
							tabZ[k][nbNodeNonDominated] = data_.getAllocationObjCost(k, i, j);
						}
						++nbNodeNonDominated;
					}
				}
			}
 
			//We have the list of non-dominated, we build the corresponding nodes

			nodes_[indexOpen].setSize(nbNodeNonDominated);
			for (unsigned int j = 0; j < nbNodeNonDominated; ++j)
			{
				for (int k = 0; k < getNbObjective(); ++k)
				{
					nodes_[indexOpen].setValue(k, j, tabZ[k][j]);
				}
			}
			
		}
	}
}

LabelSetting::~LabelSetting()
{
}

void LabelSetting::computeNode(int indexNode)
{
	//The previous node is computed
	for (unsigned int i = 0; i < nodes_[indexNode].getSize(); i++)
	{
		//We have an iterator on the place where we want to add the element
		std::list<Solution>::iterator itDest = nodes_[indexNode].labels_.begin();
		std::list<Solution>::iterator iter;
		for ( iter = nodes_[indexNode - 1].labels_.begin(); iter != nodes_[indexNode - 1].labels_.end(); iter++)
		{
			std::vector<double> obj( getNbObjective() );
			Solution sol( getNbObjective() );

			//We create new elements, one by one

			for (int k = 0; k < getNbObjective(); ++k)
			{
				obj[k] = (*iter).getObj(k) + nodes_[indexNode].getCostToEnterZ(k, i);
			}

			//We iterate over the destination list to know where to place the element in relation with Z1
			while ( (itDest != nodes_[indexNode].labels_.end() ) && ( (*itDest).getObj(0) < obj[0]) )
			{
				++itDest;
			}

			//We insert it before this element
			for (int k = 0; k < getNbObjective(); ++k)
			{
				sol.setObj(k, obj[k]);
			}
			nodes_[indexNode].labels_.insert(itDest, sol);
		}
	}

	//FILTERING ALGORITHM

	//To delete dominated nodes
	std::list<Solution>::iterator iterSuivant = nodes_[indexNode].labels_.begin();
	iterSuivant++;
	std::list<Solution>::iterator iter        = nodes_[indexNode].labels_.begin();
	
	while ( (*iter).getObj(1) > boundZ_[1] )
	{
		iter = nodes_[indexNode].labels_.erase(iter);
	}

	filterListSolution((nodes_[indexNode].labels_));

	iter = nodes_[indexNode].labels_.end();
	--iter;
	while ( (*iter).getObj(0) > boundZ_[0] )
	{
		iter = nodes_[indexNode].labels_.erase(iter);
		--iter;
	}
}

unsigned int LabelSetting::getRank() const
{
	return nbRank_;
}

void LabelSetting::compute()
{
	for (unsigned int i = 1; i < nbRank_; i++)
	{
		computeNode(i);
		nodes_[i-1].clearLabels();
	}
}

void LabelSetting::print()
{
	if (Argument::verbose)
	{
		for (unsigned int i = 0; i < nbRank_; i++)
		{
			std::cout << "(LS) Node " << i << std::endl;
			for (unsigned int j = 0; j < nodes_[i].getSize(); j++)
			{
				for (int k = 0; k < getNbObjective(); ++k)
				{
					if (k > 0) std::cout << '*';
					std::cout << nodes_[i].getCostToEnterZ(k, j);
				}
				std::cout << std::endl;
			}
		}
	}
}
