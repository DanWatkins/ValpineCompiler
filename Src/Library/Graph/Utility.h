//==================================================================================================================|
// Created 2015.02.08 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#ifndef _vc_graph_Utility_h
#define _vc_graph_Utility_h

#include <Library.h>

#include "Class.h"

namespace vc { namespace graph
{
	class TypeExpression;

	/**
	 * @brief 
	 */
	class Utility
	{
	public:
		static void breakUpByWhitespace(const QString &what, QStringList &list);

		static bool parseTypeExpression(QStringListIterator &i, TypeExpression &typeExpression);

		/**
		 * @returns true if \what looks like an id and is not a reserved id.
		 */
		static bool couldBeIdentifier(const QString &what);

		/**
		 * @returns true if \p what looks like an id and is not a reserved id (except for reserved types).
		 */
		static bool couldBeTypeIdentifier(const QString &what);


		static void breakUpOperators(QString &what, const QStringList &operators);


		static Class::AccessType accessTypeForString(const QString what);

	private:
		struct ReservedIds
		{
			QStringList general, types, control, typeDeclarators, access, casting;

			bool anyContain(const QString &str)
			{
				return	(general.contains(str) ||
						types.contains(str) ||
						control.contains(str) ||
						typeDeclarators.contains(str) ||
						access.contains(str) ||
						casting.contains(str));
			}
		};

		static ReservedIds mReservedIds;
		static ReservedIds buildReservedIds();
	};
}}

#endif
