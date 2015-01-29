//==================================================================================================================|
// Created 2015.01.28 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include <CppCompiler.h>
#include <Library.h>

void CppCompiler::run()
{
	QString program = "g++";
	QStringList arguments;
	arguments << "Main.cpp" << "-o" << "Main.exe";

	QObject::connect(&mCompilerProcess, SIGNAL(error(QProcess::ProcessError)),
					 this, SLOT(onCompileProcessError(QProcess::ProcessError)));

	QObject::connect(&mCompilerProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
					 this, SLOT(onCompileProcessFinished(int, QProcess::ExitStatus)));

	mCompilerProcess.start(program, arguments);
	mCompilerProcess.waitForFinished(2);

	if (mCompilerProcess.error() == QProcess::ProcessError::Timedout)
	{
		qDebug() << "Compile timed out after " << mCompileTimeoutMs/1000 << " seconds.";
	}
}



void CppCompiler::onCompileProcessError(QProcess::ProcessError error)
{
	qDebug() << "Error while compiling with G++, code=" << error;
}


void CppCompiler::onCompileProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug() << "Compilation finished";
}