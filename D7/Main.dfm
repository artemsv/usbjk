object Form1: TForm1
  Left = 97
  Top = 148
  Width = 650
  Height = 457
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object Label1: TLabel
    Left = 136
    Top = 48
    Width = 33
    Height = 16
    Caption = 'Code'
    Visible = False
  end
  object Label2: TLabel
    Left = 248
    Top = 48
    Width = 40
    Height = 16
    Caption = 'Param'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object Label3: TLabel
    Left = 344
    Top = 48
    Width = 47
    Height = 16
    Caption = 'Param2'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object Label4: TLabel
    Left = 152
    Top = 16
    Width = 130
    Height = 16
    Caption = 'Device not connected'
  end
  object Label5: TLabel
    Left = 400
    Top = 48
    Width = 72
    Height = 16
    Caption = 'Poll interval:'
  end
  object Button1: TButton
    Left = 16
    Top = 8
    Width = 113
    Height = 25
    Caption = 'Open Device'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 16
    Top = 72
    Width = 113
    Height = 25
    Caption = 'DeviceIoControl'
    TabOrder = 1
    Visible = False
    OnClick = Button2Click
  end
  object cbxIoctlCode: TComboBox
    Left = 136
    Top = 72
    Width = 97
    Height = 24
    Style = csDropDownList
    ItemHeight = 16
    TabOrder = 2
    Visible = False
    Items.Strings = (
      'SAY_HELLO'
      'SAY_GOODBAY'
      'PLAY_BAR'
      'GETUSBINFO')
  end
  object Memo1: TMemo
    Left = 8
    Top = 248
    Width = 625
    Height = 169
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Lucida Console'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 3
  end
  object heParam: THexEdit
    Left = 248
    Top = 72
    Width = 81
    Height = 24
    Color = clSkyBlue
    NumBase = ebHex
    MaxValue = 0
    MinValue = 0
    TabOrder = 4
    Validate = False
    Value = 0
    Visible = False
  end
  object btnClear: TButton
    Left = 560
    Top = 4
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 5
    OnClick = btnClearClick
  end
  object heParam2: THexEdit
    Left = 344
    Top = 72
    Width = 81
    Height = 24
    Color = clSkyBlue
    NumBase = ebHex
    MaxValue = 0
    MinValue = 0
    TabOrder = 6
    Validate = False
    Value = 0
    Visible = False
  end
  object PageControl1: TPageControl
    Left = 8
    Top = 72
    Width = 617
    Height = 169
    ActivePage = TabSheet1
    TabOrder = 7
    object TabSheet1: TTabSheet
      Caption = 'Device info'
      object Shape1: TShape
        Left = 320
        Top = 16
        Width = 57
        Height = 65
      end
      object b1: TShape
        Left = 328
        Top = 24
        Width = 17
        Height = 17
        Brush.Color = clGray
        Shape = stCircle
      end
      object b2: TShape
        Left = 352
        Top = 24
        Width = 17
        Height = 17
        Brush.Color = clGray
        Shape = stCircle
      end
      object b3: TShape
        Left = 328
        Top = 48
        Width = 17
        Height = 17
        Brush.Color = clGray
        Shape = stCircle
      end
      object b4: TShape
        Left = 352
        Top = 48
        Width = 17
        Height = 17
        Brush.Color = clGray
        Shape = stCircle
      end
      object Shape2: TShape
        Left = 344
        Top = 112
        Width = 185
        Height = 9
      end
      object Shape3: TShape
        Left = 344
        Top = 80
        Width = 9
        Height = 33
      end
      object Shape4: TShape
        Left = 528
        Top = 16
        Width = 73
        Height = 113
      end
      object Label6: TLabel
        Left = 536
        Top = 32
        Width = 58
        Height = 16
        Caption = 'Protected'
        Transparent = True
      end
      object Label7: TLabel
        Left = 542
        Top = 48
        Width = 45
        Height = 16
        Caption = 'System'
        Transparent = True
      end
      object btnDevDesc: TButton
        Left = 16
        Top = 16
        Width = 121
        Height = 25
        Caption = 'Device descriptor'
        TabOrder = 0
        Visible = False
        OnClick = btnDevDescClick
      end
      object btnDevStr: TButton
        Left = 16
        Top = 64
        Width = 89
        Height = 25
        Caption = 'Device string'
        TabOrder = 1
        Visible = False
        OnClick = btnDevStrClick
      end
      object seStrIndex: TSpinEdit
        Left = 120
        Top = 64
        Width = 57
        Height = 26
        MaxValue = 0
        MinValue = 0
        TabOrder = 2
        Value = 0
        Visible = False
      end
      object btnConfigDescr: TButton
        Left = 144
        Top = 16
        Width = 121
        Height = 25
        Caption = 'Config Descriptor'
        TabOrder = 3
        Visible = False
        OnClick = btnConfigDescrClick
      end
    end
  end
  object btnRead: TButton
    Left = 432
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Read'
    TabOrder = 8
    Visible = False
    OnClick = btnReadClick
  end
  object cbIntControl: TCheckBox
    Left = 304
    Top = 16
    Width = 145
    Height = 17
    Caption = 'Enable Polling'
    TabOrder = 9
  end
  object cbPollInterval: TComboBox
    Left = 480
    Top = 40
    Width = 73
    Height = 24
    Style = csDropDownList
    ItemHeight = 16
    TabOrder = 10
    Items.Strings = (
      '200'
      '400'
      '600'
      '800'
      '1000')
  end
  object btnApply: TButton
    Left = 560
    Top = 40
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 11
    OnClick = btnApplyClick
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 464
    Top = 8
  end
end
