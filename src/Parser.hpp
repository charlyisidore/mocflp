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

#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <iostream>
#include <math.h>

#include "Data.hpp"

/*!
* \file Parser.hpp
* \brief Class of the \c Parser.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright 
*
* This class represents a parser to read data from an instance file.
*
*/

/*! \namespace std
* 
* Using the standard namespace std of the IOstream library of C++.
*/
using namespace std;

/*! \class Parser
* \brief Class to represent a \c Parser.
*
*  This class represents a \c Parser with all its methods to read and parse an instance file.
*/
class Parser
{
public:
	/*!
	*	\brief \e Static method to parse the instance.
	*
	*	\param[in] filename : A const char which represents the instance file.
	*	\return A \c Data object which contains all the values of the instance file.
	*/
    static Data *Parsing(const char *filename);
    
private:
	/*!
	*	\brief \e Static method to skip a line from the instance.
	*
	*	\param[in] file : A ifstream (IOstream) which represents the file readed.
	*/
    static void ignoreLine(ifstream &file);
    /*!
	*	\brief \e Static method to skip a character from the instance.
	*
	*	\param[in] file : A ifstream (IOstream) which represents the file readed.
	*/
    static void ignoreChar(ifstream &file);
};

#endif
