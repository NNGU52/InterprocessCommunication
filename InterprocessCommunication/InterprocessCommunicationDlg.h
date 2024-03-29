
// InterprocessCommunicationDlg.h : файл заголовка
//

#pragma once
#include "afxwin.h"


// диалоговое окно CInterprocessCommunicationDlg
class CInterprocessCommunicationDlg : public CDialogEx
{
// Создание
public:
	CInterprocessCommunicationDlg(CWnd* pParent = NULL);	// стандартный конструктор

// Данные диалогового окна
	enum { IDD = IDD_INTERPROCESSCOMMUNICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Текст поля ввода
	CEdit text;
	// Для указания на отображаемый в памяти файл
	HANDLE fileMap;
	// Для отображения файла в адресном пространстве текущего процесса
	PVOID mapView;
	// Название события (когда обновится текст)
	LPCWSTR eventName = L"InterprocessCommunicationEvent";
	// Название события (завершить выполненеи)
	LPCWSTR eventNameClose = L"InterprocessCommunicationEventAllClose";
	// Имя мьютекса (именованный)
	LPCWSTR mutexName = L"InterprocessCommunicationMutex";
	// Максимальная длина сообщения
	unsigned int maxLength = 4096;
	// Указатель на событие изменения даных
	HANDLE TextChangeEvent;
	// Указатель на событие завершения работы
	HANDLE AllClose;
	// Указатель на поток (приём сообщений)
	HANDLE hThread;
	// Указатель на поток (смена роли)
	HANDLE hThreadMutex;
	// Дескриптор mutex
	HANDLE hMutex;
	// Указывает на то, является ли данный экземпляр приложения отправителем
	BOOL sender = FALSE;

	static DWORD WINAPI Thread(LPVOID param);
	static DWORD WINAPI ThreadMutex(LPVOID param);

	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnBnClickedButtonRun();
	BOOL CInterprocessCommunicationDlg::PreTranslateMessage(MSG* pMsg);
};
