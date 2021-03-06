//==================================================================================================================|
// Created 2015.01.29 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include "Mocker.h"
#include "DeclarationBlock.h"
#include "Variable.h"
#include "Utility.h"
#include "Class.h"

namespace vc { namespace mocker
{
	void Mocker::mock(const graph::Graph &graph, QIODevice &outDevice_Implementation, QIODevice &outDevice_Header)
	{
		buildBlock(graph.block());		

		QTextStream outStream(&outDevice_Implementation);
		for (const QString &line : mData.includes)
		{
			outStream << line << "\n";
		}

		for (const QString &line : mData.body)
		{
			outStream << line << "\n";
		}

		//build the header
		outStream.setDevice(&outDevice_Header);
		mData.header.buildHeader(outStream);
	}


	void Mocker::buildBlock(const graph::Block &block)
	{
		QListIterator<graph::Statement*> iter(block.statements());

		while (iter.hasNext())
		{
			graph::Statement *statement = iter.next();

			if (graph::Preprocessor *preprocessor = dynamic_cast<graph::Preprocessor*>(statement))
			{
				mData.includes.append(preprocessor->verbatim());
			}
			else if (graph::Function *function = dynamic_cast<graph::Function*>(statement))
			{
				//go into deceleration block
				DeclarationBlock decBlock(mData, *function);
			}
			else if (graph::Variable *variable = dynamic_cast<graph::Variable*>(statement)) // --happens with only global variables
			{
				Variable::createVar(mData, *variable);
			}
			else if (graph::Class *classDef = dynamic_cast<graph::Class*>(statement))
			{
				Class mockClass(mData, *classDef);
			}
			else
			{
				mData.body.append(Utility::createTabs(mData.scope) + statement->verbatim());
			}
		}
	}
}}