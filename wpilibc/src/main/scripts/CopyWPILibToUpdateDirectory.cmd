rem cd C:\WindRiver\workspace\WorkbenchUpdate
cd ..\..\WorkbenchUpdate


mkdir vxworks-6.3\target\h\WPILib\Buttons
mkdir vxworks-6.3\target\h\WPILib\CAN
mkdir vxworks-6.3\target\h\WPILib\ChipObject
mkdir vxworks-6.3\target\h\WPILib\CInterfaces
mkdir vxworks-6.3\target\h\WPILib\Commands
mkdir vxworks-6.3\target\h\WPILib\NetworkCommunication
mkdir vxworks-6.3\target\h\WPILib\SmartDashboard
mkdir vxworks-6.3\target\h\WPILib\visa
mkdir vxworks-6.3\target\h\WPILib\Vision
mkdir vxworks-6.3\target\h\WPILib\Vision2009

mkdir vxworks-6.3\target\h\WPILib\LiveWindow
mkdir vxworks-6.3\target\h\WPILib\networktables
mkdir vxworks-6.3\target\h\WPILib\tables
mkdir vxworks-6.3\target\h\WPILib\networktables2
mkdir vxworks-6.3\target\h\WPILib\networktables2\client
mkdir vxworks-6.3\target\h\WPILib\networktables2\connection
mkdir vxworks-6.3\target\h\WPILib\networktables2\server
mkdir vxworks-6.3\target\h\WPILib\networktables2\stream
mkdir vxworks-6.3\target\h\WPILib\networktables2\thread
mkdir vxworks-6.3\target\h\WPILib\networktables2\type
mkdir vxworks-6.3\target\h\WPILib\networktables2\util


del vxworks-6.3\target\h\WPIlib\*.h
del vxworks-6.3\target\h\WPIlib\Buttons\*.h
del vxworks-6.3\target\h\WPIlib\CAN\*.h
del vxworks-6.3\target\h\WPIlib\ChipObject\*.h
del vxworks-6.3\target\h\WPIlib\CInterfaces\*.h
del vxworks-6.3\target\h\WPIlib\Commands\*.h
del vxworks-6.3\target\h\WPIlib\NetworkCommunication\*.h
del vxworks-6.3\target\h\WPIlib\SmartDashboard\*.h
del vxworks-6.3\target\h\WPIlib\visa\*.h
del vxworks-6.3\target\h\WPIlib\Vision\*.h
del vxworks-6.3\target\h\WPIlib\Vision2009\*.h

del vxworks-6.3\target\h\WPIlib\LiveWindow\*.h
del vxworks-6.3\target\h\WPIlib\networktables\*.h
del vxworks-6.3\target\h\WPIlib\tables\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\client\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\connection\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\server\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\stream\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\thread\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\type\*.h
del vxworks-6.3\target\h\WPIlib\networktables2\util\*.h

del vxworks-6.3\target\lib\WPILib.a


copy C:\WindRiver\workspace\WPILib\*.h vxworks-6.3\target\h\WPILib
copy C:\WindRiver\workspace\WPILib\Buttons\*.h vxworks-6.3\target\h\WPILib\Buttons
copy C:\WindRiver\workspace\WPILib\CAN\*.h vxworks-6.3\target\h\WPILib\CAN
copy C:\WindRiver\workspace\WPILib\ChipObject\*.h vxworks-6.3\target\h\WPILib\ChipObject
copy C:\WindRiver\workspace\WPILib\CInterfaces\*.h vxworks-6.3\target\h\WPILib\CInterfaces
copy C:\WindRiver\workspace\WPILib\Commands\*.h vxworks-6.3\target\h\WPILib\Commands
copy C:\WindRiver\workspace\WPILib\NetworkCommunication\*.h vxworks-6.3\target\h\WPILib\NetworkCommunication
copy C:\WindRiver\workspace\WPILib\SmartDashboard\*.h vxworks-6.3\target\h\WPILib\SmartDashboard
copy C:\WindRiver\workspace\WPILib\visa\*.h vxworks-6.3\target\h\WPILib\visa
copy C:\WindRiver\workspace\WPILib\Vision\*.h vxworks-6.3\target\h\WPILib\Vision
copy C:\WindRiver\workspace\WPILib\Vision2009\*.h vxworks-6.3\target\h\WPILib\Vision2009

copy C:\WindRiver\workspace\WPILib\LiveWindow\*.h vxworks-6.3\target\h\WPILib\LiveWindow
copy C:\WindRiver\workspace\WPILib\networktables\*.h vxworks-6.3\target\h\WPILib\networktables
copy C:\WindRiver\workspace\WPILib\tables\*.h vxworks-6.3\target\h\WPILib\tables
copy C:\WindRiver\workspace\WPILib\networktables2\*.h vxworks-6.3\target\h\WPILib\networktables2
copy C:\WindRiver\workspace\WPILib\networktables2\client\*.h vxworks-6.3\target\h\WPILib\networktables2\client
copy C:\WindRiver\workspace\WPILib\networktables2\connection\*.h vxworks-6.3\target\h\WPILib\networktables2\connection
copy C:\WindRiver\workspace\WPILib\networktables2\server\*.h vxworks-6.3\target\h\WPILib\networktables2\server
copy C:\WindRiver\workspace\WPILib\networktables2\stream\*.h vxworks-6.3\target\h\WPILib\networktables2\stream
copy C:\WindRiver\workspace\WPILib\networktables2\thread\*.h vxworks-6.3\target\h\WPILib\networktables2\thread
copy C:\WindRiver\workspace\WPILib\networktables2\type\*.h vxworks-6.3\target\h\WPILib\networktables2\type
copy C:\WindRiver\workspace\WPILib\networktables2\util\*.h vxworks-6.3\target\h\WPILib\networktables2\util


copy C:\WindRiver\workspace\WPILib\PPC603gnu\WPILib\Debug\WPILib.a vxworks-6.3\target\lib

cd C:\WindRiver\workspace\WPILib\Scripts
