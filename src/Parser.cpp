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

#include "Parser.hpp"

Data *Parser::Parsing(const char *filename)
{
	// TODO: extend to p-objective
	std::ifstream file(filename);
    
	if(file.is_open())
	{
		unsigned int nbFacility;
		unsigned int nbCustomer;
		unsigned int i, j;
		unsigned short x, y;
		double v;
		
		//Number of Facilities and Stores
		file >> nbCustomer;
		ignoreLine(file);
		file >> nbFacility;
		ignoreLine(file);
		
		Data *d = new Data(nbFacility, nbCustomer, filename);
        
		//Coordinates of Facility
		for(i = 0; i < nbFacility; ++i)
		{
			x = 0, y = 0;
			Facility facility(x,y);
			d->addFacility(facility);
		}
        
		//Coordinates of Customer
		for(i = 0; i < nbCustomer; ++i)
		{
			x = 0, y = 0;
			Customer customer(x,y);
			d->addCustomer(customer);
		}
        
		//Allocation cost w.r.t. objective 1
		for(i = 0; i < nbCustomer; ++i)
		{
			for(j = 0; j < nbFacility; ++j)
			{
				file >> v;
				d->setAllocationObjCost(0, i, j, v);
			}
			ignoreLine(file);
		}
		ignoreLine(file);
        
		//Allocation cost w.r.t. objective 2
		for(i = 0; i < nbCustomer; ++i)
		{
			for(j = 0; j < nbFacility; ++j)
			{
				file >> v;
				d->setAllocationObjCost(1, i, j, v);
			}
			ignoreLine(file);
		}
		ignoreLine(file);
        
		//Location cost w.r.t. objective 1 for Facility
		for(i = 0; i < nbFacility; ++i)
		{			
			file >> v;
			d->getFacility(i).setLocationObjCost(0, v);
		}
		ignoreLine(file);
        
		//Location cost w.r.t. objective 2 for Facility
		for(i = 0; i < nbFacility; ++i)
		{
			file >> v;
			d->getFacility(i).setLocationObjCost(1, v);
		}
        
		return d;
	}
	else
	{
		std::cerr << "Can't open instance" << std::endl;
		return 0;
	}
}

void Parser::ignoreLine(std::ifstream &file)
{
	file.ignore(2000000000, '\n');
}

void Parser::ignoreChar(std::ifstream &file)
{
	char c;	
	file >> c;
}
