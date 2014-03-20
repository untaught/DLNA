
// DLNA_PDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Unicast.h"
#include "Multicast.h"
#include "TCP.h"
#include "Device.h"
#include "DevList.h"
#include "Controller.h"
#include "Cap.h"

// CDLNA_PDlg dialog
class CDLNA_PDlg : public CDialog//, public Controller
{
// Construction
public:
	CDLNA_PDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DLNA_P_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
//OWN DECLARATIONS
private:
  Cap test;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    CEdit EBox;
    CComboBox CBox;
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    CComboBox CBoxDev;
    afx_msg void OnBnClickedButton4();
};
