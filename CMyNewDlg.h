#pragma once


// CMyNewDlg 对话框

class CMyNewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMyNewDlg)

public:
	CMyNewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMyNewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CString m_username;
//	CString m_password;
	afx_msg void OnBnClickedOk();
	CString username;
	CString password;
};
