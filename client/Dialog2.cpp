// Dialog2.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "Dialog2.h"
#include "AccessDB.h"
#include "MsgEx.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void test2(void *p);
/////////////////////////////////////////////////////////////////////////////
// Dialog2 dialog

Dialog2::Dialog2(CWnd* pParent /*=NULL*/)
	: CDialog(Dialog2::IDD, pParent)
{
	//{{AFX_DATA_INIT(Dialog2)
	//}}AFX_DATA_INIT
}


void Dialog2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Dialog2)
	DDX_Control(pDX, IDC_EDIT3, m_bul);
	DDX_Control(pDX, IDC_EDIT2, m_Xxk);
	DDX_Control(pDX, IDC_EDIT1, m_name);
	//}}AFX_DATA_MAP
	// ����socket 
	clsoc = socket(AF_INET, SOCK_STREAM, 0);
	// ����Э���ַ��
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;//Э��汾
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);
	connect(clsoc, (sockaddr*)&addr, sizeof addr);
	m_name.SetWindowText(recname);
	_beginthread(test2,0,this);
}


BEGIN_MESSAGE_MAP(Dialog2, CDialog)
	//{{AFX_MSG_MAP(Dialog2)
	ON_BN_CLICKED(IDC_BUTTON1, OnSend)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Dialog2 message handlers
void test2(void *p){
	Dialog2 *cDlg=(Dialog2*)p;
	AccessDB db;
	db.OpenDataBase("chatlog.mdb");
	SYSTEMTIME st;
	char name1[64];//������Ϣ���û�
	char recname1[64];//������Ϣ���û�
	char recvbuff[1024];//���յ�����Ϣ
	char buff[1024];
	CString msg, rec,time;
	while(cDlg->flag)
	{
		memset(recname1,0,64);
		memset(name1, 0, 64);
		memset(recvbuff, 0, 1024);
		memset(buff, 0, 1024);
		recv(cDlg->clsoc, recvbuff, 64, 0);
		if (!strcmp(recvbuff, "to")) {
			recv(cDlg->clsoc, recname1, 64, 0);
			rec =recname1;//�����û�
			if (!rec.Compare(cDlg->name)) {//�Ƿ��͸���ǰ�û�,������(���ҵ�)
				recv(cDlg->clsoc, name1, 64, 0);
				msg=name1;//�����û�
				if (!msg.Compare(cDlg->recname)) {//�Ƿ�Ϊ��ǰ�Ի����û����͵���Ϣ(˭����)
					memset(recvbuff, 0, 1024);
					recv(cDlg->clsoc, recvbuff, 1024, 0);
					GetLocalTime(&st);
					time.Format("%04d/%02d/%02d  %02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
					sprintf(buff, "%02d:%02d:%02d %s˵:%s",st.wHour,st.wMinute,st.wSecond, (char*)(LPCSTR)cDlg->recname, recvbuff);//���յ���Ϣ
					cDlg->ADDLog(buff);
					msg=recvbuff;
					//���������¼��
					CString str = "insert into ["+cDlg->name+"] ([time],[name],[msg])values(('"+time+"'),('"+cDlg->recname+"'),('"+msg+"'))";
					db.Update((char*)(LPCSTR)str);
				}
				else {
					sprintf(buff, "%s����һ����Ϣ", name1);//�����û��������û�������Ϣ
					cDlg->ADDLog(buff);
				}
			}
		}
	}
	db.Close();
}

void Dialog2::ADDLog(CString log) 
{
	// TODO: Add your control notification handler code here
	CString msg;
	msg.Format("%s\n",(char*)(LPCSTR)log);
	m_Xxk.SetSel(-1);
	m_Xxk.ReplaceSel((LPCTSTR)msg);
	m_Xxk.SetSel(-1);
}

void Dialog2::OnSend() 
{
	CString str,time;
	AccessDB db;
	db.OpenDataBase("chatlog.mdb");
	SYSTEMTIME st;
	// TODO: Add your control notification handler code here
	m_bul.GetWindowText(str);
	if (!str.Compare("")) {
		this->MessageBox("˵��ʲô��");
		return;
	}
	send(clsoc,"to",2,0);
	Sleep(100);
	send(clsoc,(char*)(LPCSTR)recname,strlen(recname),0);//�����û�
	Sleep(100);
	send(clsoc,(char*)(LPCSTR)name, strlen(name),0);//�����û�
	Sleep(100);
	send(clsoc, str, strlen(str), 0);
	ADDLog(str);
	GetLocalTime(&st);
	time.Format("%04d/%02d/%02d  %02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	CString str1 = "insert into ["+name+"] ([time],[name],[msg],[rename])values(('"+time+"'),('"+name+"'),('"+str+"'),('"+recname+"'))";
	db.Update((char*)(LPCSTR)str1);
	db.Close();
}


void Dialog2::OnButton2() 
{
	// TODO: Add your control notification handler code here
	MsgEx pDlg;
	pDlg.name=name;//��ǰ�û�
	pDlg.recname=recname;//�����û�
	pDlg.DoModal();
}


void Dialog2::OnDestroy()
{
	CDialog::OnDestroy();
	// TODO: �ڴ˴������Ϣ����������
	flag = false;
	send(clsoc, "squit", 5, 0);//������ɾ��˽���׽���
	
}
