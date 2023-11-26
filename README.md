# cowhook from 10/29/23

## Getting Started with Cowhook on Linux

Cowhook is a versatile cheat for enhancing your Team Fortress 2 (TF2) gaming experience on Linux. This comprehensive guide will walk you through the process of downloading, installing, and effectively utilizing Cowhook on your system.
Dependencies

While not mandatory, it is highly recommended to install the dialog package for an enhanced user interface.

All other essential dependencies will be automatically handled by the dependencycheck script.
Downloading

Begin by opening a terminal window and executing the following command to obtain Cowhook:

```shell

git clone https://github.com/cowhookdev/cathook
cd cathook
./install-all

# This command sequence will initiate the download and create the cathook directory, housing all necessary source files.
 Injecting

To seamlessly integrate Cowhook into your TF2 gameplay, run the subsequent command while inside the source folder:

```shell

sudo ./attach

# This action will inject the libcathook.so library into the hl2_linux process.

Once successfully injected, you can access the menu by simply pressing the INSERT key.

## FAQ

How do I open the menu?
You can access the menu by pressing the INSERT key while your TF2 window is in focus.

Where is my configuration file saved?
Your configuration files are securely stored within the cathook folder, conveniently located in your /opt directory (/opt/cathook/data). These configuration files are presented in a human-readable format and can be easily edited using your preferred text editor.

Embrace the power of Cowhook and elevate your Team Fortress 2 gameplay to new heights!
