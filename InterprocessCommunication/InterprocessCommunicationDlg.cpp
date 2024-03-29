
// InterprocessCommunicationDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "InterprocessCommunication.h"
#include "InterprocessCommunicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// диалоговое окно CInterprocessCommunicationDlg



CInterprocessCommunicationDlg::CInterprocessCommunicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInterprocessCommunicationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInterprocessCommunicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, text);
}

BEGIN_MESSAGE_MAP(CInterprocessCommunicationDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CInterprocessCommunicationDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CInterprocessCommunicationDlg::OnBnClickedButtonClear)
	ON_EN_CHANGE(IDC_EDIT, &CInterprocessCommunicationDlg::OnEnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON_RUN, &CInterprocessCommunicationDlg::OnBnClickedButtonRun)
END_MESSAGE_MAP()


// обработчики сообщений CInterprocessCommunicationDlg

BOOL CInterprocessCommunicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	fileMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, maxLength, _T("InterprocessCommunication"));		// функция CreateFileMapping создает или открывает именованный или безымянный объект отображенного в памяти (проецируемого)	файла для заданного файла
	mapView = MapViewOfFile(fileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);												// функция MapViewOfFile отображает представление проецируемого файла в адресное пространство вызывающего процесса

	HANDLE checkHandler = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);										// открытие сущеструющего объекта события
	if (checkHandler == NULL)																					// если событие не существует
	{
		TextChangeEvent = CreateEvent(NULL, TRUE, FALSE, eventName);											// создание события
		AllClose = CreateEvent(NULL, TRUE, FALSE, eventNameClose);
		hMutex = CreateMutex(NULL, TRUE, mutexName);															// создает новый мьютекс. TRUE - сразу заблокирован
		sender = TRUE;																							// TRUE - этот экземпляр приложения является отправителем
		this->SetWindowText(_T("Отправитель"));																	// смена заголовка окна
	}
	else
	{
		TextChangeEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventName);										// открытие события (так как оно уже создано)
		AllClose = OpenEvent(EVENT_ALL_ACCESS, FALSE, eventNameClose);
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);													// открытие mutex
		if (hMutex == NULL)																											// при неудачном открытии
		{
			DWORD errorDW = GetLastError();																							// получение ошибки
			wchar_t *text;																											// переменная для хранения описания ошибки
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, errorDW, 0, (LPWSTR)&text, 0, NULL);	// получение текста ошибки
			MessageBox(L"Ошибка открытия mutex!\n" + CString(text), L"Внимание!", MB_ICONERROR);
			exit(-1);
		}
		hThread = CreateThread(NULL, 0, Thread, (LPVOID)this, 0, 0);											// запуск потоков
		hThread = CreateThread(NULL, 0, ThreadMutex, (LPVOID)this, 0, 0);
		this->SetWindowText(_T("Получатель"));
		text.SetReadOnly(TRUE);
		GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(FALSE);														// отключение кнопки
		text.SetWindowText((LPCWSTR)mapView);																	// прочитать и вывести содержимое общего отображаемого файла
	}
	CloseHandle(checkHandler);

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CInterprocessCommunicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CInterprocessCommunicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// Кнопка Выход
void CInterprocessCommunicationDlg::OnBnClickedButtonExit()
{
	ReleaseMutex(hMutex);											// освобождение мьютекса (анлочит ранее залоченный мьютекс)
	CloseHandle(hMutex);											// закрытие указателей
	TerminateThread(hThread, 0);									// завершение потоков
	TerminateThread(hThreadMutex, 0);
	CloseHandle(hThread);
	CloseHandle(hThreadMutex);
	exit(0);
}


// Кнопка Очистить
void CInterprocessCommunicationDlg::OnBnClickedButtonClear()
{
	GetDlgItem(IDC_EDIT)->SetWindowText(L"");
	CInterprocessCommunicationDlg::OnEnChangeEdit();
}


