//==================================================================================================================|
// Created 2015.01.29 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#ifndef _vc_graph_Graph_h
#define _vc_graph_Graph_h

#include "Statement.h"

#include "Preprocessor.h"
#include "Function.h"
#include "ControlStructure.h"
#include "Block.h"
#include "Class.h"
#include "Variable.h"
#include "Expression.h"

namespace vc { namespace graph
{
	/**
	 * @brief An in-memory representation of a Valpine source file using a graph like data structure.
	 */
	class Graph
	{
	public:
		Block& block() { return mBlock; }
		const Block& block() const { return mBlock; }

		//TODO we need a move constructor

	private:
		Block mBlock;
	};
}}

#endif
