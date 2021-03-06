//==================================================================================================================|
// Created 2015.01.29 by Daniel L. Watkins
//
// Copyright (C) 2015 Valpineware
// This file is licensed under the MIT License.
//==================================================================================================================|

#include <QQuickView>

#include <Library.h>

class Application : public QQuickView
{
	Q_OBJECT

public:
	void init();

private slots:
	void onButtonCompileClicked();

private:
	void keyPressEvent(QKeyEvent *ev) override;
	
	QObject *mTextField_parseFilepath, *mTextField_mockFilepath, *mTextField_outputFilepath;
};