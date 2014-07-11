unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls;

type
  TForm1 = class(TForm)
    Button1: TButton;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    Button7: TButton;
    Button8: TButton;
    Button9: TButton;
    procedure Button1Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Button7Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure Button9Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

function InitHelper: integer; external 'Helper.dll' name 'InitHelper';
function OpenDevice: integer; external 'Helper.dll' name 'OpenDevice';
function CloseDevice: integer; external 'Helper.dll' name 'CloseDevice';
function GetIntState: integer; external 'Helper.dll' name 'GetIntState';

procedure Log(msg: string; res: integer);
begin
  OutputDebugString(PChar(msg + ' = ' + IntToStr(res)));
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
  Log('InitHelper', InitHelper());
end;

procedure TForm1.Button3Click(Sender: TObject);
begin
  Log('CloseDevice', CloseDevice());
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  Log('Opendevice', Opendevice());
end;

procedure TForm1.Button4Click(Sender: TObject);
begin
  Log('GetIntState', GetIntState());
end;

procedure TForm1.Button5Click(Sender: TObject);
begin
  //try
  asm
{    mov eax, 0BABABABAh
    ud2}
    xor ebx, ebx
    mov eax, 90
    div ebx
 
  end;

  //except
  //end;
end;

procedure TForm1.FormCreate(Sender: TObject);
var
  r: HMENU;
begin
  //r := GetSystemMenu(Handle, false);
  //DeleteMenu(r, SC_CLOSE, MF_BYCOMMAND);


  //SetWindowLong(Handle, GWL_STYLE,
    //GetWindowLong(Handle, GWL_STYLE) and not WS_SYSMENU);
end;

procedure TForm1.Button6Click(Sender: TObject);
var
  _edx, _eax: integer;
begin
  asm
    rdtsc
    mov _edx, edx
    mov _eax, eax
  end;

  ShowMessage('Time = ' + IntToStr(_edx) + ' -- ' + IntToStr(_eax));
end;

procedure TForm1.Button7Click(Sender: TObject);
begin
  asm
    int 3
  end
end;

procedure TForm1.Button8Click(Sender: TObject);
begin
  asm
    mov eax, 100
    mov eax, [eax]
  end
end;

procedure TForm1.Button9Click(Sender: TObject);
begin
  asm
    ud2
  end;
end;

end.
 