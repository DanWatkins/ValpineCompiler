//==================================================================================================================|
// Created 2015.02.22 by John Koehn
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include "Class.h"
#include "Variable.h"
#include "DeclarationBlock.h"
#include "Utility.h"

namespace vc { namespace mocker
{
	Class::Class(MockerData &data, const graph::Class &classDef)
	{
		mScope = data.scope;
		buildClass(data, classDef);

		//add include to header file
		//data.header.addFunctionDec("#include \"" + mClassName + "\"");
	}

	void Class::buildClass(MockerData &data, const graph::Class &classDef)
	{
		mClassName = classDef.id();
		
		//add a new class to the header data
		data.header.addClass(mClassName);

		//loop through member and build the graph
		auto iter = vc::makeIter(classDef.members());
		while (iter.hasNext())
		{
			const graph::Class::Member *member = iter.next();

			switch (member->accessType)
			{
			case graph::Class::Private:
				buildMember(data, *member, scopePrivate);
				break;
			case graph::Class::Public:
				buildMember(data, *member, scopePublic);
				break;
			case graph::Class::Protected:
				buildMember(data, *member, scopeProtected);
			}
		}
	}
	
	void Class::buildMember(MockerData &data, const graph::Class::Member &member, const ScopeState state)
	{
		graph::Statement *statement = member.statement;
		if (auto * variable = dynamic_cast<graph::Variable*>(statement))
		{
			Variable::createVarClass(data, *variable, mClassName, state);
		}
		else if (auto * function = dynamic_cast<graph::Function*>(statement))
		{
			DeclarationBlock block(data, *function, mClassName, state);
		}
		else
		{
			data.body.append(Utility::createTabs(mScope));
		}
	}

}}