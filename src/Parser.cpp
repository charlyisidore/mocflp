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
#include "Argument.hpp"

Data *Parser::Parsing(const std::string & filename)
{
	// TODO: extend to p-objective
	std::ifstream file(filename.c_str());
	int num_objectives( 2 );

	if (file.is_open())
	{
		int nbFacility;
		int nbCustomer;
		double v;

		//Number of Facilities and Stores
		file >> nbCustomer;
		file >> nbFacility;

		Data *d = new Data(nbFacility, nbCustomer, filename);

		// Coordinates of Facility
		for (int i = 0; i < nbFacility; ++i)
		{
			Facility facility;
			d->addFacility(facility);
		}

		// Coordinates of Customer
		for (int i = 0; i < nbCustomer; ++i)
		{
			Customer customer;
			d->addCustomer(customer);
		}

		// Allocation cost w.r.t. objective k
		for (int k = 0; k < num_objectives; ++k )
		{
			for (int i = 0; i < nbCustomer; ++i)
			{
				for (int j = 0; j < nbFacility; ++j)
				{
					file >> v;
					d->setAllocationObjCost(k, i, j, v);
				}
			}
		}

		// Location cost w.r.t. objective 1 for Facility
		for (int k = 0; k < num_objectives; ++k )
		{
			for (int i = 0; i < nbFacility; ++i)
			{			
				file >> v;
				d->getFacility(i).setLocationObjCost(k, v);
			}
		}

		// Capacitated instances
		if (Argument::capacitated)
		{
			// Demand for Customer
			for (int i = 0; i < nbCustomer; ++i)
			{			
				file >> v;
				d->getCustomer(i).setDemand(v);
			}

			// Capacity for Facility
			for (int i = 0; i < nbFacility; ++i)
			{			
				file >> v;
				d->getFacility(i).setCapacity(v);
			}
		}

		return d;
	}
	else
	{
		std::cerr << "Can't open instance" << std::endl;
		return 0;
	}
}

