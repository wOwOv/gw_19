#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>
#include <conio.h>
#include <locale.h>

SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;
HRESULT hResult;

int main()
{
    setlocale(LC_ALL, ""); //유니코드 스탠다드 출력 환경 세팅

    WCHAR State[1024];
    WCHAR Message[1024];
    SQLSMALLINT MsgLen;
    int NativeError;

    TCHAR BuffID[40] = { 0, };
    TCHAR BuffPass[40] = { 0, };
    TCHAR BuffNickname[40] = {0,};
    TCHAR BuffScore[40] = { 0, };

    SQLLEN lBuffID=0, lBuffPass=0, lBuffNickname=0, lBuffScore=0;

    //바이너리
    SQLLEN Size = SQL_LEN_DATA_AT_EXEC(512);
    SQLPOINTER Token;

    //환경설정을 할당하고 버전 속성을 설정한다.
    if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS) return 0;
    if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS) return 0;

    //접속핸들을 할당하고 연결한다.
    if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS) return 0;
    //접속핸들에 대한 에러 메시지 처리
    memset(Message, 0, 1024);
    MsgLen = 0;
    SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
    wprintf(L"%s\n", Message);

    //접속하기
    //SQLConnect(hDbc,(SQLTCHAR*)"testODBCName",SQL_NTS,(SQLTCHAR*)NULL,SQL_NTS,(SQLTCHAR*)NULL,SQL_NTS);

    WCHAR szConnect[1024] = L"Driver={MySQL ODBC 9.4 Unicode Driver};Server=127.0.0.1;Database=test;User=root;Password=ditto1234;PORT=2170;Option=3;";
    SQLTCHAR OutCon[255];
    SQLSMALLINT cbOutCon;

    SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)szConnect, wcslen(szConnect), OutCon, sizeof(OutCon) / 2, &cbOutCon, SQL_DRIVER_NOPROMPT);



    //접속에 대한 에러 메시지 처리
    SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
    wprintf(L"%s\n", Message);

    //명령핸들을 할당한다.
    if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS) return false;

    //바인딩하기(select문에서 데이터목록을 가져오기위해...)
    SQLBindCol(hStmt, 1, SQL_C_TCHAR, BuffID, 40, (SQLLEN*)&lBuffID);
    SQLBindCol(hStmt, 2, SQL_C_TCHAR, BuffPass, 40, (SQLLEN*)&lBuffPass);
    SQLBindCol(hStmt, 3, SQL_C_TCHAR, BuffNickname, 40, (SQLLEN*)lBuffNickname);
    SQLBindCol(hStmt, 4, SQL_C_TCHAR, BuffScore, 40, (SQLLEN*)&lBuffScore);
    SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

    //*
    //실행하기(INSERT INTO문)
    SQLExecDirect(hStmt, (SQLTCHAR*)L"INSERT INTO account(Pass,Nickname,Score) VALUES('6666','Mareep','13')", SQL_NTS);
    SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
    wprintf(L"%s\n\n", Message);
    //*/

    //실행하기(SELECT문 바인딩으로 출력)
    SQLExecDirect(hStmt, (SQLTCHAR*)L"SELECT * FROM account", SQL_NTS);
    SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);


    //결과출력
    while (SQLFetch(hStmt) != SQL_NO_DATA)
    {
        wprintf(L"ID: %s, Pass: %s, Nickname: %s, Score: %s\n", BuffID, BuffPass, BuffNickname, BuffScore);
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

        /*
        //쿼리 내에서 결과로 다시 쿼리를 날려야 하는 경우
        SQLSTMT hStmt2;
        if(SQLAllocHandle(SQL_HANDLE_STMT,hDbc,&hStmt2)!=SQL_SUCCESS) return false;
        hResult=SQLBindCol(hStmt2,1,SQL_C_CHAR,BuffPass,8,(SQLLEN*)&lBuffId);
        
        hResult=SQLExecDirect(hStmt2,(SQLTCHAR*)"SELECT Num FROM tblUser",SQL_NTS);
        SQLGetDiagRec(SQL_HANDLE_STMT, hStmt2, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

        while(SQLFetch(hStmt2)!=SQL_NO_DATA)
        {
        printf("ID: %s\n",BuffPass);
        printf("ID: %s, Pass: %s, Name: %s\n",BuffId,BuffPass,BuffName);
        }
        SQLCloseCursor(hStmt2);
        SQLFreeHandle(SQL_HANDLE_STMT,hStmt2);
        */
    }

    //마무리
    SQLCloseCursor(hStmt);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    SQLDisconnect(hDbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    SQLFreeHandle(SQL_HANDLE_ENV, hEnv);

    return 0;
}


/*
//SQL 에러의 내용을 한글로 자세히 알려주는 함수
SQLGetDiagRec
(
    SQL_HANDLE_ENV
    DBC
    STMT,
    핸들, 1, (SQLCHAR*)State, (SQLINTEGER*)&NativeError,
    (SQLCHAR*)Message, //에러메시지가 저장되는 버퍼
    MsgSize,            //버퍼 사이즈
    &MsgLen)            //버퍼 길이

    * 컬럼의 수를 리턴해주는 함수
    SQLNumResultCols(hStmt, (int)&컬럼값);
    
*컬럼의 이름을 얻어온다.
SQLDescribeCol(hStmt,컬럼번호, Buffer,sizeof(BuffeR),NULL,NULL,NULL,NULL,NULL)
)
*/