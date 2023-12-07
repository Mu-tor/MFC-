// Dialog.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "Dialog.h"
#include "Dialog2.h"
#include "AccessDB.h"
#include "MsgEx.h"
#include "Inform.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void test(void *p);
void recmsg(void *p);
/////////////////////////////////////////////////////////////////////////////
// Dialog dialog


Dialog::Dialog(CWnd* pParent /*=NULL*/)
	: CDialog(Dialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(Dialog)
	//}}AFX_DATA_INIT
}


void Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Dialog)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_Bul, m_bul);
	DDX_Control(pDX, IDC_Xxk, m_Xxk);
	DDX_Control(pDX, IDC_Name, m_name);
	CString title = "bilibili:Ұ��ľͷ��";
	this->SetWindowText(title);
	//}}AFX_DATA_MAP
	m_name.SetWindowText(name);
	m_list.InsertColumn(0,"�û�",0,80);
	m_list.InsertColumn(1,"״̬",0,60);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT );
	char up[6];
	recv(clsoc,up,6,0);
	update();
	_beginthread(test,0,this);
}


BEGIN_MESSAGE_MAP(Dialog, CDialog)
	//{{AFX_MSG_MAP(Dialog)
	ON_BN_CLICKED(IDC_Send, OnSend)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// Dialog message handlersvoid test(void *Socket)
