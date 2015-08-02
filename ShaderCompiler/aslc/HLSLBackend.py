import _winreg

def findSdkPath():
	with _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows Kits\Installed Roots') as key:
		_winreg.QueryValue(key, 'KitsRoot10')
		return str(_winreg[0])
	
def findFXC():
	return findSdkPath() + r'\bin\fxc.exe'
