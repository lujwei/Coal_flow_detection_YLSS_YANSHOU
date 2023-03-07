// MAINDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFCApplication3.h"
#include "MAINDlg.h"
#include "afxdialogex.h"

using namespace std;
// MAINDlg 对话框

IMPLEMENT_DYNAMIC(MAINDlg, CDialogEx)

MAINDlg::MAINDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

MAINDlg::~MAINDlg()
{
}

void MAINDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
}


BEGIN_MESSAGE_MAP(MAINDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &MAINDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL MAINDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DWORD dwStyle = m_list.GetExtendedStyle();   //listcontrol部分  
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_list.SetExtendedStyle(dwStyle);
	//m_list.InsertColumn(0, _T(""), LVCFMT_CENTER, 120);
	m_list.InsertColumn(0, _T("编号"), LVCFMT_CENTER, 100);
	m_list.InsertColumn(1, _T("时间"), LVCFMT_CENTER, 150);
	
	m_list.InsertColumn(2, _T("总体积/立方分米"), LVCFMT_CENTER, 150);
	//m_list.InsertItem(0, _T("模板数据"), LVCFMT_CENTER); //如果报错可改为m_list.InsertItem(0,_T("模板数据"),LVCFMT_CENTER); 
	//m_list.InsertItem(1, _T("目标数据"), LVCFMT_CENTER);
	//m_list.InsertItem(2, _T("误差比对"), LVCFMT_CENTER);
	return TRUE;

}

// MAINDlg 消息处理程序


void MAINDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码


	string str = "C:\\data\\lishi3.txt";

	const char*p = str.data();
	FILE* fp;
	float point;
	int count = 0;
	fp = fopen(p, "r");

	float TmpBatchPoint1s[200000];

	while (fscanf(fp, "%f", &point) != EOF)
	{
		TmpBatchPoint1s[count] = point;

		count++;
	}
	fclose(fp);

	
	for (int i = (count / 7); i >0 ; i--)
	{




		CString str_angle_time;
		CString str_angle_volume;
		CString _index;
		_index.Format(_T("%d"), (count / 7) - i + 1);

		m_list.InsertItem((count / 7) - i, _index, LVCFMT_CENTER);
		str_angle_time.Format(_T("%.0f-%.0f-%.0f-%.0f-%.0f-%.0f"), TmpBatchPoint1s[i * 7 - 7], TmpBatchPoint1s[i * 7 - 6], TmpBatchPoint1s[i * 7 - 5], TmpBatchPoint1s[i * 7 - 4], TmpBatchPoint1s[i * 7 - 3], TmpBatchPoint1s[i * 7 - 2]);
		m_list.SetItemText((count / 7) - i, 1, str_angle_time);
		str_angle_volume.Format(_T("%0.2f"), TmpBatchPoint1s[i * 7 - 1]);
		m_list.SetItemText((count / 7) - i, 2, str_angle_volume);
	}
	
}
