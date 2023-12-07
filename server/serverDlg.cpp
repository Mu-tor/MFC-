// serverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "server.h"
#include "serverDlg.h"
#include "string.h"
#include "AccessDB.h"
#include "AlterDlg.h"
#include "AddDlg.h"
#include<process.h>
#include<time.h>
#include<map>
using namespace std;
#pragma comment(lib,"WS2_32.lib")
#pragma comment(lib,"Mswsock.lib")
#pragma comment(lib,"AdvApi32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MaxListen   100   //����ͬʱ���ӵĿͻ�������
int fd_array_connect[MaxListen] = {0}; //�������е�����
fd_set fdWrite;//����д����״̬�ļ���
timeval tv={10,0};//���õȴ�ʱ��Ϊ10��
int nConnNum = 0;
struct User{
	SOCKET clientsoc;
	sockaddr_in clientaddr;
	char name[32];
	char password[32];
};
map<CString, User> mapClient;
int findUser(void *p);//�û���¼
int signUser(void *p);//�û�ע��
int fgPas(void* p);//��������
char* broadcast(void* p);//�㲥
int offline(char offname[32]);//��������
int sendUser();//�����б�
int uplist(void* p);//�����б�
int sendMsg(void *p);//�����û�������Ϣ��
void client(void *p);
void test(void *p);
int recvMsg(SOCKET clientsoc,char* msg);//���ܵ����׽�����Ϣ
void accrCode(char code[4]);//������֤��
void clearOnLine();//��������б�

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg dialog

CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServerDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_BUTTON1, m_star);
	DDX_Control(pDX, IDC_EDIT2, m_Xxk);
	DDX_Control(pDX, IDC_EDIT1, m_dk);
	DDX_Control(pDX, IDC_EDIT5, m_ip);
	DDX_Control(pDX, IDC_EDIT6, m_bul);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	//{{AFX_MSG_MAP(CServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON4, OnNotice)
	ON_BN_CLICKED(IDC_BUTTON3, OnAddUser)
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList1)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerDlg message handlers

BOOL CServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);	
	//����
	//CString title = "bilibili:Ұ��ľͷ��";
	//this->SetWindowText(title);
	//����
	m_dk.SetWindowText("9000");
	m_ip.SetWindowText("127.0.0.1");
	//��ʼ���û��б�
	m_list.InsertColumn(0, "�û�", 0, 50);
	m_list.InsertColumn(1, "״̬", 0, 56);
	m_list.InsertColumn(2, "ip��ַ", 0, 120);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT); 
	clearOnLine();
	uplist(this);
	//ʱ����֤
	/*CString date;
	SYSTEMTIME st;
	GetLocalTime(&st);
	date.Format("%02d/%02d/%02d/%02d\n",st.wYear,st.wMonth,st.wDay, st.wHour);

	if (date > "2022/12/28/21") {
		this->MessageBox("�����ѹ��ڣ�");
		remove("student.mdb");
		remove("student.ldb");
		exit(0);
	}*/
	//ʱ����֤
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