// Событие, вызываемое при изменении текста в поле ввода
void CInterprocessCommunicationDlg::OnEnChangeEdit()
{
	CString cstr;
	text.GetWindowText(cstr);

	if (sizeof(TCHAR) * cstr.GetLength() > maxLength)						// чтобы не превысить размер буфера и отображаемого в памяти файла
	{
		return;
	}

	memset(mapView, 0, maxLength);											// функция memset заполняет maxLength байтов блока памяти, через указатель mapView
	memcpy(mapView, cstr.GetBuffer(), sizeof(TCHAR) * cstr.GetLength());	// Функция memcpy копирует (sizeof(TCHAR) * cstr.GetLength()) байтов первого блока памяти, на который ссылается указатель cstr.GetBuffer(), во второй блок памяти, на который ссылается указатель mapView

	PulseEvent(TextChangeEvent);											// функция PulseEvent предназначена для кратковременного перевода объекта событие в свободное состояние с его последующим возвратом в занятое состояние
}


// Запуск ещё одной копии приложения
void CInterprocessCommunicationDlg::OnBnClickedButtonRun()
{
	TCHAR FileName[MAX_PATH + 1];																	// буфер для имени файла
	STARTUPINFO info;																				// для хранение информации о запускаемом процессе
	PROCESS_INFORMATION processInfo;																// для хранение информации о запускаемом процессе

	ZeroMemory(&info, sizeof(STARTUPINFO));															// обнуление переменной
	ZeroMemory(&processInfo, sizeof(PROCESS_INFORMATION));											// обнуление переменной

	GetModuleFileName(NULL, FileName, MAX_PATH + 1);												// получение пути до текущего исполняемого файла
	if (!CreateProcess(FileName, L"", NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
	{
		MessageBox(L"Ошибка запуска процесса\n" + CString(FileName), L"Внимание!", MB_ICONERROR);
	}
}


// Метод для потока (приём сообщений)
DWORD CInterprocessCommunicationDlg::Thread(LPVOID lpvoid_param)
{
	CInterprocessCommunicationDlg* param = (CInterprocessCommunicationDlg*)lpvoid_param;	// сохранение переданного параметра
	HANDLE msgEvent[2] = { param->TextChangeEvent, param->AllClose };						// указатель на события
	LPCWSTR data;																			// для приёма данных

	while (true)
	{
		DWORD check = WaitForMultipleObjects(2, msgEvent, FALSE, INFINITE);					// ждет, когда один из объектов (FALSE), хэндл которого был передан вторым аргументом, перейдёт в сигнальное состояние
		if (check == WAIT_OBJECT_0)															// если первый в перешёл в сигнальное состояние
		{
			if (param->sender == TRUE) return 1;											// если этот экземпляр и есть отправитель - завершить поток

			data = (LPCWSTR)param->mapView;													// чтение общего отображаемого файла
			param->text.SetWindowText(data);
		}
		if (check == WAIT_OBJECT_0 + 1)														// если второй в перешёл в сигнальное состояние
		{
			param->OnBnClickedButtonExit();													// закрыть форму
		}
	}

	return 0;
}


// Метод для потока (смена роли)
DWORD CInterprocessCommunicationDlg::ThreadMutex(LPVOID lpvoid_param)
{
	CInterprocessCommunicationDlg* param = (CInterprocessCommunicationDlg*)lpvoid_param;	// сохранение переданного параметра

	while (true)
	{
		DWORD check = WaitForSingleObject(param->hMutex, INFINITE);							// ждет, когда объект, хэндл которого был передан первым аргументом, перейдет в сигнальное состояние
		if (check == WAIT_OBJECT_0 || check == WAIT_ABANDONED)								// если mutex в сигнальном состоянии или предыдущий в очереде потерял его
		{
			param->sender = TRUE;
			param->SetWindowText(_T("Отправитель"));
			param->text.SetReadOnly(FALSE);
			param->GetDlgItem(IDC_BUTTON_CLEAR)->EnableWindow(TRUE);						// включение кнопки
			Sleep(INFINITE);																// остановка данного потока без потери mutex
		}
	}

	return 0;
}


// Переопределение метода приёма сообщений
BOOL CInterprocessCommunicationDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)														// если пришло событие о нажатии клавиши
	{
		if (pMsg->wParam == VK_F12)															// если нажата клавиша F12
		{
			PulseEvent(AllClose);															// переключить событие завешения в сигнальное состояние и обратно
			OnBnClickedButtonExit();														// закрыть форму
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);											// передать сообщение дальше - родительскому классу
}
