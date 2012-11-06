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

LabelSetting::LabelSetting(Box &box):
boundZ_(2),
data_(box.getData())
{
    boundZ_[0] = box.getMaxZ1();
    boundZ_[1] = box.getMaxZ2();
    nbRank_ = box.getnbCustomerNotAffected() + 1;
    
    //Compute n level + initial level
    nodes_ = new Node[nbRank_];
    
    nodes_[0].setSize(1);
    
    nodes_[0].labels_.push_front( Solution(box.getOriginZ1(),box.getOriginZ2()) );
    
    unsigned int nbNodeNonDominated;
    //In each line, there is maximum getNbFacilityOpen() nodes
    std::vector<double> tabZ1(box.getNbFacilityOpen()),
                        tabZ2(box.getNbFacilityOpen());
    
    int indexOpen = 0;
    for(unsigned int i = 0; i < data_.getnbCustomer(); i++)
    {
        //If the customer is not affected 
        if( !box.isAssigned(i) )
        {
            indexOpen ++;
            nbNodeNonDominated = 0;
            for(unsigned int j = 0; j < data_.getnbFacility(); j++)
            {
                //Find the list of potential allocations
                if( box.isOpened(j) )
                {
                    //If facility is opened, we test if the allocation isn't dominated
                    bool toKeep =  true;
                    for(unsigned int k = 0; (k < nbNodeNonDominated) && toKeep; k++)
                    {
                        if(data_.getAllocationObj1Cost(i,j) < tabZ1[k] && data_.getAllocationObj2Cost(i,j) < tabZ2[k])
                        {
                            //If the allocation is lower on 2 costs, we replace existing values
                            tabZ1[k] = data_.getAllocationObj1Cost(i,j);
                            tabZ2[k] = data_.getAllocationObj2Cost(i,j);
                            toKeep = false;
                        }
                        else if(data_.getAllocationObj1Cost(i,j) > tabZ1[k] && data_.getAllocationObj2Cost(i,j) > tabZ2[k])
                        {	
                            //We delete don't keep it because it is dominated
                            toKeep = false;
                        }
                    }
                    if(toKeep)
                    {
                        //We keep it as non dominated
                        tabZ1[nbNodeNonDominated] = data_.getAllocationObj1Cost(i,j);
                        tabZ2[nbNodeNonDominated] = data_.getAllocationObj2Cost(i,j);
                        nbNodeNonDominated++;
                    }
                }
            }
            
            //We have the list of non-dominated, we build the corresponding nodes
            
            nodes_[indexOpen].setSize(nbNodeNonDominated);
            for(unsigned int j = 0; j < nbNodeNonDominated; j++)
            {
                nodes_[indexOpen].setValues(j, tabZ1[j], tabZ2[j]);
            }
            
        }
    }
}

LabelSetting::~LabelSetting()
{
	delete[] nodes_;
}

void LabelSetting::computeNode(int indexNode)
{
	//The previous node is computed
	for(unsigned int i = 0; i < nodes_[indexNode].getSize(); i++)
	{
		//We have an iterator on the place where we want to add the element
		list<Solution>::iterator itDest = nodes_[indexNode].labels_.begin();
		list<Solution>::iterator iter;
		for( iter = nodes_[indexNode - 1].labels_.begin(); iter != nodes_[indexNode - 1].labels_.end(); iter++)
		{
			//We create new elements, one by one
			double obj1 = (*iter).getObj1() + nodes_[indexNode].getCostToEnterZ1(i);
			double obj2 = (*iter).getObj2() + nodes_[indexNode].getCostToEnterZ2(i);
			
			//We iterate over the destination list to know where to place the element in relation with Z1
			while( (itDest != nodes_[indexNode].labels_.end() ) && ( (*itDest).getObj1() < (obj1)) )
			{
				itDest++;
			}
			
			//We insert it before this element
			nodes_[indexNode].labels_.insert(itDest, Solution(obj1,obj2));
		}
	}
    
	//FILTERING ALGORITHM
    
	//To delete dominated nodes
	list<Solution>::iterator iterSuivant = (nodes_[indexNode]).labels_.begin();
	iterSuivant++;
	list<Solution>::iterator iter        = (nodes_[indexNode]).labels_.begin();
	
	while( (*iter).getObj2() > boundZ_[1] )
	{
		iter = nodes_[indexNode].labels_.erase(iter);
	}
    
	filterListSolution((nodes_[indexNode].labels_));
    
	iter = (nodes_[indexNode]).labels_.end();
	iter--;
	while ( (*iter).getObj1() > boundZ_[0] )
	{
		iter = nodes_[indexNode].labels_.erase(iter);
		iter--;
	}
}

unsigned int LabelSetting::getRank() const
{
	return nbRank_;
}

void LabelSetting::compute()
{
	for(unsigned int i = 1; i < nbRank_; i++)
	{
		computeNode(i);
		nodes_[i-1].clearLabels();
	}
}

void LabelSetting::print()
{
	if(modeVerbose)
	{
		for(unsigned int i = 0; i < nbRank_; i++)
		{
			cout << "(LS) Node " << i << endl;
			for(unsigned int j = 0; j < nodes_[i].getSize(); j++)
			{		
				cout << nodes_[i].getCostToEnterZ1(j) << "*" << nodes_[i].getCostToEnterZ2(j) << endl;
			}
		}
	}
}
