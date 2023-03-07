#pragma once
#include <string.h>
#include <fstream>
#include <iostream> 
// MAINDlg 对话框

class MAINDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MAINDlg)

public:
	MAINDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MAINDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list;
	afx_msg void OnBnClickedOk();
};
