// CMyNewDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication3.h"
#include "CMyNewDlg.h"
#include "afxdialogex.h"
#include "MAINDlg.h"

// CMyNewDlg 对话框

IMPLEMENT_DYNAMIC(CMyNewDlg, CDialogEx)

CMyNewDlg::CMyNewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	
	, username(_T(""))
	, password(_T(""))
{

}

CMyNewDlg::~CMyNewDlg()
{
}

void CMyNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_EDIT1, m_username);
	//  DDX_Control(pDX, IDC_PASSWORD, m_password);
	DDX_Text(pDX, IDC_EDIT1, username);
	DDX_Text(pDX, IDC_PASSWORD, password);
}


BEGIN_MESSAGE_MAP(CMyNewDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMyNewDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMyNewDlg 消息处理程序


void CMyNewDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE); // 更新控件对应变量的值
	CEdit*pmyEdit = (CEdit*)GetDlgItem(IDC_PASSWORD); // 密码编辑框控件的指针
	pmyEdit->SetPasswordChar('*'); // 显示格式为星号
	if (username == "admin" && password == "123456")
	{
		// 进入主对话框界面
		CDialogEx::OnOK(); // 关闭登陆界面
		MAINDlg MAINDlg;
		MAINDlg.DoModal(); // 弹出主对话框（模态对话框）
	}
	else
	{
		// 弹出窗口提示用户名或密码不正确
		MessageBox(_T("用户名或密码不正确！"), _T("提示"), MB_OK);
		// 清空用户名和密码，便于重新输入
		username = "";
		password = "";
		UpdateData(FALSE); // 更新编辑框控件的显示值
	}
	
	CDialogEx::OnOK();
}
