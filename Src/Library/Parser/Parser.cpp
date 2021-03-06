//==================================================================================================================|
// Created 2015.01.29 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include "Parser.h"
#include <Graph/Utility.h>

namespace vc { namespace parser
{
	bool Parser::parseFile(const QString &filepath)
	{
		mCurrentGraph = graph::Graph();
		QFile file(filepath);

		if (!file.open(QIODevice::ReadOnly))
			return false;

		QTextStream textStream(&file);
		while (!textStream.atEnd())
		{
			QString line = textStream.readLine();
			mLineBuffer.append(line);
		}

		removeComments();
		adjustCurlyBraces();
		matchParenthesis();
		adjustStatements();

		//parse the global block (entire file) starting at line 0
		parseStatement_block(0, &mCurrentGraph.block());

		return true;
	}

	
	bool Parser::isBlankLine(const QString &line)
	{
		return line == "";
	}


	int Parser::parseStatement_preprocessor(int index, graph::Block &parent)
	{
		//TODO for now we will assume all preprocessor statements are 1 liners
		parent.appendStatement(graph::Preprocessor::make(mLineBuffer[index]));
		return index;
	}


	int Parser::parseStatement_classBlock(int index, graph::Class &host)
	{
		//if next line is an opening brace, it must be a block
		if (mLineBuffer.count() > index+1  &&  !QRegExp("\\s*\\{\\s*").exactMatch(mLineBuffer[++index]))
		{
			return index;
		}

		auto currentAccessType = graph::Class::Private;

		while (++index < mLineBuffer.count()  &&  !QRegExp("\\s*\\}\\s*").exactMatch(mLineBuffer[index]))
		{
			QString &line = mLineBuffer[index];
			
			//look for access modifiers
			auto accessType = graph::Utility::accessTypeForString(line);
			if (accessType != graph::Class::None)
			{
				currentAccessType = accessType;
			}
			//otherwise look for statements
			else
			{
				auto member = new graph::Class::Member;
				member->accessType = currentAccessType;

				// Statement : Function
				if (auto f = graph::Function::make(line, graph::ScopeType::ClassBlock))
				{
					index = parseStatement_subBlock(index+1, f->block());
					member->statement = f;
				}
				// Statement : Variable
				else
				{
					member->statement = graph::Variable::make(line);
				}

				host.addMember(member);
			}
		}

		return index;
	}


	int Parser::parseStatement_subBlock(int index, graph::Block &block)
	{
		//if next line is an opening brace, it must be a block
		if (QRegExp("\\s*\\{\\s*").exactMatch(mLineBuffer[index]))
		{
			index = parseStatement_block(++index, &block);
		}

		return index;
	}


	int Parser::parseStatement_block(int index, graph::Block *host)
	{
		while (index < mLineBuffer.count()  &&  !QRegExp("\\s*\\}\\s*").exactMatch(mLineBuffer[index]))
		{
			QString& line = mLineBuffer[index];

			// Statement : Preprocessor
			if (line.startsWith("#"))
			{
				index = parseStatement_preprocessor(index, *host);
			}
			else if (auto f = graph::Function::make(line, graph::ScopeType::ExecutionBlock))
			{
				host->appendStatement(f);
				index = parseStatement_subBlock(index+1, f->block());
			}
			else if (auto cs = graph::ControlStructure::make(line))
			{
				host->appendStatement(cs);
				index = parseStatement_subBlock(index+1, cs->block());
			}
			else if (auto cls = graph::Class::make(line))
			{
				host->appendStatement(cls);
				index = parseStatement_classBlock(index, *cls);
			}
			else if (auto variable = graph::Variable::make(line))
			{
				host->appendStatement(variable);
			}
			//anonymous block
			else if (QRegExp("\\s*\\{\\s*").exactMatch(line))
			{
				graph::Block *block = new graph::Block;
				host->appendStatement(block);
				index = parseStatement_subBlock(index, *block);
			}

			// Blank line
			else if (isBlankLine(line))
			{
				//do nothing
			}
			else
			{
				host->appendStatement(new graph::Statement(line));
			}

			index++;
		}

		return index;
	}


