
// DLNA_PDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Unicast.h"
#include "Multicast.h"
#include "TCPClient.h"
#include "TCPServer.h"
#include "Device.h"
#include "DevList.h"
#include "Controller.h"
#include "afxmt.h"

// CDLNA_PDlg dialog
class CDLNA_PDlg : public CDialog, public Controller
{
// Construction
public:
	CDLNA_PDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DLNA_P_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
//OWN DECLARATIONS
    virtual void OnHttpResponseReceived(LPSTR response, BOOL xml, BOOL alive);
    virtual void OnConnectionTimeout();
    virtual void OnAVTransSetResp(LPSTR response);
    virtual void OnFileOpenError();
    virtual void OnSearchComplete();
    virtual void OnSearchStarted();
    virtual void OnBadDeviceLocation();
    virtual void OnNewDeviceFound();
    virtual void OnDeviceRemoved();
    virtual void OnNoNotifyForTooLong(LPSTR sn);
private:
    TCPClient tcpSockClient;
    TCPServer tcpSockServer;
    Unicast unicastSock;
    Multicast multicastSock;
    DevList devList;
    CMutex m_mutex;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CComboBox CBox;
    CComboBox CBoxDev;
    CEdit EditBox;
    CButton btn2;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton4();
    afx_msg void OnCbnSelchangeCombo2();
};
