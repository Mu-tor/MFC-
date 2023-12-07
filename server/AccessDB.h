#pragma once
#include "MyDate.h"

//����ADO������
#import "msado15.dll" no_namespace rename("EOF","adoEOF")
class AccessDB
{
public:
	AccessDB(void);
	~AccessDB(void);

	BOOL OpenDataBase(char * dbName);
	BOOL Query(char * strSql);
	BOOL Update(char * strSql);

	BOOL IsEof(){
		return m_dbRecordSet->GetadoEOF();
	}

	void Next(){
		m_dbRecordSet->MoveNext();
	}

	void CloseConn(){
		m_dbConnection->Close();
	}

	void CloseRecord()
	{
		m_dbRecordSet->Close();
	}

	void Close(){
		if (m_dbRecordSet)
		{
			m_dbRecordSet->Close();	
			m_dbRecordSet =NULL;
		}
		if (m_dbConnection)
		{
			m_dbConnection->Close();
			m_dbConnection = NULL;
		}
	}

	void ReadString(char *,char *);
	double ReadDouble(char *);
	MyDate ReadDate(char *);
	MyDate ReadDateTime(char *);

	_ConnectionPtr	m_dbConnection; //���ݿ����Ӷ���
	_RecordsetPtr   m_dbRecordSet;	//���ݲ�ѯ�������
};