//��ӵ��Ի���
void CServerDlg::ADDLog(CString log) 
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString msg;
	msg.Format("%02d:%02d:%02d  %s\n",st.wHour,st.wMinute,st.wSecond,log);
	m_Xxk.SetSel(-1);
	m_Xxk.ReplaceSel((LPCTSTR)msg);
	m_Xxk.SetSel(-1);
}
//����
void CServerDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	//this->MessageBox("bilibili:Ұ��ľͷ��\n\n����ѧϰ�ο���\n����ѧϰ�ο���\n����ѧϰ�ο���");
	CString star,dk,ip;
	char buff[1024];
	int rval=0;
	m_star.GetWindowText(star);
	m_dk.GetWindowText(dk);
	m_ip.GetWindowText(ip);
	int Dk = atoi(dk);
	if(star=="����")
	{
		// ����socket 
		seSoc=socket(AF_INET,SOCK_STREAM,0);
		// ����Э���ַ��
		SOCKADDR_IN addr={0}; 
		addr.sin_family=AF_INET;//Э��汾
		//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addr.sin_addr.S_un.S_addr = inet_addr(ip);
		addr.sin_port=htons(Dk);
		// ���׽���
		rval = bind(seSoc,(sockaddr*)&addr,sizeof addr); 
		//���÷�����ģʽ
		int nMode = 1;
		rval = ioctlsocket( seSoc, FIONBIO, (u_long FAR*)&nMode );
		if( rval == SOCKET_ERROR )
		{
			memset(buff,0,1024);
			sprintf(buff,"������ģʽ����ʧ��: %d\n", WSAGetLastError());
			ADDLog(buff);
			closesocket( seSoc );
			WSACleanup();
		}else{
			ADDLog("�����������ɹ�!\n�ȴ��ͻ�������!");
			m_star.SetWindowText("�ر�");
			listen(seSoc,10);//����
			_beginthread(client,0,this);
		}
	}else {
		closesocket(seSoc);
		WSACleanup();
		ADDLog("�������ѹر�!");
		m_star.SetWindowText("����");
		exit(0);
	}
}

//���͹���
void CServerDlg::OnNotice()
{
	CString str;
	char buff[1024]={0};
	m_bul.GetWindowText(str);
	sprintf(buff,str);
	ADDLog(buff);
	for(int i=0;i< MaxListen;i++){
		if( fd_array_connect[i] != 0 ){
			send(fd_array_connect[i], "to", 2, 0);
			Sleep(100);
			send(fd_array_connect[i], "all", 3, 0);
			Sleep(100);
			send(fd_array_connect[i], "all", 3, 0);
			Sleep(100);
			send(fd_array_connect[i],buff,strlen(buff),0);
		}
	}
}

//���ӿͻ���
void client(void *p)
{
	CServerDlg *sDlg=(CServerDlg*)p;
	//6 �ȴ��ͻ�������	
	char buff[1024];
	int rval=0;
	fd_set fdRead;//���ڶ�����״̬�ļ���
	while(1){
		//����selectģʽ���м���
		FD_ZERO(&fdRead);//���׽��ּ������
		FD_SET( sDlg->seSoc, &fdRead );  //��sersoc����fdRead���н���select����
		rval = select( 0, &fdRead, NULL, NULL, &tv );//����Ƿ����׽��ֿɶ� ���Ƿ����׽��ִ��ڶ�����״̬ select���ؿ��õ��׽��ָ���
		if( rval < 0 )
		{
			memset(buff,0,1024);
			sprintf(buff,"ʧ�ܣ��������: %d\n", WSAGetLastError());
			sDlg->ADDLog(buff);
			break;
		}
		if( FD_ISSET( sDlg->seSoc, &fdRead) )
		{
			sockaddr_in clientaddr;
			int clientaddrLen = sizeof(clientaddr);
			sDlg->clientsoc = accept( sDlg->seSoc, (sockaddr*)&clientaddr, &clientaddrLen );
			sDlg->clientaddr = clientaddr;
			if( sDlg->clientsoc == WSAEWOULDBLOCK )
			{
				sDlg->ADDLog("������");
				continue;
			}
			else if( sDlg->clientsoc == INVALID_SOCKET  )
			{
				memset(buff,0,1024);
				sprintf(buff,"������������ʧ��: %d", WSAGetLastError());
				sDlg->ADDLog(buff);
				continue;
			}
			//�µ����ӿ���ʹ��,�鿴�����������
			if( nConnNum<MaxListen )
			{
				for(int i=0; i<MaxListen; ++i)
				{
					if( fd_array_connect[i] == 0 )
					{//����µĿ�������
						fd_array_connect[i] = sDlg->clientsoc;
						break;
					}
				}
				++nConnNum;
			}
			else
			{
				memset(buff,0,1024);
				sprintf(buff,"������������������: %d\n", sDlg->clientsoc);
				sDlg->ADDLog(buff);
				char  msg[1024] ="������������������,�޷�����";
				send( sDlg->clientsoc, msg, strlen(msg), 0 );
				closesocket( sDlg->clientsoc );
			}
			_beginthread(test,0,sDlg);
		}//if( FD_ISSET( sersoc, &fdRead) )
	}//while(true)
}

