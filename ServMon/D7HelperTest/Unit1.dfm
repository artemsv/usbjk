object Form1: TForm1
  Left = 344
  Top = 230
  Width = 376
  Height = 182
  Caption = 'Helper Test Application'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 24
    Top = 24
    Width = 75
    Height = 25
    Caption = 'InitHelper'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 24
    Top = 88
    Width = 75
    Height = 25
    Caption = 'OpenDevice'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 136
    Top = 24
    Width = 75
    Height = 25
    Caption = 'CloseDevice'
    TabOrder = 2
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 136
    Top = 88
    Width = 75
    Height = 25
    Caption = 'GetIntState'
    TabOrder = 3
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 24
    Top = 120
    Width = 75
    Height = 25
    Caption = '#DE'
    TabOrder = 4
    OnClick = Button5Click
  end
  object Button6: TButton
    Left = 136
    Top = 120
    Width = 75
    Height = 25
    Caption = 'RDTSC'
    TabOrder = 5
    OnClick = Button6Click
  end
  object Button7: TButton
    Left = 256
    Top = 24
    Width = 75
    Height = 25
    Caption = 'INT 3'
    TabOrder = 6
    OnClick = Button7Click
  end
  object Button8: TButton
    Left = 248
    Top = 88
    Width = 75
    Height = 25
    Caption = '#GP'
    TabOrder = 7
    OnClick = Button8Click
  end
  object Button9: TButton
    Left = 248
    Top = 120
    Width = 75
    Height = 25
    Caption = '#UD'
    TabOrder = 8
    OnClick = Button9Click
  end
end
