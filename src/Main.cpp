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
* \file Main.cpp
* \brief Main of the software.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License
*
*/

#include <sys/time.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstdlib>

#include "Argument.hpp"
#include "Parser.hpp"
#include "Data.hpp"
#include "Functions.hpp"
#include "ToFile.hpp"

/*!
*	\defgroup global Global Variables
*/

/*!
*	\defgroup main Main
*/

/*!
*	\fn int main(int argc, char *argv[])
*	\ingroup main
*
*	\brief This is the main of the software.
*	\param[in] argc : An integer which represents the number of arguments passed to the line command.
*	\param[in] argv : An array of character which represents all the arguments.
*/
int main(int argc, char *argv[])
{
	Data* data;
	std::list<Solution> allSolution;

	long int nbToCompute;
	long int nbWithNeighbor;
	long int nbSolLS(0);
	long int boxBeforeFitlering(0);
	long int boxAfterFiltering(0);
	long int boxAfterReconstruction(0);
	long int numberBoxComputed(0);
	long double boxTotal(0);

	//Time strucure
	struct timeval start, end, beginB, endB, beginBF, endBF, beginRC, endRC, beginLS, endLS;

	//## READING ARGUMENTS ##

	Argument::parse( argc, argv );

	if ( Argument::help )
	{
		Argument::usage( argv[0] );
		return 0;
	}
	else if ( Argument::filename.empty() )
	{
		std::cout << "Type for help : " << argv[0] << " --help" << std::endl;
		return 0;
	}

	if (Argument::verbose)
	{
		Argument::print( std::clog );
	}

	//## END READING ARGUMENTS ##

	std::srand( Argument::random_seed );

	data = Parser::Parsing(Argument::filename);

	if (Argument::mode_export)
	{
		ToFile::removeFiles();
		computeCorrelation(*data);
	}

	if (Argument::verbose)
	{
		std::clog << std::endl
		<< "+" << std::setfill('-') << std::setw(15) << "+"
			<< " INSTANCE " << "+" << std::setw(16) << "+" << std::endl
			<< std::setfill(' ')
			<< " " << std::setw(20) << std::left
			<< "Name " << "|" << std::setw(20) << std::right
			<< data->getFileName() << " " << std::endl
		<< " " << std::setw(20) << std::left
			<< "Facility " << "|" << std::setw(20) << std::right
			<< data->getnbFacility() << " " << std::endl
		<< " " << std::setw(20) << std::left
			<< "Customer " << "|" << std::setw(20) << std::right
			<< data->getnbCustomer() << " " << std::endl
		<< " " << std::setw(20) << std::left
			<< "Correlation " << "|" << std::setw(20) << std::right
			<< computeCorrelation(*data) << " " << std::endl
		<< "+" << std::setfill('-') << std::setw(42) << "+" << std::endl
		<< std::endl;
	}

	gettimeofday(&start, 0);
	
	//#############################
	//## Functions to create Box ##
	
		boxTotal = std::pow((long double)2, (int)data->getnbFacility() )-1;
	
		std::vector<Box*> vectorBox;
	
		gettimeofday(&beginB, 0);		
		numberBoxComputed = createBox(vectorBox, *data);		
		boxBeforeFitlering = vectorBox.size();		
		gettimeofday(&endB, 0);
       
		if (Argument::verbose)
		{
			std::clog << "+" << std::setfill('-') << std::setw(18) << "+"
				<< " BOX " << "+" << std::setw(18) << "+" << std::endl
			<< std::setfill (' ')
				<< "+" << std::setfill(' ') << std::setw(10) << " "
				<< " ! before filtering ! " << " " << std::setw(9) << "+" << std::endl
			<< " " << std::setw(20) << std::left
				<< "Box Total " << "|" << std::setw(20) << std::right
				<< boxTotal << " " << std::endl
			<< " " << std::setw(20) << std::left
				<< "Box Computed " << "|" << std::setw(20) << std::right
				<< numberBoxComputed << " " << std::endl
			<< " " << std::setw(20) << std::left
				<< "Box Non-Dominated" << "|" << std::setw(20) << std::right
				<< boxBeforeFitlering << " " << std::endl
			<< " " << std::setw(20) << std::left
				<< "Time (ms) " << "|" << std::setw(20) << std::right
				<< (1000*time_s_Diff(beginB,endB) + time_ms_Diff(beginB,endB)) << " " << std::endl
			<< "+" << std::setfill('-') << std::setw(42) << "+" << std::endl
			<< std::endl;
		}
	
	//##################################################
	//## Functions to filter Boxes and reconstruction ##
	
		std::vector<Box*> vectorBoxFinal;
	
		if (Argument::filtering)
		{
			gettimeofday(&beginBF, 0);
			boxFiltering(vectorBox, *data, nbToCompute, nbWithNeighbor);
			gettimeofday(&endBF, 0);
		
			boxAfterFiltering = vectorBox.size();	
			
			if (Argument::reconstruction)
			{
				gettimeofday(&beginRC, 0);
				recomposition(vectorBox, vectorBoxFinal, *data, nbToCompute, nbWithNeighbor);
				gettimeofday(&endRC, 0);
				boxAfterReconstruction = vectorBoxFinal.size();
			}
			else
			{
				vectorBoxFinal = vectorBox;
			}
		
			if (Argument::verbose)
			{
				std::clog << "+" << std::setfill('-') << std::setw(13)
					<< "+" << " BOX FILTERING " << "+" << std::setw(13) << "+" << std::endl
				<< std::setfill(' ')
				<< "+" << std::setfill(' ') << std::setw(10) << " "
					<< " ! after filtering !  " << " " << std::setw(9) << "+" << std::endl
				<< " " << std::setw(20) << std::left
					<< "Box " << "|" << std::setw(20) << std::right
					<< boxAfterFiltering << " " << std::endl
				<< " " << std::setw(20) << std::left
					<< "Time (ms) " << "|" << std::setw(20) << std::right
					<< (1000*time_s_Diff(beginBF,endBF) + time_ms_Diff(beginBF,endBF)) << " " << std::endl;
				if (Argument::reconstruction)
				{
					std::clog << "+" << std::setfill(' ') << std::setw(8) << " "
						<< " ! after reconstruction !  " << " " << std::setw(6) << "+" << std::endl
					<< " " << std::setw(20) << std::left
						<< "Box " << "|" << std::setw(20) << std::right
						<< boxAfterReconstruction << " " << std::endl
					<< " " << std::setw(20) << std::left
						<< "Time (ms) " << "|" << std::setw(20) << std::right
						<< (1000*time_s_Diff(beginRC,endRC) + time_ms_Diff(beginRC,endRC)) << " " << std::endl;
				}
				std::clog << "+" << std::setfill('-') << std::setw(42) << "+" << std::endl << std::endl;
			}
	        
		}
		else
		{
			vectorBoxFinal = vectorBox;
		}
	
	//###########################
	//## LABEL SETTING or MOGA ##

		if ( Argument::moga )
		{
			gettimeofday(&beginLS, 0);
			nbSolLS = runMOGA(vectorBoxFinal, *data, allSolution);
			gettimeofday(&endLS, 0);
		}
		else // use Label Setting
		{
			gettimeofday(&beginLS, 0);
			nbSolLS = runLabelSetting(vectorBoxFinal, *data, allSolution);
			gettimeofday(&endLS, 0);
		}

		if (Argument::verbose)
		{
			std::clog << "+" << std::setfill('-') << std::setw(Argument::moga?17:13) << "+"
				<< (Argument::moga ? " MOGA " : " LABEL SETTING ")
				<< "+" << std::setw(Argument::moga?18:13) << "+" << std::endl
			<< std::setfill (' ') << " " << std::setw(20) << std::left
				<< "Sol computed " << "|" << std::setw(20) << std::right
				<< nbSolLS << " " << std::endl
			<< " " << std::setw(20) << std::left
				<< "Time (ms) " << "|" << std::setw(20) << std::right
				<< (1000*time_s_Diff(beginLS,endLS) + time_ms_Diff(beginLS,endLS)) << " " << std::endl
			<< "+" << std::setfill('-') << std::setw(42) << "+" << std::endl;
		}
	
	//#########
	//## END ##		
       
	gettimeofday(&end, 0);
	std::clog << std::endl
	<< "+" << std::setfill('-') << std::setw(15) << "+"
		<< " SYNTHESIS " << "+" << std::setw(15) << "+" << std::endl
	<< std::setfill(' ') << " " << std::setw(20) << std::left
		<< "Instance " << "|" << std::setw(20) << std::right
		<< data->getFileName() << " " << std::endl
	<< " " << std::setw(20) << std::left
		<< "Solutions " << "|" << std::setw(20) << std::right
		<< nbSolLS << " " << std::endl
	<< " " << std::setw(20) << std::left
		<< "Total Time (ms) " << "|" << std::setw(20) << std::right
		<< (1000*time_s_Diff(start,end) + time_ms_Diff(start,end)) << " " << std::endl
	<< "+" << std::setfill('-') << std::setw(42) << "+" << std::endl << std::endl;
	
	if (Argument::mode_export)
	{
		if( system("./plot/plot.sh") != 0 )
		{
			std::cerr << " Failure in the execution of the script " << std::endl;
		}
	}
	else
	{
		std::list<Solution>::iterator iter;
		for ( iter = allSolution.begin(); iter != allSolution.end(); ++iter )
		{
			for (int k = 0; k < (*iter).getNbObjective(); ++k)
			{
				if (k > 0) std::cout << ' ';
				std::cout << (*iter).getObj(k);
			}
			std::cout << std::endl;
		}
	}

	vectorBox.clear();
	vectorBoxFinal.clear();		
	delete data;
    
	return 0;
}