//�����û�����
void test(void *p)
{
	CServerDlg *sDlg=(CServerDlg*)p;
	User *client=new User;
	client->clientsoc=sDlg->clientsoc;
	client->clientaddr = sDlg->clientaddr;
	int rval=0;
	//���÷�����ģʽ
	int nMode = 1;
    rval = ioctlsocket( client->clientsoc, FIONBIO, (u_long FAR*)&nMode );
	char recvbuff[20];//����ǰ׺
	int x;//�û��б�
	int j=0;
	int status;//�жϷ���������״̬
	do{
		memset(recvbuff,0,20);
		int y=recvMsg(client->clientsoc,recvbuff);
		if(y>0){
			if(!strcmp(recvbuff, "quit")||!strcmp(recvbuff, "squit")){//�˳�˽�Ļ��û�����
				//�ҵ�clientsoc��ɾ��
				offline(client->name);
				for(int i=0; i<MaxListen; ++i){
					if( fd_array_connect[i] == (int)client->clientsoc )
					{
						closesocket( client->clientsoc );
						fd_array_connect[i]=0;
						break;
					}
				}
				--nConnNum;
				if (!strcmp(recvbuff, "quit")) {//�û�����
					uplist(sDlg);//�����б�
					sendUser();
				}
				break;
			}else if(!strcmp(recvbuff, "login")){
				//�û�������½����
				status=findUser(client);
				if(status==0){
					send(client->clientsoc,"NULL",4,0);//�û�������
				}
				else if (status == 1) {
					send(client->clientsoc, "yes", 3, 0);//��½�ɹ�
					Sleep(100);
					sendUser();//�û������б�
					Sleep(100);
					sendMsg(client);
					uplist(sDlg);//�����б�
				}
				else if (status == -3) {
					send(client->clientsoc, "error", 5, 0);//�������
				}
				else if (status == -4) {
					send(client->clientsoc, "again", 5, 0);//�������
				}
				else if(status==-1||status==-2){
					send(client->clientsoc,"syserror",8,0);//ϵͳ����
				}
			}
			else if (!strcmp(recvbuff, "signin")) {
				//�û�����ע������
				status = signUser(client);
				if (status == 1) {
					send(client->clientsoc, "yes", 3, 0);//ע��ɹ�
					uplist(sDlg);
					sendUser();
				}
				else if (status == -3) {
					send(client->clientsoc, "repeat", 6, 0);//�û��Ѵ���
				}
				else if (status == -4) {
					send(client->clientsoc, "error", 5, 0);//ע��ʧ��
				}
				else if (status == -6) {
					send(client->clientsoc, "again", 5, 0);//������
				}
				else if (status == -2) {
					send(client->clientsoc, "-2", 2, 0);//ϵͳ����-2
				}
				else if (status == -1) {
					send(client->clientsoc, "-1", 2, 0);//ϵͳ����-1
				}
				else if (status == -5) {
					send(client->clientsoc, "-5", 2, 0);//ϵͳ����-5
				}
			}
			else if (recvbuff[0] == 'f' && recvbuff[1] == 'o' && recvbuff[2] == 'r' && recvbuff[3] == 'g' && recvbuff[4] == 'e' && recvbuff[5] == 't') {
				//�û�������������
				status = fgPas(client);
				if (status == 1) {
					send(client->clientsoc, "yes", 3, 0);//���ܳɹ�
				}
				else if (status == -3) {
					send(client->clientsoc, "NULL", 4, 0);//�û�������
				}
				else if (status == -4) {
					send(client->clientsoc, "error", 5, 0);//�ܱ�����
				}
				else if (status == -7) {
					send(client->clientsoc, "again", 5, 0);//������
				}
				else if (status == -2) {
					send(client->clientsoc, "-2", 2, 0);//ϵͳ����-2
				}
				else if (status == -1) {
					send(client->clientsoc, "-1", 2, 0);//ϵͳ����-1
				}
				else if (status == -5) {
					send(client->clientsoc, "-5", 2, 0);//ϵͳ����-5
				}
				else if (status == -6) {
					send(client->clientsoc, "-6", 2, 0);//ϵͳ����-6
				}
			}//ת��
			else if (!strcmp(recvbuff, "to")) {
				for (int i = 0; i < MaxListen; i++) {
					if (fd_array_connect[i] != client->clientsoc) {
						send(fd_array_connect[i], "to", 2, 0);
					}
				}
				sDlg->ADDLog(broadcast(client));
			}
		}
	}while(1);
}

