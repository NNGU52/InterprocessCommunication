
// InterprocessCommunication.h : главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CInterprocessCommunicationApp:
// О реализации данного класса см. InterprocessCommunication.cpp
//

class CInterprocessCommunicationApp : public CWinApp
{
public:
	CInterprocessCommunicationApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CInterprocessCommunicationApp theApp;