void test(void *p){
	//������Ϣ
	Dialog *cDlg=(Dialog*)p;
	char recvbuff[1024];
	CString time,name,msg,buff;
	AccessDB db;
	db.OpenDataBase("chatlog.mdb");
	SYSTEMTIME st;
	int rval;
	recmsg(cDlg);
	while(1){
		memset(recvbuff,0,1024);
		rval=recv(cDlg->clsoc,recvbuff,64,0);
		msg=recvbuff;
		if(msg=="update")
			cDlg->update();
		else if (msg=="to") {
			memset(recvbuff, 0, 1024);
			recv(cDlg->clsoc, recvbuff, 1024, 0);
			msg = recvbuff;
			//�Ƿ���Ȩ�޽�����Ϣ,������
			if (msg=="all" || msg == cDlg->name) {
				memset(recvbuff, 0, 1024);
				recv(cDlg->clsoc, recvbuff, 1024, 0);
				name=recvbuff;
				memset(recvbuff, 0, 1024);
				recv(cDlg->clsoc, recvbuff, 1024, 0);
				GetLocalTime(&st);
				time.Format("%04d/%02d/%02d  %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
				if (msg == cDlg->name){
				//�Ƿ�Ϊ˽��
					if(cDlg->strname!= name){
						//�Ƿ��Ѵ�˽�ĶԻ���
						msg=recvbuff;
						buff.Format("%02d:%02d:%02d  %s����˵���Ļ�:%s",st.wHour,st.wMinute,st.wSecond,name,msg);
						cDlg->ADDLog(buff);
						//���������¼��
						CString str = "insert into ["+cDlg->name+"] ([time],[name],[msg])values(('"+time+"'),('"+name+"'),('"+msg+"'))";
						db.Update((char*)(LPCSTR)str);
					}
				}else{
					msg=recvbuff;
					if(name=="all"){
						/*CString str="select "+msg+" from [����] where time ='"+time+"'";
						db.Query((char *)(LPCSTR)str);
						if(db.IsEof()){*/
							CString str1 = "insert into [����] ([time],[msg])values(('"+time+"'),('"+msg+"'))";
							db.Update((char*)(LPCSTR)str1);
						//}
						buff.Format("����:%s",msg);
						cDlg->MessageBox(buff);
					}else{
						buff.Format("%02d:%02d:%02d  %s�Դ��˵:%s",st.wHour,st.wMinute,st.wSecond, name, msg);//ȫ����Ϣ
						cDlg->ADDLog(buff);
						//���������¼��
						CString str = "insert into [all] ([time],[name],[msg])values(('"+time+"'),('"+name+"'),('"+msg+"'))";
						db.Update((char*)(LPCSTR)str);
					}
				}
			}else{
				recv(cDlg->clsoc, recvbuff, 1024, 0);//���͵����׽��ֵ���Ϣ�����Ƚ��գ�Ȼ����
				recv(cDlg->clsoc, recvbuff, 1024, 0);
			}
		}else if(rval<0){
			cDlg->MessageBox("�������չ��ˣ���һ�Ұ�");
			exit(0); 
			DestroyWindow(0);
		}
	}
	db.Close();
}

void Dialog::ADDLog(CString log) 
{
	// TODO: Add your control notification handler code here
	CString msg;
	msg.Format("%s\n",(char*)(LPCSTR)log);
	m_Xxk.SetSel(-1);
	m_Xxk.ReplaceSel((LPCTSTR)msg);
	m_Xxk.SetSel(-1);
}

//����������Ϣ
void recmsg(void *p)
{
	Dialog *cDlg=(Dialog*)p;
	char recvbuff[255] = {0};
	CString time,name,msg;
	int num=0;
	AccessDB db;
	db.OpenDataBase("chatlog.mdb");
	recv(cDlg->clsoc,recvbuff,6,0);
	msg = recvbuff;
	if(!strcmp(recvbuff, "offmsg")){
		cDlg->ADDLog("���յ���������Ϣ:");
		memset(recvbuff, 0, 255);
		recv(cDlg->clsoc, recvbuff, 255, 0);
		while(recvbuff[0]!='e'||recvbuff[1]!='n'||recvbuff[2]!='d'){
			time=recvbuff;
			memset(recvbuff, 0, 255);
			recv(cDlg->clsoc, recvbuff, 255, 0);
			name=recvbuff;
			memset(recvbuff, 0, 255);
			recv(cDlg->clsoc, recvbuff, 255, 0);
			msg.Format("%s\t%s˵:%s",time,name,recvbuff);
			cDlg->ADDLog(msg);
			msg=recvbuff;
			CString str = "insert into ["+cDlg->name+"] ([time],[name],[msg])values(('"+time+"'),('"+name+"'),('"+msg+"'))";
			db.Update((char*)(LPCSTR)str);
			memset(recvbuff, 0, 255);
			recv(cDlg->clsoc, recvbuff, 255, 0);
			num++;
		}
		if(!num)
			cDlg->ADDLog("��");
		db.Close();
	}
}

//������Ϣ
void Dialog::OnSend()
{
	CString str,time;
	AccessDB db;
	SYSTEMTIME st;
	db.OpenDataBase("chatlog.mdb");
	// TODO: Add your control notification handler code here	
	m_bul.GetWindowText(str);
	if (!str.Compare("")) {
		this->MessageBox("˵��ʲô��");
		return;
	}
	send(clsoc,"to",2,0);
	Sleep(100);
	send(clsoc,"all",3,0);
	Sleep(100);
	send(clsoc,(char*)(LPCSTR)name,strlen(name),0);
	Sleep(100);
	send(clsoc,str,strlen(str),0);
	ADDLog(str);
	GetLocalTime(&st);
	time.Format("%04d/%02d/%02d  %02d:%02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	CString str1 = "insert into [all] ([time],[name],[msg],[rename])values(('"+time+"'),('"+name+"'),('"+str+"'),('all'))";
	db.Update((char*)(LPCSTR)str1);
	db.Close();
}

//�ͻ��˹ر�
void Dialog::OnDestroy()
{
	// TODO: Add your message handler code here
	CDialog::OnDestroy();
	send(clsoc,"quit",4,0);
}

//�����û��б�
void Dialog::update()
{
	char recvbuff[64] = { 0 };
	int i=0;
	m_list.DeleteAllItems();// ���������
	recv(clsoc,recvbuff,64,0);
	CString msg;
	msg = recvbuff;
	while (msg!="endl") {
		m_list.InsertItem(i, msg);//����ǳƵ��û��б�
		memset(recvbuff,0,64);
		recv(clsoc,recvbuff,64,0);
		m_list.SetItemText(i, 1, recvbuff[0] == 49 ? "����" : "����");//�������״̬
		memset(recvbuff,0,64);
		recv(clsoc,recvbuff,64,0);
		msg = recvbuff;
		i++;
	}
}

//��˽�ĶԻ���
void Dialog::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nselect;
	nselect=m_list.GetSelectionMark();
	strname=m_list.GetItemText(nselect,0);
	Dialog2 pDlg;
	pDlg.name=name;//��ǰ�û�
	pDlg.recname = strname;//�����û�
	pDlg.flag = true;//�����û�
	pDlg.DoModal();
	strname = "";
	*pResult = 0;
}

void Dialog::OnButton2() 
{
	// TODO: Add your control notification handler code here
	MsgEx pDlg;
	pDlg.name="all";//��ǰ�û�
	pDlg.recname="all";//�����û�
	pDlg.DoModal();
}

void Dialog::OnButton4() 
{
	// TODO: Add your control notification handler code here
	Inform pDlg; //����
	pDlg.DoModal();
}
