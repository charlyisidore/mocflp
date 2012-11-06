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

#include "Node.hpp"

Node::Node()
{	
}

Node::Node(int size) :
	size_(size),
	costToEnterZ_(2, std::vector<double>(size_))
{
}

Node::~Node()
{
}

void Node::setSize(unsigned int s)
{
	size_ = s;
	costToEnterZ_.resize(2, std::vector<double>(size_));
}

unsigned int Node::getSize() const
{
	return size_;
}

double Node::getCostToEnterZ1(int i) const
{
	return costToEnterZ_[0][i];
}

double Node::getCostToEnterZ2(int i) const
{
	return costToEnterZ_[1][i];
}

void Node::clearLabels()
{
	labels_.clear();
}

void Node::setValues(int index, double z1, double z2)
{
	costToEnterZ_[0][index] = z1;
	costToEnterZ_[1][index] = z2;
}

void Node::print()
{
	if(modeVerbose)
	{
		std::cout << "(N) Print Node" << std::endl;
		int i = 0;
		for( std::list<Solution>::iterator iter = labels_.begin(); iter != labels_.end(); ++iter )
		{
			++i;
			std::cout << i << '\t' << (*iter).getObj1() << '\t' << (*iter).getObj2() << std::endl;	
		}
	}
}