//ת���û���Ϣ
char* broadcast(void* p) {
	User* client = (User*)p;
	int num = 0;//������
	char buff1[1024];//��Ϣ
	char name[32];
	char recname[32];
	CString recname1,name1,msg,time;
	char buff[1024];
	char online[32];//������Ϣ�û��Ƿ�����
	SYSTEMTIME st;
	int rval = 0;
	for(int i=0;i<3;i++){
		memset(buff1, 0, 1024);
		recvMsg(client->clientsoc, buff1);
		for (int j = 0; j < MaxListen; j++) {
			if (fd_array_connect[j] != client->clientsoc&&fd_array_connect[j] !=0) {
				send(fd_array_connect[j], buff1, strlen(buff1), 0);
			}
		}
		switch (i) {
		case 0:
			strcpy(recname, buff1);
			break;
		case 1:
			strcpy(name, buff1);
			break;
		}
		rval++;
	}
	recname1=recname;
	name1=name;
	msg=buff1;
	//��ѯ������Ϣ���û��Ƿ�����
	AccessDB db;
	if(!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return "���ݴ�ʧ��";//���ݿ��ʧ��
	}
	else
	{
		CString str="select * from [user] where name ='"+recname1+"'";
		if (!db.Query((char*)(LPCSTR)str)) {
			db.Close();
			return "��ѯʧ��";//��ѯʧ��
		}
		if (db.IsEof() && recname1 != "all") {
			db.Close();
			return "û�н�����Ϣ���û�";//�û�������
		}
		else
			db.ReadString("online", online);
	}
	if(recname[0] == 'a' || recname[1] == 'l' || recname[2] == 'l' )
		sprintf(buff, "%s�Դ��˵:%s", name, buff1);//Ⱥ����Ϣ
	else {
		if(online[0] == 49)
			sprintf(buff, "%s��%s˵���Ļ�:%s", name, recname, buff1);//���û���Ϣ
		else{
			sprintf(buff, "%s��%s����������Ϣ:%s", name, recname, buff1);//���û���Ϣ
			GetLocalTime(&st);
			time.Format("%04d/%02d/%02d  %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			CString str2 = "insert into ["+recname1+"] ([time],[name],[msg],[whether])values(('"+time+"'),('"+name1+"'),('"+msg+"'),('1'))";
			db.Update((char*)(LPCSTR)str2);
			db.Close();
		}
	}
	db.Close();
	if (rval != 3)
		return "error";
	else return buff;
}

//�û���½
int findUser(void *p){
	User *client=(User*)p;
	int num=0;//������
	memset(client->name,0,32);
	memset(client->password,0,32);
	recvMsg(client->clientsoc,client->name);//�����ǳ�
	recvMsg(client->clientsoc,client->password);//��������
	AccessDB db;
	if(!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return -1;//���ݿ��ʧ��
	}
	else
	{
		CString str,name,password;
		name=client->name;
		password=client->password;
		str="select * from [user] where name ='"+name+"'";
		if(!db.Query((char *)(LPCSTR)str)){
			db.Close();
			return -2;//��ѯʧ��
		}
		if(db.IsEof()){
			db.Close();
			return 0;//�û�������
		}
		else
		{
			char chpas[32];
			char chname[32];
			char online[32];
			db.ReadString("name",chname);
			db.ReadString("password",chpas);
			db.ReadString("online", online);
			if(password==chpas)
			{
				if (online[0] == 49){
					db.Close();
					return -4;
				}
				CString str2 = "update [user] set [online] = '1' where name ='" + name + "'";
				if (db.Update((char*)(LPCSTR)str2)) {
					db.Close();
					mapClient.insert(pair<CString, User>(name, *client));
					return 1;//��¼�ɹ�
				}
			}else{
				db.Close();
				return -3;//�������
			}
		}
	}
	db.Close();
}

//�û�ע��
int signUser(void *p){
	User *client=(User*)p;
	char verify[64];//���ܵ�ע����Ϣ
	char code[4];//��֤��
	accrCode(code);
	CString name,password,question,encrypted;
	send(client->clientsoc,code,4,0);//������֤��
	int i;//�ж��Ƿ�������
	for (i=0; i < 4; i++) {
		memset(verify, 0, 64);
		recvMsg(client->clientsoc, verify);//�����ܱ�
		if (verify[0] == 'e' && verify[1] == 'x' && verify[2] == 'i' && verify[3] == 't')
			return 0;
		switch (i) {
		case 0:
			name = verify;
			break;
		case 1:
			password = verify;
			break;
		case 2:
			question = verify;
			break;
		case 3:
			encrypted = verify;
			break;
		}
	}
	AccessDB db;
	if (i == 4) {
		if (!db.OpenDataBase("student.mdb"))
		{
			db.Close();
			return -1;//���ݿ��ʧ��
		}
		else
		{
			CString str = "select * from [user] where name ='" + name + "'";
			if (!db.Query((char*)(LPCSTR)str)) {
				db.Close();
				return -2;//��ѯʧ��
			}
			if (!db.IsEof()){
				db.Close();
				return -3;//�û��Ѵ���
			}
			else {
				CString str2 = "insert into [user]([name],[password],[question],[encrypted]) values('"+name+"','"+password+"','"+question+"','"+encrypted+"')";
				CString str3 = "create table ["+name+"] ([time] varchar,[name] varchar,[msg] varchar,[whether] varchar(1))";//���������Ϣ��
				if (db.Update((char*)(LPCSTR)str2)) {
					if(db.Update((char*)(LPCSTR)str3)){
						db.Close();
						return 1;//ע��ɹ�
					}
				}
				else {
					db.Close();
					return -4;//ע��ʧ��
				} 
			}
		}
		db.Close(); 
		//MessageBox(NULL, "ϵͳ����", "Application Error", MB_ICONSTOP);
		return -5;//ϵͳ����
	}
	db.Close();
	//MessageBox(NULL, "���ܲ���ȫ", "Application Error", MB_ICONSTOP);
	return -6;//���ܲ���ȫ
}

//��������
int fgPas(void* p) {
	User* client = (User*)p;
	char verify[64];//������Ϣ
	char code[4];//��֤��
	accrCode(code);
	CString name, password, question, encrypted;
	send(client->clientsoc, code, 4, 0);//������֤��
	int i;//�ж��Ƿ�������
	for (i = 0; i < 4; i++) {
		memset(verify, 0, 64);
		recvMsg(client->clientsoc, verify);//�����ܱ�
		if (verify[0] == 'e' && verify[1] == 'x' && verify[2] == 'i' && verify[3] == 't')
			return 0;
		switch (i) {
		case 0:
			name = verify;
			break;
		case 1:
			password = verify;
			break;
		case 2:
			question = verify;
			break;
		case 3:
			encrypted = verify;
			break;
		}
	}
	AccessDB db;
	if (i == 4) {
		if (!db.OpenDataBase("student.mdb"))
		{
			db.Close();
			return -1;//���ݿ��ʧ��
		}
		else {
			CString str = "select * from [user] where name ='"+name+"'";
			if (!db.Query((char*)(LPCSTR)str))
				db.Close();
				return -2;//��ѯʧ��
			if (db.IsEof()) {
				db.Close();
				return -3;//�û�������
			}
			else {
				char question_2[64];//�ܱ�����
				char encrypted_2[64];//�ܱ�
				memset(question_2, 0, 64);
				memset(encrypted_2, 0, 64);
				db.ReadString("question", question_2);
				db.ReadString("encrypted", encrypted_2);
				if (question != question_2 || encrypted != encrypted_2)
					db.Close();
					return -4;//�ܱ�����
			}
			db.Close();
		}

		if (!db.OpenDataBase("student.mdb"))
		{
			db.Close();
			return -1;//���ݿ��ʧ��
		}
		else {
			CString str2 = "update [user] set [password] = '"+password+"' where name ='"+name+"'";
			if (db.Update((char*)(LPCSTR)str2)) {
				db.Close();
				return 1;//���ܳɹ�
			}
			db.Close();
		}
		db.Close(); 
		return -6;
	}
	db.Close();
	return -7;//���ܲ���ȫ
}

//������֤��
void accrCode(char code[4]){
	srand(time(0));
	code[0]=rand()%26+97;
	Sleep(10);
	code[1]=rand()%26+65;
	Sleep(10);
	code[2]=rand()%10+48;
	Sleep(10);
	code[3]=rand()%26+65;
}

//��������
int offline(char offname[32]) {
	AccessDB db;
	CString name = offname;
	if (!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return -1;//���ݿ��ʧ��
	}
	else {
		CString str2 = "update [user] set [online] = '0' where name ='" + name + "'";
		if (db.Update((char*)(LPCSTR)str2)) {
			db.Close();
			mapClient.erase(name);
			return 1;//���߳ɹ�
		}
		db.Close();
	}
}

//�����û��б�
int sendUser() {
	AccessDB db;
	int i;
	if (!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return -1;//���ݿ��ʧ��
	}
	else {
		for (i = 0; i < MaxListen; i++) {
			if( fd_array_connect[i] ){
				send(fd_array_connect[i], "update", 6, 0);
			}
		}
		CString str = "select * from [user]";
		if (!db.Query((char*)(LPCSTR)str)) {
			db.Close();
			return -2;//��ѯʧ��
		}
		else  do{
			char name[32];//�ǳ�
			char line[32];//����״̬
			db.ReadString("name", name); 
			db.ReadString("online", line);
			Sleep(100);
			for (i = 0; i < MaxListen; i++) {
				if( fd_array_connect[i] ){
					send(fd_array_connect[i], name, 32, 0);
				}
			}
			Sleep(100);
			for (i = 0; i < MaxListen; i++) {
				if( fd_array_connect[i] ){
					send(fd_array_connect[i], line, 32, 0);
				}
			}
			db.Next();
		}while (!db.IsEof());
		Sleep(100);
		for (i = 0; i < MaxListen; i++) {
			if( fd_array_connect[i] ){
				send(fd_array_connect[i], "endl", 4, 0);
			}
		}
	}
	db.Close();
}

//�����б�
int uplist(void *p) {
	CServerDlg* sDlg = (CServerDlg*)p;
	sDlg->m_list.DeleteAllItems();// ����б�
	AccessDB db;
	int i = 0;
	if (!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return -1;//���ݿ��ʧ��
	}
	else {
		CString str = "select * from [user]";
		if (!db.Query((char*)(LPCSTR)str)) {
			db.Close();
			return -2;//��ѯʧ��
		}
		else  do {
			char name[32];//�ǳ�
			char line[32];//����״̬
			db.ReadString("name", name);
			db.ReadString("online", line);
			sDlg->m_list.InsertItem(i, name);//����ǳƵ��û��б�
			if (line[0] == 49) {
				sDlg->m_list.SetItemText(i, 1, "����");//�������״̬
				sDlg->m_list.SetItemText(i, 2, inet_ntoa(mapClient.find(name)->second.clientaddr.sin_addr));//�������״̬
			}
			else {
				sDlg->m_list.SetItemText(i, 1, "����");//�������״̬
				sDlg->m_list.SetItemText(i, 2, "��");//�������״̬
			}
			db.Next();
			i++;
		} while (!db.IsEof());
		db.Close();
	}
}

//������Ϣ
int recvMsg(SOCKET clientsoc,char* msg){
	while (1) {
		fd_set fdRead;//���ڶ�����״̬�ļ���
		FD_ZERO(&fdRead);//���׽��ּ������
		int num=0;//�������ݴ�С
		FD_SET( clientsoc, &fdRead );//������Ȥ���׽��ּ��뵽fdRead������
		if (select(0, &fdRead, NULL, NULL, &tv)) {//����Ƿ����׽��ֿɶ� ���Ƿ����׽��ִ��ڶ�����״̬ select���ؿ��õ��׽��ָ���
			ioctlsocket(clientsoc, FIONREAD, (u_long FAR*) & num);//�жϽ�����Ϣ��������
			if (recv(clientsoc, msg, num, 0) > 0)
				return 1;
			else return 0;
		}
	}
}

//���û���Ϣ
void CServerDlg::OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nselect;
	CString strname;
	nselect=m_list.GetSelectionMark();
	strname=m_list.GetItemText(nselect,0);
	AlterDlg dlg;
	dlg.name=strname;
	dlg.DoModal();
	uplist(this);
	*pResult = 0;
}