	void Parser::removeBlankLines()
	{
		for (int i=0; i<mLineBuffer.count(); i++)
		{
			if (QRegExp("\\s*").exactMatch(mLineBuffer[i]))
				mLineBuffer.remove(i--);
		}
	}


	void Parser::removeComments()
	{
		for (int i=0; i<mLineBuffer.count(); i++)
		{
			QString &line = mLineBuffer[i];

			int singleStart = line.indexOf("//");
			int multiStart = line.indexOf("/*");

#define PRECEDES(a, b) a > 0 && (b == -1 || a < b)

			if (PRECEDES(singleStart, multiStart))
				line.chop(line.count() - singleStart);
			else if (PRECEDES(multiStart, singleStart))
			{
				//Find the line with an end tag. Clear lines in between.
				for (int j=i; j<mLineBuffer.count(); j++)
				{
					int multiEnd = mLineBuffer[j].indexOf("*/");

					if (multiEnd != -1)
					{
						mLineBuffer[j].remove(multiStart, multiEnd-multiStart+2);
						break;
					}
					else
						mLineBuffer[j].clear();
				}
			}

#undef PRECEDES
		}

		removeBlankLines();
	}


	void cleanBrace(QVector<QString> &lineBuffer, const QString &what)
	{
		for (int i=0; i<lineBuffer.count(); i++)
		{
			//if this line doesn't have a {, or if the { is by itself, just go to the next line
			if (!lineBuffer[i].contains(what) || QRegExp("\\s*\\" + what + "\\s*").exactMatch(lineBuffer[i]))
				continue;

			//move everything after and including the first {
			int index = lineBuffer[i].indexOf(what);

			if (index != -1)
			{
				//is there anything other than whitespace before this?
				bool onlyPreWhiteSpace = QRegExp("\\s*\\" + what + ".*").exactMatch(lineBuffer[i]);

				int endCount = lineBuffer[i].count()-index;

				//if there is only pre whitespace, no need to move the { down with this line
				if (onlyPreWhiteSpace)
					endCount--;

				QString chopped = lineBuffer[i].right(endCount);

				lineBuffer[i].chop(endCount);
				lineBuffer.insert(i+1, 1, chopped);
			}
		}
	}


	void Parser::adjustCurlyBraces()
	{
		cleanBrace(mLineBuffer, "{");
		cleanBrace(mLineBuffer, "}");
	}


	void Parser::matchParenthesis()
	{
		for (int i=0; i<mLineBuffer.count(); i++)
		{
			QString &line = mLineBuffer[i];
			int openCount = line.count("(");
			int closedCount = line.count(")");
			
			if (openCount > closedCount)
			{
				int still = openCount-closedCount;
				int j=i+1;

				while (still > 0 && j < mLineBuffer.count())
				{
					QString &nextLine = mLineBuffer[j];
					int index = nextLine.indexOf(")");

					if (index != -1)
					{
						line.append(nextLine.left(index+1));
						nextLine.remove(0, index+1);
						still--;
					}
					else
						j++;
				}
			}
			else if (closedCount > openCount)
			{
				qDebug() << "Error: Too many closing parenthisis encountered on line " << i;
			}
		}

		removeBlankLines();
	}


	void Parser::adjustStatements()
	{
		for (int i=0; i<mLineBuffer.count(); i++)
		{
			QString &line = mLineBuffer[i];

			if (QRegExp(".*;\\s*\\S+.*").exactMatch(line))
			{
				//if we are in between two parenthesis, ignore this
				int index = line.indexOf(";");
				int openBefore = line.leftRef(index+1).count("(");
				int closedBefore = line.leftRef(index+1).count(")");

				if (openBefore > closedBefore)
					continue;

				//move everything after this semi-colon to the next line
				int chopCount = line.count() - index - 1;
				QString chopped = line.right(chopCount);
				line.chop(chopCount);
				mLineBuffer.insert(i+1, 1, chopped);
			}
		}
	}
}}