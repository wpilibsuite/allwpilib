gz_msgs
=======
gz_msgs is a library that allows the transmission of messages
to and from Java and C++ programs.

gz_msgs currently requires libprotobuf-dev on Debian like systems.

If it's not found via pkg-config, then it's build is diabled.

You can force it by specifying -PmakeSim on the gradle command line.

If you are installing FRCSim with the script, then this *should* have be done for you.