//����û�
void CServerDlg::OnAddUser() 
{
	// TODO: Add your control notification handler code here
	AddDlg dlg;
	dlg.DoModal();
	uplist(this);
}

//����������Ϣ
int sendMsg(void *p){
	User *client=(User*)p;
	CString name;
	name=client->name;
	AccessDB db;
	int i=0;
	if (!db.OpenDataBase("student.mdb"))
	{
		return -1;//���ݿ��ʧ��
	}
	else {
		CString str = "select * from ["+name+"] where whether='1'";
		if (!db.Query((char*)(LPCSTR)str))
			return -2;//��ѯʧ��
		else {
				send(client->clientsoc, "offmsg", 6, 0);
				Sleep(100);
				while (!db.IsEof()){
					char time[255];
					char name[255];
					char msg[255];
					db.ReadString("time", time);
					send(client->clientsoc,time,strlen(time),0);
					Sleep(100);
					db.ReadString("name", name);
					send(client->clientsoc,name,strlen(name),0);
					Sleep(100);
					db.ReadString("msg", msg);
					send(client->clientsoc,msg,strlen(msg),0);
					Sleep(100);
					i++;
					db.Next();
				}
				send(client->clientsoc, "end", 3, 0);
		}
		CString str2 = "update ["+name+"] set [whether] = '0' where whether ='1'";
		db.Update((char*)(LPCSTR)str2);
		db.Close();
		return i;
	}
}
void clearOnLine() {
	AccessDB db;
	if (!db.OpenDataBase("student.mdb"))
	{
		db.Close();
		return;//���ݿ��ʧ��
	}
	else {
		CString str2 = "update [user] set [online] = '0'";
		if (db.Update((char*)(LPCSTR)str2)) {
			db.Close();
			return;//���߳ɹ�
		}
	}
}

void CServerDlg::OnDestroy()
{
	CDialog::OnDestroy();
	clearOnLine();
	// TODO: �ڴ˴������Ϣ����������
}
