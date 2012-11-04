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

#include "Facility.hpp"

Facility::Facility(unsigned short &x, unsigned short &y)
: coordX_(x), coordY_(y), locationObj1Cost_(0), locationObj2Cost_(0)
{
}

unsigned short Facility::getCoordX() const
{
    return coordX_;
}

unsigned short Facility::getCoordY() const
{
    return coordY_;
}

double Facility::getLocationObj1Cost() const
{
    return locationObj1Cost_;
}

double Facility::getLocationObj2Cost() const
{
    return locationObj2Cost_;
}

void Facility::setLocationObj1Cost(double &val)
{
    locationObj1Cost_ = val;
}

void Facility::setLocationObj2Cost(double &val)
{
    locationObj2Cost_ = val;
}

ostream &operator<<(ostream &out, const Facility *fac)
{
    out << "[" << fac->getCoordX() << "," << fac->getCoordY() << "]"
    << "\n\tCost Obj 1 =" << fac->getLocationObj1Cost()
    << "\n\tCost Obj 2 =" << fac->getLocationObj2Cost();
    return out;
}
