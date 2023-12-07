// SigninDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "SigninDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SigninDlg dialog


SigninDlg::SigninDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SigninDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SigninDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SigninDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SigninDlg)
	DDX_Control(pDX, IDC_EDIT7, m_code2);
	DDX_Control(pDX, IDC_EDIT6, m_code1);
	DDX_Control(pDX, IDC_COMBO1, m_question);
	DDX_Control(pDX, IDC_EDIT4, m_encrypted);
	DDX_Control(pDX, IDC_EDIT3, m_password2);
	DDX_Control(pDX, IDC_EDIT2, m_password1);
	DDX_Control(pDX, IDC_EDIT1, m_name);
	//}}AFX_DATA_MAP
	// ����socket 
	clsoc=socket(AF_INET,SOCK_STREAM,0);
	// ����Э���ַ��
	SOCKADDR_IN addr={0}; 
	addr.sin_family=AF_INET;//Э��汾
	addr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	addr.sin_port=htons(9000);
	int r=connect(clsoc,(sockaddr*)&addr,sizeof addr);
	if(r==-1)
	{
		MessageBox("��������û˯�Ѱɣ�");
		closesocket(clsoc);
		exit(0);
	}
	send(clsoc,"signin",6,0);
	memset(code,0,5);
	recv(clsoc,code,5,0);
	m_code1.SetWindowText(code);
}


BEGIN_MESSAGE_MAP(SigninDlg, CDialog)
	//{{AFX_MSG_MAP(SigninDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SigninDlg message handlers

void SigninDlg::OnOK()
{
	// TODO: Add extra validation here
	CString name,password1,password2,question,encrypted,code2;
	char recvbuff[20]={0};
	m_name.GetWindowText(name);
	m_password1.GetWindowText(password1);
	m_password2.GetWindowText(password2);
	m_question.GetWindowText(question);
	m_encrypted.GetWindowText(encrypted);
	m_code2.GetWindowText(code2);
	if(name==""||password1==""||password2==""||question==""||encrypted==""){
		MessageBox("��ʵ�ѱ�������");
		return;
	}else if(password1!=password2){
		MessageBox("�����ˣ�������������벻һ����");
		password2=="";
		return;
	}else if (strcmp(strlwr(code),strlwr((LPSTR)(LPCTSTR)code2))) {
		MessageBox("��֤�붼�����");
		code2=="";
		return;
	}else {
		send(clsoc,(char*)(LPCSTR)name,strlen(name),0);
		Sleep(100);
		send(clsoc,(char*)(LPCSTR)password1,strlen(password1),0);
		Sleep(100);
		send(clsoc,(char*)(LPCSTR)question,strlen(question),0);
		Sleep(100);
		send(clsoc,(char*)(LPCSTR)encrypted,strlen(encrypted),0);
		int rval=recv(clsoc,recvbuff,20,0);
		if(rval>0){
			if(!strcmp(recvbuff, "yes")){
				MessageBox("�ɹ�Ǳ��з����!");
				exit(0);
			}else if(!strcmp(recvbuff, "repeat")){
				MessageBox("����Ƕ��Ѿ�����!");
				this->EndDialog(-1);//�رնԻ���
			}else if(!strcmp(recvbuff, "error")){
				MessageBox("Ǳ��ʧ��!");
				this->EndDialog(-1);//�رնԻ���
			}else if(!strcmp(recvbuff, "again")){
				MessageBox("����һ�ΰ�!");
				this->EndDialog(-1);//�رնԻ���
			}
			else {
				MessageBox("��������̫��!");
				this->EndDialog(-1);//�رնԻ���
			}
		}
	}
	closesocket(clsoc);
}

void SigninDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	send(clsoc,"exit",5,0);
	closesocket(clsoc);
}

void SigninDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
	send(clsoc,"exit",5,0);
	closesocket(clsoc);
}
