cd C:\WindRiver\vxworks-6.3\target

rd /s /q h\WPILib

mkdir h\WPILib
mkdir h\WPILib\Buttons
mkdir h\WPILib\CAN
mkdir h\WPILib\ChipObject
mkdir h\WPILib\CInterfaces
mkdir h\WPILib\Commands
mkdir h\WPILib\NetworkCommunication
mkdir h\WPILib\SmartDashboard
mkdir h\WPILib\visa
mkdir h\WPILib\Vision
mkdir h\WPILib\Vision2009

mkdir h\WPILib\LiveWindow
mkdir h\WPILib\networktables
mkdir h\WPILib\tables
mkdir h\WPILib\networktables2
mkdir h\WPILib\networktables2\client
mkdir h\WPILib\networktables2\connection
mkdir h\WPILib\networktables2\server
mkdir h\WPILib\networktables2\stream
mkdir h\WPILib\networktables2\thread
mkdir h\WPILib\networktables2\type
mkdir h\WPILib\networktables2\util



copy c:\WindRiver\workspace\WPILib\*.h h\WPILib
copy c:\WindRiver\workspace\WPILib\Buttons\*.h h\WPILib\Buttons
copy c:\WindRiver\workspace\WPILib\CAN\*.h h\WPILib\CAN
copy c:\WindRiver\workspace\WPILib\ChipObject\*.h h\WPILib\ChipObject
copy C:\WindRiver\workspace\WPILib\CInterfaces\*.h h\WPILib\CInterfaces
copy C:\WindRiver\workspace\WPILib\Commands\*.h h\WPILib\Commands
copy C:\WindRiver\workspace\WPILib\NetworkCommunication\*.h h\WPILib\NetworkCommunication
copy C:\WindRiver\workspace\WPILib\SmartDashboard\*.h h\WPILib\SmartDashboard
copy c:\WindRiver\workspace\WPILib\visa\*.h h\WPILib\visa
copy c:\WindRiver\workspace\WPILib\Vision\*.h h\WPILib\Vision
copy c:\WindRiver\workspace\WPILib\Vision2009\*.h h\WPILib\Vision2009

copy C:\WindRiver\workspace\WPILib\LiveWindow\*.h h\WPILib\LiveWindow
copy C:\WindRiver\workspace\WPILib\networktables\*.h h\WPILib\networktables
copy C:\WindRiver\workspace\WPILib\tables\*.h h\WPILib\tables
copy C:\WindRiver\workspace\WPILib\networktables2\*.h h\WPILib\networktables2
copy C:\WindRiver\workspace\WPILib\networktables2\client\*.h h\WPILib\networktables2\client
copy C:\WindRiver\workspace\WPILib\networktables2\connection\*.h h\WPILib\networktables2\connection
copy C:\WindRiver\workspace\WPILib\networktables2\server\*.h h\WPILib\networktables2\server
copy C:\WindRiver\workspace\WPILib\networktables2\stream\*.h h\WPILib\networktables2\stream
copy C:\WindRiver\workspace\WPILib\networktables2\thread\*.h h\WPILib\networktables2\thread
copy C:\WindRiver\workspace\WPILib\networktables2\type\*.h h\WPILib\networktables2\type
copy C:\WindRiver\workspace\WPILib\networktables2\util\*.h h\WPILib\networktables2\util



copy C:\WindRiver\workspace\WPILib\PPC603gnu\WPILib\Debug\WPILib.a lib

rem copy c:\WindRiver\workspace\WorkbenchUpdate\frc_20*.zip c:\WindRiver\WPILib\cRIO_Images

cd \WindRiver\workspace\WPILib\Scripts
