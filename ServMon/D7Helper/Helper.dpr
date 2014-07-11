library Helper;

uses
  Windows, Main;

{$R *.res}

exports
  InitHelper name 'InitHelper',
  OpenDevice name 'OpenDevice',
  CloseDevice name 'CloseDevice',
  GetIntState name 'GetIntState';
end.
