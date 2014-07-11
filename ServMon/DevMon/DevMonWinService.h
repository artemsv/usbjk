#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Text;
using namespace System::Collections;
using namespace System::ServiceProcess;
using namespace System::ComponentModel;
using namespace System::Diagnostics;
using namespace System::Threading;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Runtime::InteropServices;
using namespace System::Windows::Forms;

namespace DevMon {

	/// <summary>
	/// Summary for ServMonWinService
	/// </summary>
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	public ref class DevMonWinService : public System::ServiceProcess::ServiceBase
	{
		[DllImport("Helper.dll")]
		static int OpenDevice();

		[DllImport("Helper.dll")]
		static int CloseDevice();

		[DllImport("Helper.dll")]
		static int InitHelper();

		[DllImport("Helper.dll")]
		static int GetIntState();

	private:
		const static int PASSWORD_LEGTH = 10;
		const static int REC_BUF_SIZE = 1;
		static String^ PASSWORD = "usbdevmon";
		static String^ LOG_FILE_PATH = "DevMon.log";
		bool bDevExists;
		bool bOnStop;
		Thread^ thread;
		TcpListener^ tcpListener;
	public:
		DevMonWinService();
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~DevMonWinService();

		virtual void OnStart(array<String^>^ args) override;
		virtual void OnStop() override;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->CanStop = true;
			this->CanPauseAndContinue = false;
			this->AutoLog = true;
			this->ServiceName = L"DevMon";
		}
#pragma endregion

	private: 
			
		System::Void Log(String^ st);
		bool ReadCommand(NetworkStream^ stream, String^% result);
		System::String^ DevStateToBitStr(int intState);
		System::Void Listen();
	};
}
