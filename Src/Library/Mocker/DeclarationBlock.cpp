//==================================================================================================================|
// Created 2015.02.20 by John Koehn
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include "Utility.h"
#include "Variable.h"
#include "ControlStructure.h"
#include "Function.h"
#include "DeclarationBlock.h"

namespace vc { namespace mocker
{
	DeclarationBlock::DeclarationBlock(MockerData &data, const graph::Function &function, QString classID, const ScopeState state)
	{
		mFunctions.enqueue(&function);

		createFunction(data, classID, state);
	}


	void DeclarationBlock::buildBlock(const graph::Block &block, MockerData &data, QQueue<const graph::Function*> &functions)
	{
		QListIterator<graph::Statement*> iter(block.statements());
		data.body.append(Utility::createTabs(data.scope) + "{");

		//increase scope level
		data.scope += 1;

		while (iter.hasNext())
		{
			graph::Statement *statement = iter.next();

			if (auto variable = dynamic_cast<graph::Variable*>(statement))
			{
				Variable::createVar(data, *variable);
			}
			else if (auto control = dynamic_cast<graph::ControlStructure*>(statement))
			{
				ControlStructure structure(*control, data, functions);
			}
			else if (auto function = dynamic_cast<graph::Function*>(statement))
			{
				functions.enqueue(function);
			}
			else if (auto preprocessor = dynamic_cast<graph::Preprocessor*>(statement))
			{
				data.body.append(Utility::createTabs(data.scope) + preprocessor->verbatim());
			}
			else if (statement)
			{ 
				data.body.append(Utility::createTabs(data.scope) + statement->verbatim());
			}
		}

		data.scope -= 1;
		data.body.append(Utility::createTabs(data.scope) + "}");
	}

	void DeclarationBlock::createFunction(MockerData &data, QString &classID, const ScopeState state)
	{
		Function newFunction(data, *(mFunctions.dequeue()), classID, mFunctions, state);

		while (!mFunctions.isEmpty())
		{
			//build the nested function and add any new nested functions to the end of the queue
			Function nestedFunction(data, *(mFunctions.dequeue()), classID, mFunctions, state);
		}
	}
}}