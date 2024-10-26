![Cascades theme utility by Vortesys](/misc/cascadesbeta.jpg)

## What is *Cascades*?
*Cascades* is a theme utility designed to make your computer look like classic operating systems such as Windows 3.1 and OS/2. Using the same method that Stardock's *Windowblinds* and Microsoft's *UxTheme* employ, every titlebar and button is skinned with a vintage look.

## What does it run on?
Support and compatibility for *Cascades* is provided for all versions of Windows XP and newer. This program is currently untested and unsupported on any non-x86/AMD64 platforms. This program is not compatible with Windows 2000 and older.

## Where do I get it?
The [releases page](https://github.com/Vortesys/Cascades/releases/latest) has the latest stable release of *Cascades*. Builds for every commit can be found in the GitHub Actions artifacts of each commit. In the future, this program will be bundled with others and form a program suite to bring back the early 90s desktop paradigm.

## How do I build it?
Cascades should compile easily with Visual Studio 2019 and newer.

# What are the directories for?

CscdCfg is the configuration utility, the program that you actually interact with.
CscdSvc is the service that runs Cascades. It contains the code that actually shows you the theme.
NTStyle is a DLL that exports drawing functions that are called by the service component. This is the component to look at if you want to modify the visual appearance of Cascades. In the future it will be easier to theme Cascades.

## How can I help?
Submitting [issues](/issues) and reporting or reproducing bugs is a major help. The next level is to contribute code and create a [pull request](/pulls). All feature requests are welcome but not all of them may be added in. If you're not sure about something, ask!

## What does it look like right now?
![image](https://github.com/Vortesys/Cascades/assets/36094486/8c676043-1558-4b65-a270-2441b7f8406d)
![image](https://github.com/Vortesys/Cascades/assets/36094486/a4a9735c-500b-439f-af45-33ebc4c28185)
