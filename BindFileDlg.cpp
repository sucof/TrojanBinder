////////////////////////////////////////////////////////////////////////////
// 类名：CBindFileDlg
// 功能：进行可执行文件的绑定工作。
// 作者：徐景周(Johnny Xu, xujingzhou2016@gmail.com)
// 组织：未来工作室(Future Studio)
// 日期：2001.12.1
////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "BindFile.h"
#include "BindFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct MODIFY_DATA {
     unsigned int finder; // 常量(定位自身)
     _off_t my_length;      //文件长度(自身)
} modify_data = {0x12345678, 0};

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CWBButton	m_AboutOK;        //改为位图按钮显示
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
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
	DDX_Control(pDX, IDOK, m_AboutOK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBindFileDlg dialog

CBindFileDlg::CBindFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBindFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBindFileDlg)
	m_strFirstPath = _T("");
	m_strSecondPath = _T("");
	m_strFinalPath = _T("");
	m_Parts = _T("状态显示:");
	m_Sync = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBindFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBindFileDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_SECONDBROWSE, m_SecondBrowse);
	DDX_Control(pDX, IDC_FIRSTBROWSE, m_FirstBrowse);
	DDX_Control(pDX, IDC_FINALBROWSE, m_FinalBrowse);
	DDX_Control(pDX, IDC_BUTTON_BINDFILE, m_BindFile);
	DDX_Control(pDX, IDC_BUTTON_ABOUT, m_About);
	DDX_Control(pDX, IDC_SECONDPATH, m_SecondPath);
	DDX_Control(pDX, IDC_FIRSTPATH, m_FirstPath);
	DDX_Control(pDX, IDC_FINALPATH, m_FinalPath);
	DDX_Text(pDX, IDC_FIRSTPATH, m_strFirstPath);
	DDX_Text(pDX, IDC_SECONDPATH, m_strSecondPath);
	DDX_Text(pDX, IDC_FINALPATH, m_strFinalPath);
	DDX_Text(pDX, IDC_PARTS, m_Parts);
	DDX_Radio(pDX, IDC_RADIO_SYNC, m_Sync);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBindFileDlg, CDialog)
	//{{AFX_MSG_MAP(CBindFileDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnButtonAbout)
	ON_BN_CLICKED(IDC_FIRSTBROWSE, OnFirstBrowse)
	ON_BN_CLICKED(IDC_SECONDBROWSE, OnSecondBrowse)
	ON_BN_CLICKED(IDC_FINALBROWSE, OnFinalBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BINDFILE, OnButtonBindFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBindFileDlg message handlers

BOOL CBindFileDlg::OnInitDialog()
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
	
    //把两个文件选择路径的静态框，设置内部显示方式为,
	//在显示长路径名时，省略中间路径，只显示两边路径。
    m_FinalPath.SubclassDlgItem (IDC_FINALPATH, this);
	m_FinalPath.SetPath (TRUE);
	m_FirstPath.SubclassDlgItem (IDC_FIRSTPATH, this);
	m_FirstPath.SetPath (TRUE);
	m_SecondPath.SubclassDlgItem (IDC_SECONDPATH, this);
	m_SecondPath.SetPath (TRUE);

	//设置位图按钮
	m_FirstBrowse.LoadBitmaps(IDB_BITMAP2,6, 1, 1, 1, 1 );
	m_FirstBrowse.SetFontColor(RGB(240,200,90));
	m_SecondBrowse.LoadBitmaps(IDB_BITMAP2,6, 1, 1, 1, 1 );
	m_SecondBrowse.SetFontColor(RGB(240,200,90));
	m_FinalBrowse.LoadBitmaps(IDB_BITMAP2,6, 1, 1, 1, 1 );
	m_FinalBrowse.SetFontColor(RGB(240,200,90));
	m_BindFile.LoadBitmaps(IDB_BITMAP1,5, 5, 5, 5, 4 );
	m_About.LoadBitmaps(IDB_BITMAP1,5, 5, 5, 5, 4 );
	m_Cancel.LoadBitmaps(IDB_BITMAP1,5, 5, 5, 5, 4 );

	//在此初始化渐变色进度条
	m_Progress.SetRange32(1,500);
    m_Progress.SetBkColor(RGB(160,180,220));
	m_Progress.ShowPercent(true);	
	m_Progress.SetPos(500);

	//初始置各文件名变量为空
	strFirstFilePath = "";
	strSecondFilePath = "";
	strFinalFilePath = "";

	//初始化变量
	prog1_length = 0;
	his_name = "";
	buf = NULL;
	//获取自身文件名
	::GetModuleFileName(0, my_name, sizeof(my_name));

	struct _stat ST; 
	_stat(my_name, &ST);
	//此外加入捆绑器程序的最终大小，来判断是绑定文件还是分解执行文件
	if(ST.st_size > 1.6*1024*1024) //184*1024) 
	{
        Unbind_and_Run(); //分离文件并运行
	    exit(0);    //直接退出程序,不显示捆绑程序画面
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBindFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBindFileDlg::OnPaint() 
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
HCURSOR CBindFileDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//枚举所有图标资源
BOOL CALLBACK my_enum_res_callback(
 HMODULE hExe,   // 资源句柄
 LPCTSTR lpszType,  // 资源类型
 LPTSTR lpszName,   // 资源名称
 LPARAM lParam    // 自定义消息参数
)
{
     HRSRC hRsrc = 0;
     HGLOBAL hMem;
     DWORD nDataLen;
     NEWHEADER* pDirHeader;
     RESDIR* pResDir;
     BYTE* pData;
     unsigned int k;

     my_enum_res_callback_data* pMyDataStruct = (my_enum_res_callback_data*)lParam;

     hRsrc = FindResource(hExe, lpszName, RT_GROUP_ICON);
     hMem = LoadResource(hExe, hRsrc);
     pDirHeader = (NEWHEADER*)LockResource(hMem);
     pResDir = (RESDIR*)(pDirHeader+1);

     for (k = 0; k < pDirHeader->ResCount; k++)
     {
          if (pResDir[k].BytesInRes == pMyDataStruct->pcResDir->BytesInRes &&
               pResDir[k].BitCount == pMyDataStruct->pcResDir->BitCount &&
               pResDir[k].Planes == pMyDataStruct->pcResDir->Planes &&
               memcmp(&pResDir[k].Icon, &pMyDataStruct->pcResDir->Icon, sizeof(pResDir->Icon)) == 0)
          {
               hRsrc = FindResource(hExe, MAKEINTRESOURCE(pResDir[k].IconCursorId), RT_ICON); 
               hMem = LoadResource(hExe, hRsrc );

               nDataLen = SizeofResource( hExe, hRsrc );
               pData =(BYTE*)LockResource(hMem);

#ifdef DEBUG_PRINT
               fprintf(stderr, "\t目录%d中发现 %d-th 图标，序号(ID)=%d (大小: %d)\n", 
                    k, lpszName, pResDir[k].IconCursorId, nDataLen);
#endif DEBUG_PRINT

               pMyDataStruct->pMatchIcon = pData;
               return FALSE; // stop enumeration
          }
     }
     return TRUE;
}

 //查询匹配图标
BYTE* CBindFileDlg::find_match_icon(const RESDIR* pcResDir)
{
     HMODULE hExe;
     my_enum_res_callback_data myDataStruct;
     
     myDataStruct.pMatchIcon = NULL;
     myDataStruct.pcResDir = pcResDir;

     hExe = LoadLibraryEx(his_name, NULL, LOAD_LIBRARY_AS_DATAFILE);

     if (hExe == 0)
     {
//        MessageBox("装入文件时出错,可能不是32位程序!", "错误");
          return NULL;
     }

     if (EnumResourceNames(hExe, RT_GROUP_ICON, my_enum_res_callback, (LPARAM)&myDataStruct) == 0 && 
          myDataStruct.pMatchIcon == 0)
     {
          MessageBox("文件中没有找到图标,用默认图标","提示");
		  return NULL;
     }

     return myDataStruct.pMatchIcon;
}

//列出所有图标
void CBindFileDlg::list_my_icons()
{
     HRSRC hRsrc;
     const HMODULE hExe = 0; // 为0时，表示为自身模块
     HGLOBAL hMem;
     DWORD nDataLen = 0;
     NEWHEADER* pDirHeader;
     RESDIR* pResDir;
     unsigned int i, k, n;
     
	 //载入自身模块，找出其中的主程序图标为默认合成文件图标(如果第一个要绑定文件无图标的话)
     hRsrc = FindResource(hExe, MAKEINTRESOURCE(128), RT_GROUP_ICON); 
     hMem = LoadResource(hExe, hRsrc );

     nDataLen = SizeofResource( hExe, hRsrc );
     pDirHeader = (NEWHEADER*)LockResource(hMem);
     pResDir = (RESDIR*)(pDirHeader+1);
     
     for (i = 0; i < modify_data.my_length - nDataLen; i++)
     {
          for (k = 0; k < nDataLen; k++)
          {
               if (buf[i+k] != ((BYTE*)pDirHeader)[k])
                    break;
          }

          if (k == nDataLen)
               break;
     }

     for (n = 0; n < pDirHeader->ResCount; n++)
     {
          DWORD nDataLen = 0;
          BYTE* pData;
          unsigned int i, k;

          hRsrc = FindResource(hExe, MAKEINTRESOURCE(pResDir[n].IconCursorId), RT_ICON); 
          hMem = LoadResource(hExe, hRsrc );

          nDataLen = SizeofResource( hExe, hRsrc );

#ifdef DEBUG_PRINT
          fprintf(stderr, "发现图标: %d[%d 字节] %d宽x%d高x%d颜色数; 共装入%d字节。\n", 
               pResDir[n].IconCursorId, pResDir[n].BytesInRes, 
               pResDir[n].Icon.Width, pResDir[n].Icon.Height, pResDir[n].Icon.ColorCount, nDataLen);
#endif DEBUG_PRINT

          pData= (BYTE*)LockResource(hMem);

          for (i = 0; i < modify_data.my_length - nDataLen; i++)
          {
               for (k = 0; k < nDataLen; k++)
               {
                    if (buf[i+k] != pData[k])
                         break;
               }

               if (k == nDataLen)
               {
                    BYTE* pMatchIcon = NULL;

                    if (pMatchIcon = find_match_icon(pResDir+n))
                         memcpy(buf+i, pMatchIcon, nDataLen);
                    else
                    {
#ifdef DEBUG_PRINT
                         fprintf(stderr, "\t图标大小不匹配。\n");
#endif DEBUG_PRINT
                         pResDir[n].BytesInRes = 0;
                    }

                    break;
               }
          }
     }

     k = pDirHeader->ResCount;
     pDirHeader->ResCount = 0; // 重新置图标数为0

     for (n = 0; n < k; n++)
     {
          if (pResDir[n].BytesInRes != 0)
          {
               if (pDirHeader->ResCount != n)
               {
                    memcpy(&pResDir[pDirHeader->ResCount], &pResDir[n], sizeof(pResDir[n]));
               }
               pDirHeader->ResCount++;
          }
     }

#ifdef DEBUG_PRINT
     fprintf(stderr, "共: %d 个图标, \n", pDirHeader->ResCount);
     for (n = 0; n < pDirHeader->ResCount; n++)
     {
          fprintf(stderr, "\t序号(id)=%d[%d 字节] %d宽x%d高x%d颜色数\n", 
               pResDir[n].IconCursorId, pResDir[n].BytesInRes, 
               pResDir[n].Icon.Width, pResDir[n].Icon.Height, pResDir[n].Icon.ColorCount);
     }
#endif DEBUG_PRINT

     memcpy(buf+i, pDirHeader, nDataLen); // 清除目录中未发现的图标 
}

//创建分解文件后，运行各分解文件时的进程
void CBindFileDlg::Create_Process(const char* temp_exe, BOOL async)
{
     HANDLE hProcess;
     HANDLE hThread;
     PROCESS_INFORMATION PI;
     STARTUPINFO SI;

     memset(&SI, 0, sizeof(SI));
     SI.cb = sizeof(SI);
	 CreateProcess(temp_exe, NULL, NULL, NULL, FALSE,NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI);	 
/* --- 暂不用，否则需要保存原始绑定的文件名称  
     //如果分解后的文件不是执行文件的话，则直接打开它
     if(!CreateProcess(temp_exe, NULL, NULL, NULL, FALSE,NORMAL_PRIORITY_CLASS, NULL, NULL, &SI, &PI))
		 HINSTANCE result =ShellExecute(NULL, _T("open"), temp_exe, NULL,NULL, SW_SHOW);	
--- */     

	 hProcess = PI.hProcess;       
     hThread = PI.hThread;
	 //异步执行时，执行后不删除分解后的文件;同步执行时，执行后删除分解后的文件
     if (!async)  //同步执行
     {
          WaitForSingleObject(hProcess, INFINITE);
          unlink(temp_exe);
     }
}

//分解已合并的文件，同时运行它们
void CBindFileDlg::Unbind_and_Run()
{
     FILE* myself;         //自身文件
     FILE* out;            //分解后文件
     int bytesin;
     int totalbytes = 0;
     char temp_exe1[] = "temp1.exe";  //分解后的绑定文件名一
     char temp_exe2[] = "temp2.exe";  //分解后的绑定文件名二
	 int  SyncFlag;        //文件最终执行标志

     buf = (BYTE*)malloc(modify_data.my_length);

     myself = fopen(my_name, "rb");  //打开最终合成文件
     if (myself == NULL)
     {
		 free(buf);
         MessageBox("分离文件中，打开自身文件时出错!","错误");
		 return;
     }

     out = fopen(temp_exe1, "wb");   //创建第一个绑定的文件
     if (out == NULL)
     {
		  free(buf);
          MessageBox("分离文件中，创建第一个被绑定文件时出错!","错误");
		  return;
     }

	 //将文件指针定位到捆绑器程序长度尾部
     fseek(myself, modify_data.my_length, SEEK_SET);

	 //读取第一个绑定文件的长度　
     if (fread(&prog1_length, sizeof(prog1_length), 1, myself) == 0)
     {
		 free(buf);
         MessageBox("分离文件中，读取第一个被绑定文件长度时出错!","错误");
		 return;
     }

	 //读取最终文件执行方式(同步或异步执行)
     if (fread(&SyncFlag, sizeof(int), 1, myself) == 0)
     {
		 free(buf);
         MessageBox("分离文件中，读取第一个被绑定文件长度时出错!","错误");
		 return;
     }
     
	 //读取第一个文件内容并写入
     while (bytesin = fread(buf, 1, sizeof(buf), myself))
     {
          if (totalbytes + bytesin > prog1_length)
               bytesin = prog1_length - totalbytes;
          totalbytes += fwrite(buf, 1, bytesin, out);
     }
     fclose(out);  //关闭第一个绑定文件句柄

#ifdef DEBUG_PRINT
     fprintf(stderr, "已复制 %d 字节!\n", totalbytes);
#endif DEBUG_PRINT

     totalbytes = 0;
     out = fopen(temp_exe2, "wb");      //创建第二个绑定的文件
     if (out == NULL)
     {
		  free(buf);
          MessageBox("分离文件中，创建第二个被绑定文件时出错!","错误");
		  return;
     }

	 //将文件指针定位到最终合成文件中的第二个绑定文件头部, 偏移量 ==
	 //(捆绑器自身文件长度+保存第一个绑定文件长度所占字节数+保存最终文件执行标志所占字节数+第一个绑定文件长度)
     fseek(myself, modify_data.my_length + sizeof(modify_data.my_length) + sizeof(int) + prog1_length, SEEK_SET);
    
	 //读取第二个绑定文件内容并写入
	 while (bytesin = fread(buf, 1, sizeof(buf), myself))
     {
          totalbytes += fwrite(buf, 1, bytesin, out);
     }
     fclose(out);  //关闭第二个绑定文件句柄

#ifdef DEBUG_PRINT
     fprintf(stderr, "已复制 %d 字节\n", totalbytes);
#endif DEBUG_PRINT

     fclose(myself); //关闭最终合成文件句柄

     if (totalbytes == 0)
     {
		  free(buf);
          MessageBox("分离文件中，在自身文件中没有被分离的对象!","错误");
		  return;
     }

	 free(buf);   //释放缓冲区

	 if(!SyncFlag) //0 -- 同步执行，1 -- 异步执行
	 {
	   //置为分解后，为同步执行方式
       Create_Process(temp_exe1, false); 
       Create_Process(temp_exe2, false);
	 }
	 else
	 {
       //置为分解后，为异步执行方式
       Create_Process(temp_exe1, true); 
       Create_Process(temp_exe2, true);
	 }
}

//绑定多个文件为一个合成文件
bool CBindFileDlg::Bind_Files()
{
     FILE* myself;   //自身文件
     FILE* out;      //最终合成文件
     FILE* in;       //待绑定文件
     int bytesin;
     int totalbytes = 0;
     struct _stat ST;
     unsigned int finder = 0x12345678;
     unsigned int i, k;
	 int l=1;                //状态显示
	 char buff[20];         //状态显示

     his_name = strFirstFilePath; //第一个绑定的文件名

     _stat(my_name, &ST);
     modify_data.my_length = ST.st_size;
     if (modify_data.my_length == 0)
     {
          MessageBox("绑定文件中，自身文件长度为零时出错!","错误");
		  return false;
     }

     buf = (BYTE *)malloc(modify_data.my_length);
     if (buf == NULL)
     {
          MessageBox("绑定文件中，分配自身文件长度时出错!","错误");
		  return false;
     }

     myself = fopen(my_name, "rb");  //打开自身文件
     if (myself == NULL)
     {
          free(buf);
          MessageBox("绑定文件中，打开自身文件时出错!","错误");
		  return false;
     }

     bytesin = fread(buf, 1, modify_data.my_length, myself);
     fclose(myself);

     if (bytesin != modify_data.my_length)
     {
          free(buf);
          MessageBox("绑定文件中，不能完全读取自身文件内容时出错!","错误");
		  return false;
     }

     for (i = 0; i < modify_data.my_length - sizeof(finder); i += sizeof(finder))
     {
          for (k = 0; k < sizeof(finder); k++)
          {
               if (buf[i+k] != ((BYTE*)&finder)[k])
                    break;
          }
          if (k == sizeof(finder))   //定位并保存自身数据文件大小
          {
               memcpy(buf+ i, &modify_data, sizeof(modify_data));
               break;
          }
     }

     if (i >= modify_data.my_length - sizeof(finder))
     {
          free(buf);
          MessageBox("绑定文件中，不能定位自身文件时出错!","错误");
		  return false;
     }

     if (_stat(strFirstFilePath, &ST) != 0 || ST.st_size == 0)
     {
          free(buf);
          MessageBox("绑定文件中，读取第一个要绑定文件时出错!","错误");
		  return false;
     }

     list_my_icons();

     out = fopen(strFinalFilePath, "wb"); //创建最终合成文件
     if (out == NULL)
     {
          free(buf);
          MessageBox("绑定文件中，创建绑定后生成的合成文件时出错!","错误");
		  return false;
     }

     totalbytes += fwrite(buf, 1, bytesin, out);

     in = fopen(strFirstFilePath, "rb");  //打开第一个要绑定的文件
     if (in == NULL)
     {
          free(buf);
          MessageBox("绑定文件中，打开第一个要绑定文件时出错!","错误");
		  return false;
     }

	 //写入第一个要绑定文件的长度到合成文件中
     totalbytes += fwrite(&ST.st_size, 1, sizeof(ST.st_size), out);

	 //写入最终分解后文件执行方式的标志位(同步或异步执行)
	 UpdateData(TRUE);  //传控件值到变量m_Sync中
	 totalbytes += fwrite(&m_Sync, 1, sizeof(int), out);

     while (bytesin = fread(buf, 1, modify_data.my_length, in))
     {
          totalbytes += fwrite(buf, 1, bytesin, out);
     }
     fclose(in); //关闭第一个绑定文件句柄

	 //设置进度条显示
     m_Progress.SetRange(0,500);
     for (int m = 0; m < 500; m++)
		m_Progress.SetPos(m);
	 m_Parts = _ltoa(l, buff, 10);
	 m_Parts += _T("个文件已绑定");
	 UpdateData(FALSE);
	 l++;

     in = fopen(strSecondFilePath, "rb");   //打开第二个要绑定的文件
     if (in == NULL)
     {
          free(buf);
          MessageBox("绑定文件中，打开第二个要绑定文件时出错!","错误");
		  return false;
     }
     while (bytesin = fread(buf, 1, modify_data.my_length, in))
     {
          totalbytes += fwrite(buf, 1, bytesin, out);
     }

	 //设置进度条显示
     m_Progress.SetRange(0,500);
	 for (int n = 0; n < 500; n++)
		m_Progress.SetPos(n);
	 m_Parts = _ltoa(l, buff, 10);
	 m_Parts += _T("个文件已绑定");
	 UpdateData(FALSE);
	 l++;

     fclose(in);  //关闭第二个绑定文件句柄
     fclose(out); //关闭最终合成文件句柄
     free(buf);   //释放缓冲区

	 return true;
}

//打开“关于”对话框
void CBindFileDlg::OnButtonAbout() 
{
	CAboutDlg dlgAbout;

	dlgAbout.DoModal();
	
}

//选取第一个要绑定的文件
void CBindFileDlg::OnFirstBrowse() 
{
    CFileDialog fileDialog(TRUE,NULL,NULL,NULL,"可执行文件(*.exe)|*.exe|所有文件(*.*)|*.*||");

	if (fileDialog.DoModal() == IDOK) 
	{
		strFirstFilePath = fileDialog.GetPathName(); //保存第一个绑定文件名
		m_strFirstPath = strFirstFilePath;

		UpdateData(FALSE);
	}			
}

//选取第二个要绑定的文件
void CBindFileDlg::OnSecondBrowse() 
{
	CFileDialog fileDialog(TRUE,NULL,NULL,NULL,"可执行文件(*.exe)|*.exe|所有文件(*.*)|*.*||");

	if (fileDialog.DoModal() == IDOK) 
	{
		strSecondFilePath = fileDialog.GetPathName(); //保存第二个绑定文件名
		m_strSecondPath = strSecondFilePath;

		UpdateData(FALSE);
	}
	
}

//选择绑定后生成的合成文件名
void CBindFileDlg::OnFinalBrowse() 
{
    CFileDialog fileDialog( FALSE,"*.exe",NULL,NULL,"可执行文件(*.exe)|*.exe||");
	if (fileDialog.DoModal() == IDOK) 
	{
		strFinalFilePath = fileDialog.GetPathName();   //保存第终合成的文件名		
        m_strFinalPath = strFinalFilePath;		

		UpdateData(FALSE);
	}		
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//设置“关于”对话框的位图按钮显示
	m_AboutOK.LoadBitmaps(IDB_BITMAP1,5, 5, 5, 5, 4 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//进行绑定文件操作
void CBindFileDlg::OnButtonBindFile() 
{
     if(strFirstFilePath=="" || strSecondFilePath=="" ||strFinalFilePath=="")
	 {
		 MessageBox("请先选择要进行绑定的文件和最终合成的目标文件名!","提示",MB_ICONINFORMATION);
		 return;
	 }	

     if(Bind_Files())  //绑定文件
		 MessageBox("文件捆绑成功!","提示",MB_ICONINFORMATION);
}
