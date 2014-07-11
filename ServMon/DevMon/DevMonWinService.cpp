// ServMon.cpp : main Windows Service project file.

#include "stdafx.h"
#include <string.h>
#include "DevMonWinService.h"

using namespace DevMon;
using namespace System::Text;
using namespace System::Security::Policy;
using namespace System::Reflection;

//To install/uninstall the service, type: "DevMon.exe -Install [-u]"
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc >= 2)
	{
		if (argv[1][0] == _T('/'))
		{
			argv[1][0] = _T('-');
		}

		if (_tcsicmp(argv[1], _T("-Install")) == 0)
		{
			array<String^>^ myargs = System::Environment::GetCommandLineArgs();
			array<String^>^ args = gcnew array<String^>(myargs->Length - 1);

			// Set args[0] with the full path to the assembly,
			Assembly^ assem = Assembly::GetExecutingAssembly();
			args[0] = assem->Location;

			Array::Copy(myargs, 2, args, 1, args->Length - 1);
			AppDomain^ dom = AppDomain::CreateDomain(L"execDom");
			Type^ type = System::Object::typeid;
			String^ path = type->Assembly->Location;
			StringBuilder^ sb = gcnew StringBuilder(path->Substring(0, path->LastIndexOf(L"\\")));
			sb->Append(L"\\InstallUtil.exe");
			Evidence^ evidence = gcnew Evidence();
			dom->ExecuteAssembly(sb->ToString(), evidence, args);
		}
	}
	else
	{
		ServiceBase::Run(gcnew DevMonWinService());
	}
}


namespace DevMon 
{
	DevMonWinService::DevMonWinService()
	{
		InitializeComponent();
		//
		//TODO: Add the constructor code here
		//
	}

	DevMonWinService::~DevMonWinService()
	{
		if (components)
		{
			delete components;
		}
	}

	void DevMonWinService::OnStart(array<String^>^ args)
	{
		if (InitHelper())
		{
			bDevExists = OpenDevice();

			if (bDevExists)
			{
				thread = gcnew Thread(gcnew ThreadStart(this, 
					&DevMonWinService::Listen));

				thread->Start();

				bOnStop = false;
				Log("Service was started successfully");
			}
			else
			{
				Log("USB Guard Device wasn't opened");
			}
		}
		else
		{
			Log("Helper DLL wasn't initialized");
			//Log("Service was started successfully");
		}

	}

	void DevMonWinService::OnStop() 
	{
		bOnStop = true;

		if (tcpListener)
			tcpListener->Stop();

		while (thread->IsAlive)
		{
			Thread::Sleep(200);
		}

		Log("Service was stopped successfully");
	}


	System::Void DevMonWinService::Log(String^ st)
	{
		StringBuilder^ sb = gcnew StringBuilder(String::Format("{0} - {1}", 
			DateTime::Now.ToString("u")->Replace("Z", String::Empty), st));

		Debug::WriteLine(sb);

		String^ stLogFilePath = Path::Combine(Application::StartupPath, LOG_FILE_PATH);

		try
		{
			File::AppendAllText(stLogFilePath, sb->Append(Environment::NewLine)->ToString());
		}
		catch(Exception  ^ex)
		{
			Debug::WriteLine("Log: Exception:");
			Debug::WriteLine(ex->Message);
		}
	}

	bool DevMonWinService::ReadCommand(NetworkStream^ stream, String^% result)
	{
		array<Byte>^bytes = gcnew array<Byte>(REC_BUF_SIZE);

		int totalLength = 0;
		int _Length = 0;

		totalLength = 0;
		result = String::Empty;

		do
		{	
			_Length = stream->Read(bytes, 0, REC_BUF_SIZE);

			if (_Length > 0)
			{
				result = result + Encoding::ASCII->GetString(bytes, 0, _Length);
				totalLength += _Length;
			}else
			{
				Log("Connection was disconnected by client");
				return true;
			}
		}while (totalLength < PASSWORD->Length);

		return false;
	}

	System::String^ DevMonWinService::DevStateToBitStr(int intState)
	{
		String^ stDevState = String::Empty;

		int bit = 1;

		while (bit < 16)
		{
			if ((intState & bit) == bit)
				stDevState += "0";
			else
				stDevState += "1";

			bit <<= 1;
		}

		return stDevState;
	}

	System::Void DevMonWinService::Listen()
	{
		tcpListener = gcnew TcpListener(23000);
		TcpClient^ tcpClient = nullptr;
		NetworkStream^ stream = nullptr;
		array<Byte>^ sendBuffer;
		bool disconnected;
		String^ st = String::Empty;
		String^ stDevState = String::Empty;

		for (;;)
		{
			Thread::Sleep(1000);

			tcpListener->Start();
			Log("Listener started");

			try
			{
				tcpClient = tcpListener->AcceptTcpClient();
			}
			catch(SocketException^ ex)
			{
				//  а если сокет закрыли просто так, то служба висит???
				tcpClient = nullptr;
				Log("Listening was aborted");
			}

			if (tcpClient)
			{
				Log("Client accepted");
				stream = tcpClient->GetStream();
				disconnected = ReadCommand(stream, st);

				if (!disconnected)
				{
					if (st->Equals(dynamic_cast<String^>(PASSWORD)))
					{
						do
						{	
							if (bOnStop)
								break;

							Thread::Sleep(1000);

							if (stream->CanWrite && tcpClient->Connected)
							{
								try
								{
									if (OpenDevice())
									{
										int intState = GetIntState() & 0x0f;
										stDevState = DevStateToBitStr(intState);
										CloseDevice();
									}else
										stDevState = "----";

									sendBuffer = Encoding::ASCII->GetBytes(stDevState + Environment::NewLine);
									stream->Write(sendBuffer, 0, sendBuffer->Length);
								}
								catch(Exception^ ex)
								{
									Log(ex->Message);
									break;
								}
							}else
							{
								Log("Write Error");
								break;
							}

						} while (true);
					}
					else
						Log("Incorrect client. Disconnected.");

				}

				tcpClient->Close();
			}

			tcpListener->Stop();

			if (bOnStop)
				break;
		}
	}
}